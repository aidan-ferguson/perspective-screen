#include "DebugRenderer.h"
#include "PerspectiveRenderer.h"

int main() {
	KinectSensor sensor;
	//DebugRenderer renderer(std::make_shared<KinectSensor>(sensor));
	//renderer.MainLoop();
	//std::shared_ptr<KinectSensor> sensor(nullptr);
	PerspectiveRenderer perspective_renderer(std::make_shared<KinectSensor>(sensor));
	perspective_renderer.MainLoop();
}