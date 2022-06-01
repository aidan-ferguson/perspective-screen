#include <stdexcept>
#include <assert.h>
#include <iostream>

#include "KinectSensor.h"

KinectSensor::KinectSensor() 
{
	assert( GetDefaultKinectSensor(&sensor) == S_OK );

	if (sensor) {
		assert( sensor->Open() == S_OK );
	}
	else {
		throw std::runtime_error("Kinect sensor was not retreived correctly");
	}
}

void KinectSensor::OpenColourStream()
{
	IColorFrameSource* framesource = nullptr;
	assert( sensor->get_ColorFrameSource(&framesource) == S_OK );
	assert( framesource->OpenReader(&colourReader) == S_OK );
	if (framesource) {
		framesource->Release();
	}
}

void KinectSensor::GetColourFrame(byte* data)
{
	IColorFrame* frame = NULL;
	if (colourReader->AcquireLatestFrame(&frame) == S_OK) {
		if (frame) {
			std::cout << "here" << std::endl;
			frame->CopyConvertedFrameDataToArray(width * height * 4, data, ColorImageFormat_Bgra);
			frame->Release();
		}
		else {
			std::cerr << "Could not acquire latest frame" << std::endl;
		}
	}
}

KinectSensor::~KinectSensor() {
	if (sensor) {
		sensor->Close();
	}
}