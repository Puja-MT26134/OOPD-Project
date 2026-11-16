// Group ID: 79
#include "grp79_thesis_record.h"

#include <utility>

#include "grp79_exceptions.h"

ThesisRecord::ThesisRecord(Term startTerm) : startTerm_(startTerm) {}

std::string ThesisRecord::stateName() const {
    switch (state_) {
        case ThesisState::Registered: return "REGISTERED";
        case ThesisState::InProgress: return "IN_PROGRESS";
        case ThesisState::AtRisk: return "AT_RISK";
        case ThesisState::Terminated: return "TERMINATED";
        case ThesisState::Final: return "FINAL";
    }
    return "UNKNOWN";
}

void ThesisRecord::applyProgress(const Grade& grade, const Term& term) {
    // ---- checks (nothing is modified yet) ----
    if (state_ == ThesisState::Final) {
        throw GradeAlreadyFinalException("thesis already has final letter " +
                                         letter_->symbol());
    }
    if (state_ == ThesisState::Terminated) {
        throw ThesisTerminatedException("thesis is TERMINATED; no further grades");
    }
    if (!grade.isSatisfactory() && !grade.isUnsatisfactory()) {
        throw GradeTypeMismatchException("progress grade must be S or X, not " +
                                         grade.symbol());
    }
    if (term < startTerm_) {
        throw InvalidThesisTransitionException(
            "progress term " + term.toString() +
            " is before the registration term " + startTerm_.toString());
    }
    if (lastProgressTerm_.has_value()) {
        if (term == *lastProgressTerm_) {
            throw GradeAlreadyFinalException(
                "second progress grade in term " + term.toString());
        }
        if (term < *lastProgressTerm_) {
            throw InvalidThesisTransitionException(
                "progress term " + term.toString() + " is before the last one, " +
                lastProgressTerm_->toString());
        }
    }
    // ---- state change (cannot throw) ----
    if (grade.isSatisfactory()) {
        state_ = ThesisState::InProgress;
    } else {
        state_ = (state_ == ThesisState::AtRisk) ? ThesisState::Terminated
                                                 : ThesisState::AtRisk;
    }
    lastProgressTerm_ = term;
}

void ThesisRecord::convert(const Grade& letter) {
    if (!letter.isLetter()) {
        throw GradeTypeMismatchException("CONVERT needs a letter grade, not " +
                                         letter.symbol());
    }
    if (state_ == ThesisState::Final) {
        throw GradeAlreadyFinalException("thesis was already converted to " +
                                         letter_->symbol());
    }
    if (state_ == ThesisState::Terminated) {
        throw ThesisTerminatedException("thesis is TERMINATED; cannot convert");
    }
    if (state_ != ThesisState::InProgress) {
        throw InvalidThesisTransitionException(
            "CONVERT is illegal from " + stateName() +
            "; thesis must be IN_PROGRESS");
    }
    letter_ = letter;
    state_ = ThesisState::Final;
}