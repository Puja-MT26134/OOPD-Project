#include "course_material.h"

#include <utility>

CourseMaterial::CourseMaterial(std::string courseCode, std::string owner,
                               std::string title)
    : courseCode_(std::move(courseCode)),
      owner_(std::move(owner)),
      title_(std::move(title)) {}

Syllabus::Syllabus(std::string courseCode, std::string owner, std::string title)
    : CourseMaterial(std::move(courseCode), std::move(owner), std::move(title)) {}

std::unique_ptr<CourseMaterial> Syllabus::clone() const {
    return std::make_unique<Syllabus>(*this);
}

LectureNotes::LectureNotes(std::string courseCode, std::string owner,
                           std::string title)
    : CourseMaterial(std::move(courseCode), std::move(owner), std::move(title)) {}

std::unique_ptr<CourseMaterial> LectureNotes::clone() const {
    return std::make_unique<LectureNotes>(*this);
}

Assignment::Assignment(std::string courseCode, std::string owner,
                       std::string title)
    : CourseMaterial(std::move(courseCode), std::move(owner), std::move(title)) {}

std::unique_ptr<CourseMaterial> Assignment::clone() const {
    return std::make_unique<Assignment>(*this);
}

MaterialBundle MaterialBundle::copyFor(const std::string& newOwner) const {
    MaterialBundle copy;
    for (const auto& item : items_) {
        std::unique_ptr<CourseMaterial> duplicate = item->clone();   // deep copy
        duplicate->setOwner(newOwner);
        copy.add(std::move(duplicate));
    }
    return copy;
}