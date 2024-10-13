#include <Voxels.h>

class Sandbox : public VoxelEngine::App
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

	VO_INFO("App created");
	
	sandbox->Run();

	VO_INFO("App stopped running");

	delete sandbox;

	VO_INFO("App closed");
}
