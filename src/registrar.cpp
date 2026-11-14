#include "registrar.h"

#include <utility>

#include "exceptions.h"
#include "gpa.h"
#include "eligibility.h"

void Registrar::setProgram(const ProgramRules& rules) {
    if (!courses_.empty()) {
        throw ParseException("PROGRAM must come before the first COURSE");
    }
    rules_ = rules;
}

Prerequisite Registrar::resolvePrerequisite(const std::string& ownCode,
                                            const PrerequisiteSpec& spec) const {
    if (spec.code == ownCode) {
        throw InvalidCourseException("course " + ownCode +
                                     " cannot be its own prerequisite");
    }
    const auto it = courses_.find(spec.code);
    if (it == courses_.end()) {
        throw InvalidCourseException("course " + ownCode +
                                     ": unknown prerequisite " + spec.code);
    }
    const Course& prerequisite = *it->second;
    if (spec.minSymbol.empty()) {
        return Prerequisite{spec.code, prerequisite.defaultMinimumGrade()};
    }
    if (!Grade::isKnownSymbol(spec.minSymbol)) {
        throw InvalidCourseException("course " + ownCode +
                                     ": bad minimum grade '" + spec.minSymbol +
                                     "'");
    }
    const Grade minimum = Grade::fromSymbol(spec.minSymbol);
    if (!prerequisite.earnsCredits(minimum)) {
        throw InvalidCourseException(
            "course " + ownCode + ": minimum grade " + minimum.symbol() +
            " is not a passing grade of " + spec.code);
    }
    return Prerequisite{spec.code, minimum};
}

void Registrar::addCourse(const CourseSpec& spec) {
    if (courses_.count(spec.code) != 0) {
        throw DuplicateEntityException("course " + spec.code +
                                       " already exists");
    }
    std::vector<Prerequisite> prerequisites;
    for (const PrerequisiteSpec& pre : spec.prerequisites) {
        prerequisites.push_back(resolvePrerequisite(spec.code, pre));
    }
    std::unique_ptr<Course> course =
        makeCourse(spec.type, spec.code, spec.credits, spec.category,
                   std::move(prerequisites), spec.antiRequisites);
    courses_.emplace(spec.code, std::move(course));   // insert last
}

void Registrar::addStudent(const std::string& roll, const std::string& name) {
    if (students_.count(roll) != 0) {
        throw DuplicateEntityException("student " + roll + " already exists");
    }
    students_.try_emplace(roll, roll, name);
}

const Student& Registrar::requireStudent(const std::string& roll) const {
    const auto it = students_.find(roll);
    if (it == students_.end()) {
        throw UnknownStudentException("unknown student '" + roll + "'");
    }
    return it->second;
}

Student& Registrar::requireStudent(const std::string& roll) {
    const auto it = students_.find(roll);
    if (it == students_.end()) {
        throw UnknownStudentException("unknown student '" + roll + "'");
    }
    return it->second;
}

const Course& Registrar::requireCourse(const std::string& code) const {
    const auto it = courses_.find(code);
    if (it == courses_.end()) {
        throw UnknownCourseException("unknown course '" + code + "'");
    }
    return *it->second;
}

  void Registrar::registerCourse(const std::string& roll,
                                 const std::string& courseCode,
                                 const Term& term) {
      Student& student = requireStudent(roll);           // E1: student first
      const Course& course = requireCourse(courseCode);  // E1: then course
      const EligibilityChecker checker(rules_, courses_);
      const std::optional<EligibilityFailure> failure =
          checker.firstFailure(student, course, term);  // E2..E7
      if (failure.has_value()) {
          EligibilityChecker::raise(*failure);
      }
      // Eligible and already passed can only mean: grade improvement.
      std::unique_ptr<Attempt> attempt = course.makeAttempt(term);
      if (StudentSummary(student, courses_).hasPassed(courseCode)) {
          attempt->markImprovement();
      }
      student.addAttempt(std::move(attempt));  // mutate last
  }

  std::string Registrar::eligibilityVerdict(const std::string& roll,
                                            const std::string& courseCode,
                                            const Term& term) const {
      const Student& student = requireStudent(roll);
      const Course& course = requireCourse(courseCode);
      const std::optional<EligibilityFailure> failure =
          EligibilityChecker(rules_, courses_).firstFailure(student, course, term);
      if (!failure.has_value()) {
          return "YES";
      }
      return "NO " + EligibilityChecker::label(failure->code);
  }

void Registrar::recordGrade(const std::string& roll,
                            const std::string& courseCode, const Term& term,
                            const std::string& symbol) {
    Student& student = requireStudent(roll);
    const Course& course = requireCourse(courseCode);
    const Grade grade = Grade::fromSymbol(symbol);   // E_GRADE_SYMBOL
    course.validateGrade(grade);                      // E_GRADE_TYPE
    Attempt* attempt = student.findAttempt(courseCode, term);
    if (attempt == nullptr) {
        throw NotRegisteredException("student " + roll +
                                     " has no registration in " + courseCode +
                                     " for term " + term.toString());
    }
    attempt->recordGrade(grade, term);   // E_GRADE_FINAL; mutates last
}

   void Registrar::convertThesis(const std::string& roll,
                                 const std::string& courseCode,
                                 const std::string& symbol) {
       Student& student = requireStudent(roll);
       requireCourse(courseCode);                       // E_UNKNOWN_COURSE
       const Grade letter = Grade::fromSymbol(symbol);  // E_GRADE_SYMBOL
       Attempt* attempt = student.latestAttempt(courseCode);
       if (attempt == nullptr) {
           throw NotRegisteredException("student " + roll +
                                        " has no registration in " + courseCode);
       }
       attempt->convert(letter);   // E_GRADE_TYPE / E_THESIS_*; mutates last
   }

  int Registrar::gpaHundredths(const std::string& roll) const {
      return StudentSummary(requireStudent(roll), courses_).gpa().hundredths();
  }

std::string Registrar::averageLetter(const std::string& roll) const {
    return GpaAccumulator::averageLetter(gpaHundredths(roll));
}