#pragma once

// Class for retrieving and displaying graphics card information using WMI
class GraphicsCardInfo {
public:
    // Display graphics card information
    void displayInfo() const {
        SetConsoleColor(FOREGROUND_GREEN);
        std::cout << "\nGraphics Card Information:\n";
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "--------------------------\n";
        std::cout << "Model: " << model << "\n";
        std::cout << "Memory Size: " << memorySize << " MB\n";
    }

    // Retrieve graphics card information using WMI
    void retrieveInfo() {
        // Initialize WMI components and retrieve information
        if (FAILED(initializeWMI())) return;

        IEnumWbemClassObject* pEnumerator = nullptr;
        HRESULT hr = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_VideoController"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);
        if (FAILED(hr)) {
            std::cerr << "Query for operating system name failed. Error code: " << hr << std::endl;
            cleanup();
            return;
        }

        IWbemClassObject* pclsObj = nullptr;
        ULONG uReturn = 0;

        // Iterate through the enumeration and retrieve graphics card information
        while (pEnumerator) {
            hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if (uReturn == 0)
                break;

            VARIANT vtProp;

            // Retrieve graphics card model
            hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                model = _com_util::ConvertBSTRToString(vtProp.bstrVal);
                VariantClear(&vtProp);
            }

            // Retrieve graphics card memory size
            hr = pclsObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
            if (!FAILED(hr)) {
                memorySize = static_cast<int>(vtProp.uintVal / (1024 * 1024));  // Convert bytes to MB
                VariantClear(&vtProp);
            }

            pclsObj->Release();
        }

        cleanup();  // Clean up WMI components
    }

private:
    std::string model;  // Graphics card model
    int memorySize;     // Graphics card memory size in MB
    IWbemLocator* pLoc;  // Pointer to IWbemLocator interface
    IWbemServices* pSvc;  // Pointer to IWbemServices interface

    // Initialize WMI components for graphics card information retrieval
    HRESULT initializeWMI() {
        HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hr)) {
            std::cerr << "Failed to initialize COM library. Error code: " << hr << std::endl;
            return hr;
        }

        hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

        hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
            reinterpret_cast<LPVOID*>(&pLoc));
        if (FAILED(hr)) {
            std::cerr << "Failed to create IWbemLocator object. Error code: " << hr << std::endl;
            CoUninitialize();
            return hr;
        }

        hr = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0, 0, 0, 0, &pSvc);
        if (FAILED(hr)) {
            std::cerr << "Failed to connect to WMI namespace. Error code: " << hr << std::endl;
            pLoc->Release();
            CoUninitialize();
            return hr;
        }

        hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

        if (FAILED(hr)) {
            std::cerr << "Failed to set proxy blanket. Error code: " << hr << std::endl;
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return hr;
        }

        return S_OK;
    }

    // Clean up WMI components
    void cleanup() {
        if (pSvc) {
            pSvc->Release();
            pSvc = nullptr;
        }

        if (pLoc) {
            pLoc->Release();
            pLoc = nullptr;
        }

        CoUninitialize();
    }
};
