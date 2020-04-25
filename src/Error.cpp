#include <errno.h>
#include <string.h>
#include "Error.h"

static const std::size_t MAX_ERROR_BUFFER = 256;

namespace gf {

std::string Error::Message() const {
    switch (domain) {
        case ErrorDomain::System:
            std::string buf;
            buf.reserve(MAX_ERROR_BUFFER);
            #ifdef _WIN32
                strerror_s(&buf[0], buf.capacity(), code);
            #else
                strerror_r(code, &buf[0], buf.capacity());
            #endif
            
            return buf;
    }
    return std::string();
}

} // namespace gf
