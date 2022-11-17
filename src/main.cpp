#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "CursorCustodian.hpp"

#include <SDL2/SDL.h>


int main(int argc, char* args[])
{
    CursorCustodian cursor_game;

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

	return 0;
}