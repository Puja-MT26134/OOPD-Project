#ifndef ACADERP_STUDENT_H
#define ACADERP_STUDENT_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "attempt.h"
#include "term.h"

class Student {
public:
    Student(std::string roll, std::string name);
    Student(Student&&) noexcept = default;
    Student& operator=(Student&&) noexcept = default;

    const std::string& roll() const noexcept { return roll_; }
    const std::string& name() const noexcept { return name_; }

    void addAttempt(std::unique_ptr<Attempt> attempt);

    // Latest attempt of the course that a GRADE for this term belongs to.
    Attempt* findAttempt(const std::string& courseCode, const Term& term);

           // Most recently registered attempt of the course, or nullptr.
       Attempt* latestAttempt(const std::string& courseCode);

           // True when any attempt of the course is still open: pending, holding I,
    // or a thesis that is neither FINAL nor TERMINATED.
    bool hasOpenAttempt(const std::string& courseCode) const;

    // True when an earlier registration of the course was a grade improvement.
    bool hasImprovementAttempt(const std::string& courseCode) const;
    
    // Latest attempt of the course that has a final grade, or nullptr.
    const Attempt* countingAttempt(const std::string& courseCode) const;

    // The counting attempt of every course the student has attempted.
    std::vector<const Attempt*> countingAttempts() const;

    const std::map<std::string, std::vector<std::unique_ptr<Attempt>>>&
    attempts() const noexcept {
        return attempts_;
    }

private:
    std::string roll_;
    std::string name_;
    std::map<std::string, std::vector<std::unique_ptr<Attempt>>> attempts_;
};

#endif  // ACADERP_STUDENT_H