#pragma once

#include <stdexcept>
#include <assert.h>
#include <iostream>
#include <array>

#include <Kinect.h>
#include <Kinect.Face.h>

#include "PointCloud.h"

#define ASSERT_HRESULT(hr) if(hr != S_OK){ std::cout << "HRESULT error: " << std::hex << hr << std::dec << std::endl; assert(false);}

class KinectSensor {
public:
	KinectSensor();
	~KinectSensor();
	
	bool GetColourDepthPoints(std::shared_ptr<float> points);
	bool GetColouredEyePoints(std::shared_ptr<float> points);
	bool UpdateFaceData();
	int GetFirstNotableFaceIndex();
	void GetFrame();

	std::vector<std::shared_ptr<CameraSpacePoint>> eyes;
	std::vector<Vector4> face_rotations;

private:

	bool UpdateDepthBuffer();
	bool UpdateRGBBuffer();
	bool UpdateBodyData();

	IKinectSensor* sensor = nullptr;
	ICoordinateMapper* mapper = nullptr;
	IMultiSourceFrameReader* reader = nullptr;
	IMultiSourceFrame* last_frame = nullptr;
	IFaceFrameSource* face_frame_sources[BODY_COUNT];
	IFaceFrameReader* face_frame_readers[BODY_COUNT];

	unsigned int w_depth = 512;
	unsigned int h_depth = 424;

	unsigned int w_rgb = 1920;
	unsigned int h_rgb = 1080;

	float depth_overlap_colour[3] = { 0.721f, 0.8f, 0.835f };
	float eye_colour[3] = { 0.0f, 1.0f, 0.0f };
	
	std::shared_ptr<UINT16[]> depth_buffer = nullptr;
	std::shared_ptr<unsigned char[]> rgb_buffer = nullptr;
	std::vector<IBody*> bodies;

	static const DWORD FACE_FRAME_FEATURES = FaceFrameFeatures::FaceFrameFeatures_PointsInInfraredSpace | FaceFrameFeatures::FaceFrameFeatures_RotationOrientation;

	int tmp_face_index = -1;
};