#pragma once
#include "Windows.h"
#include <iostream>

#include <glm/glm.hpp>

class SharedMemory
{
private:
	// Unreal engine requires a GUID name for non Global/ shared memory regions :)
	const char* n_shared_memory = "{49249102-8471-0183-8543-087541369818}";
	int memory_sz = 12 * sizeof(float);

	int camera_position_offset = 0;
	int perspective_screen_offset = 3;

	float* s_memory = nullptr;
	HANDLE hMapFile;

public:
	SharedMemory();
	~SharedMemory();
	void UpdateCameraPosition(glm::vec3 position);
	void UpdatePerspectiveScreen(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};

