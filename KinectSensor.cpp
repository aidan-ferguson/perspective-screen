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

// Returns true if points available, false if failure (some frame was not available)
bool KinectSensor::GetColourDepthPoints(std::shared_ptr<float> points)
{
	// Also check for HRESULT errors
	IMultiSourceFrame* frame = nullptr;
	reader->AcquireLatestFrame(&frame);
	if (frame == nullptr) {
		return false;
	}

	IDepthFrame* depthframe = nullptr;
	IDepthFrameReference* depthframeref = nullptr;
	frame->get_DepthFrameReference(&depthframeref);
	depthframeref->AcquireFrame(&depthframe);
	if (depthframeref) {
		depthframeref->Release();
	}
	if (depthframe == nullptr) {
		frame->Release();
		return false;
	}

	UINT sz = 512*424;
	std::shared_ptr<UINT16[]> buf(new UINT16[sz]);
	depthframe->CopyFrameDataToArray(sz, buf.get());

	if (depthframe) depthframe->Release();


	IColorFrame* colorframe = nullptr;
	IColorFrameReference* colourframeref = nullptr;
	frame->get_ColorFrameReference(&colourframeref);
	colourframeref->AcquireFrame(&colorframe);
	if (colourframeref) {
		colourframeref->Release();
	}
	if (colorframe == nullptr) {
		frame->Release();
		return false;
	}

	std::shared_ptr<unsigned char[]> rgb_image(new unsigned char[1080 * 1920 * 4]);
	colorframe->CopyConvertedFrameDataToArray(1080 * 1920 * 4, rgb_image.get(), ColorImageFormat_Rgba);

	std::shared_ptr<CameraSpacePoint[]> camera_points(new CameraSpacePoint[512 * 424]);
	mapper->MapDepthFrameToCameraSpace(512 * 424, buf.get(), 512 * 424, camera_points.get());

	std::shared_ptr<ColorSpacePoint[]> colour_points(new ColorSpacePoint[512 * 424]);
	mapper->MapCameraPointsToColorSpace(512*424, camera_points.get(), 512*424, colour_points.get());

	for (int i = 0; i < 512*424; i++) {
		float* point = &points.get()[i * 6];

		point[0] = camera_points[i].X;
		point[1] = camera_points[i].Y;
		point[2] = camera_points[i].Z;

		ColorSpacePoint col_point = colour_points[i];

		if(col_point.X <= 1 || col_point.X >= 1920 || col_point.Y <= 1 || col_point.Y >= 1080){
			point[3] = 0.83f;
			point[4] = 0.06f;
			point[5] = 0.35f;
		}
		else {
			int x = (int)col_point.X - 1;
			int y = (int)col_point.Y - 1;
			point[3] = (float)rgb_image[4 * (x + y * 1920) + 0] / 255.0f;
			point[4] = (float)rgb_image[4 * (x + y * 1920) + 1] / 255.0f;
			point[5] = (float)rgb_image[4 * (x + y * 1920) + 2] / 255.0f;
		}
	}

	colorframe->Release();
	frame->Release();
	return true;
}

KinectSensor::~KinectSensor() {
	if (sensor) {
		sensor->Close();
	}
}