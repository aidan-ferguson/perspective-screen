#include <iostream>
#include <Windows.h>

#include "KinectSensor.h"
#include "KinectRenderer.h"

int main() {
	KinectRenderer renderer;
	renderer.MainLoop();
}