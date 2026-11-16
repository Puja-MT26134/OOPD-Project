// Group ID: 79
#ifndef ACADERP_THESIS_ATTEMPT_H
#define ACADERP_THESIS_ATTEMPT_H

#include <string>

#include "grp79_attempt.h"
#include "grp79_thesis_record.h"

// An Attempt whose grading rules are the thesis state machine.
class ThesisAttempt : public Attempt {
public:
    ThesisAttempt(std::string courseCode, Term term);

    // Progress grades may carry any later term; ThesisRecord checks the order.
    bool matchesGradeTerm(const Term& gradeTerm) const override;
    void recordGrade(const Grade& grade, const Term& gradeTerm) override;
    void convert(const Grade& letter) override;

    // True for FINAL and TERMINATED (a TERMINATED thesis has no final grade).
    bool isFinal() const override;
    const Grade* finalGrade() const override;

    const ThesisRecord& record() const noexcept { return record_; }

private:
    ThesisRecord record_;
};

#endif  // ACADERP_THESIS_ATTEMPT_H