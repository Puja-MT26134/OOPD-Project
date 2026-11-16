// Group ID: 79
#ifndef ACADERP_GRADUATION_H
#define ACADERP_GRADUATION_H

#include <string>
#include <vector>

#include "grp79_course.h"
#include "grp79_program_rules.h"
#include "grp79_student.h"

// Audits the seven graduation rules R1..R7 for one student.
class GraduationAuditor {
public:
    GraduationAuditor(const ProgramRules& rules, const CourseMap& courses);

    // The failing rules in order ("R1", "R2", ...). Empty means the student
    // graduates.
    std::vector<std::string> failedRules(const Student& student) const;

private:
    const ProgramRules& rules_;
    const CourseMap& courses_;
};

#endif  // ACADERP_GRADUATION_H