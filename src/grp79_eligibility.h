// Group ID: 79
#ifndef ACADERP_ELIGIBILITY_H
#define ACADERP_ELIGIBILITY_H

#include <optional>
#include <string>

#include "grp79_course.h"
#include "grp79_program_rules.h"
#include "grp79_student.h"
#include "grp79_term.h"

// E1 (unknown student / course) is handled by the Registrar before this runs.
enum class EligibilityCode { E2, E3, E4, E5, E6, E7 };

struct EligibilityFailure {
    EligibilityCode code;
    std::string message;
};

// Runs checks E2..E7 in order; the first failure decides.
class EligibilityChecker {
public:
    EligibilityChecker(const ProgramRules& rules, const CourseMap& courses);

    // nullopt means eligible.
    std::optional<EligibilityFailure> firstFailure(const Student& student,
                                                   const Course& course,
                                                   const Term& term) const;

    // "E2" .. "E7"
    static std::string label(EligibilityCode code);

    // Throws the exception class that matches the failing check.
    [[noreturn]] static void raise(const EligibilityFailure& failure);

private:
    const ProgramRules& rules_;
    const CourseMap& courses_;
};

#endif  // ACADERP_ELIGIBILITY_H