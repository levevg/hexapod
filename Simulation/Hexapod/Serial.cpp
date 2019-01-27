#include "Serial.h"
#include <iostream>
#include <iomanip>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#define USE_PIPES
#else
#include <termios.h>
#endif // _WIN32

//#define SERIAL_DEBUG

using namespace std;

void Serial::setup(std::string port, speed_t baud) {
    portPath = std::move(port);
    baudRate = baud;
}

bool Serial::openPOSIX()
{
#ifndef _WIN32
    fileDescriptor = ::open(portPath.c_str(), O_WRONLY | O_NOCTTY | O_NDELAY);
    if (fileDescriptor < 0) {
        cerr << "Can't open " << portPath << "\n";
        return false;
    }

    struct termios serialSet;
    memset(&serialSet, 0, sizeof(serialSet));
    serialSet.c_iflag = IGNBRK;
    serialSet.c_cflag = CS8 | CREAD | CLOCAL;
    memset(serialSet.c_cc, _POSIX_VDISABLE, NCCS);
    serialSet.c_cc[VMIN] = 0;
    serialSet.c_cc[VTIME] = 0;
    cfsetispeed(&serialSet, baudRate);
    cfsetospeed(&serialSet, baudRate);

    if (tcsetattr(fileDescriptor, TCSANOW, &serialSet) == -1) {
        cerr << "Can't set baudrate\n";
        return false;
    }

    return true;
#endif // _WIN32
}

HANDLE Serial::createPipe(const char *path) {
    return CreateNamedPipe(path,
                           PIPE_ACCESS_DUPLEX,
                           PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                           1,
                           1024*16,
                           1024*16,
                           NMPWAIT_USE_DEFAULT_WAIT,
                           nullptr);
}

bool Serial::openWin32()
{
#ifdef _WIN32
#ifdef USE_PIPES
    string misoPath = R"(\\.\pipe\)" + portPath + "_MISO";
    string mosiPath = R"(\\.\pipe\)" + portPath + "_MOSI";

    mosiPipeOpened = false;
    misoPipeOpened = false;
    mosiPipe = createPipe(mosiPath.c_str());
    if (mosiPipe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        mosiPipeOpened = true;
        mosiPipe = CreateFile(mosiPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (mosiPipe == INVALID_HANDLE_VALUE) return false;
    }
    misoPipe = createPipe(misoPath.c_str());
    if (misoPipe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        misoPipeOpened = true;
        misoPipe = CreateFile(misoPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (misoPipe == INVALID_HANDLE_VALUE) return false;
    }
    return true;
#else
    string path = R"(\\.\pipe\)" + portPath;
    fileDescriptor = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileDescriptor == INVALID_HANDLE_VALUE) {
        cerr << "Can't open " << portPath << "\n";
        return false;
    }

    DCB dcb{};
    ZeroMemory(&dcb, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(fileDescriptor, &dcb)) {
        cerr << "GetCommState" << std::endl;
        return false;
    }
    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    if (!SetCommState(fileDescriptor, &dcb)) {
        cerr << "SetCommStateFailed" << std::endl;
        return false;
    }
    return true;
#endif
#endif // _WIN32
}

bool Serial::open()
{
#ifdef _WIN32
    return openWin32();
#else
    return openPOSIX();
#endif // _WIN32
}

void Serial::close()
{
    if (fileDescriptor > 0) {
#ifdef _WIN32
        CloseHandle(fileDescriptor);
#else
        ::close(fileDescriptor);
#endif // _WIN32
    }
    fileDescriptor = nullptr;
#ifdef USE_PIPES
    CloseHandle(misoPipe);
    CloseHandle(mosiPipe);
    misoPipe = mosiPipe = nullptr;
#endif
}

void Serial::ensurePipeOpened() {
#ifdef USE_PIPES
    if (mosiPipe && !mosiPipeOpened) {
        if (ConnectNamedPipe(mosiPipe, nullptr)) {
            mosiPipeOpened = true;
        } else {
            abort();
        }
    }
    if (misoPipe && !misoPipeOpened) {
        if (ConnectNamedPipe(misoPipe, nullptr)) {
            misoPipeOpened = true;
        } else {
            abort();
        }
    }
#endif
}

void Serial::send(const char *text)
{
#ifdef SERIAL_DEBUG
    cout << text << "\n";
#endif // SERIAL_DEBUG

#ifdef _WIN32
    ensurePipeOpened();
    DWORD written;
#ifdef USE_PIPES
    WriteFile(misoPipe, text, strlen(text), &written, nullptr);
#else
    WriteFile(fileDescriptor, text, strlen(text), &written, nullptr);
#endif
#else
    write(fileDescriptor, text, strlen(text));
#endif // _WIN32
}

void Serial::send(string text)
{
    Serial::send(text.c_str());
}

void Serial::send(void *data, size_t length)
{
#ifdef SERIAL_DEBUG
    auto chars = (unsigned char *)data;
    for (size_t i = 0; i < length; ++i) {
        cout << (int)chars[i] << " ";
    }
    cout << "\n";
#endif // SERIAL_DEBUG

#ifdef _WIN32
    ensurePipeOpened();
    DWORD written;
#ifdef USE_PIPES
    WriteFile(misoPipe, data, length, &written, nullptr);
#else
    WriteFile(fileDescriptor, text, strlen(text), &written, nullptr);
#endif
#else
    write(fileDescriptor, data, length);
#endif // _WIN32
}

bool Serial::readChar(uint8_t &c) {
    DWORD read = 0;
#ifdef USE_PIPES
    ensurePipeOpened();
    ReadFile(mosiPipe, &c, 1, &read, nullptr);
#else
    if (!fileDescriptor) return false;
    ReadFile(fileDescriptor, &c, 1, &read, nullptr);
#endif
#ifdef SERIAL_DEBUG
    if (read == 1) cout << "Read char " << (int)c << "\n";
#endif
    return read == 1;
}