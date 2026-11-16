<!-- Group ID: 79 -->

# AcadERP: Academic Records and Graduation Audit System

AcadERP is a command-based C++17 program that manages student records, course registrations and grades. It supports regular, SU and thesis courses. It can check graduation status, GPA, average grade and course eligibility. If an invalid command is entered, the program displays an error with its line number and continues running.

## Build and Run

```bash
make                 # Build the program
./run_tests.sh       # Run all test cases
make materialtest    # Test deep copying of course materials
make clean           # Remove generated files
./acaderp < tests/T11.in
```

The program prints results and error codes to standard output. Readable error messages are printed to standard error.

## Tests

| Tests | What they check |
|---|---|
| T01, T11 | Project's worked example |
| T02, T03 | Grade submission, replacement and invalid grades |
| T04 | Invalid commands and course definitions |
| T05 | GPA rounding and grade boundaries |
| T06, T07 | Thesis registration and state changes |
| T08–T10 | Eligibility rules, repeated courses and grade improvement |
| T12 | Graduation rules R1–R7 |
| T13 | TRANSCRIPT output |
| T14 | State remains unchanged after failed commands |

Expected outputs were prepared according to the project rules. T11 uses B-, C- and A- on lines 13, 19 and 23.

## Design Summary

| Class | Purpose |
|---|---|
| Grade | Stores a valid grade and checks grade symbols |
| Course, RegularCourse, SuCourse, ThesisCourse | Handle the different types of courses |
| Attempt, ThesisAttempt | Store registrations and grade history |
| ThesisRecord | Manages thesis progress and status |
| Student, StudentSummary | Store student records and calculate results |
| EligibilityChecker | Checks eligibility rules E2–E7 |
| GraduationAuditor | Checks graduation rules R1–R7 |
| GpaAccumulator | Calculates GPA using integer arithmetic |
| ProgramRules, Registrar | Store program rules and handle commands |
| Command, CommandParser | Read commands and create command objects |
| CourseMaterial and related classes | Copy course materials independently using the Prototype pattern |

The program uses virtual functions to handle course-specific behaviour. `makeCourse()` is the only place where course types are selected using a switch statement. The Command pattern is used for processing commands, while the Prototype pattern is used for copying course materials.

## Exception Handling

The program uses `ErpException` as its main exception class, with five exception families and 18 specific error classes. Errors are caught by reference and reported without stopping the program.

The only `catch (...)` is in `main()`. It handles unexpected errors by printing `FATAL` to standard error and exiting with code 2.

## Exception Safety

The program checks all conditions before changing any data. Therefore, if an operation fails, the previous data remains unchanged. This is implemented in course registration, grade recording and thesis progress. T14 tests this behaviour using 20 failing commands.

## Thesis Lifecycle

A thesis can have five states: REGISTERED, IN_PROGRESS, AT_RISK, TERMINATED and FINAL.

- S changes the state to IN_PROGRESS.
- X changes it to AT_RISK.
- A second consecutive X changes it to TERMINATED.
- CONVERT is allowed only from IN_PROGRESS.

A terminated thesis does not earn credits or count towards GPA.

## TRANSCRIPT Format

```text
TRANSCRIPT <roll> <name> <CODE>@<TERM>=<g1>[><g2>...] ... EARNED=<n> GPA=<x.yy|NA>
```

The transcript shows each course, term and grade history. For example, `I>B` means an incomplete grade was later replaced by B. A `-` means no grade has been submitted. Courses are listed by code and registration order. Earned credits count each course only once.

## Assumptions

1. PROGRAM can be changed before the first COURSE. After that, it gives `E_PARSE`. Unspecified settings return to default values.
2. GRADE checks the student, course, grade symbol, course type, registration and finality in that order.
3. Grade I can only be replaced by a final grade. Submitting I again gives `E_GRADE_FINAL`.
4. An invalid or non-passing prerequisite grade gives `E_COURSE_DEF`.
5. CONVERT checks the student, course, symbol, registration and thesis status. Non-thesis courses and non-letter symbols give `E_GRADE_TYPE`.
6. Repeating a progress grade in the same term gives `E_GRADE_FINAL`. Earlier or pre-registration terms give `E_THESIS_TRANSITION`.
7. Anti-requisites work in one direction. Unknown anti-requisite codes are accepted.
8. E2 is checked before E3. E7 fails when no GPA data is available.
9. A passed course can be improved once. An open improvement registration counts as an existing pass.
10. A terminated thesis or one converted to E/F is treated as a failed attempt. A new thesis can then be registered.
11. E6 counts every registration in a term, whether graded or not. A thesis counts only in its starting term.
12. R5 requires at least one thesis. R2 and R4 check the full course catalogue. R7 ignores thesis attempts.
13. Course codes, roll numbers and names are case-sensitive. Names contain one word.
14. Credits allow up to nine digits. Negative or invalid values give `E_COURSE_DEF`. GPA thresholds can be up to 10.00.
15. Commands are case-sensitive. Blank lines and comments still count in line numbers. CRLF input is supported.
16. The sample's missing minus signs were restored: B-, C- and A-. These give the expected results of 61/7 and 153/19.

## Performance

The program was tested using `tools/gen_load.py`. Each student registers for 40 courses and runs GPA, AVG, GRAD and TRANSCRIPT.

| Students | Script Lines | Time | Memory |
|---|---:|---:|---:|
| 3,000 | 255,040 | 0.44 s | 38 MB |
| 30,000 | 2,550,040 | 4.72 s | N/A |

The program runs much faster than the required 3,000 students per hour. It uses hash maps for quick student and course searches and calculates results when needed instead of storing totals that could become incorrect.

For larger workloads, performance could be improved using GPA caching, faster input/output, pooled memory allocation and multiple threads with proper synchronization.

## Not Implemented

The optional bonus features SAVE/LOAD, CHANGE with audit log and custom allocator have not been implemented.