#pragma once

#include <Kinect.h>

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	void OpenColourStream();
	void GetColourFrame(byte*);

private:
	IKinectSensor* sensor;
	IColorFrameReader* colourReader;

	int width = 1920;
	int height = 1080;
};