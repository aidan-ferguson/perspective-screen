#include "DebugRenderer.h"
#include "PerspectiveRenderer.h"

int main() {
	//std::shared_ptr<KinectSensor> sensor(nullptr);
	//std::shared_ptr<PerspectiveRenderer> perspective_renderer(nullptr);
	std::shared_ptr<KinectSensor> sensor(new KinectSensor);
	std::shared_ptr<PerspectiveRenderer> perspective_renderer(new PerspectiveRenderer(sensor));
	DebugRenderer renderer(sensor, perspective_renderer);
	renderer.MainLoop();
}