#ifndef ACADERP_STUDENT_SUMMARY_H
#define ACADERP_STUDENT_SUMMARY_H

#include <string>

#include "course.h"
#include "gpa.h"
#include "student.h"

// Read-only view of what a student has achieved, derived from the counting
// attempt of each course only (earlier attempts never matter).
class StudentSummary {
public:
    StudentSummary(const Student& student, const CourseMap& courses);

    // Final grade of the course's counting attempt, or nullptr.
    const Grade* countingGrade(const std::string& courseCode) const;

    // True when the counting attempt passes the course (earns credits).
    bool hasPassed(const std::string& courseCode) const;

    // Credits of every passed course, each course counted once.
    int earnedCredits() const;

    // Credit-weighted grade points of regular courses and FINAL theses.
    GpaAccumulator gpa() const;

private:
    const Course* findCourse(const std::string& code) const;

    const Student& student_;
    const CourseMap& courses_;
};

#endif  // ACADERP_STUDENT_SUMMARY_H