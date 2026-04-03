#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <tchar.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")

void PrintDeviceProperties(SP_DEVINFO_DATA &devInfo, HDEVINFO hDevInfo) {
    TCHAR buffer[1024];
    DWORD size = 0;


    for (DWORD prop = SPDRP_DEVICEDESC; prop <= SPDRP_DRIVER; ++prop) {
        if (SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfo, prop, nullptr, (PBYTE)buffer, sizeof(buffer), &size)) {
            std::wcout << L"Property " << prop << L": " << buffer << std::endl;
        }
    }

 
    ULONG status, problem;
    CONFIGRET ret = CM_Get_DevNode_Status(&status, &problem, devInfo.DevInst, 0);
    if (ret == CR_SUCCESS) {
        std::wcout << L"Status flags: " << status << L", Problem code: " << problem << std::endl;
    }
    std::wcout << L"--------------------------------------\n";
}

int main() {

    HDEVINFO hDevInfo = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "Error getting device list.\n";
        return 1;
    }

    SP_DEVINFO_DATA devInfo;
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD index = 0;

    while (SetupDiEnumDeviceInfo(hDevInfo, index, &devInfo)) {
        std::wcout << L"Device #" << index << std::endl;
        PrintDeviceProperties(devInfo, hDevInfo);
        index++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return 0;
}