#include "student.h"

#include <utility>

Student::Student(std::string roll, std::string name)
    : roll_(std::move(roll)), name_(std::move(name)) {}

void Student::addAttempt(std::unique_ptr<Attempt> attempt) {
    const std::string code = attempt->courseCode();   // copy before the move
    attempts_[code].push_back(std::move(attempt));
}

Attempt* Student::findAttempt(const std::string& courseCode, const Term& term) {
    const auto it = attempts_.find(courseCode);
    if (it == attempts_.end()) {
        return nullptr;
    }
    for (auto r = it->second.rbegin(); r != it->second.rend(); ++r) {
        if ((*r)->matchesGradeTerm(term)) {
            return r->get();
        }
    }
    return nullptr;
}

   Attempt* Student::latestAttempt(const std::string& courseCode) {
       const auto it = attempts_.find(courseCode);
       if (it == attempts_.end() || it->second.empty()) {
           return nullptr;
       }
       return it->second.back().get();
   }

const Attempt* Student::countingAttempt(const std::string& courseCode) const {
    const auto it = attempts_.find(courseCode);
    if (it == attempts_.end()) {
        return nullptr;
    }
    for (auto r = it->second.rbegin(); r != it->second.rend(); ++r) {
        if ((*r)->isFinal()) {
            return r->get();
        }
    }
    return nullptr;
}

std::vector<const Attempt*> Student::countingAttempts() const {
    std::vector<const Attempt*> result;
    for (const auto& entry : attempts_) {
        const Attempt* attempt = countingAttempt(entry.first);
        if (attempt != nullptr) {
            result.push_back(attempt);
        }
    }
    return result;
}
bool Student::hasOpenAttempt(const std::string& courseCode) const {
    const auto it = attempts_.find(courseCode);
    if (it == attempts_.end()) {
        return false;
    }
    for (const auto& attempt : it->second) {
        if (!attempt->isFinal()) {
            return true;
        }
    }
    return false;
}

bool Student::hasImprovementAttempt(const std::string& courseCode) const {
    const auto it = attempts_.find(courseCode);
    if (it == attempts_.end()) {
        return false;
    }
    for (const auto& attempt : it->second) {
        if (attempt->isImprovement()) {
            return true;
        }
    }
    return false;
}

