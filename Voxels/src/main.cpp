#include "vopch.h"
#include "App.h"

// only windows
#ifdef VO_PLATFORM_WINDOWS

#ifdef VO_DEBUG
#include <crtdbg.h>
#endif

int main() {

#ifdef VO_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Game::App app(1280, 720);

	VO_TRACE("App created");

	app.Run();

	VO_TRACE("App closed");

#ifdef VO_DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;

}

#else

int main() {
	return 0;
}

#endif