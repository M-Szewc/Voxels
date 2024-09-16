#include "vopch.h"
#include "Engine.h"

// only windows
#ifdef VO_PLATFORM_WINDOWS

int main() {

	Game::Engine graphicsEngine = Game::Engine();
	return 0;

}

#else

int main() {
	return 0;
}

#endif