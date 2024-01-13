#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
using namespace Util;

namespace Strategy
{
  void SkillSet::spin(const SParam& param)
  {
    //Logger::toStdOut("%f\n", state->homeAngle[botID]);
    float omega,vl,vr ; 
    omega = param.SpinP.radPerSec ;
    vr = (omega * (MAX_BOT_SPEED))/MAX_BOT_OMEGA ;
    vl = -vr;
    //printf("%f %f\n", vl, vr);
    //comm.sendCommand(botID, 0,(vl + vr)/2, ((vl - vr)*MAX_BOT_OMEGA)/(2*MAX_BOT_SPEED), 0, false); 
#if FIRA_COMM || FIRASSL_COMM
      comm->sendCommand(botID, vl, vr);        
#else
      comm->sendCommand(botID, vr, vl);
#endif
    //comm.sendCommand(botID, 0, 0, param.SpinP.radPerSec, 0, false);
  } // spin
}
