#include "thesis_attempt.h"

#include <utility>

ThesisAttempt::ThesisAttempt(std::string courseCode, Term term)
    : Attempt(std::move(courseCode), term), record_(term) {}

bool ThesisAttempt::matchesGradeTerm(const Term& /*gradeTerm*/) const {
    return true;
}

void ThesisAttempt::recordGrade(const Grade& grade, const Term& gradeTerm) {
    record_.applyProgress(grade, gradeTerm);  // throws before changing anything
    appendGrade(grade);
}

void ThesisAttempt::convert(const Grade& letter) {
    record_.convert(letter);
    appendGrade(letter);
}

bool ThesisAttempt::isFinal() const { return record_.isClosed(); }

const Grade* ThesisAttempt::finalGrade() const { return record_.letter(); }