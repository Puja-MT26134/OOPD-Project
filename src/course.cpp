#include "course.h"

#include <utility>

#include "exceptions.h"
#include "thesis_attempt.h"

// ------------------------------------------------------------------ Course

Course::Course(std::string code, int credits, Category category,
               std::vector<Prerequisite> prerequisites,
               std::vector<std::string> antiRequisites)
    : code_(std::move(code)),
      credits_(credits),
      category_(category),
      prerequisites_(std::move(prerequisites)),
      antiRequisites_(std::move(antiRequisites)) {}

Grade Course::defaultMinimumGrade() const { return Grade::fromSymbol("D"); }

std::unique_ptr<Attempt> Course::makeAttempt(const Term& term) const {
    return std::make_unique<Attempt>(code_, term);
}

void Course::validateDefinition() const {
    if (credits_ < minCredits() || credits_ > kMaxCredits) {
        throw InvalidCourseException(
            "course " + code_ + ": credits " + std::to_string(credits_) +
            " outside " + std::to_string(minCredits()) + ".." +
            std::to_string(kMaxCredits) + " for a " + typeName() + " course");
    }
    if (category_ == Category::MandatorySu && !allowsMandatorySu()) {
        throw InvalidCourseException("course " + code_ +
                                     ": MANDATORY_SU is only valid for SU");
    }
}

// ----------------------------------------------------------- RegularCourse

void RegularCourse::validateGrade(const Grade& grade) const {
    if (!grade.isLetter() && !grade.isIncomplete()) {
        throw GradeTypeMismatchException("regular course " + code() +
                                         " does not accept grade " +
                                         grade.symbol());
    }
}

bool RegularCourse::earnsCredits(const Grade& finalGrade) const noexcept {
    return finalGrade.isLetter() && finalGrade.isPass();
}

bool RegularCourse::allowsImprovement(const Grade& finalGrade) const noexcept {
    return finalGrade.symbol() == "C-" || finalGrade.symbol() == "D";
}

// --------------------------------------------------------------- SuCourse

void SuCourse::validateGrade(const Grade& grade) const {
    if (!grade.isSatisfactory() && !grade.isUnsatisfactory() &&
        !grade.isIncomplete()) {
        throw GradeTypeMismatchException("SU course " + code() +
                                         " does not accept grade " +
                                         grade.symbol());
    }
}

bool SuCourse::earnsCredits(const Grade& finalGrade) const noexcept {
    return finalGrade.isSatisfactory();
}

Grade SuCourse::defaultMinimumGrade() const { return Grade::fromSymbol("S"); }

// ------------------------------------------------------------ ThesisCourse

void ThesisCourse::validateGrade(const Grade& grade) const {
    // Via GRADE a thesis only takes progress grades; letters come from CONVERT.
    if (!grade.isSatisfactory() && !grade.isUnsatisfactory()) {
        throw GradeTypeMismatchException("thesis " + code() +
                                         " takes only S or X via GRADE, not " +
                                         grade.symbol());
    }
}

bool ThesisCourse::earnsCredits(const Grade& finalGrade) const noexcept {
    return finalGrade.isLetter() && finalGrade.isPass();
}

std::unique_ptr<Attempt> ThesisCourse::makeAttempt(const Term& term) const {
    return std::make_unique<ThesisAttempt>(code(), term);
}


// ----------------------------------------------------------------- factory

std::unique_ptr<Course> makeCourse(CourseType type, std::string code,
                                   int credits, Category category,
                                   std::vector<Prerequisite> prerequisites,
                                   std::vector<std::string> antiRequisites) {

    std::unique_ptr<Course> course;
    switch (type) {
        case CourseType::Regular:
            course = std::make_unique<RegularCourse>(
                std::move(code), credits, category, std::move(prerequisites),
                std::move(antiRequisites));
            break;
        case CourseType::Su:
            course = std::make_unique<SuCourse>(
                std::move(code), credits, category, std::move(prerequisites),
                std::move(antiRequisites));
            break;
        case CourseType::Thesis:
            course = std::make_unique<ThesisCourse>(
                std::move(code), credits, category, std::move(prerequisites),
                std::move(antiRequisites));
            break;
    }
    course->validateDefinition();
    return course;
}