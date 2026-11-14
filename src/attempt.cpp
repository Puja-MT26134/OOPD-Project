#include "attempt.h"

#include <utility>

#include "exceptions.h"

Attempt::Attempt(std::string courseCode, Term term)
    : courseCode_(std::move(courseCode)), term_(term) {}

bool Attempt::matchesGradeTerm(const Term& gradeTerm) const {
    return term_ == gradeTerm;
}

// The second parameter has no name on purpose: a named but unused parameter
// would be a -Wunused-parameter warning, which -Werror turns into an error.
void Attempt::recordGrade(const Grade& grade, const Term& /*gradeTerm*/) {
    // All checks first ...
    if (isFinal()) {
        throw GradeAlreadyFinalException(
            "course " + courseCode_ + " term " + term_.toString() +
            " already has final grade " + history_.back().symbol());
    }
    if (grade.isIncomplete() && holdsIncomplete()) {
        throw GradeAlreadyFinalException(
            "course " + courseCode_ + " term " + term_.toString() +
            " is already Incomplete; it must be replaced by a final grade");
    }
    // ... state changes last, so a throw above leaves the attempt untouched.
    appendGrade(grade);
}

   void Attempt::convert(const Grade& ) {
       throw GradeTypeMismatchException(
           "CONVERT applies only to thesis courses (course " + courseCode_ + ")");
   }


bool Attempt::isFinal() const {
    return !history_.empty() && history_.back().isFinal();
}

const Grade* Attempt::finalGrade() const {
    return isFinal() ? &history_.back() : nullptr;
}