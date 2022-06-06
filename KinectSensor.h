#pragma once

#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <Kinect.h>

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	void GetColourFrame(byte* data);
	CameraSpacePoint* GetDepthPoints();

private:
	IKinectSensor* sensor = nullptr;
	IColorFrameReader* colour_reader = nullptr;
	ICoordinateMapper* mapper = nullptr;
	IMultiSourceFrameReader* reader = nullptr;

	int width = 512;
	int height = 424;
};