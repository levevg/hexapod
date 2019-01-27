#include "utils.h"

#include <chrono>

#ifdef _WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif // _WIN32

void delayMs(uint16_t delay)
{
    #ifdef _WIN32
    HANDLE timer;
    LARGE_INTEGER ft{};
    ft.QuadPart = -(10000*delay);
    timer = CreateWaitableTimer(nullptr, TRUE, nullptr);
    SetWaitableTimer(timer, &ft, 0, nullptr, nullptr, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
    #else
    usleep(delay*1000);
    #endif // _WIN32
}

uint64_t mstime() {
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}
