#include "DebugRenderer.h"
#include "PerspectiveRenderer.h"

int main() {
	//DebugRenderer renderer(std::make_shared<KinectSensor>(sensor));
	//renderer.MainLoop();
	std::shared_ptr<KinectSensor> sensor(new KinectSensor);
	//std::shared_ptr<KinectSensor> sensor(nullptr);
	PerspectiveRenderer perspective_renderer(sensor);
	perspective_renderer.MainLoop();
}