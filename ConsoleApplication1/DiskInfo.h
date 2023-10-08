#pragma once

// Class for retrieving disk information using WMI
class DiskInfoRetriever {
public:
    // Constructor
    DiskInfoRetriever() : pLoc(nullptr), pSvc(nullptr), pEnumerator(nullptr) {}

    // Destructor
    ~DiskInfoRetriever() {
        cleanup();
    }

    // Initialize WMI components for disk information retrieval
    void initializeWMI() {
        // Initialize security for WMI
        HRESULT hres = CoInitializeSecurity(
            nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

        // Create an IWbemLocator instance
        hres = CoCreateInstance(
            CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
            (LPVOID*)&pLoc
        );

        if (FAILED(hres)) {
            std::cerr << "Failed to create IWbemLocator object. Error code: " << hres << std::endl;
            cleanup();
            return;
        }

        // Connect to the WMI namespace
        hres = pLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc
        );

        if (FAILED(hres)) {
            std::cerr << "Could not connect to WMI namespace. Error code: " << hres << std::endl;
            cleanup();
            return;
        }

        // Set security levels for the proxy
        hres = CoSetProxyBlanket(
            pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE
        );

        if (FAILED(hres)) {
            std::cerr << "Could not set proxy blanket. Error code: " << hres << std::endl;
            cleanup();
            return;
        }
    }

    // Retrieve disk information using WMI
    void retrieveDiskInfo() {
        HRESULT hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_DiskDrive"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator
        );

        if (FAILED(hres)) {
            std::cerr << "Query for disk information failed. Error code: " << hres << std::endl;
            return;
        }

        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;

        while (pEnumerator) {
            hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (uReturn == 0)
                break;

            VARIANT vtProp;

            // Retrieve disk model
            hres = pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hres)) {
                SetConsoleColor(FOREGROUND_GREEN);
                std::wcout << "\nDisk Model: ";
                SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                std::wcout << vtProp.bstrVal << std::endl;
                VariantClear(&vtProp);
            }

            pclsObj->Release();
        }
    }

private:
    IWbemLocator* pLoc;         // Pointer to IWbemLocator interface
    IWbemServices* pSvc;        // Pointer to IWbemServices interface
    IEnumWbemClassObject* pEnumerator;  // Pointer to IEnumWbemClassObject interface

    // Clean up WMI components
    void cleanup() {
        if (pEnumerator) pEnumerator->Release();
        if (pSvc) pSvc->Release();
        if (pLoc) pLoc->Release();
        CoUninitialize();
    }
};
