project(JamQuest)

cmake_minimum_required(VERSION 3.5.1)

#set(CMAKE_FIND_DEBUG_MODE TRUE)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Library paths
#        "LibXml2_DIR": "../Common/cmake-install-${presetName}/lib/cmake/libxml2-2.11.0",
#        "tmx_DIR": "../Common/cmake-install-${presetName}/lib/cmake/tmx"

find_package(tmx REQUIRED CONFIG)

add_executable(${PROJECT_NAME}
        src/entity.cpp
        src/ground.cpp
        src/groundtile.cpp
        src/main.cpp
        src/player.cpp
        src/renderwindow.cpp
        tmx_sdl_example/tmx_sdl_example.cpp
        CursorCustodian.cpp CursorCustodian.hpp SdlApplication.cpp SdlApplication.hpp TmxExample.cpp TmxExample.hpp TileGridLayer.cpp TileGridLayer.hpp CompositionLayer.cpp CompositionLayer.hpp ObjectLayer.cpp ObjectLayer.hpp ImageLayer.cpp ImageLayer.hpp LayerGroup.cpp LayerGroup.hpp MapObject.cpp MapObject.hpp Camera2D.cpp Camera2D.hpp)

# Add all headers files under the include directory
target_include_directories(${PROJECT_NAME} PRIVATE . include)

target_link_libraries(${PROJECT_NAME} tmx)

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
