#pragma once
#include <TlHelp32.h>
uintptr_t virtualaddy;

//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
//props dvl -> @mmcopymemory for driver: https://github.com/MmCopyMemory/fud-eac-driver
#define dtbfix_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x12A, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define rw_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x13A, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define base_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x14A, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

static bool EAC = true;
typedef struct _rw {
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
    BOOLEAN write;
} rw, * prw;

typedef struct _ba {
    INT32 process_id;
    ULONGLONG* address;
} ba, * pba;
typedef struct _dtb {
    INT32 process_id;
    bool* operation;
} dtb, * dtbl;
typedef struct _ga {
    ULONGLONG* address;
} ga, * pga;

namespace mem {
    HANDLE driver_handle;
    INT32 process_id;

    bool find_driver() {
        driver_handle = CreateFileW(L"\\\\.\\ndyjan", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); //old name staydetected

        if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
            return false;

        return true;
    }

    void read_physical(PVOID address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };

        //arguments.security = code_security;
        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = FALSE;

        DeviceIoControl(driver_handle, rw_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    inline bool read1(const std::uintptr_t address, void* buffer, const std::size_t size)
    {
        if (buffer == nullptr || size == 0) {
            return false;
        }
        read_physical(reinterpret_cast<PVOID>(address), buffer, static_cast<DWORD>(size));
    }

    void write_physical(PVOID address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };

        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = TRUE;

        DeviceIoControl(driver_handle, rw_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    uintptr_t find_image() {
        uintptr_t image_address = { NULL };
        _ba arguments = { NULL };

        arguments.process_id = process_id;
        arguments.address = (ULONGLONG*)&image_address;

        DeviceIoControl(driver_handle, base_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return image_address;
    }
    bool CR3() {
        bool ret = false;
        _dtb arguments = { 0 };
        arguments.process_id = process_id;
        arguments.operation = (bool*)&ret;
        DeviceIoControl(driver_handle, dtbfix_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return ret;
    }
    INT32 find_process(LPCTSTR process_name) {
        PROCESSENTRY32 pt;
        HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pt.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hsnap, &pt)) {
            do {
                if (!lstrcmpi(pt.szExeFile, process_name)) {
                    CloseHandle(hsnap);
                    process_id = pt.th32ProcessID;
                    return pt.th32ProcessID;
                }
            } while (Process32Next(hsnap, &pt));
        }
        CloseHandle(hsnap);

        return { NULL };
    }
}

bool IsProcessRunning(LPCTSTR process_name) {
    PROCESSENTRY32 pt;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);


    if (Process32First(hsnap, &pt)) {
        do {
            if (lstrcmpi(pt.szExeFile, process_name) == 0) {
                CloseHandle(hsnap);
                return true;
            }
        } while (Process32Next(hsnap, &pt));
    }

    CloseHandle(hsnap);
    return false;
}

template <typename T>
T read(uint64_t address) {
    T buffer{ };
    mem::read_physical((PVOID)address, &buffer, sizeof(T));
    return buffer;
}
template<typename T>
bool read_array(uintptr_t address, T out[], size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        out[i] = read<T>(address + i * sizeof(T));
    }
    return true; // you can add additional checks to verify successful reads
}
template <typename T>
T write(uint64_t address, T buffer) {

    mem::write_physical((PVOID)address, &buffer, sizeof(T));
    return buffer;
}


bool is_valid(const uint64_t adress)
{
    if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
        0x7FFFFFFFFFFFFFFF) {
        return false;
    }
    return true;
}