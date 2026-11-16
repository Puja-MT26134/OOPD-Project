// Group ID: 79
// Stand-alone check that CourseMaterial::clone() makes a true deep copy.
// Built and run by "make materialtest"; it is not part of the acaderp binary.
#include <cstdlib>
#include <iostream>

#include "grp79_course_material.h"

namespace {

int failures = 0;

void check(bool condition, const char* what) {
    if (!condition) {
        std::cout << "FAIL: " << what << '\n';
        ++failures;
    }
}

}  // namespace

int main() {
    MaterialBundle original;
    auto syllabus = std::make_unique<Syllabus>("CS101", "Prof. Rao", "Syllabus");
    syllabus->addTopic("Loops");
    syllabus->addTopic("Recursion");
    auto notes = std::make_unique<LectureNotes>("CS101", "Prof. Rao", "Notes");
    notes->addLecture("Lecture 1: Intro");
    auto homework = std::make_unique<Assignment>("CS101", "Prof. Rao", "HW1");
    homework->addProblem("Write fizzbuzz");
    original.add(std::move(syllabus));
    original.add(std::move(notes));
    original.add(std::move(homework));

    MaterialBundle copy = original.copyFor("Prof. Iyer");

    check(copy.items().size() == 3, "copy has the same number of items");
    check(copy.items()[0].get() != original.items()[0].get(), "copies are distinct objects");
    check(copy.items()[0]->kind() == "Syllabus", "clone keeps the concrete type");
    check(copy.items()[1]->kind() == "LectureNotes", "clone keeps the concrete type (notes)");
    check(copy.items()[2]->kind() == "Assignment", "clone keeps the concrete type (assignment)");
    check(copy.items()[0]->owner() == "Prof. Iyer", "copy belongs to the new instructor");
    check(original.items()[0]->owner() == "Prof. Rao", "original keeps its owner");

    // Edit the copy; the original must not change.
    auto* copiedSyllabus = dynamic_cast<Syllabus*>(copy.items()[0].get());
    auto* originalSyllabus = dynamic_cast<Syllabus*>(original.items()[0].get());
    check(copiedSyllabus != nullptr && originalSyllabus != nullptr, "syllabus casts work");
    copiedSyllabus->setTopic(0, "Pointers");
    copiedSyllabus->addTopic("Sorting");
    check(originalSyllabus->topics().size() == 2, "original topic count unchanged");
    check(originalSyllabus->topics()[0] == "Loops", "original topic text unchanged");
    check(copiedSyllabus->topics()[0] == "Pointers", "copy topic text edited");

    auto* copiedHw = dynamic_cast<Assignment*>(copy.items()[2].get());
    auto* originalHw = dynamic_cast<Assignment*>(original.items()[2].get());
    copiedHw->setProblem(0, "Write a sorter");
    check(originalHw->problems()[0] == "Write fizzbuzz", "original assignment unchanged");

    if (failures == 0) {
        std::cout << "material test: all checks passed\n";
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}