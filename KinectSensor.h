#pragma once

#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <Kinect.h>

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	void OpenColourStream();
	void GetColourFrame(byte*);

private:
	IKinectSensor* sensor = nullptr;
	IColorFrameReader* colour_reader = nullptr;

	int width = 1920;
	int height = 1080;
};