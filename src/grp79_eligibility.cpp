// Group ID: 79
#include "grp79_eligibility.h"

#include "grp79_exceptions.h"
#include "grp79_student_summary.h"

EligibilityChecker::EligibilityChecker(const ProgramRules& rules,
                                       const CourseMap& courses)
    : rules_(rules), courses_(courses) {}

std::string EligibilityChecker::label(EligibilityCode code) {
    switch (code) {
        case EligibilityCode::E2: return "E2";
        case EligibilityCode::E3: return "E3";
        case EligibilityCode::E4: return "E4";
        case EligibilityCode::E5: return "E5";
        case EligibilityCode::E6: return "E6";
        case EligibilityCode::E7: return "E7";
    }
    return "E?";
}

void EligibilityChecker::raise(const EligibilityFailure& failure) {
    switch (failure.code) {
        case EligibilityCode::E2: throw AlreadyPassedException(failure.message);
        case EligibilityCode::E3: throw DuplicateRegistrationException(failure.message);
        case EligibilityCode::E4: throw PrerequisiteNotMetException(failure.message);
        case EligibilityCode::E5: throw AntiRequisiteException(failure.message);
        case EligibilityCode::E6: throw CreditLimitExceededException(failure.message);
        case EligibilityCode::E7: break;
    }
    throw ThesisNotAllowedException(failure.message);
}

std::optional<EligibilityFailure> EligibilityChecker::firstFailure(
    const Student& student, const Course& course, const Term& term) const {
    const StudentSummary summary(student, courses_);
    const std::string& code = course.code();

    // E2: already passed, unless the grade-improvement rule applies.
    if (summary.hasPassed(code)) {
        const Grade& grade = *summary.countingGrade(code);
        const bool improvementOk =
            course.allowsImprovement(grade) && !student.hasImprovementAttempt(code);
        if (!improvementOk) {
            return EligibilityFailure{
                EligibilityCode::E2,
                "course " + code + " already passed with grade " + grade.symbol()};
        }
    }

    // E3: an open registration in the same course.
    if (student.hasOpenAttempt(code)) {
        return EligibilityFailure{EligibilityCode::E3,
                                  "course " + code + " already has an open registration"};
    }

    // E4: every prerequisite passed at or above its minimum grade.
    for (const Prerequisite& pre : course.prerequisites()) {
        const Grade* grade = summary.countingGrade(pre.code);
        if (grade == nullptr || !grade->meetsMinimum(pre.minGrade)) {
            return EligibilityFailure{
                EligibilityCode::E4, "prerequisite " + pre.code + " (minimum " +
                                         pre.minGrade.symbol() + ") is not met"};
        }
    }

    // E5: no anti-requisite already passed.
    for (const std::string& anti : course.antiRequisites()) {
        if (summary.hasPassed(anti)) {
            return EligibilityFailure{EligibilityCode::E5,
                                      "anti-requisite " + anti + " is already passed"};
        }
    }

    // E6: credit load of the term (a thesis counts only in its start term).
    int loaded = 0;
    for (const auto& entry : student.attempts()) {
        const auto found = courses_.find(entry.first);
        if (found == courses_.end()) {
            continue;
        }
        for (const auto& attempt : entry.second) {
            if (attempt->term() == term) {
                loaded += found->second->credits();
            }
        }
    }
    if (loaded + course.credits() > rules_.maxLoad) {
        return EligibilityFailure{
            EligibilityCode::E6,
            "term " + term.toString() + " would carry " +
                std::to_string(loaded + course.credits()) + " credits; maxload is " +
                std::to_string(rules_.maxLoad)};
    }

    // E7: thesis only.
    if (course.type() == CourseType::Thesis) {
        if (summary.earnedCredits() < rules_.thesisMinCredits) {
            return EligibilityFailure{
                EligibilityCode::E7,
                "earned credits " + std::to_string(summary.earnedCredits()) +
                    " are below thesis_min_credits " +
                    std::to_string(rules_.thesisMinCredits)};
        }
        const GpaAccumulator gpa = summary.gpa();
        if (gpa.empty() || gpa.hundredths() < rules_.thesisMinGpaHundredths) {
            return EligibilityFailure{EligibilityCode::E7,
                                      "GPA is below thesis_min_gpa (or there is no GPA yet)"};
        }
        for (const auto& entry : student.attempts()) {
            const auto found = courses_.find(entry.first);
            if (found == courses_.end() || found->second->type() != CourseType::Thesis) {
                continue;
            }
            if (student.hasOpenAttempt(entry.first) || summary.hasPassed(entry.first)) {
                return EligibilityFailure{
                    EligibilityCode::E7,
                    "another thesis (" + entry.first + ") is open or passed"};
            }
        }
    }
    return std::nullopt;
}