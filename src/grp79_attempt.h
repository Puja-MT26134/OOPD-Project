// Group ID: 79
#ifndef ACADERP_ATTEMPT_H
#define ACADERP_ATTEMPT_H

#include <string>
#include <vector>

#include "grp79_grade.h"
#include "grp79_term.h"

class Attempt {
public:
    Attempt(std::string courseCode, Term term);
    virtual ~Attempt() = default;
    Attempt(const Attempt&) = delete;
    Attempt& operator=(const Attempt&) = delete;

    const std::string& courseCode() const noexcept { return courseCode_; }
    const Term& term() const noexcept { return term_; }
    const std::vector<Grade>& history() const noexcept { return history_; }

    // True when a GRADE line carrying this term belongs to this attempt.
    virtual bool matchesGradeTerm(const Term& gradeTerm) const;

    // Records a grade already validated against the course type.
    // Throws GradeAlreadyFinalException and leaves the attempt unchanged.
    virtual void recordGrade(const Grade& grade, const Term& gradeTerm);

           // CONVERT. Only a thesis attempt supports it; others throw
       // GradeTypeMismatchException.
       virtual void convert(const Grade& letter);

    // True once the attempt has a final (non-I) outcome.
    virtual bool isFinal() const;

    // The final grade, or nullptr while pending or holding I.
    virtual const Grade* finalGrade() const;

          // True when this registration was made for grade improvement.
      bool isImprovement() const noexcept { return improvement_; }
      void markImprovement() noexcept { improvement_ = true; }

    bool isPending() const noexcept { return history_.empty(); }
    bool holdsIncomplete() const noexcept {
        return !history_.empty() && history_.back().isIncomplete();
    }

protected:
    void appendGrade(const Grade& grade) { history_.push_back(grade); }

private:
    std::string courseCode_;
    Term term_;
    std::vector<Grade> history_;
    bool improvement_ = false;
};

#endif  // ACADERP_ATTEMPT_H