#ifndef SERIAL_H
#define SERIAL_H

#include <fcntl.h>
#include <unistd.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#define COM_HANDLE HANDLE
typedef uint32_t speed_t;
#else
#include <termios.h>
    #define COM_HANDLE int
#endif // _WIN32

class Serial
{
public:
    void setup(std::string port, speed_t baud);
    bool open();
    void close();

    void send(const char *text);
    void send(std::string text);
    void send(void *data, size_t length);
    bool readChar(uint8_t &c);

private:
    bool openPOSIX();
    bool openWin32();

    COM_HANDLE fileDescriptor = nullptr;
    std::string portPath;
    speed_t baudRate = 0;

    HANDLE createPipe(const char *path);
    bool mosiPipeOpened, misoPipeOpened;
    HANDLE misoPipe = nullptr, mosiPipe = nullptr;

    void ensurePipeOpened();
};

#endif // SERIAL_H
