#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VALUE_NAME 256
#define MAX_VALUE_DATA 1024
#define MAX_KEY_NAME 256

// Functie pentru afisarea REG_BINARY in hex
void PrintBinary(BYTE* data, DWORD size) {
    for (DWORD i = 0; i < size; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// Functie pentru afisarea REG_MULTI_SZ linie cu linie
void PrintMultiSz(char* data, DWORD size) {
    char* p = data;
    while (*p) {
        printf("\t- %s\n", p);
        p += strlen(p) + 1; // trece la urmatorul string
    }
}

// Afiseaza valorile unei chei
void ListRegistryValues(HKEY hKey) {
    DWORD valueCount = 0;
    DWORD maxValueNameLen = 0;

    if (RegQueryInfoKeyA(hKey, NULL, NULL, NULL, NULL, NULL, NULL,
                         &valueCount, &maxValueNameLen, NULL, NULL, NULL) != ERROR_SUCCESS) {
        printf("Error querying key info.\n");
        return;
    }

    if (valueCount == 0) {
        printf("No values in this key.\n");
        return;
    }

    char valueName[MAX_VALUE_NAME];
    BYTE valueData[MAX_VALUE_DATA];

    for (DWORD i = 0; i < valueCount; i++) {
        DWORD nameSize = MAX_VALUE_NAME;
        DWORD dataSize = MAX_VALUE_DATA;
        DWORD type = 0;

        if (RegEnumValueA(hKey, i, valueName, &nameSize, NULL, &type, valueData, &dataSize) == ERROR_SUCCESS) {
            printf("\nValue %lu: %s\n", i + 1, (nameSize == 0) ? "(Default)" : valueName);
            switch (type) {
                case REG_SZ:
                    printf("\tType: STRING\n\tValue: %s\n", (char*)valueData);
                    break;
                case REG_EXPAND_SZ:
                    printf("\tType: EXPAND_STRING\n\tValue: %s\n", (char*)valueData);
                    break;
                case REG_DWORD:
                    printf("\tType: DWORD\n\tValue: %lu\n", *(DWORD*)valueData);
                    break;
                case REG_QWORD:
                    printf("\tType: QWORD\n\tValue: %llu\n", *(unsigned long long*)valueData);
                    break;
                case REG_MULTI_SZ:
                    printf("\tType: MULTI_SZ\n");
                    PrintMultiSz((char*)valueData, dataSize);
                    break;
                case REG_BINARY:
                    printf("\tType: BINARY\n\tValue (hex): ");
                    PrintBinary(valueData, dataSize);
                    break;
                default:
                    printf("\tType: Unknown\n");
                    break;
            }
        } else {
            printf("Error reading value %lu\n", i);
        }
    }
}

// Parcurge recursiv subcheile
void TraverseRegistry(HKEY hKey, const char* subKeyPath) {
    HKEY hSubKey;

    if (RegOpenKeyExA(hKey, subKeyPath, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) {
        printf("Cannot open key: %s\n", subKeyPath);
        return;
    }

    printf("\n==============================\n");
    printf("KEY: %s\n", subKeyPath);
    printf("==============================\n");

    ListRegistryValues(hSubKey);

    // Lista subcheilor
    DWORD subKeyCount = 0;
    DWORD maxSubKeyLen = 0;

    if (RegQueryInfoKeyA(hSubKey, NULL, NULL, NULL, &subKeyCount, &maxSubKeyLen, NULL,
                         NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
        RegCloseKey(hSubKey);
        return;
    }

    char subKeyName[MAX_KEY_NAME];

    for (DWORD i = 0; i < subKeyCount; i++) {
        DWORD nameLen = MAX_KEY_NAME;
        if (RegEnumKeyExA(hSubKey, i, subKeyName, &nameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            char newPath[MAX_KEY_NAME * 2];
            sprintf_s(newPath, sizeof(newPath), "%s\\%s", subKeyPath, subKeyName);
            TraverseRegistry(hKey, newPath);
        }
    }

    RegCloseKey(hSubKey);
}

int main() {
    const char* startPath = "SYSTEM\\CurrentControlSet\\Control\\Session Manager";

    // Cheia de start
    TraverseRegistry(HKEY_LOCAL_MACHINE, startPath);

    return 0;
}