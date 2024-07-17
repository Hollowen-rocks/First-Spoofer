# HWID Spoofing Tool

This tool allows you to spoof various hardware identifiers (HWIDs) on a Windows system. It is primarily designed for testing and educational purposes.

## Prerequisites

- Windows OS
- C++ compiler (compatible with C++11)
- Administrative privileges (required for some operations)

## Libraries Used

- `<iostream>`: Standard Input/Output streams.
- `<Windows.h>`: Windows API functions.
- `<Iphlpapi.h>`: IP Helper API for network adapter information.
- `<Wbemidl.h>`: Windows Management Instrumentation (WMI) API.
- `<comdef.h>`, `<Shlobj.h>`: COM APIs for handling registry and special folders.
- `<string>`, `<thread>`, `<chrono>`, `<vector>`, `<stdexcept>`, `<array>`, `<sstream>`: Standard C++ libraries for various functionalities.

## Functionality

The program performs the following HWID spoofing operations:

1. **MAC Address Spoofing**: Changes MAC address of all network adapters.
2. **Registry Keys Spoofing**: Modifies the machine GUID stored in the registry.
3. **Computer Name Spoofing**: Changes the computer name.
4. **Username Spoofing**: Updates the username.
5. **Disk Serials Spoofing**: Alters serial numbers of all disk drives.
6. **NVIDIA GPU UUID Spoofing**: Modifies the UUID of NVIDIA GPUs.
7. **Volume Serial Numbers Spoofing**: Changes volume serial numbers of all disk drives.

## Usage

1. Compile the program using a C++ compiler.
2. Run the compiled executable with administrative privileges.
3. Follow on-screen prompts to complete HWID spoofing.
4. Restart the system for changes to take effect.

## Notes

- Some spoofing operations may require a system restart to apply changes.
- Use caution and ensure you have backups or restore points before applying changes.

## Disclaimer

This tool is intended for educational and testing purposes only. Use at your own risk.
