#include "SharedMemory.h"

SharedMemory::SharedMemory()
{
    TCHAR szName[] = TEXT("PerspectiveScreen");
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, memory_sz, szName);
    if (hMapFile == NULL) {
        std::cout << "Failed to create file mapping: " << GetLastError() << std::endl;
        exit(1);
    }

    s_memory = (float*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, memory_sz);

    if (s_memory == nullptr)
    {
        std::cout << "Memory mapping failed:" << GetLastError() << std::endl;

        CloseHandle(hMapFile);

        exit(1);
    }
}

SharedMemory::~SharedMemory()
{
    UnmapViewOfFile(s_memory);
    CloseHandle(hMapFile);
}

void SharedMemory::UpdateCameraPosition(glm::vec3 position)
{
    memcpy(s_memory + camera_position_offset, &position, sizeof(float) * 3);
}

void SharedMemory::UpdatePerspectiveScreen(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    memcpy(s_memory + perspective_screen_offset, &position, sizeof(float) * 3);
    memcpy(s_memory + perspective_screen_offset + 3, &rotation, sizeof(float) * 3);
    memcpy(s_memory + perspective_screen_offset + 6, &scale, sizeof(float) * 3);
}
