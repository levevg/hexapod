//
// Created by UnSeen on 28-Jan-18.
//

#include <GL/freeglut.h>
#include <iterator>
#include <sstream>
#include "CommandLine.h"
#include "Utils.h"

static std::vector<std::string> knownCommands = {"exit", "quit"};

void CommandLine::keyPressed(int key) {
    switch (key) {
        case VK_BACK: {
            if (!command.empty()) command.pop_back();
            break;
        }
        case VK_RETURN: {
            if (command.empty()) {
                deactivate();
                return;
            }
            execute(command);
            addToHistory(command);
            command = "";
            break;
        }
        case VK_UP: {
            if (verticalCursor != history.begin()) {
                verticalCursor--;
                command = *verticalCursor;
            }
            break;
        }
        case VK_DOWN: {
            if (verticalCursor != history.end()) {
                verticalCursor++;
                command = verticalCursor != history.end() ? *verticalCursor : "";
            }
            break;
        }
        case VK_LEFT: {
            break;
        }
        case VK_RIGHT: {
            break;
        }
        case VK_ESCAPE: {
            if (command.empty()) {
                deactivate();
                return;
            }
            command = "";
        }
        case VK_TAB: {
            autocomplete();
            break;
        }
        default: {
            command += (char)key;
        }
    }
}

std::string CommandLine::displayableString() const {
    int time = glutGet(GLUT_ELAPSED_TIME);
    return "> " + command + (time % 1000 < 500 ? "|" : "");
}

void CommandLine::execute(std::string cmd) {
    std::stringstream ss(cmd);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> parts(begin, end);

    if (parts.empty()) return;

    if (parts.size() > 1) {
        try {
            std::vector<double> paramsD;
            for (auto param = parts.begin()+1; param != parts.end(); ++param) {
                paramsD.emplace_back(std::stod(*param));
            }
        } catch (...) {
        }
    }

    if (parts[0] == "exit" || parts[0] == "quit") {
        glutLeaveMainLoop();
        return;
    }

    Utils::setStatus("Unknown command `" + cmd + "`");
}

//void CommandLine::registerCommand(std::string cmd, handler) {
//
//}

void CommandLine::addToHistory(std::string cmd) {
    history.push_back(cmd);
    verticalCursor = history.end();
}

void CommandLine::deactivate() {
    Utils::setStatus("");
    _active = false;
}

void CommandLine::autocomplete() {
    for (auto& known: knownCommands) {
        if (known.compare(0, command.length(), command) == 0) {
            command = known;
            break;
        }
    }
}