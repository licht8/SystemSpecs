#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#include "ProcessorInfo.h"
#include "SetConsoleColor.h"
#include "MemoryInfo.h"
#include "DiskInfo.h"
#include "GraphicsCardInfo.h"
#include "MotherboardInfo.h"
#include "FileReader.h"

typedef NTSTATUS(WINAPI* RtlGetVersionFunc)(PRTL_OSVERSIONINFOW);

//class WindowsVersionChecker {
//public:
//    WindowsVersionChecker() : hMod(GetModuleHandle(L"ntdll.dll")) {}
//
//    bool getVersion(RTL_OSVERSIONINFOW& rovi) {
//        if (hMod) {
//            RtlGetVersionFunc rtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(GetProcAddress(hMod, "RtlGetVersion"));
//            if (rtlGetVersion != nullptr) {
//                rovi.dwOSVersionInfoSize = sizeof(rovi);
//
//                if (rtlGetVersion(&rovi) == 0) {
//                    return true;
//                }
//            }
//        }
//        return false;
//    }
//
//private:
//    HMODULE hMod;
//};

int main() {

    // Code for positioning the console window
    // (code to center the console window on the screen)
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = 1000;
    int windowHeight = 600;

    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    HWND console = GetConsoleWindow();
    MoveWindow(console, windowX, windowY, windowWidth, windowHeight, TRUE);

    std::cout << "\t" << std::endl;

    // Display a logo from a file (logo.txt)
    std::string filename = "logo.txt";
    FileReader fileReader(filename);
    fileReader.readFileAndPrint();

    ProcessorInfo infoRetriever;
    infoRetriever.initializeWMI();
    infoRetriever.retrieveProcessorInfo();

    MemoryInfo memoryInfoRetriever;
    memoryInfoRetriever.printMemoryInfo();

    DiskInfoRetriever diskInfoRetriever;
    diskInfoRetriever.initializeWMI();
    diskInfoRetriever.retrieveDiskInfo();

    GraphicsCardInfo gpuInfo;
    gpuInfo.retrieveInfo();
    gpuInfo.displayInfo();

    MotherboardInfo motherboardInfo;
    motherboardInfo.GetMotherboardInfo();

    SetConsoleColor(FOREGROUND_INTENSITY);
        
    // 23232
    //3232

    std::cout << "\nPress any key to exit...";
    std::cin.get();

    return 0;
}