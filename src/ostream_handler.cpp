#include "ostream_handler.hpp"

#include <iostream>

/* ----- Alias ----- */

using OstreamHandler = loggerlib::detail::OstreamHandler;

/* ----- class OstreamHandler. Public static methods ----- */

void OstreamHandler::writeOstream(
    std::ostream* ostream,
    std::string_view text
) {
    *(ostream) << text;
}

void OstreamHandler::cerr(
    std::string_view text
) {
    writeOstream(
        &std::cerr,
        text
    );
}

/* ----- END ----- */
