#include "skillSet.h"

namespace Strategy
{
  void SkillSet::velocity(const SParam& param)
  {
    comm->sendCommand(botID,param.VelocityP.vl,param.VelocityP.vr);
  } // velocity
}
