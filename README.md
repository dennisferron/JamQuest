# Example CLion SDL Game

This is an example or template project for a simple game with CMake, CLion, SDL, and compiles with Emscripten.

## Prerequisites
Msys2, MinGW64, SDL packages (for MSys), Python (for Windows)

TODO: Check these links.  (We want to use the Msys2 packages anyway, but would be good to read docs on these pages.)
[Mingw64](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-win32/seh/x86_64-8.1.0-release-win32-seh-rt_v6-rev0.7z/download), 
[SDL2](https://www.libsdl.org/download-2.0.php), 
[SDL_Image](https://www.libsdl.org/projects/SDL_image/), 
[SDL_TTF](https://www.libsdl.org/projects/SDL_ttf/), 
[SDL_Mixer](https://www.libsdl.org/projects/SDL_mixer/)
[emscripten](https://emscripten.org/docs/getting_started/downloads.html)

Note: when installing emscripten, in order for setting the global path or environment variables
to work, start cmd.exe with "Run as Administrator" when doing that step.

## Background
### CMakeLists

Since Emscripten builds in SDL_* libraries, in your Emscripten CMake target you actually 
have to _not_ do any of
the things like find_package, target_link_libraries, etc. for SDL::*.

In the CMakeLists.txt, add an if statement to check if it's an Emscripten build.

In place of linking SDL, use target_compile_options and target_link_options with the syntax:
"SHELL: -s USE_SDL_FOO=# -s USE_SDL_BAR=# etc."  We use the "SHELL:" syntax to ensure CMake
doesn't mistakenly try to consolidate the multiple "-s" arguments into one flag. 

It's also necessary to configure CLion itself to have a build configuration for Emscripten.

### Toolchain

The toolchain needs to be configured for Emscripten, but that only needs to be done once
per CLion install not once per project.  The C compiler and C++ compiler get changed to:

	<somewhere>\emsdk\upstream\emscripten\emcc.bat
	<somewhere>\emsdk\upstream\emscripten\em++.bat

I'm still using the settings of bundled MinGW, bundled CMake, bundled GDB, because that's
what the tutorial said to.  (https://tech-en.netlify.app/articles/en522284/index.html)

<somewhere> of course being the location of your Emscripten sdk folder.  Installing Emscripten is outside the scope of these notes.  I will just say make sure you use the right Python and the Windows commandline (not Msys2 or Git Bash or Powershell) when running Emscripten's installation bat, and Cmd.exe "Run As Administrator" is needed (one time only) when running the command to permanently set path and environment variables.

### CMake Profiles

Right underneath Toolchains is the CMake tab, which does need to be configured per-project.    (This can also be done in the profiles pop-up render_window CLion gives when you first open a new project.)

Setting up a profile is required in order for a Release build option to be present in CLion's build/run options after the reload of a CMakeLists.txt.  If the configure step fails for any reason, the build option for it won't be present in the UI later.

Click the + for a new profile.  Use the following values:
Name: Release-Emscripten
Build Type: Release
Toolchain: Emscripten (what we configured in Toolchain, above)
Generator: Use default (Ninja)
CMake options: -DCMAKE_TOOLCHAIN_FILE=C:\Users\dferr\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake

### Web Server

Open a Windows command prompt (non-Admin)
cd to the project's directory
cd to the "cmake-build-release-emscripten" subfolder
run "python3 -m http.server"

It should say, "Serving HTTP on :: port 8000 (http://[::]:8000/) ..."

In a browser, navigate to "http://localhost:8000/".

Note: if nothing happens in the browser but also no more messages show after the "Serving ..." message in the command prompt, pressing Ctl-C in the command prompt render_window may print backed up error messages and Python exception information.

In the directory listing in the browser, click "project-name.html".

## Compile Args
### Files for Web

Use the option "--preload-file <path>" with Emscripten to embed files the program can load.
The "<path>" can be an individual file, or a folder, e.g. the game's resources folder.
This creates a file "${PROJECT_NAME}.data" which should be deployed alongside the HTML files.
(There's no need to copy the resources folder itself to the output directory.)
This is actually an in-memory virtual filesystem, which emulates normal paths and C file API.

### Files for Native

The resources folder needs to be copied to the output directory.
(Or the working directory could be set one level up, or the exe could be programmed to look in project folder...)
TODO:  Look up how I did this with CMake configuration in other projects.

### SDL Args for Native

Unlike Emscripten, we use find_package for the SDL libraries on the native build. 
This was frustrating because several things which in theory should "just work", just don't.
There seem to have been three eras of how the SDL find_package scripts are/were distributed.
First users would write their own CMake scripts for finding SDL.
Then find_package capability for SDL was built into CMake.  (I might be confused between the historical order between the first and second.)
Lastly SDL began to include the scripts themselves, and CMake pulled out the built-in support.
(Or CMake pulled out the built-in support and voluntold SDL distribute their own find_package scripts.
I'm unclear who initiated that change first.)
Anyway the side effect of this history is different example projects may variously assume CMake built-in or
SDL-distributed find_package SDL scripts, or include a redundant exact copy of the SDL CMake scripts or roll their own entirely.
The major problem this causes (at least, I think this is the cause of this problem) is that
in some examples in some contexts the syntax is "SDL::Foo" and in others "SDL_Foo".
I don't know what the significance of the double colon syntax is, and at this point I'm afraid to ask.
All I know is that when I transitioned example code from example-project-included SDL CMake
scripts to Msys2 SDL package installed CMake scripts, I had to replace all the double colons
with single underscores to fix configuration errors and build errors.
This is complicated by the fact that, apparently, not all of the "ancillary" SDL libraries (like gfx)
HAVE CMake scripts included with the Msys2 package, or at least not ones that my CMake picks up. 
So sometimes you do still need to retain the find_package script(s) that came with the example project.
(These Msys2 packaged CMake scripts can be found in "C:\msys64\mingw64\lib\cmake\SDL2*" folders BTW.)

Then there's the SDL2::Main or SDL2::SDL2Main or SDL2_Main or WhateverTF it's named which is 
a whole can of worms in itself.  As you can guess one problem is it seems to be named different
in different versions of SDL2 find_package scripts, probably in response to the warning CMake
gives about the base name not being the same as what was passed to find_package, since I guess
find_package(SDL2) results in getting SDL2Main, etc.  I'm not a fan of libraries using preprocessor
hackery to redefine main, which leads to the second problem of ended up with either too many
or not enough "main" in your executable and linker errors therefrom.  For the example
code I started from, I found I had to add the following:
"target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_MAIN_HANDLED)"
which is weird because the Makefile the example came with (before I ported to CMakeLists.txt)
did not contain this flag in its command lines.  But this was what it needed, and it worked.

### SDL Args for Emscripten

Emscripten uses a built-in SDL.  Some other common libraries are also built-in.  These are enabled by passing a "-s USE_FOO_LIB=ver" argument for each library.  In certain cases the multiple different -s arguments could get incorrectly folded together by configuration or build steps; preceding the list of them with "SHELL: " tells the relevant tool not to do this.

The "-s" args should be passed in both the compilation and linking steps; it enables both the relevant include files and the static libs.  I set a CMake variable, "EMSCRIPTEN_SDL_OPTS", for this and use it with "target_compile_options" and "target_link_options" (_not_ "target_link_libraries").

(There are additional Emscripten linker options to pass in the target_compile_options such as "--preload-file" and "--emrun".  I don't put those in "EMSCRIPTEN_SDL_OPTS" since they're only relevant to linking.  Instead, I pass those options separately in target_link_options.)

Example:
```
if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX .html)

    set(EMSCRIPTEN_SDL_OPTS
"SHELL: -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png] -s USE_SDL_GFX=2")

    target_compile_options(${PROJECT_NAME} PUBLIC
            "${EMSCRIPTEN_SDL_OPTS}")
    target_link_options(${PROJECT_NAME} PUBLIC
            "${EMSCRIPTEN_SDL_OPTS}"
            --preload-file ../res
            --emrun)
else()
	# Native compile and link options
	# ...
endif()
```

## Project Organization

### Resources folders

In simple projects, it might make sense to just reference the resources
folder in the project directory proper, as opposed to copying it to the
cmake-build-foo folders.  With Emscripten, we can just pass ../resources
as the --preload-file argument (it will be mapped to just "./resources",
or you can use the @-syntax to specify a different path).  For parity
between the two builds, we can just set the working directory for the
native code Debug build to "../" so it can access the resources folder
the same way.

Pros: faster build, only one copy floating around
of resource files.
Cons: potential accidental modification or write of
files within project directory.

For more complex projects, certain resource files may themselves be
make targets.  Then it makes sense to use the cmake-build-foo folder
as the resource folder source, for both the Emscripten and Debug build.

However for resources which are a straight file-copy rather than built
by a tool, it seems to be harder than it should be to do this simply and
reliably with CMake.  One of the subtle issues is that a CMake file copy
command executes at configure time, but we want to re-sync asset files
at build time not only configure time.  (This would be fine if the assets
were truly readonly, but presumably they might be changed in development.)

## Code Changes for Emscripten

### Main Loop

You have to modify a game's main loop to allow Emscripten to yield control, or else you'll only get a black screen in the browser.

Add the appropriate header(s) at the very top of main.cpp:
```
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
```

Implement the content of your main loop as a function which executes one frame of the game loop when called.  Then use an #ifdef to select between running the while loop manually (native) or pass it as a callback to Emscripten.
```
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(
        [](void* arg)
        {
            static_cast<CursorCustodian*>(arg)->gameLoop();
        },
        &cursor_game,
        0,
        1
    );
#else
	while (cursor_game.is_game_running())
	{
    	cursor_game.gameLoop();
    	SDL_Delay(16);
	}
#endif
```

The "emscripten_set_main_loop_arg" form allows passing the root "Game" or "Application" object which we then static_cast in the lambda to call the member function.  If your game doesn't use a root object to hold all state data (if it uses global variables instead), there's another form "emscripten_set_main_loop" that just calls a namespace-level function.

## Credits
Cursor Custodian was used as example for code and project organization.
Ground tiles from Kenney Pixel Platformer farm expansion.
