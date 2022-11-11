project(JamQuest)

cmake_minimum_required(VERSION 3.5.1)

# The Basic SDL tutorial I saw had FindSDL2*.cmake scripts
# in a subdir.  I don't know if they're needed.
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

# Add all c source files under the src directory
file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Add all headers files under the include directory
target_include_directories(${PROJECT_NAME} PRIVATE include)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .html)

    #LIBS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
    set(EMSCRIPTEN_SDL_OPTS
"SHELL: -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png] -s USE_SDL_GFX=2")

    target_compile_options(${PROJECT_NAME} PUBLIC
            "${EMSCRIPTEN_SDL_OPTS}")
    target_link_options(${PROJECT_NAME} PUBLIC
            "${EMSCRIPTEN_SDL_OPTS}"
            --preload-file ../res
            --emrun)
else()
    #find_package(Freetype REQUIRED)

    list(APPEND CMAKE_MODULE_PATH
            /msys64/mingw64/lib/cmake/SDL2_image
            /msys64/mingw64/lib/cmake/SDL2
    )

    find_package(SDL2 REQUIRED)
    #target_link_libraries(${PROJECT_NAME} SDL2main)
    target_link_libraries(${PROJECT_NAME} SDL2)
    target_link_libraries(${PROJECT_NAME} SDL2main)
    #add_definitions(-DSDL_MAIN_HANDLED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_MAIN_HANDLED)

    find_package(SDL2_image REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2_Image)

    find_package(SDL2_ttf REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2_TTF)

    #find_package(SDL2_net REQUIRED)
    #target_link_libraries(${PROJECT_NAME} SDL2::Net)

    find_package(SDL2_mixer REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2_Mixer)

    #find_package(SDL2_gfx REQUIRED)
    #target_link_libraries(${PROJECT_NAME} SDL2::GFX)
endif()
