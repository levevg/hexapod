//
// Created by UnSeen on 28-Jan-18.
//

#ifndef LIDARHEXAPOD_COMMANDLINE_H
#define LIDARHEXAPOD_COMMANDLINE_H

#include <string>
#include <vector>

class CommandLine {
public:
    CommandLine() { _active = false; };
    void keyPressed(int key);
    std::string displayableString() const;
    void execute(std::string cmd);
    void activate() { _active = true; };
    void deactivate();
    bool isActive() const { return _active; };
private:
    bool _active;
    std::string command;
    std::vector<std::string> history;
    std::vector<std::string>::iterator verticalCursor;
    void addToHistory(std::string cmd);
    void autocomplete();

};


#endif //LIDARHEXAPOD_COMMANDLINE_H
