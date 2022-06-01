#include <iostream>
#include <Windows.h>

#include "KinectSensor.h"


int main() {
	byte* dest = new byte[1080 * 1920 * 4];
	KinectSensor kinect;
	kinect.OpenColourStream();
	kinect.GetColourFrame(dest);
	kinect.~KinectSensor();
}