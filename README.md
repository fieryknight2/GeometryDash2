# Geometry Dash 2

A remake of my old Geometry Dash game which was originally written
using SDL, this version is improved and rewritten using SFML.

- Assets were made using [Pixelorama](https://orama-interactive.itch.io/pixelorama)
- Fonts were taken from [fontspace.com](https://www.fontspace.com/), specific credtis are given
  in [font-credits.txt](assets/fonts/font-credits.txt)
- Music does not exist yet.

## Building

Compile with CMake, which will automatically download SFML, TinyXML, and SimpleLogger.
SFML has a few dependencies listed
here: [SFML Dependencies](https://www.sfml-dev.org/tutorials/2.6/compile-with-cmake.php)

## Running

You can run the executable from the build directory or from the project directory
as CMake will automatically copy the assets to the build directory (this may change in the future).
