/* Created by Matthew Brown on 6/19/2024 */
#include "GeometryDash.h"
#include "simplelogger.hpp"

int main(int argc, char *argv[])
{
    SL_CAPTURE_EXCEPTIONS();
    SL_LOG_TO_FILE("GeometryDash.log", slog::LogFileMode::APPEND);
#ifdef NDEBUG
    SL_LOG_VERSION_INFO("Geometry Dash", "0.0.1");

    slog::SimpleLogger::GlobalLogger()->setMinLogLevel(slog::LogLevel::INFO);
#else
    SIMPLE_LOGGER_LOG_VERSION_INFO();
    SL_LOG_VERSION_INFO("Geometry Dash", "0.0.1");

    SL_LOG_TO_FILE("GeometryDash-debug-recent.log", slog::LogFileMode::OVERWRITE);
    slog::SimpleLogger::GlobalLogger()->getLogger(2)->setMinLogLevel(slog::LogLevel::DEBUG);

    SL_LOG_TO_FILE("GeometryDash-debug.log", slog::LogFileMode::APPEND);
    slog::SimpleLogger::GlobalLogger()->getLogger(3)->setMinLogLevel(slog::LogLevel::DEBUG);
#endif // NDEBUG

    // Load settings from file, maybe allow settings from command line args in the future?
    GeometryDash::LoadSettings();

    // Run the game
    while (GeometryDash::getInstance().run())
    {
        GeometryDash::Reset();
        // Run while the game is in a loop
        GeometryDash::Restart = false;
    }

    return 0; // Not necessary, just looks cleaner to me
}
