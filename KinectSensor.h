#pragma once

#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <Kinect.h>

// temp
#include <vector>
#include "PointCloud.h"

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	void GetColourFrame(byte* data);
	std::vector<Point> GetDepthPoints();

private:
	IKinectSensor* sensor = nullptr;
	IColorFrameReader* colour_reader = nullptr;
	ICoordinateMapper* mapper = nullptr;
	IMultiSourceFrameReader* reader = nullptr;

};