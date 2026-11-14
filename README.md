# AcadERP: Academic Records and Graduation Audit System

A command-driven C++17 program. It reads a script from standard input, records
grades for regular, SU and thesis courses, and answers four audit questions:
graduation, GPA, average grade and course eligibility. Every illegal action
raises a typed exception that is caught at the command loop, printed as
`ERROR <line> <CODE>` and never stops the run.

## Build and run

    make                 # builds ./acaderp with -std=c++17 -Wall -Wextra -pedantic -Werror
    ./run_tests.sh       # runs every tests/*.in and diffs against tests/*.out
    make materialtest    # builds and runs the CourseMaterial deep-copy check
    make clean
    ./acaderp < tests/T11.in

Standard output carries results and `ERROR <line> <CODE>` lines. A readable
message for each error goes to standard error.

## Tests

| Test | Covers |
|---|---|
| T01 | worked sample from the project document, first part |
| T02, T03 | grade submission: I replaced once, final grades locked, symbol and type errors |
| T04 | parse errors and course-definition errors |
| T05 | GPA rounding (8.125 -> 8.13, 3.50 and 9.50 band edges) |
| T06, T07 | thesis lifecycle: transitions, CONVERT errors, TERMINATED, thesis registered again |
| T08, T09, T10 | eligibility E1 to E7, repeats, grade improvement, thesis rules |
| T11 | the complete worked sample (note: its grades on lines 13, 19, 23 are B-, C-, A-) |
| T12 | graduation rules R1 to R7, from all failing to YES |
| T13 | TRANSCRIPT |
| T14 | strong exception guarantee |

Expected outputs were worked out from the rules in the project document (T01 and T11
come directly from its worked sample) and checked against the program's output.

## Design summary

| Class | Responsibility |
|---|---|
| `Grade` | immutable value object; only obtainable through `Grade::fromSymbol`, which throws `InvalidGradeSymbolException` |
| `Course` (abstract), `RegularCourse`, `SuCourse`, `ThesisCourse` | per-type behaviour in virtual functions: `validateGrade`, `countsInGpa`, `earnsCredits`, `minCredits`, `defaultMinimumGrade`, `allowsImprovement`, `makeAttempt`. The only `switch` on the course type is the creation factory `makeCourse` |
| `Attempt`, `ThesisAttempt` | one registration with its grade history. `ThesisAttempt` replaces the grading rules with the thesis state machine |
| `ThesisRecord` | holds the thesis state and is the only code allowed to change it |
| `Student` | attempts grouped by course code; finds the counting attempt |
| `StudentSummary` | earned credits, GPA, passed courses, all from counting attempts only |
| `EligibilityChecker` | checks E2 to E7 in order; first failure decides |
| `GraduationAuditor` | rules R1 to R7 |
| `GpaAccumulator` | exact integer GPA arithmetic; no floating point anywhere |
| `ProgramRules` | PROGRAM values and defaults |
| `Registrar` | facade used by the commands |
| `Command` subclasses, `CommandParser` | Command pattern: the parser builds one object per line and touches no state |
| `CourseMaterial`, `Syllabus`, `LectureNotes`, `Assignment`, `MaterialBundle` | Prototype pattern: `clone()` returns a deep copy so a new instructor can edit their copy |

Exceptions: `ErpException` (derives from `std::runtime_error`, carries the code)
with five sibling families (`InputException`, `EntityException`, `GradeException`,
`RegistrationException`, `ComputationException`) and 18 leaf classes. Family and
base constructors are protected, so only the leaf classes can be thrown. Handlers
catch by reference and throw by value. The only `catch (...)` is the outer guard
in `main`, which prints `FATAL` to standard error and exits with code 2.

Strong exception guarantee: every mutating operation checks everything first and
changes state last (see `Registrar::registerCourse`, `Attempt::recordGrade`,
`ThesisRecord::applyProgress`). T14 fires 20 failing commands and shows that
state is identical before and after.

Thesis state machine (`ThesisRecord`): REGISTERED, IN_PROGRESS, AT_RISK,
TERMINATED, FINAL. S goes to IN_PROGRESS. X goes to AT_RISK, and a second X in a
row goes to TERMINATED. CONVERT is legal only from IN_PROGRESS.

## TRANSCRIPT format

One line:

    TRANSCRIPT <roll> <name> <CODE>@<TERM>=<g1>[><g2>...] ... EARNED=<n> GPA=<x.yy|NA>

Each attempt shows its grade history joined by `>`; `I>B` means Incomplete later
replaced by B; `-` means no grade yet. Attempts are listed by course code, then in
registration order. EARNED counts each course once from its counting attempt.

## Assumptions

The project document leaves these open; these are the rules this program uses.

1. `PROGRAM` after the first `COURSE` gives `E_PARSE`. A second `PROGRAM` before any
   course replaces the first one; keys not given revert to their defaults.
2. `GRADE` checks in this order: student, course, symbol (`E_GRADE_SYMBOL`), course
   type (`E_GRADE_TYPE`), registration (`E_NOT_REGISTERED`), finality (`E_GRADE_FINAL`).
3. A second `I` on a registration that already holds `I` gives `E_GRADE_FINAL`,
   since `I` may only be replaced by a final grade.
4. A `PRE=` minimum grade that is not a known symbol, or not a passing grade of the
   prerequisite course, gives `E_COURSE_DEF`.
5. `FATAL` is written to standard error.
6. `CONVERT` checks student, course, symbol, then registration (latest attempt of
   that course). A non-letter symbol (`S`, `X`, `I`) gives `E_GRADE_TYPE`, checked
   before the thesis state. `CONVERT` on a registered non-thesis course gives
   `E_GRADE_TYPE`. `CONVERT` on a `TERMINATED` thesis gives `E_THESIS_TERMINATED`.
7. A progress grade in the same term as the previous one gives `E_GRADE_FINAL`; an
   earlier term, or a term before registration, gives `E_THESIS_TRANSITION`.
8. Anti-requisites work in one direction only: course A blocks registration for A
   when A lists the course in its own `ANTI=`. An unknown code in `ANTI=` is
   accepted (it can never be passed). A course listing itself is not rejected.
9. E7 with no GPA data fails, consistent with R6.
10. E2 runs before E3. A student with an open improvement registration therefore gets
    `E_ALREADY_PASSED`, not `E_DUP_REG`.
11. A registration counts as a grade improvement when the counting attempt was
    already passed at registration time; each course allows it once.
12. A `TERMINATED` thesis counts as a failed attempt (no credits, no GPA), so a new
    thesis may be registered. A thesis converted to E or F is also a failed attempt.
13. E6 counts every registration of the term (graded or not, passed or failed).
    A thesis counts only in its start term.
14. R5 fails when the student has no thesis at all. R2 and R4 look at the whole
    catalogue. R7 ignores thesis attempts, because R5 covers them.
15. Course codes, roll numbers and names are case-sensitive; names are one token.
16. Credits are parsed as integers of up to 9 digits (more is `E_PARSE`); a negative
    or out-of-range value is `E_COURSE_DEF`. GPA thresholds accept `d`, `d.d` or
    `d.dd` up to 10.00.
17. Command names are case-sensitive. A line whose first token starts with `#` is a
    comment. Blank and comment lines still count for line numbers. CRLF is accepted.
18. The project document's sample lost its minus signs in text extraction. Lines 13,
    19 and 23 are `B-`, `C-` and `A-`; only these give the stated 61/7 and 153/19.

## Performance

The requirement is about 3000 students per hour, which is under one student per
second. Measured with `tools/gen_load.py` (each student takes 40 courses and then
runs GPA, AVG, GRAD and TRANSCRIPT):

| Scale | Script lines | Time | Peak memory |
|---|---|---|---|
| 3000 students | 255,040 | 0.44 s | 38 MB |
| 30,000 students | 2,550,040 | 4.72 s | n/a |

That is several orders of magnitude faster than needed, and growth is roughly
linear. Reasons:

- Students are in an `unordered_map` by roll number and courses in an
  `unordered_map` by code: O(1) average lookup.
- Each student keeps attempts in a `map` by course code, so the counting attempt of
  one course is found without scanning other courses: O(log C + k), where C is the
  number of courses the student attempted and k the attempts of that course.
- GPA, GRAD, ELIGIBLE and TRANSCRIPT are computed from counting attempts on demand,
  O(C) to O(C * k) per query, with no stored totals that could go stale.
- The parser builds objects without touching state, so a rejected line costs nothing.

If load grew by several orders of magnitude: cache a `GpaAccumulator` per student
and update it when a grade changes; call `std::ios::sync_with_stdio(false)` and
avoid flushing; replace the per-attempt heap allocation with a pooled allocator
(this is the allocator bonus idea); shard students by roll number across worker
threads (queries are `const`, so reads are safe; writes need a lock per student).

## Not implemented

The optional bonus items (SAVE/LOAD, CHANGE with audit log, custom allocator) are
not implemented.
