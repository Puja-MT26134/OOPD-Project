#ifndef ACADERP_PROGRAM_RULES_H
#define ACADERP_PROGRAM_RULES_H

#include "grade.h"

// Values from the PROGRAM command. GPA thresholds are stored in hundredths
// (5.00 -> 500) so no floating point is ever needed.
struct ProgramRules {
    int totalCredits = 160;
    int electiveCredits = 24;
    int minGpaHundredths = 500;
    Grade thesisMinGrade = Grade::fromSymbol("C");
    int thesisMinCredits = 100;
    int thesisMinGpaHundredths = 600;
    int maxLoad = 24;
};

#endif  // ACADERP_PROGRAM_RULES_H