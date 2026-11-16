// Group ID: 79
#include "grp79_graduation.h"

#include "grp79_student_summary.h"

GraduationAuditor::GraduationAuditor(const ProgramRules& rules,
                                     const CourseMap& courses)
    : rules_(rules), courses_(courses) {}

std::vector<std::string> GraduationAuditor::failedRules(
    const Student& student) const {
    const StudentSummary summary(student, courses_);
    std::vector<std::string> failed;

    // R1: earned credits >= total (each course counted once).
    if (summary.earnedCredits() < rules_.totalCredits) {
        failed.push_back("R1");
    }

    // R2: every CORE course in the catalogue is passed.
    // R3: passed ELECTIVE credits >= elective.
    // R4: every MANDATORY_SU course has counting grade S.
    bool allCorePassed = true;
    bool allMandatorySuDone = true;
    int electiveCredits = 0;
    for (const auto& entry : courses_) {
        const Course& course = *entry.second;
        const bool passed = summary.hasPassed(course.code());
        switch (course.category()) {
            case Category::Core:
                allCorePassed = allCorePassed && passed;
                break;
            case Category::Elective:
                if (passed) {
                    electiveCredits += course.credits();
                }
                break;
            case Category::MandatorySu:
                allMandatorySuDone = allMandatorySuDone && passed;
                break;
            case Category::Open:
                break;
        }
    }
    if (!allCorePassed) {
        failed.push_back("R2");
    }
    if (electiveCredits < rules_.electiveCredits) {
        failed.push_back("R3");
    }
    if (!allMandatorySuDone) {
        failed.push_back("R4");
    }

    // R5: some thesis is FINAL with a letter at or above thesis_min_grade.
    bool thesisOk = false;
    for (const auto& entry : courses_) {
        if (entry.second->type() != CourseType::Thesis) {
            continue;
        }
        const Grade* grade = summary.countingGrade(entry.first);
        if (grade != nullptr && grade->isLetter() &&
            grade->meetsMinimum(rules_.thesisMinGrade)) {
            thesisOk = true;
        }
    }
    if (!thesisOk) {
        failed.push_back("R5");
    }

    // R6: rounded GPA >= mingpa; no GPA data also fails.
    const GpaAccumulator gpa = summary.gpa();
    if (gpa.empty() || gpa.hundredths() < rules_.minGpaHundredths) {
        failed.push_back("R6");
    }

    // R7: no regular or SU registration is pending or holds I.
    bool anyOpen = false;
    for (const auto& entry : student.attempts()) {
        const auto found = courses_.find(entry.first);
        if (found == courses_.end() || found->second->type() == CourseType::Thesis) {
            continue;
        }
        for (const auto& attempt : entry.second) {
            if (!attempt->isFinal()) {
                anyOpen = true;
            }
        }
    }
    if (anyOpen) {
        failed.push_back("R7");
    }
    return failed;
}