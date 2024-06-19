include(FetchContent)

# SFML
message(STATUS "Fetching SFML")
FetchContent_Declare(
        sfml
        URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.zip
)
FetchContent_MakeAvailable(sfml)

message(STATUS "Fetching SimpleLogger")
FetchContent_Declare(
        simplelogger
        GIT_REPOSITORY https://github.com/fieryknight2/SimpleLogger.git
        GIT_TAG main
)

FetchContent_MakeAvailable(simplelogger)