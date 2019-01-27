#include <iostream>
#include "Serial.h"
#include "Config.h"
#include "CatBehavior.h"
#include "LidarAPI.h"
#include "LidarBehavior.h"
#include "SquareTestBehavior.h"

using namespace std;

Hexapod hexapod;
Serial servosPort;
Serial lidarPort;
LidarAPI lidar(&lidarPort);

int main(int argc, char *argv[])
{
    if (!Config::load()) return 1;

    if (!servosPort.open()) return 1;
    if (!lidarPort.open()) return 1;

    auto *behavior = new LidarBehavior(&hexapod, &lidar);
    //auto *behavior = new SquareTestBehavior(&hexapod);
    hexapod.setBehavior(behavior);

    do { } while (hexapod.live());

    delete behavior;

    servosPort.close();
    lidarPort.close();

    getchar();
    return 0;
}
