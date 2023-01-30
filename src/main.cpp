#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "CursorCustodian.hpp"
#include "TmxExample.hpp"

#include <SDL2/SDL.h>

int main2(int argc, char* args[]);

int main(int argc, char* args[])
{
    //return main2(argc, args);

    //CursorCustodian application;
    TmxExample application;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(
        [](void* arg)
        {
            static_cast<CursorCustodian*>(arg)->gameLoop();
        },
        &application,
        0,
        1
    );
#else
	while (application.is_game_running())
	{
    	application.gameLoop();
    	SDL_Delay(16);
	}
#endif

	return 0;
}