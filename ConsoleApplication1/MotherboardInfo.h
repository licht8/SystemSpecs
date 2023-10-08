#pragma once

// Class for retrieving and displaying motherboard information using WMI
class MotherboardInfo {
private:
    IWbemLocator* pLocator;    // Pointer to IWbemLocator interface
    IWbemServices* pServices;  // Pointer to IWbemServices interface

public:
    // Constructor: Initialize COM, WMI components, and connect to the WMI namespace
    MotherboardInfo() : pLocator(nullptr), pServices(nullptr) {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr)) {
            std::cerr << "Failed to initialize COM library. Error code: " << hr << std::endl;
            return;
        }

        hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

        hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
            reinterpret_cast<void**>(&pLocator));

        if (FAILED(hr)) {
            std::cerr << "Failed to create IWbemLocator instance. Error code: " << hr << std::endl;
            Cleanup();
            return;
        }

        hr = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0,
            nullptr, nullptr, &pServices);

        if (FAILED(hr)) {
            std::cerr << "Failed to connect to WMI. Error code: " << hr << std::endl;
            Cleanup();
            return;
        }

        hr = CoSetProxyBlanket(pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
            RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

        if (FAILED(hr)) {
            std::cerr << "Failed to set proxy blanket. Error code: " << hr << std::endl;
            Cleanup();
        }
    }

    // Destructor: Clean up WMI components and COM
    ~MotherboardInfo() {
        Cleanup();
    }

    // Clean up WMI components and COM
    void Cleanup() {
        if (pServices) {
            pServices->Release();
            pServices = nullptr;
        }

        if (pLocator) {
            pLocator->Release();
            pLocator = nullptr;
        }

        CoUninitialize();
    }

    // Retrieve and display motherboard information using WMI
    void GetMotherboardInfo() {
        if (!pServices) {
            std::cerr << "WMI services not available." << std::endl;
            return;
        }

        IEnumWbemClassObject* pEnumerator = nullptr;

        HRESULT hr = pServices->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_BaseBoard"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);

        if (FAILED(hr)) {
            std::cerr << "Query for Win32_BaseBoard failed. Error code: " << hr << std::endl;
            return;
        }

        IWbemClassObject* pClassObj = nullptr;
        ULONG uReturn = 0;

        // Iterate through the enumeration and retrieve motherboard information
        while (pEnumerator) {
            hr = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &uReturn);

            if (uReturn == 0)
                break;

            VARIANT vtProp;

            // Retrieve motherboard product
            hr = pClassObj->Get(L"Product", 0, &vtProp, 0, 0);

            if (SUCCEEDED(hr)) {
                SetConsoleColor(FOREGROUND_GREEN);
                std::wcout << L"\nMotherboard Product: ";
                SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                std::wcout << vtProp.bstrVal << L"\n";
                VariantClear(&vtProp);
            }

            // Retrieve motherboard manufacturer
            hr = pClassObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);

            if (SUCCEEDED(hr)) {
                SetConsoleColor(FOREGROUND_GREEN);
                std::wcout << L"Manufacturer: ";
                SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                std::wcout << vtProp.bstrVal << L"\n";
                VariantClear(&vtProp);
            }

            pClassObj->Release();
        }

        if (pEnumerator) {
            pEnumerator->Release();
            pEnumerator = nullptr;
        }
    }
};
