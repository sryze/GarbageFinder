#ifndef GF_ERROR_H
#define GF_ERROR_H

#include <string>

namespace gf {

enum class ErrorDomain {
    Unknown,
#ifdef _WIN32
    Win32,
#endif
    Errno
};

class Error
{
public:
    Error():
        domain(ErrorDomain::Unknown),
        code(0)
    {}

    Error(ErrorDomain domain, int code):
        domain(domain),
        code(code)
    {}

    static Error Success() {
        return success;
    }

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
    std::string MessageOrDefault(
        const std::string &defaultMessage = "Unknown error") const;

private:
    ErrorDomain domain;
    int code;

private:
    static Error success;
};

} // namspace gf

#endif
