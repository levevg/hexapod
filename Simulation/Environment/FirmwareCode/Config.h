#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <tuple>
#include "Hexapod.h"
#include "Serial.h"

extern Hexapod hexapod;
extern Serial servosPort;
extern Serial lidarPort;

class Config
{
    public:

        static bool load();

        template <typename T>
            static T getValue(std::string key);

    private:

        static bool splitKeyValue(char *line, std::tuple<std::string, std::string> &keyAndValue);

        static void loadConfiguration();
        static void loadLimbConfig(LimbIndex index, std::string prefix);

        static std::map<std::string, std::string> dictionary;
};

#endif // CONFIG_H
