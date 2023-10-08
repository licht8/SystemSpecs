#pragma once

// Class for retrieving and displaying memory information
class MemoryInfo {
public:
    // Constructor: Initialize memoryStatus with memory information
    MemoryInfo() {
        memoryStatus.dwLength = sizeof(memoryStatus);
        GlobalMemoryStatusEx(&memoryStatus);
    }

    // Print memory information
    void printMemoryInfo() const {
        // Print total and available physical memory in MB
        SetConsoleColor(FOREGROUND_GREEN);
        std::cout << "\nMemory Information:\n";
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "  Total Physical Memory: " << memoryStatus.ullTotalPhys / (1024 * 1024) << " MB\n";
        std::cout << "  Available Physical Memory: " << memoryStatus.ullAvailPhys / (1024 * 1024) << " MB\n";
    }

private:
    MEMORYSTATUSEX memoryStatus;  // Structure to hold memory information
};
