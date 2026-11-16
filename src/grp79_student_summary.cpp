// Group ID: 79
#include "grp79_student_summary.h"

StudentSummary::StudentSummary(const Student& student, const CourseMap& courses)
    : student_(student), courses_(courses) {}

const Course* StudentSummary::findCourse(const std::string& code) const {
    const auto it = courses_.find(code);
    return it == courses_.end() ? nullptr : it->second.get();
}

const Grade* StudentSummary::countingGrade(const std::string& courseCode) const {
    const Attempt* attempt = student_.countingAttempt(courseCode);
    return attempt == nullptr ? nullptr : attempt->finalGrade();
}

bool StudentSummary::hasPassed(const std::string& courseCode) const {
    const Course* course = findCourse(courseCode);
    const Grade* grade = countingGrade(courseCode);
    return course != nullptr && grade != nullptr && course->earnsCredits(*grade);
}

int StudentSummary::earnedCredits() const {
    int total = 0;
    for (const Attempt* attempt : student_.countingAttempts()) {
        if (hasPassed(attempt->courseCode())) {
            total += findCourse(attempt->courseCode())->credits();
        }
    }
    return total;
}

GpaAccumulator StudentSummary::gpa() const {
    GpaAccumulator accumulator;
    for (const Attempt* attempt : student_.countingAttempts()) {
        const Course* course = findCourse(attempt->courseCode());
        const Grade* grade = attempt->finalGrade();
        if (course != nullptr && course->countsInGpa() && grade != nullptr &&
            grade->isLetter()) {
            accumulator.add(course->credits(), grade->points());
        }
    }
    return accumulator;
}