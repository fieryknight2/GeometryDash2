include(FetchContent)

# SFML
message(STATUS "Fetching SFML")
FetchContent_Declare(
        sfml
        URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.zip
)
FetchContent_MakeAvailable(sfml)

set(ENABLE_STD_FORMAT ON CACHE BOOL "Enable std::format")
message(STATUS "Fetching SimpleLogger")
FetchContent_Declare(
        simplelogger
        GIT_REPOSITORY https://github.com/fieryknight2/SimpleLogger.git
        GIT_TAG main
)

FetchContent_MakeAvailable(simplelogger)

message(STATUS "Fetching TinyXML2")
UNSET(BUILD_SHARED_LIBS)
FetchContent_Declare(
        tinyxml2
        URL https://github.com/leethomason/tinyxml2/archive/refs/tags/10.0.0.zip
)
FetchContent_MakeAvailable(tinyxml2)