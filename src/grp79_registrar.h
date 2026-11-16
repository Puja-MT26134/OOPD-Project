// Group ID: 79
#ifndef ACADERP_REGISTRAR_H
#define ACADERP_REGISTRAR_H

#include <memory>
#include <string>
#include <unordered_map>

#include "grp79_course.h"
#include "grp79_program_rules.h"
#include "grp79_student.h"
#include "grp79_term.h"

// Facade used by the command objects. Every mutating method validates first
// and changes state last, so a thrown exception leaves nothing modified.
class Registrar {
public:
    // Only legal before the first COURSE; otherwise ParseException.
    void setProgram(const ProgramRules& rules);

    void addCourse(const CourseSpec& spec);
    void addStudent(const std::string& roll, const std::string& name);

    void registerCourse(const std::string& roll, const std::string& courseCode,
                        const Term& term);
    void recordGrade(const std::string& roll, const std::string& courseCode,
                     const Term& term, const std::string& symbol);

                         // One line: name, every attempt as CODE@TERM=grade>grade..., EARNED, GPA.
    std::string transcriptLine(const std::string& roll) const;
    
                         // "YES" or "NO R1,R5" (the failing rules, in order).
    std::string graduationVerdict(const std::string& roll) const;

                           // "YES" or "NO E4"; throws for an unknown student or course.
      std::string eligibilityVerdict(const std::string& roll,
                                     const std::string& courseCode,
                                     const Term& term) const;
                            void convertThesis(const std::string& roll, const std::string& courseCode,
                          const std::string& symbol);

    // GPA in hundredths; throws NoGpaDataException.
    int gpaHundredths(const std::string& roll) const;
    std::string averageLetter(const std::string& roll) const;

    const ProgramRules& rules() const noexcept { return rules_; }

private:
    const Student& requireStudent(const std::string& roll) const;
    Student& requireStudent(const std::string& roll);
    const Course& requireCourse(const std::string& code) const;
    Prerequisite resolvePrerequisite(const std::string& ownCode,
                                     const PrerequisiteSpec& spec) const;

    ProgramRules rules_;
    CourseMap courses_;
    std::unordered_map<std::string, Student> students_;
};

#endif  // ACADERP_REGISTRAR_H