/* Created by Matthew Brown on 6/19/2024 */
#include "GeometryDash.h"
#include "simplelogger.hpp"

int main(int argc, char *argv[])
{
    SL_CAPTURE_EXCEPTIONS();
    SL_LOG_TO_FILE("GeometryDash.log", slog::LogFileMode::APPEND);
#ifdef NDEBUG
    SL_GET_CONSOLE_LOGGER()->setMinLogLevel(slog::LogLevel::WARNING);
    slog::SimpleLogger::GlobalLogger()->setMinLogLevel(slog::LogLevel::WARNING);
#else
    SIMPLE_LOGGER_LOG_VERSION_INFO();
    SL_LOG_VERSION_INFO("Geometry Dash", "0.0.1");
#endif // NDEBUG

    // Run the game
    GeometryDash::getInstance().run();

    return 0; // Not necessary, just looks cleaner to me
}
