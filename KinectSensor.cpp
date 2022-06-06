#include "KinectSensor.h"

KinectSensor::KinectSensor() 
{
	assert( GetDefaultKinectSensor(&sensor) == S_OK );

	if (sensor) {
		assert( sensor->Open() == S_OK );

		sensor->get_CoordinateMapper(&mapper);
		sensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color, &reader);
	}
	else {
		throw std::runtime_error("Kinect sensor was not retreived correctly");
	}
}

void KinectSensor::GetColourFrame(byte* data)
{
	IColorFrame* frame = NULL;
	if (colour_reader->AcquireLatestFrame(&frame) == S_OK) {
		if (frame) {
			frame->CopyConvertedFrameDataToArray(width * height * 4, data, ColorImageFormat_Bgra);
			frame->Release();
		}
		else {
			std::cerr << "Could not acquire latest frame" << std::endl;
		}
	}
}

CameraSpacePoint* KinectSensor::GetDepthPoints() 
{
	IMultiSourceFrame* frame = NULL;
	while(frame == NULL)
		reader->AcquireLatestFrame(&frame);
	IDepthFrame* depthframe;
	IDepthFrameReference* frameref = NULL;
	frame->get_DepthFrameReference(&frameref);
	frameref->AcquireFrame(&depthframe);

	unsigned int sz;
	unsigned short* buf;
	depthframe->AccessUnderlyingBuffer(&sz, &buf);

	CameraSpacePoint* depth2xyz = new CameraSpacePoint[512 * 424];
	mapper->MapDepthFrameToCameraSpace(width * height, buf, width * height, depth2xyz);

	return depth2xyz;
}

KinectSensor::~KinectSensor() {
	if (sensor) {
		sensor->Close();
	}
}