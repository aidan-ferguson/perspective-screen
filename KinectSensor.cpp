#include "KinectSensor.h"

KinectSensor::KinectSensor() 
{
	assert( GetDefaultKinectSensor(&sensor) == S_OK );
	sensor->get_CoordinateMapper(&mapper);

	if (sensor) {
		assert( sensor->Open() == S_OK );

		// TODO: induvidual frame readers? (as GetHeadPoint doesn't need depth or colour)
		sensor->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Depth | 
											FrameSourceTypes::FrameSourceTypes_Color |
											FrameSourceTypes::FrameSourceTypes_Body, &reader);

		// Open sources and readers for the face tracking
		for (int i = 0; i < BODY_COUNT; i++)
		{
			if (CreateFaceFrameSource(sensor, 0, FACE_FRAME_FEATURES, &face_frame_sources[i]) != S_OK) {
				std::cout << "ensure that the NuiDatabase folder is located in the executable working dictory" << std::endl;
				assert(false);
			}
			face_frame_sources[i]->OpenReader(&face_frame_readers[i]);
		}
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
	depth_buffer = std::shared_ptr<UINT16[]>(new UINT16[sz]);
	depthframe->CopyFrameDataToArray(sz, depth_buffer.get());
	depthframe->Release();

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
	colorframe->Release();

	std::shared_ptr<CameraSpacePoint[]> camera_points(new CameraSpacePoint[512 * 424]);
	mapper->MapDepthFrameToCameraSpace(512 * 424, depth_buffer.get(), 512 * 424, camera_points.get());

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

	frame->Release();
	return true;
}

UINT16 KinectSensor::AverageDepth(int x, int y, int sample_size) {
	std::unique_ptr<UINT16[]> depth_values(new UINT16[sample_size*sample_size]);

	for (int x = 0; x < sample_size; x++) {
		for (int y = 0; y < sample_size; y++) {
			UINT16 depth_value = depth_buffer[(int)((x-(sample_size/2)) + (y-(sample_size/2)) * 424.0f)];
			depth_values[x + y] = depth_value;
		}
	}

	unsigned int sum = 0;
	for (int i = 0; i < sample_size * sample_size; i++) {
		sum += depth_values[i];
	}

	return (UINT16)(sum / (sample_size * sample_size));
}

bool KinectSensor::GetEyePoints(std::shared_ptr<float> points, std::shared_ptr<float> depth_points)
{
	IMultiSourceFrame* frame = nullptr;
	reader->AcquireLatestFrame(&frame);

	if (frame == nullptr) {
		return false;
	}

	IBody* bodies[BODY_COUNT] = {0};

	IBodyFrame* bodyframe = nullptr;
	IBodyFrameReference* bodyframeref = nullptr;
	frame->get_BodyFrameReference(&bodyframeref);
	bodyframeref->AcquireFrame(&bodyframe);
	if (bodyframeref) {
		bodyframeref->Release();
	}
	if (bodyframe == nullptr) {
		frame->Release();
		return false;
	}
	
	bodyframe->GetAndRefreshBodyData(BODY_COUNT, bodies);
	bodyframe->Release();

	for (int face = 0; face < BODY_COUNT; ++face) {
		IFaceFrame* pFaceFrame = nullptr;
		if (face_frame_readers[face]->AcquireLatestFrame(&pFaceFrame) != S_OK) {
			return false;
		}
		
		BOOLEAN bFaceTracked = false;
		if (pFaceFrame->get_IsTrackingIdValid(&bFaceTracked) != S_OK) {
			return false;
		}

		if (bFaceTracked) {

			IFaceFrameResult* pFaceFrameResult = nullptr;
			pFaceFrame->get_FaceFrameResult(&pFaceFrameResult);

			PointF facePoints[FacePointType::FacePointType_Count];
			pFaceFrameResult->GetFacePointsInInfraredSpace(FacePointType::FacePointType_Count, facePoints);

			PointF left_eye_infrared = facePoints[FacePointType::FacePointType_EyeLeft];
			PointF right_eye_infrared = facePoints[FacePointType::FacePointType_EyeRight];

			DepthSpacePoint left_eye_depth_point;
			left_eye_depth_point.X = left_eye_infrared.X;
			left_eye_depth_point.Y = left_eye_infrared.Y;

			DepthSpacePoint right_eye_depth_point;
			right_eye_depth_point.X = right_eye_infrared.X;
			right_eye_depth_point.Y = right_eye_infrared.Y;

			int average_x = (right_eye_depth_point.X + left_eye_depth_point.X) / 2;
			int average_y = (right_eye_depth_point.Y + left_eye_depth_point.Y) / 2;
			UINT16 depth_to_eyes = depth_buffer[average_x + average_y * 512];//AverageDepth(depth_point.X, depth_point.Y, 3);

			CameraSpacePoint* camera_point = new CameraSpacePoint;
			mapper->MapDepthPointToCameraSpace(left_eye_depth_point, depth_to_eyes, camera_point);

			std::cout << average_x << ", " << average_y << ", " << depth_to_eyes << std::endl;

			float* point = &points.get()[0];
			point[0] = camera_point->X;
			point[1] = camera_point->Y;
			point[2] = camera_point->Z;

			point[3] = 0.0f;
			point[4] = 1.0f;
			point[5] = 0.0f;

			pFaceFrameResult->Release();
			
		}
		else {
			// Face tracking not valid, attempt to fix
			IBody* pBody = bodies[face];
			if (pBody != nullptr)
			{
				BOOLEAN bTracked = false;
				pBody->get_IsTracked(&bTracked);

				UINT64 bodyTId;
				if (bTracked)
				{
					// get the tracking ID of this body
					HRESULT hr = pBody->get_TrackingId(&bodyTId);
					if (SUCCEEDED(hr))
					{
						// update the face frame source with the tracking ID
						face_frame_sources[face]->put_TrackingId(bodyTId);
					}
				}
			}
		}

		// TODO: FOR MORE FACES REMOVE BREAK
		//break;
	}

	frame->Release();
	return true;
}

KinectSensor::~KinectSensor() {
	if (sensor) {
		sensor->Close();
	}
}