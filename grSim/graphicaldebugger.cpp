#include "graphicaldebugger.h"

GraphicalDebugger::GraphicalDebugger():frameNo(0),NUM_FRAMES(MAX_FRAMES)
{
    frameNo = 0;
    NUM_FRAMES = MAX_FRAMES;
}

void GraphicalDebugger::reset() {
    //frameNo=(frameNo+1)%NUM_FRAMES;
    circleList[frameNo].clear();
    lineList[frameNo].clear();
}
void GraphicalDebugger::addCircle(int x, int y, int radius, unsigned int color) {
    DebugCircle d(x,y,radius, color);
    circleList[frameNo].push_back(d);
}
void GraphicalDebugger::addLine(int x1, int y1, int x2, int y2, unsigned int color) {
    DebugLine d(x1, y1, x2, y2, color);
    lineList[frameNo].push_back(d);
}
