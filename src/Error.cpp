#include <errno.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif
#include "Error.h"

static const std::size_t MAX_ERROR_BUFFER = 256;

namespace gf {

Error Error::success;

std::string Error::Message() const {
    switch (domain) {
        case ErrorDomain::Errno: {
            char buffer[MAX_ERROR_BUFFER];
            #ifdef _WIN32
                auto result = strerror_s(buffer, sizeof(buffer), code);
                if (result == 0) {
                    return std::string(buffer);
                }
                break;
            #else
                return strerror_r(code, buffer, sizeof(buffer));
            #endif
        }
#ifdef _WIN32
        case ErrorDomain::Win32: {
            char *buffer = nullptr;
            auto length = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                nullptr,
                code,
                0,
                (char *)&buffer,
                0,
                nullptr);
            if (length == 0) {
                break;
            }
            if (length > 2) {
                buffer[length - 2] = '\0'; // Remove trailing \r\n
            }
            std::string message(buffer);
            LocalFree(buffer);
            return message;
        }
#endif
    }
    return std::string();
}

std::string Error::MessageOrDefault(const std::string &defaultMessage) const {
    auto message = Message();
    return message.empty() ? defaultMessage : message;
}

} // namespace gf
