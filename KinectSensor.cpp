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
			// Init the vector arrays to the correct size
			bodies.push_back(nullptr);
			eyes.push_back(nullptr);
			eyes.push_back(nullptr);
			face_rotations.push_back(Vector4());

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

/// <summary>
/// Attempt to acquire a new multi-source frame
/// </summary>
void KinectSensor::GetFrame()
{
	if (last_frame != nullptr) {
		last_frame->Release();
	}
	reader->AcquireLatestFrame(&last_frame);
}

/// <summary>
/// Update the depth_buffer member variable with a depth texture consiting of UINT16 values representing mm distance from sensor
/// </summary>
/// <returns>true on depth value update, false otherwise</returns>
bool KinectSensor::UpdateDepthBuffer() 
{
	if (last_frame == nullptr) {
		return false;
	}

	IDepthFrame* depthframe = nullptr;
	IDepthFrameReference* depthframeref = nullptr;
	last_frame->get_DepthFrameReference(&depthframeref);
	depthframeref->AcquireFrame(&depthframe);
	if (depthframeref) {
		depthframeref->Release();
	}
	if (depthframe == nullptr) {
		return false;
	}

	depth_buffer = std::shared_ptr<UINT16[]>(new UINT16[w_depth * h_depth]);
	depthframe->CopyFrameDataToArray(w_depth * h_depth, depth_buffer.get());
	depthframe->Release();

	return true;
}

/// <summary>
/// Update the rgb_buffer member variable, a rgba array (1920*1080*4) unsigned chars
/// </summary>
/// <returns>true on rgb value update, false otherwise</returns>
bool KinectSensor::UpdateRGBBuffer() 
{
	if (last_frame == nullptr) {
		return false;
	}

	IColorFrame* colorframe = nullptr;
	IColorFrameReference* colourframeref = nullptr;
	last_frame->get_ColorFrameReference(&colourframeref);
	colourframeref->AcquireFrame(&colorframe);
	if (colourframeref) {
		colourframeref->Release();
	}
	if (colorframe == nullptr) {
		return false;
	}

	rgb_buffer = std::shared_ptr<unsigned char[]>(new unsigned char[w_rgb * h_rgb * 4]);
	colorframe->CopyConvertedFrameDataToArray(w_rgb * h_rgb * 4, rgb_buffer.get(), ColorImageFormat_Rgba);
	colorframe->Release();

	return true;
}

/// <summary>
/// Updates the bodies member array with new data
/// </summary>
/// <returns>true upon new body data</returns>
bool KinectSensor::UpdateBodyData()
{
	if (last_frame == nullptr) {
		return false;
	}

	IBodyFrame* bodyframe = nullptr;
	IBodyFrameReference* bodyframeref = nullptr;
	last_frame->get_BodyFrameReference(&bodyframeref);
	bodyframeref->AcquireFrame(&bodyframe);
	if (bodyframeref) {
		bodyframeref->Release();
	}
	if (bodyframe == nullptr) {
		return false;
	}
	bodyframe->GetAndRefreshBodyData(BODY_COUNT, bodies.data());
	bodyframe->Release();

	return true;
}

/// <summary>
/// Populates an array with 3d points and corresponding rgb colours
/// </summary>
/// <param name="points">Pointer to points to be updated</param>
/// <returns>true when points array has been modified</returns>
bool KinectSensor::GetColourDepthPoints(std::shared_ptr<float> points)
{
	// Also check for HRESULT errors
	if (!UpdateDepthBuffer() || !UpdateRGBBuffer()) {
		return false;
	}

	std::shared_ptr<CameraSpacePoint[]> camera_points(new CameraSpacePoint[w_depth * h_depth]);
	mapper->MapDepthFrameToCameraSpace(w_depth*h_depth, depth_buffer.get(), w_depth * h_depth, camera_points.get());

	std::shared_ptr<ColorSpacePoint[]> colour_points(new ColorSpacePoint[w_depth * h_depth]);
	mapper->MapCameraPointsToColorSpace(w_depth * h_depth, camera_points.get(), w_depth * h_depth, colour_points.get());

	for (unsigned int i = 0; i < w_depth * h_depth; i++) {
		float* point = &points.get()[i * 6];
		memcpy(point, &camera_points[i], 3 * sizeof(float));

		ColorSpacePoint col_point = colour_points[i];
		if (col_point.X < 1 || col_point.X >= w_rgb || col_point.Y < 1 || col_point.Y >= h_rgb) {
			memcpy(&point[3], &depth_overlap_colour, 3 * sizeof(float));
		}
		else {
			int x = (int)col_point.X - 1;
			int y = (int)col_point.Y - 1;
			point[3] = (float)rgb_buffer[4 * (x + y * w_rgb) + 0] / 255.0f;
			point[4] = (float)rgb_buffer[4 * (x + y * w_rgb) + 1] / 255.0f;
			point[5] = (float)rgb_buffer[4 * (x + y * w_rgb) + 2] / 255.0f;
		}
	}

	return true;
}

/// <summary>
/// Updates the 3d positions of every tracked eye
/// </summary>
/// <returns>true on update success</returns>
bool KinectSensor::UpdateFaceData()
{
	if (!UpdateBodyData() || !UpdateDepthBuffer()) {
		return false;
	}

	for (int face = 0; face < BODY_COUNT; face++) {
		IFaceFrame* face_frame = nullptr;
		if (face_frame_readers[face]->AcquireLatestFrame(&face_frame) != S_OK) {
			return false;
		}

		BOOLEAN is_face_tracked = false;
		if (face_frame->get_IsTrackingIdValid(&is_face_tracked) != S_OK) {
			return false;
		}

		if (is_face_tracked) {
			// We have a face tracked, get the face properties and update the vector lists
			available_faces[face] = true;

			IFaceFrameResult* face_frame_result = nullptr;
			face_frame->get_FaceFrameResult(&face_frame_result);

			face_frame_result->get_FaceRotationQuaternion(&face_rotations[face]);

			PointF face_points[FacePointType::FacePointType_Count];
			face_frame_result->GetFacePointsInInfraredSpace(FacePointType::FacePointType_Count, face_points);

			PointF left_eye_infrared = face_points[FacePointType::FacePointType_EyeLeft];
			PointF right_eye_infrared = face_points[FacePointType::FacePointType_EyeRight];

			DepthSpacePoint left_eye_depth_point;
			memcpy(&left_eye_depth_point, &left_eye_infrared, sizeof(DepthSpacePoint));

			DepthSpacePoint right_eye_depth_point;
			memcpy(&right_eye_depth_point, &right_eye_infrared, sizeof(DepthSpacePoint));

			int average_x = (int)((right_eye_depth_point.X + left_eye_depth_point.X) / 2.0f);
			int average_y = (int)((right_eye_depth_point.Y + left_eye_depth_point.Y) / 2.0f);
			UINT16 depth_to_eyes = depth_buffer[average_x + average_y * w_depth];

			std::shared_ptr<CameraSpacePoint> left_eye_point(new CameraSpacePoint);
			mapper->MapDepthPointToCameraSpace(left_eye_depth_point, depth_to_eyes, left_eye_point.get());
			std::shared_ptr<CameraSpacePoint> right_eye_point(new CameraSpacePoint);
			mapper->MapDepthPointToCameraSpace(right_eye_depth_point, depth_to_eyes, right_eye_point.get());
			
			eyes[face + 0] = left_eye_point;
			eyes[face + 1] = right_eye_point;

			face_frame_result->Release();
		}
		else {
			// Face tracking not valid, attempt to by checking if body is tracked then informing the face frame source
			available_faces[face] = false;
			IBody* body = bodies[face];
			if (body != nullptr) {
				BOOLEAN is_body_tracked = false;
				body->get_IsTracked(&is_body_tracked);

				if (is_body_tracked) {
					UINT64 body_id;
					body->get_TrackingId(&body_id);
					face_frame_sources[face]->put_TrackingId(body_id);
				}
			}
		}
	}

	return true;
}

/// <summary>
/// Populates an array with the positions (and some static colour) of the eyes of a person
/// </summary>
/// <param name="points">Pointer to start of array which holds points for eye tracking visualisation</param>
/// <returns>true on eye point update</returns>
bool KinectSensor::GetColouredEyePoints(std::shared_ptr<float> points)
{
	/*if (!UpdateFaceData()) {
		return false;
	}*/
	
	for (int i = 0; i < eyes.size(); i++) {
		if (eyes[i].get() != nullptr) {
			float* eye_point = &points.get()[i * 6];
			memcpy(&eye_point[0], eyes[i].get(), sizeof(CameraSpacePoint));
			memcpy(&eye_point[3], eye_colour, 3 * sizeof(float));
		}
	}

	return true;
}

std::array<bool, BODY_COUNT> KinectSensor::GetAvailableFaces()
{
	return available_faces;
}

std::vector<std::shared_ptr<CameraSpacePoint>> KinectSensor::GetEyePositions(unsigned int face_id)
{
	std::vector<std::shared_ptr<CameraSpacePoint>> points;
	points.push_back(eyes[face_id + 0]);
	points.push_back(eyes[face_id + 1]);
	return points;
}

KinectSensor::~KinectSensor() {
	if (sensor) {
		sensor->Close();
	}
}