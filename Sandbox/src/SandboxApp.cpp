#include <Voxels.h>

class Sandbox : public Game::App
{
public:
	Sandbox(int width, int height)
		:App(width, height) {

	}

	~Sandbox()
	{

	}
};

int main()
{
	Sandbox* sandbox = new Sandbox(1280, 720);

	VO_TRACE("App created");
	
	sandbox->Run();

	VO_TRACE("App closed");

	delete sandbox;
}
