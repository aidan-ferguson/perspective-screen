#pragma once

#include <stdexcept>
#include <assert.h>
#include <iostream>

#include <Kinect.h>
#include <Kinect.Face.h>

#include "PointCloud.h"

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	bool GetColourDepthPoints(std::shared_ptr<float> points);
	bool GetEyePoints(std::shared_ptr<float> points, std::shared_ptr<float> depth_points);

private:
	UINT16 AverageDepth(int x, int y, int sample_size);

	IKinectSensor* sensor = nullptr;
	IColorFrameReader* colour_reader = nullptr;
	ICoordinateMapper* mapper = nullptr;
	IMultiSourceFrameReader* reader = nullptr;
	
	std::shared_ptr<UINT16[]> depth_buffer;

	IFaceFrameSource* face_frame_sources[BODY_COUNT];
	IFaceFrameReader* face_frame_readers[BODY_COUNT];

	static const DWORD FACE_FRAME_FEATURES = FaceFrameFeatures::FaceFrameFeatures_PointsInInfraredSpace;


};