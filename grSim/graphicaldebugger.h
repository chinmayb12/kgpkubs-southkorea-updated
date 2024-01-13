#ifndef GRAPHICALDEBUGGER_H
#define GRAPHICALDEBUGGER_H
#include <vector>
#define MAX_FRAMES 1
using namespace std;
class GraphicalDebugger
{
private:
    class DebugCircle {
    public:
        int x;
        int y;
        int radius;
        unsigned int color;
        DebugCircle(int x1, int y1, int radius1, unsigned int color) {
            x=x1;
            y=y1;
            radius=radius1;
            this->color = color;
        }
    };
    class DebugLine {
    public:
        int x1;
        int x2;
        int y1;
        int y2;
        unsigned int color;
        DebugLine(int ix1, int iy1, int ix2, int iy2, unsigned int color) {
            x1 = ix1;
            y1 = iy1;
            x2 = ix2;
            y2 = iy2;
            this->color = color;
        }
    };
    int frameNo;
public:
    GraphicalDebugger();

    int NUM_FRAMES;
    vector<DebugCircle> circleList[MAX_FRAMES];
    vector<DebugLine> lineList[MAX_FRAMES];

    void reset();
    void addCircle(int x, int y, int radius, unsigned int color);
    void addLine(int x1, int y1, int x2, int y2, unsigned int color);
};

#endif // GRAPHICALDEBUGGER_H
