#pragma once

#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <Kinect.h>

// temp
#include <vector>
#include <iterator>
#include "PointCloud.h"

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	bool GetColourDepthPoints(std::shared_ptr<float> points);

private:
	IKinectSensor* sensor = nullptr;
	IColorFrameReader* colour_reader = nullptr;
	ICoordinateMapper* mapper = nullptr;
	IMultiSourceFrameReader* reader = nullptr;

};