# Dependencies

This project uses an out-of-tree strategy for managing dependencies.
That is, rather than include dependencies (3rd party libraries)
in the source tree of JamQuest, we instead place the 3rd party libraries
source folders at the same level (peers) of the JamQuest folder,
and (after building them), install them all to a folder named "Common".

To make this easier, the CMakePresets.json file is provided (it
may alternatively be re/named CMakeUserPresets.json; the latter form
of the name merely indicates it should not be checked in to source control).
The CMakePresets.json (CMakeUserPresets.json) file both defines
an Emscripten build configuration (as well as regular debug and release)
and sets up the "../Common" install directory with appropriate
subdirectories therein for the different build configurations.
JamQuest's CMakePresets.json file isn't specific to a single project
and can be dropped into any 3rd party library's source directory
as long as that library builds with CMake.  The only customization
that must be done is if the library is in a different or deeper
subdirectory then the "../Common" relative references will need to
be edited to make sense from this other location.

Edit: some of the above is no longer true; the CMakeUserPresets.json
in my Bullet3 folder now defines CMake variables to turn off
Bullet demos and examples in the Emscripten profile.
TODO: Create a folder in this repo for bullet-specific copy of CMakeUserPresets.json.
(Or just define those variables anyway in JamQuest's?  It doesn't hurt anything.)

Unfortunately the CMAKE_FIND_ROOT_PATH variable seems to need
an absolute path (and as such comes with a default value keyed
to my own machine - so you'll have to change it).
TODO: Figure out a way to make this not use C:/Users/dferr.
Same with the Emscripten path; it uses absolute path.

Once you have placed the CMakePresets.json file in the project, reopening
the project with CLion should cause CLion to say it detected "6 new presets".
You will need to enable the 3 of the 6 presets which have names of the form
"configuration-toolchain - build-configuration-toolchain" i.e. the ones that
include the word "build".  The other three are incomplete.  You might want
to also disable the default "Debug" profile.
