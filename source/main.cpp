#include "speed_camera.cpp"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage: speed_camera <VIDEO_PATH>" << std::endl;
	}

	SpeedCamera().startVideo(argv[1]);

	return 0;
}
