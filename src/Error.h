#ifndef GF_ERROR_H
#define GF_ERROR_H

#include <string>

namespace gf {

enum class ErrorDomain {
    Unknown,
#ifdef _WIN32
    Win32,
#endif
    System
};

class Error
{
public:
    Error():
        domain(ErrorDomain::Unknown),
        code(0)
    {}

    Error(ErrorDomain domain, int code):
        domain(ErrorDomain::Unknown),
        code(code)
    {}

    operator int() const {
        return code;
    }

    ErrorDomain Domain() const {
        return domain;
    }

    int Code() const {
        return code;
    }

    std::string Message() const;

private:
    ErrorDomain domain;
    int code;
};

} // namspace gf

#endif
