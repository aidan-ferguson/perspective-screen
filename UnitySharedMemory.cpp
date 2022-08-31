#include "UnitySharedMemory.h"

UnitySharedMemory::UnitySharedMemory()
{
    TCHAR szName[] = TEXT("UnityPerspectiveScreen");
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

    std::cout << "memory mapped" << std::endl;
}

UnitySharedMemory::~UnitySharedMemory()
{
    UnmapViewOfFile(s_memory);
    CloseHandle(hMapFile);
}

void UnitySharedMemory::UpdateCameraPosition(glm::vec3 position)
{
    memcpy(s_memory + camera_position_offset, &position, sizeof(float) * 3);
}

void UnitySharedMemory::UpdatePerspectiveScreen(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    memcpy(s_memory + perspective_screen_offset, &position, sizeof(float) * 3);
    memcpy(s_memory + perspective_screen_offset + 3, &rotation, sizeof(float) * 3);
    memcpy(s_memory + perspective_screen_offset + 6, &scale, sizeof(float) * 3);
}
