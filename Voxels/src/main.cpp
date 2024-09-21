#include "vopch.h"
#include "App.h"


// only windows
#ifdef VO_PLATFORM_WINDOWS

int main() {

	Game::App app(640, 480);
	app.Run();
	return 0;

}

#else

int main() {
	return 0;
}

#endif