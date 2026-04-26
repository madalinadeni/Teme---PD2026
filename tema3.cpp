#include <windows.h>

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE statusHandle;

void WINAPI ServiceCtrlHandler(DWORD ctrlCode) {
    if (ctrlCode == SERVICE_CONTROL_STOP) {
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(statusHandle, &serviceStatus);
    }
}

void WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    statusHandle = RegisterServiceCtrlHandler("HelloService", ServiceCtrlHandler);

    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    SetServiceStatus(statusHandle, &serviceStatus);

    HANDLE hEventSource = RegisterEventSource(NULL, "HelloService");
    const char* message = "Hello World from Windows Service!";
    ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0,
                NULL, 1, 0, &message, NULL);
    DeregisterEventSource(hEventSource);

    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(statusHandle, &serviceStatus);

    while (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
        Sleep(1000);
    }
}

int main() {
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { (LPSTR)"HelloService", (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    StartServiceCtrlDispatcher(serviceTable);
    return 0;
}