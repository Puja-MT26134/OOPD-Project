#ifndef ACADERP_EXCEPTIONS_H
#define ACADERP_EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <utility>

// Root of every error raised on purpose. The constructor is protected, so
// only the family and leaf classes below can actually be thrown.
class ErpException : public std::runtime_error {
public:
    const std::string& code() const noexcept { return code_; }

protected:
    ErpException(std::string code, const std::string& message)
        : std::runtime_error(message), code_(std::move(code)) {}

private:
    std::string code_;
};

// ---------------------------------------------------------------- families
// All five derive directly from ErpException (they are siblings).

class InputException : public ErpException {
protected:
    InputException(std::string code, const std::string& message)
        : ErpException(std::move(code), message) {}
};

class EntityException : public ErpException {
protected:
    EntityException(std::string code, const std::string& message)
        : ErpException(std::move(code), message) {}
};

class GradeException : public ErpException {
protected:
    GradeException(std::string code, const std::string& message)
        : ErpException(std::move(code), message) {}
};

class RegistrationException : public ErpException {
protected:
    RegistrationException(std::string code, const std::string& message)
        : ErpException(std::move(code), message) {}
};

class ComputationException : public ErpException {
protected:
    ComputationException(std::string code, const std::string& message)
        : ErpException(std::move(code), message) {}
};

// ------------------------------------------------------------------ input

class ParseException : public InputException {
public:
    explicit ParseException(const std::string& message)
        : InputException("E_PARSE", message) {}
};

// ----------------------------------------------------------------- entity

class DuplicateEntityException : public EntityException {
public:
    explicit DuplicateEntityException(const std::string& message)
        : EntityException("E_DUPLICATE", message) {}
};

class UnknownStudentException : public EntityException {
public:
    explicit UnknownStudentException(const std::string& message)
        : EntityException("E_UNKNOWN_STUDENT", message) {}
};

class UnknownCourseException : public EntityException {
public:
    explicit UnknownCourseException(const std::string& message)
        : EntityException("E_UNKNOWN_COURSE", message) {}
};

class InvalidCourseException : public EntityException {
public:
    explicit InvalidCourseException(const std::string& message)
        : EntityException("E_COURSE_DEF", message) {}
};

// ------------------------------------------------------------------ grade

class InvalidGradeSymbolException : public GradeException {
public:
    explicit InvalidGradeSymbolException(const std::string& message)
        : GradeException("E_GRADE_SYMBOL", message) {}
};

class GradeTypeMismatchException : public GradeException {
public:
    explicit GradeTypeMismatchException(const std::string& message)
        : GradeException("E_GRADE_TYPE", message) {}
};

class NotRegisteredException : public GradeException {
public:
    explicit NotRegisteredException(const std::string& message)
        : GradeException("E_NOT_REGISTERED", message) {}
};

class GradeAlreadyFinalException : public GradeException {
public:
    explicit GradeAlreadyFinalException(const std::string& message)
        : GradeException("E_GRADE_FINAL", message) {}
};

class InvalidThesisTransitionException : public GradeException {
public:
    explicit InvalidThesisTransitionException(const std::string& message)
        : GradeException("E_THESIS_TRANSITION", message) {}
};

class ThesisTerminatedException : public GradeException {
public:
    explicit ThesisTerminatedException(const std::string& message)
        : GradeException("E_THESIS_TERMINATED", message) {}
};

// ----------------------------------------------------------- registration

class AlreadyPassedException : public RegistrationException {
public:
    explicit AlreadyPassedException(const std::string& message)
        : RegistrationException("E_ALREADY_PASSED", message) {}
};

class DuplicateRegistrationException : public RegistrationException {
public:
    explicit DuplicateRegistrationException(const std::string& message)
        : RegistrationException("E_DUP_REG", message) {}
};

class PrerequisiteNotMetException : public RegistrationException {
public:
    explicit PrerequisiteNotMetException(const std::string& message)
        : RegistrationException("E_PREREQ", message) {}
};

class AntiRequisiteException : public RegistrationException {
public:
    explicit AntiRequisiteException(const std::string& message)
        : RegistrationException("E_ANTIREQ", message) {}
};

class CreditLimitExceededException : public RegistrationException {
public:
    explicit CreditLimitExceededException(const std::string& message)
        : RegistrationException("E_CREDIT_LIMIT", message) {}
};

class ThesisNotAllowedException : public RegistrationException {
public:
    explicit ThesisNotAllowedException(const std::string& message)
        : RegistrationException("E_THESIS_NOT_ALLOWED", message) {}
};

// ------------------------------------------------------------ computation

class NoGpaDataException : public ComputationException {
public:
    explicit NoGpaDataException(const std::string& message)
        : ComputationException("E_NO_GPA", message) {}
};

#endif  // ACADERP_EXCEPTIONS_H