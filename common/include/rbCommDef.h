/*
 * This file contains the definition for the following datastructures :
 *
 * 1. SSLPacket (struct) : Low Level(PWM) Command Protocol for sending command to the robots
 * 2. Circle(class) : Parameters of a circle
 * 3. Line(class) : Parameters of a line
 * 4. Text(class) : Parameters of Text
 * 5. Element(union) : Contains Circle,Line,Text datatypes
 * 6. CanvasColor(enum) : Enumerates Colours for the canvas
 * 7. ElementType : Enumerates the type of element - Circle,Line,Text
 * 8. CanvasItem(class) : Containd Canvas Info
 * 9. BotCommand(class) : High Level Command Protocol for sending command to the robots
 * 10. SimPacket(class) : Contains BotCommand and CanvasItem as its members
 * 11. VisionInfo(class) : Comprises of the bots and the ball coordinates and other information derived from Computer Vision
 * 12. RefBoxCmd(class) : Protocols for Referee Box Commands
 */

#pragma once
#ifndef RB_COMM_DEF_H
#define RB_COMM_DEF_H

#include <stdint.h>
#include <string>
#include "comdef.h"
#include "geometry.hpp"
#include "command.pb.h"
#include "common.pb.h"

namespace HAL
{
   //Combined FIRA Packet:
  typedef struct CombinedFIRAPacket
  {
    int8_t preamble;                 // Preamble for the data packet        B0.0 - B0.7... 126 = BLUE, 127 = YELLOW
    int8_t data[10];                 // Bot data, vl vr, id 0-4. ie vl0, vr0, vl1, vr1, ..., vl4, vr4
	//int8_t timestamp;                // timestamp
  } CombinedFIRAPacket;
  
  //----- Protocols for sending commands to the Robocup SSL Simulator -----//
  static const int   MAX_DEBUG_ITEMS  = 20;

// When compiled in linux, the shared memory is identified by an unique integer
  const int SHM_BLUE_ALIAS   = 5678; // Some arbitrary number
  const int SHM_YELLOW_ALIAS = 1234; // Some arbitrary number

  typedef struct
  {
    int x, y;
    int radius;
  } Circle;

  typedef struct
  {
    int x1, y1;
    int x2, y2;
  } Line;

  typedef struct
  {
    char str[12];
    int x, y;
  } Text;

  typedef union
  {
    Circle circle;
    Line   line;
    Text   text;
  } Element;

  enum CanvasColor
  {
    RED,
    BLUE,
    BLACK,
    WHITE
  };

  enum ElementType
  {
    CIRCLE,
    LINE,
    TEXT,
    NONE
  };

  typedef struct
  {
    ElementType type;
    Element     element;
    CanvasColor color;
  } CanvasItem;

  typedef struct
  {
    float v_x;
    float v_y;
    float v_t;
    float kickPower;  // Must be within [0..1]
    bool  dribble;
  } BotCommand;

  typedef struct
  {
    BotCommand bot[Strategy::HomeTeam::SIZE];
    int        numItems;                // Must be within [0..MAX_DEBUG_ITEMS]
    CanvasItem items[MAX_DEBUG_ITEMS];
  } SimPacket;


  //------ Protocols for receiving Vision information ------//

#if FIRA_COMM || FIRASSL_COMM

  /* Index of this array represents the ID of the bot markers and the value corresponding
   * to the index represents the ID assigned to that bot in the Strategy module
   */
  static int BlueMarkerMap[]   = {0, 1, 2, 3, 4, -1, -1, -1, -1, -1, -1, -1};
  static int YellowMarkerMap[] = {0, 1, 2, 3, 4, -1, -1, -1, -1, -1, -1, -1};

#endif

  class VisionInfo
  {
  public:
    int             frameNum;

    Point2D<int>    ballPos;
    Vector2D<float> ballVel;

    Point2D<int>    bluePos[Simulator::BlueTeam::SIZE];
    Vector2D<float> blueVel[Simulator::BlueTeam::SIZE];
    float           blueAngle[Simulator::BlueTeam::SIZE];
    float           blueAngVel[Simulator::BlueTeam::SIZE];

    Point2D<int>    yellowPos[Simulator::YellowTeam::SIZE];
    Vector2D<float> yellowVel[Simulator::YellowTeam::SIZE];
    float           yellowAngle[Simulator::YellowTeam::SIZE];
    float           yellowAngVel[Simulator::YellowTeam::SIZE];

    inline VisionInfo() :
      frameNum(-1)
    { }
  };

  static std::string      REF_BOX_MULTICAST_ADDR = "224.5.23.1";
  static const unsigned short REF_BOX_MULTICAST_PORT = 30002;
  
  class RefBoxCmd
  {
  public:
    char           cmd;            // current referee command
    unsigned char  cmdCounter;     // increments each time new command is set
    unsigned char  goalsBlue;      // current score for blue team
    unsigned char  goalsYellow;    // current score for yellow team
    unsigned short timeRemaining;  // seconds remaining for current game stage (network byte order)

    RefBoxCmd() :
      cmd(0),
      cmdCounter(-1),
      goalsBlue(0),
      goalsYellow(0),
      timeRemaining(0)
    { }
  };
} // namespace HAL

#endif // RB_COMM_DEF_H
