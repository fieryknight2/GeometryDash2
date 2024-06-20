/* Created by Matthew Brown on 6/19/2024 */
#include "GeometryDash.h"
#include "simplelogger.hpp"

int main(int argc, char *argv[])
{
    SL_CAPTURE_EXCEPTIONS();
    SL_LOG_TO_FILE("GeometryDash.log", slog::LogFileMode::APPEND);

    SIMPLE_LOGGER_LOG_VERSION_INFO();
    SL_LOG_VERSION_INFO("Geometry Dash", "0.0.1");

    // Run the game
    GeometryDash::getInstance().run();

    return 0; // Not necessary, just looks cleaner to me
}
