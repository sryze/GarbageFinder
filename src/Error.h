#ifndef GF_ERROR_H
#define GF_ERROR_H

#include <string>

namespace gf {

enum class ErrorDomain {
#ifdef _WIN32
    Win32,
#endif
    System
};

class Error
{
public:
    Error():
        code(0)
    {}

    Error(ErrorDomain domain, int code):
        code(code)
    {}

    operator bool() const {
        return code == 0;
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
