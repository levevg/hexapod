#include "Config.h"
#include <map>
#include <string>
#include <iostream>
#include <tuple>
#include <sstream>
#include "Serial.h"
#include "Hexapod.h"
#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <exception>

using namespace std;

map<string, string> Config::dictionary;

void Config::loadConfiguration()
{
    string port = Config::getValue<string>("PORT_SERVOS");
    auto speed = Config::getValue<long>("BAUD_SERVOS");
    speed_t baud = 0;
    #ifdef _WIN32
    baud = static_cast<speed_t>(speed);
    #else
    switch (speed) {
        case 2400: baud = B2400; break;
        case 38400: baud = B38400; break;
        case 9600: baud = B9600; break;
        case 115200: baud = B115200; break;
        default: throw runtime_error("Invalid BAUD in config");
    }
    #endif // _WIN32
    servosPort.setup(port, baud);

    port = Config::getValue<string>("PORT_LIDAR");
    speed = Config::getValue<long>("BAUD_LIDAR");
    baud = 0;
    #ifdef _WIN32
    baud = static_cast<speed_t>(speed);
    #else
    switch (speed) {
        case 2400: baud = B2400; break;
        case 38400: baud = B38400; break;
        case 9600: baud = B9600; break;
        case 115200: baud = B115200; break;
        default: throw runtime_error("Invalid BAUD in config");
    }
    #endif // _WIN32
    lidarPort.setup(port, baud);

    Geometry g {};
    g.tipToWrist = Config::getValue<float>("TIP_TO_WRIST") / 1000;
    g.wristToKnee = Config::getValue<float>("WRIST_TO_KNEE") / 1000;
    g.kneeToShoulder = Config::getValue<float>("KNEE_TO_SHOULDER") / 1000;
    g.centerToShoulder = Config::getValue<float>("CENTER_TO_SHOULDER") / 1000;
    g.kneeToBottom = Config::getValue<float>("KNEE_TO_BOTTOM") / 1000;
    g.kneeMinAngle = Config::getValue<float>("KNEE_MIN_ANGLE")*M_PI/180.0;
    g.kneeMaxAngle = Config::getValue<float>("KNEE_MAX_ANGLE")*M_PI/180.0;
    g.wristMinAngle = Config::getValue<float>("WRIST_MIN_ANGLE")*M_PI/180.0;
    g.wristMaxAngle = Config::getValue<float>("WRIST_MAX_ANGLE")*M_PI/180.0;
    g.shoulderMinAngle = Config::getValue<float>("SHOULDER_MIN_ANGLE")*M_PI/180.0;
    g.shoulderMaxAngle = Config::getValue<float>("SHOULDER_MAX_ANGLE")*M_PI/180.0;
    g.stanceKneeNormalized = Config::getValue<float>("STANCE_KNEE");
    g.stanceWristNormalized = Config::getValue<float>("STANCE_WRIST");
    g.stanceShoulderNormalized = Config::getValue<float>("STANCE_SHOULDER");
    hexapod.setGeometry(g);

    Config::loadLimbConfig(liLeftFront, "LEFT_FRONT");
    Config::loadLimbConfig(liLeftMiddle, "LEFT_MID");
    Config::loadLimbConfig(liLeftRear, "LEFT_REAR");
    Config::loadLimbConfig(liRightFront, "RIGHT_FRONT");
    Config::loadLimbConfig(liRightMiddle, "RIGHT_MID");
    Config::loadLimbConfig(liRightRear, "RIGHT_REAR");
}

void Config::loadLimbConfig(LimbIndex index, std::string prefix)
{
    hexapod.initLimb(index,
        hexapod.initJoint(Config::getValue<int>(prefix+"_WRIST_CHANNEL"),
                          Config::getValue<float>(prefix+"_WRIST_MIN"),
                          Config::getValue<float>(prefix+"_WRIST_MAX")),
        hexapod.initJoint(Config::getValue<int>(prefix+"_KNEE_CHANNEL"),
                          Config::getValue<float>(prefix+"_KNEE_MIN"),
                          Config::getValue<float>(prefix+"_KNEE_MAX")),
        hexapod.initJoint(Config::getValue<int>(prefix+"_SHOULDER_CHANNEL"),
                          Config::getValue<float>(prefix+"_SHOULDER_MIN"),
                          Config::getValue<float>(prefix+"_SHOULDER_MAX")),
        Config::getValue<float>(prefix+"_DIRECTION")*M_PI/180
    );
}

bool Config::load()
{
    FILE *configFile = fopen("config.txt", "r");
    if (!configFile) {
        cerr << "Couldn't load config";
        return false;
    }

    char line[200];
    while (fgets(line, 200, configFile)) {
        tuple<string, string> item;
        if (Config::splitKeyValue(line, item)) {
            dictionary[get<0>(item)] = get<1>(item);
        }
    }

    try {
        Config::loadConfiguration();
    } catch (exception &e) {
        cerr << e.what() << "\n";
        return false;
    }

    fclose(configFile);
    return true;
}

bool Config::splitKeyValue(char *line, std::tuple<std::string, std::string> &keyAndValue)
{
    int i = 0;
    while (line[i]) {
        if (line[i] == '=') {
            line[i] = 0;
            string key(line), value(line+i+1);
            keyAndValue = make_tuple(key, value);
            return true;
        }
        ++i;
    }
    return false;
}

template <typename T> T Config::getValue(std::string key)
{
    auto i = dictionary.find(key);
    if (i == dictionary.end()) {
        throw runtime_error("Config key " + key + " not found");
    }
    string value = dictionary[key];
    istringstream stream(value);
    T converted;
    stream >> converted;
    return converted;
}
