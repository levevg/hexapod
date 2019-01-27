#include <GL/freeglut.h>
#include <cstdint>
#include "Camera.h"
#include "Utils.h"
#include "FPSCounter.h"
#include "VirtualRoom.h"
#include "Pod.h"
#include "FirmwareCode/Config.h"
#include "FirmwareCode/Serial.h"
#include "ComReciever.h"

const GLfloat zNear = 0.1, zFar = 100;

FPSCounter fps;
CommandLine commandLine;
Camera camera, camera3d;
VirtualRoom room;
Hexapod hexapod;
Serial servosPort;
Serial lidarPort;
Pod pod(Point2D(-1, 0.2, false), 0, &room);

bool followPod = true;
bool mode3d = true;

void display() {
    fps.tick();
    pod.tick();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);

    glLineWidth(1);
    glPointSize(1);

    if (mode3d) {
        camera3d.apply();
    } else {
        camera.apply();
    }

    pod.display(followPod, mode3d);

    glColor4f(1, 1, 1, 0.2);
    room.displayWalls();

    Utils::displayStatus();
    Utils::displayFPS("FPS: " + fps.fpsString());
    Utils::displayCommandLine(commandLine);
    Utils::displayText(5, -50, 0xFFFFFFFF, pod.debugString());

    glutSwapBuffers();
}

void tick() {
    glutPostRedisplay();
}

void keypress(unsigned char key, int x, int y) {
    if (commandLine.isActive()) {
        commandLine.keyPressed(key);
        return;
    }

    Camera *cam = mode3d ? &camera3d : &camera;

    switch (key) {
        case VK_ESCAPE: {
            glutLeaveMainLoop();
            break;
        }
        case VK_RETURN: {
            commandLine.activate();
            break;
        }
        case VK_TAB: {
            followPod = !followPod;
            break;
        }
        case VK_SPACE: {
            mode3d = !mode3d;
            break;
        }
        case 'w': case 'W': case 246: case 214: case 38: {
            cam->moveDown(fps.lastFrameInterval());
            break;
        }
        case 43: case 61: { // +=
            cam->moveForward(fps.lastFrameInterval());
            break;
        }
        case 45: { // -
            cam->moveBackward(fps.lastFrameInterval());
            break;
        }
        case 'a': case 'A': case 244: case 212: case 39: {
            cam->moveLeft(fps.lastFrameInterval());
            break;
        }
        case 's': case 'S': case 251: case 219: case 40: {
            cam->moveUp(fps.lastFrameInterval());
            break;
        }
        case 'd': case 'D': case 226: case 194: case 37: {
            cam->moveRight(fps.lastFrameInterval());
            break;
        }
        case ',': {
            pod.speedTimeDown();
            break;
        }
        case '.': {
            pod.speedTimeUp();
            break;
        }
        case '/': {
            pod.speedTimeReset();
            break;
        }
        default: {
            Utils::setStatus(std::to_string(key));
        }
    }
}

Point2D screenToWorld(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    double ox, oy, oz = zFar / ( zFar - zNear ) + zFar * zNear / ( zFar - zNear) / modelview[14];
    gluUnProject(x, viewport[3] - y, oz, modelview, projection, viewport, &ox, &oy, &oz);
    return {ox, oy, false};
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        pod.setLocation(screenToWorld(x, y));
    }
    if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
        pod.lookAt(screenToWorld(x, y));
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP : keypress(VK_UP, x, y); break;
        case GLUT_KEY_DOWN : keypress(VK_DOWN, x, y); break;
        case GLUT_KEY_LEFT : keypress(VK_LEFT, x, y); break;
        case GLUT_KEY_RIGHT : keypress(VK_RIGHT, x, y); break;
        default: break;
    }
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, width/(float)height, zNear, zFar);
}

void setup() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("LidarHexapod");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(tick);
    glutKeyboardFunc(keypress);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseClick);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Utils::setStatus("");
    //commandLine.activate();

    camera.lookFromPointToPoint(Point3D(0, 0, 7),
                                Point3D(0, 0, 0),
                                Point3D(0, 1, 0));

    camera3d.lookFromPointToPoint(Point3D(1, -3, 3.5),
                                  Point3D(0, 0, 0),
                                  Point3D(0, 0, 1));
}

int main(int argc, char **argv) {
    if (!Config::load()) return 1;
    if (!lidarPort.open()) return 1;
    if (!servosPort.open()) return 1;
    hexapodComThread = std::thread(hexapodComThreadFunc);
    hexapodComThread.detach();
    glutInit(&argc, argv);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    setup();
    glutMainLoop();
    hexapodComThreadRun = false;
    //lidarPort.close();
    //servosPort.close();
    return 0;
}