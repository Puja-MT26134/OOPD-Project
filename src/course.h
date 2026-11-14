#ifndef ACADERP_COURSE_H
#define ACADERP_COURSE_H

#include <memory>
#include <string>
#include <vector>

#include "attempt.h"
#include "grade.h"
#include "term.h"

#include <unordered_map>

enum class CourseType { Regular, Su, Thesis };
enum class Category { Core, Elective, MandatorySu, Open };

// A prerequisite after resolution: course code plus minimum grade.
struct Prerequisite {
    std::string code;
    Grade minGrade;
};

// A prerequisite as written in the script; empty minSymbol = use the default.
struct PrerequisiteSpec {
    std::string code;
    std::string minSymbol;
};

// Raw COURSE command contents, handed from the parser to the Registrar.
struct CourseSpec {
    std::string code;
    CourseType type;
    int credits;
    Category category;
    std::vector<PrerequisiteSpec> prerequisites;
    std::vector<std::string> antiRequisites;
};

class Course {
public:
    static constexpr int kMaxCredits = 24;

    Course(std::string code, int credits, Category category,
           std::vector<Prerequisite> prerequisites,
           std::vector<std::string> antiRequisites);
    virtual ~Course() = default;
    Course(const Course&) = delete;
    Course& operator=(const Course&) = delete;

    const std::string& code() const noexcept { return code_; }
    int credits() const noexcept { return credits_; }
    Category category() const noexcept { return category_; }
    const std::vector<Prerequisite>& prerequisites() const noexcept {
        return prerequisites_;
    }
    const std::vector<std::string>& antiRequisites() const noexcept {
        return antiRequisites_;
    }

    virtual CourseType type() const noexcept = 0;
    virtual std::string typeName() const = 0;

    // Throws GradeTypeMismatchException if this type does not accept it.
    virtual void validateGrade(const Grade& grade) const = 0;

    // Whether this type can ever enter the GPA (a final letter is also needed).
    virtual bool countsInGpa() const noexcept = 0;

    // True when this final grade passes the course and earns its credits.
    virtual bool earnsCredits(const Grade& finalGrade) const noexcept = 0;

    // Not pure: sensible defaults that SuCourse overrides.
    virtual int minCredits() const noexcept { return 1; }
    virtual bool allowsMandatorySu() const noexcept { return false; }
    virtual Grade defaultMinimumGrade() const;
          // True when this passing final grade still allows one registration for
      // grade improvement (regular courses with C- or D only).
      virtual bool allowsImprovement(const Grade& /*finalGrade*/) const noexcept {
          return false;
      }

    // Creates the attempt object for a new registration.
    virtual std::unique_ptr<Attempt> makeAttempt(const Term& term) const;

    // Throws InvalidCourseException on a bad credit value or category.
    void validateDefinition() const;

private:
    std::string code_;
    int credits_;
    Category category_;
    std::vector<Prerequisite> prerequisites_;
    std::vector<std::string> antiRequisites_;
};

  using CourseMap = std::unordered_map<std::string, std::unique_ptr<Course>>;


class RegularCourse : public Course {
public:
    using Course::Course;
    CourseType type() const noexcept override { return CourseType::Regular; }
    std::string typeName() const override { return "REGULAR"; }
    void validateGrade(const Grade& grade) const override;
    bool countsInGpa() const noexcept override { return true; }
    bool earnsCredits(const Grade& finalGrade) const noexcept override;
          bool allowsImprovement(const Grade& finalGrade) const noexcept override;
};

class SuCourse : public Course {
public:
    using Course::Course;
    CourseType type() const noexcept override { return CourseType::Su; }
    std::string typeName() const override { return "SU"; }
    void validateGrade(const Grade& grade) const override;
    bool countsInGpa() const noexcept override { return false; }
    bool earnsCredits(const Grade& finalGrade) const noexcept override;
    int minCredits() const noexcept override { return 0; }
    bool allowsMandatorySu() const noexcept override { return true; }
    Grade defaultMinimumGrade() const override;
};

class ThesisCourse : public Course {
public:
    using Course::Course;
    CourseType type() const noexcept override { return CourseType::Thesis; }
    std::string typeName() const override { return "THESIS"; }
    void validateGrade(const Grade& grade) const override;
    bool countsInGpa() const noexcept override { return true; }
    bool earnsCredits(const Grade& finalGrade) const noexcept override;
           std::unique_ptr<Attempt> makeAttempt(const Term& term) const override;
};

// The one place that maps CourseType to a concrete class (creation only).
// Validates the definition before returning.
std::unique_ptr<Course> makeCourse(CourseType type, std::string code,
                                   int credits, Category category,
                                   std::vector<Prerequisite> prerequisites,
                                   std::vector<std::string> antiRequisites);

#endif  // ACADERP_COURSE_H