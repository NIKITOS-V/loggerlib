#ifndef __LOGGER_LIB_OSTREAM_HANDLER__
#define __LOGGER_LIB_OSTREAM_HANDLER__

#include <ostream>
#include <string_view>

namespace loggerlib::detail {
    class OstreamHandler {
    public: // Public static methods

        static void writeOstream(
            std::ostream* ostream,
            std::string_view text
        );

        static void cerr(
            std::string_view text
        );

    }; // class OstreamHandler
} // namespace LoggerLib::detail

#endif // __LOGGER_LIB_OSTREAM_HANDLER__
