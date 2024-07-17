#include <iostream>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <stdexcept>
#include <array>
#include <sstream>
#include <Lmcons.h> 
#include <Shlobj.h> 

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Advapi32.lib")

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void WaitAndPrint(const std::string& message, int color, int seconds) {
    SetColor(color);
    std::cout << message << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

std::string ExecuteCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("_popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
void SpoofMACAddress() {
    try {
        ULONG bufferLength = 0;
        GetAdaptersInfo(NULL, &bufferLength);
        std::vector<BYTE> buffer(bufferLength);
        PIP_ADAPTER_INFO adapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.data());
        GetAdaptersInfo(adapterInfo, &bufferLength);

        PIP_ADAPTER_INFO adapter = adapterInfo;
        while (adapter) {
            SetColor(10); 
            std::cout << "Original MAC Address: ";
            for (UINT i = 0; i < adapter->AddressLength; i++) {
                printf("%02X", adapter->Address[i]);
                if (i < adapter->AddressLength - 1) printf("-");
            }
            std::cout << std::endl;

            std::ostringstream newMACAddress;
            for (int i = 0; i < 6; i++) {
                newMACAddress << std::hex << (rand() % 256);
                if (i < 5) newMACAddress << ":";
            }
            SetColor(11);
            std::cout << "New MAC Address: " << newMACAddress.str() << std::endl;

            std::string disableCmd = "netsh interface set interface \"" + std::string(adapter->Description) + "\" admin=disable";
            ExecuteCommand(disableCmd);
            std::this_thread::sleep_for(std::chrono::seconds(1)); 

            std::string setAddressCmd = "netsh interface set interface \"" + std::string(adapter->Description) + "\" newname=\"TemporaryName\"";
            ExecuteCommand(setAddressCmd);

            std::string setMACCmd = "netsh interface set interface \"TemporaryName\" newaddress=" + newMACAddress.str();
            ExecuteCommand(setMACCmd);

            std::string enableCmd = "netsh interface set interface \"TemporaryName\" admin=enable";
            ExecuteCommand(enableCmd);

            std::string renameCmd = "netsh interface set interface \"TemporaryName\" newname=\"" + std::string(adapter->Description) + "\"";
            ExecuteCommand(renameCmd);

            SetColor(10); 
            std::cout << "MAC Address spoofed successfully." << std::endl;

            adapter = adapter->Next;
        }
    }
    catch (const std::exception& ex) {
        SetColor(12); 
        std::cerr << "Error spoofing MAC Address: " << ex.what() << std::endl;
    }
}

void SpoofRegistryKeys() {
    try {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            char originalGUID[64] = { 0 };
            DWORD size = sizeof(originalGUID);
            if (RegQueryValueExA(hKey, "MachineGuid", NULL, NULL, (LPBYTE)originalGUID, &size) == ERROR_SUCCESS) {
                SetColor(10);
                std::cout << "Original Machine GUID: " << originalGUID << std::endl;
            }
            else {
                RegCloseKey(hKey);
                throw std::runtime_error("Failed to get the machine GUID. Error: " + std::to_string(GetLastError()));
            }

            std::string newGUID = std::to_string(rand()) + "-" + std::to_string(rand()) + "-" + std::to_string(rand()) + "-" + std::to_string(rand());
            SetColor(11); 
            std::cout << "New Machine GUID: " << newGUID << std::endl;

            if (RegSetValueExA(hKey, "MachineGuid", 0, REG_SZ, (const BYTE*)newGUID.c_str(), static_cast<DWORD>(newGUID.size() + 1)) == ERROR_SUCCESS) {
                SetColor(10); 
                std::cout << "Machine GUID changed successfully." << std::endl;
            }
            else {
                throw std::runtime_error("Failed to change the machine GUID. Error: " + std::to_string(GetLastError()));
            }

            RegCloseKey(hKey);
        }
        else {
            throw std::runtime_error("Failed to open registry key. Error: " + std::to_string(GetLastError()));
        }
        WaitAndPrint("Registry Keys spoofed successfully.", 10, 2);
    }
    catch (const std::exception& ex) {
        SetColor(12); 
        std::cerr << "Error spoofing Registry Keys: " << ex.what() << std::endl;
    }
}

void SpoofComputerName() {
    try {
        DWORD bufferSize = MAX_COMPUTERNAME_LENGTH + 1;
        char originalName[MAX_COMPUTERNAME_LENGTH + 1];
        if (GetComputerNameA(originalName, &bufferSize)) {
            SetColor(10); 
            std::cout << "Original Computer Name: " << originalName << std::endl;
        }
        else {
            throw std::runtime_error("Failed to get the computer name. Error: " + std::to_string(GetLastError()));
        }

        std::string newName = "PC-" + std::to_string(rand());
        SetColor(11); 
        std::cout << "New Computer Name: " << newName << std::endl;

        if (SetComputerNameA(newName.c_str())) {
            SetColor(10); 
            std::cout << "Computer Name changed successfully." << std::endl;
        }
        else {
            throw std::runtime_error("Failed to change the computer name. Error: " + std::to_string(GetLastError()));
        }
        WaitAndPrint("Computer Name spoofed successfully.", 10, 2);
    }
    catch (const std::exception& ex) {
        SetColor(12);
        std::cerr << "Error spoofing Computer Name: " << ex.what() << std::endl;
    }
}


void SpoofUsername() {
    try {
        DWORD bufferSize = UNLEN + 1;
        char originalUsername[UNLEN + 1];
        if (GetUserNameA(originalUsername, &bufferSize)) {
            SetColor(10);
            std::cout << "Original Username: " << originalUsername << std::endl;
        }
        else {
            throw std::runtime_error("Failed to get the username. Error: " + std::to_string(GetLastError()));
        }

        std::string newUsername = "User-" + std::to_string(rand());
        SetColor(11);
        std::cout << "New Username: " << newUsername << std::endl;

        std::string changeUsernameCmd = "net user " + std::string(originalUsername) + " /fullname:" + newUsername;
        ExecuteCommand(changeUsernameCmd);

        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH, NULL, NULL);

        SetColor(10); 
        std::cout << "Username changed successfully. The system will need to be restarted for changes to take effect." << std::endl;

        WaitAndPrint("Username spoofed successfully.", 10, 2);
    }
    catch (const std::exception& ex) {
        SetColor(12); 
        std::cerr << "Error spoofing Username: " << ex.what() << std::endl;
    }
}

void SpoofDiskSerials() {
    try {
        for (int i = 0; i < 26; ++i) {
            std::string drive = std::string(1, 'A' + i) + ":\\";
            DWORD serialNumber = 0;
            if (GetVolumeInformationA(drive.c_str(), NULL, 0, &serialNumber, NULL, NULL, NULL, 0)) {
                SetColor(10); 
                std::cout << "Original Serial Number of drive " << drive << ": " << serialNumber << std::endl;

                DWORD newSerialNumber = static_cast<DWORD>(rand());
                SetColor(11); 
                std::cout << "New Serial Number of drive " << drive << ": " << newSerialNumber << std::endl;

                std::string command = "fsutil volume dismount " + drive;
                ExecuteCommand(command);
                std::string setSerialCmd = "fsutil volume setserial " + drive + " " + std::to_string(newSerialNumber);
                ExecuteCommand(setSerialCmd);

                SetColor(10); 
                std::cout << "Serial Number of drive " << drive << " spoofed successfully." << std::endl;
            }
        }
        WaitAndPrint("Disk Serials spoofed successfully.", 10, 2);
    }
    catch (const std::exception& ex) {
        SetColor(12);
        std::cerr << "Error spoofing Disk Serials: " << ex.what() << std::endl;
    }
}

void SpoofNvidiaGpuUUID() {
    try {
        std::string originalUUID = ExecuteCommand("nvidia-smi --query-gpu=gpu_uuid --format=csv,noheader");
        SetColor(10);
        std::cout << "Original GPU UUID: " << originalUUID << std::endl;

        std::string newUUID = "GPU-NEWUUID" + std::to_string(rand());
        SetColor(11); 
        std::cout << "New GPU UUID: " << newUUID << std::endl;

        ExecuteCommand("nvidia-smi --gpu-reset");
        ExecuteCommand("nvidia-smi --gpu-uuid=" + newUUID);

        SetColor(10);
        std::cout << "GPU UUID spoofed successfully." << std::endl;
        WaitAndPrint("NVIDIA GPU UUID spoofed successfully.", 10, 2);
    }
    catch (const std::exception& ex) {
        SetColor(12);
        std::cerr << "Error spoofing NVIDIA GPU UUID: " << ex.what() << std::endl;
    }
}

void SpoofVolumeSerialNumbers() {
    try {
        for (int i = 0; i < 26; ++i) {
            std::string drive = std::string(1, 'A' + i) + ":";
            DWORD serialNumber = 0;
            if (GetVolumeInformationA(drive.c_str(), NULL, 0, &serialNumber, NULL, NULL, NULL, 0)) {
                SetColor(10); 
                std::cout << "Original Volume Serial Number of drive " << drive << ": " << serialNumber << std::endl;

                DWORD newSerialNumber = static_cast<DWORD>(rand());
                SetColor(11);
                std::cout << "New Volume Serial Number of drive " << drive << ": " << newSerialNumber << std::endl;

                std::string command = "fsutil volume dismount " + drive;
                ExecuteCommand(command);
                std::string setSerialCmd = "fsutil volume setserial " + drive + " " + std::to_string(newSerialNumber);
                ExecuteCommand(setSerialCmd);

                SetColor(10);
                std::cout << "Volume Serial Number of drive " << drive << " spoofed successfully." << std::endl;
            }
        }
        WaitAndPrint("Volume Serial Numbers spoofed successfully.", 10, 2);
    }
    catch (const std::exception& ex) {
        SetColor(12); 
        std::cerr << "Error spoofing Volume Serial Numbers: " << ex.what() << std::endl;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    SetColor(14); 
    std::cout << "Spoofing HWID..." << std::endl;
    SpoofMACAddress();
    SpoofRegistryKeys();
    SpoofComputerName();
    SpoofUsername();
    SpoofDiskSerials();
    SpoofNvidiaGpuUUID();
    SpoofVolumeSerialNumbers();


    SetColor(10);
    std::cout << "HWID spoofing complete. The system will need to be restarted for all changes to take effect." << std::endl;
    std::cout << "Do you want to restart now? (y/n): ";
    char response;
    std::cin >> response;
    if (response == 'y' || response == 'Y') {
        MessageBox(NULL, L"Restarting", L"Notification", MB_OK | MB_ICONINFORMATION);

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ExecuteCommand("shutdown /r /t 0");
    }

    SetColor(7); 
    return 0;
}
