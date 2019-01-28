#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <windows.h>

using namespace cv;
using namespace std;


VideoCapture webcam;

bool isPointNear(const Point& lhs, const Point& rhs) {
    return ((lhs.x - rhs.x)*(lhs.x - rhs.x) + (lhs.y - rhs.y)*(lhs.y - rhs.y)) < 10;
}

bool beACat(Mat &rawFrame, Mat *debugFrame, Point &dot) {
    if (debugFrame) {
        debugFrame->create(rawFrame.rows, rawFrame.cols, rawFrame.type());
        rawFrame.copyTo(*debugFrame);
    }

    Mat dilated, eroded;
    inRange(rawFrame, Scalar(100, 100, 100), Scalar(255, 255, 255), dilated);
    dilated.copyTo(eroded);
    dilate(dilated, dilated, getStructuringElement(MORPH_RECT, Size(9, 9), Point(4, 4)));
    erode(eroded, eroded, getStructuringElement(MORPH_RECT, Size(19, 19), Point(9, 9)));
    for(int y = 0; y < eroded.rows; y++) {
        for(int x = 0; x < eroded.cols; x++) {
            if (eroded.at<uchar>(y, x) && dilated.at<uchar>(y, x)) {
                floodFill(dilated, Point(x, y), Scalar(0));
            }
        }
    }

    vector<Point> pts;
    if (!countNonZero(dilated)) return false;
    findNonZero(dilated, pts);

    vector<int> grouped;
    int groupsCount = partition<Point>(pts, grouped, isPointNear);
    vector<vector<Point> > spots(groupsCount);
    vector<Point> centroids(groupsCount, Point(0,0));
    for (int i = 0; i < pts.size(); ++i) {
        spots[grouped[i]].push_back(pts[i]);
        centroids[grouped[i]] += pts[i];
    }
    for (int i = 0; i < groupsCount; ++i) {
        centroids[i].x /= spots[i].size();
        centroids[i].y /= spots[i].size();
    }
    cvtColor(rawFrame, eroded, CV_BGR2HSV);

    double bestH = 0, cH = 130;

    for (int i = 0; i < groupsCount; ++i) {
        vector<Point> spot = spots[i];
        double h = 0, n = 0, dx = 0, dy = 0;
        for (int j = 0; j < spot.size(); ++j) {
            Point p = spot[j];
            Vec3b c = eroded.at<Vec3b>(p);
            if (c[2] > 100) {
                n += 1;
                h += c[0];
            }

            dx += (p.x - centroids[i].x)*(p.x - centroids[i].x);
            dy += (p.y - centroids[i].y)*(p.y - centroids[i].y);
        }
        if (n > 0) h /= n;
        dx = sqrt(dx/spot.size());
        dy = sqrt(dy/spot.size());

        if (dx / dy > 4 || dy / dx > 4) continue;

        if ((bestH - cH)*(bestH - cH) > (h - cH)*(h - cH)) {
            bestH = h;
            dot.x = centroids[i].x;
            dot.y = centroids[i].y;
        }

        if (debugFrame) {
            char text[100];
            sprintf(text, "H=%.1f d=%.3f %.3f", h, dx, dy);
            putText(*debugFrame, text, centroids[i], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    if(!webcam.open(0)) {
        cerr << "Can't open camera";
        return 1;
    }

    webcam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT, 360);

    Mat rawFrame, debugFrame;
    Point dot;

    while (1) {
        webcam >> rawFrame;
        if(rawFrame.empty()) continue;
        //beACat(rawFrame, &debugFrame, dot);
        Sleep(1000);
        imshow("SpiderCat", rawFrame);
        if(waitKey(1) == 27) break;
    }

    return 0;
}
