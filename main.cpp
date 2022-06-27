#include "DebugRenderer.h"
#include "PerspectiveRenderer.h"

int main() {
	//std::shared_ptr<KinectSensor> sensor(nullptr);
	std::shared_ptr<KinectSensor> sensor(new KinectSensor);
	DebugRenderer renderer(sensor);
	renderer.MainLoop();
}