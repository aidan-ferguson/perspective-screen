#include "KinectSensor.h"

KinectSensor::KinectSensor() 
{
	assert( GetDefaultKinectSensor(&sensor) == S_OK );
	sensor->get_CoordinateMapper(&mapper);

	if (sensor) {
		assert( sensor->Open() == S_OK );

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
			frame->CopyConvertedFrameDataToArray(1080 * 1920 * 4, data, ColorImageFormat_Bgra);
			frame->Release();
		}
		else {
			std::cerr << "Could not acquire latest frame" << std::endl;
		}
	}
}

std::vector<Point> KinectSensor::GetDepthPoints() 
{
	// Need to release some objects in the real impl

	IMultiSourceFrame* frame = NULL;
	while(frame == NULL)
		reader->AcquireLatestFrame(&frame);

	IDepthFrame* depthframe;
	IDepthFrameReference* depthframeref = NULL;
	frame->get_DepthFrameReference(&depthframeref);
	depthframeref->AcquireFrame(&depthframe);
	if (depthframeref) depthframeref->Release();

	UINT sz;
	UINT16* buf = new UINT16[512*424];
	depthframe->AccessUnderlyingBuffer(&sz, &buf);

	//mapper->MapDepthFrameToCameraSpace(512 * 424, buf, 512 * 424, depth2xyz);

	//ColorSpacePoint* depth2rgb = new ColorSpacePoint[512 * 424];
	//mapper->MapDepthFrameToColorSpace(512 * 424, buf, 512 * 424, depth2rgb);

	CameraSpacePoint* depth2xyz = new CameraSpacePoint[1920 * 1080];
	mapper->MapColorFrameToCameraSpace(512 * 424, buf, 1920 * 1080, depth2xyz);

	if (depthframe) depthframe->Release();

	IColorFrame* colorframe;
	IColorFrameReference* frameref = NULL;
	frame->get_ColorFrameReference(&frameref);
	frameref->AcquireFrame(&colorframe);
	if (frameref) frameref->Release();
	unsigned char* rgbimage = new unsigned char[1080 * 1920 * 4];
	colorframe->CopyConvertedFrameDataToArray(1080 * 1920 * 4, rgbimage, ColorImageFormat_Rgba);

	std::vector<Point> points;


	for (int i = 0; i < 1920*1080; i++) {
		CameraSpacePoint camp = depth2xyz[i];
		Point point(glm::vec3(camp.X, camp.Y, camp.Z), glm::vec3((float)rgbimage[4 * i + 0] / 255.0f, (float)rgbimage[4 * i + 1] / 255.0f, (float)rgbimage[4 * i + 2] / 255.0f));
		points.push_back(point);
	}

	return points;
}

KinectSensor::~KinectSensor() {
	if (sensor) {
		sensor->Close();
	}
}