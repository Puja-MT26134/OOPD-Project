#ifndef ACADERP_THESIS_RECORD_H
#define ACADERP_THESIS_RECORD_H

#include <optional>
#include <string>

#include "grade.h"
#include "term.h"

enum class ThesisState { Registered, InProgress, AtRisk, Terminated, Final };

// Holds the thesis state and is the ONLY code allowed to change it.
// Every method checks everything first and mutates last (strong guarantee).
class ThesisRecord {
public:
    explicit ThesisRecord(Term startTerm);

    ThesisState state() const noexcept { return state_; }
    const Term& startTerm() const noexcept { return startTerm_; }
    std::string stateName() const;

    // FINAL or TERMINATED: nothing more can happen to this attempt.
    bool isClosed() const noexcept {
        return state_ == ThesisState::Terminated || state_ == ThesisState::Final;
    }

    // The converted letter grade, or nullptr until CONVERT succeeds.
    const Grade* letter() const noexcept {
        return letter_.has_value() ? &*letter_ : nullptr;
    }

    // GRADE with S or X. Throws GradeAlreadyFinal / ThesisTerminated /
    // InvalidThesisTransition.
    void applyProgress(const Grade& grade, const Term& term);

    // CONVERT with a letter. Legal only from IN_PROGRESS.
    void convert(const Grade& letter);

private:
    ThesisState state_ = ThesisState::Registered;
    Term startTerm_;
    std::optional<Term> lastProgressTerm_;
    std::optional<Grade> letter_;
};

#endif  // ACADERP_THESIS_RECORD_H