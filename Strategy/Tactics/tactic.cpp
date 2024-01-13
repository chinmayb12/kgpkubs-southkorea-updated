#include "tactic.h"
#include "beliefState.h"
#include "logger.h"
#include "pose.h"
using namespace std;
#define D_CAPTURE_BALL 0
#define D_BLOCK_BALL   1
const int APPROACH_CLEARANCE = (BOT_BALL_THRESH +BOT_RADIUS);
#define sigmoid(x) (1./(1.+exp(-(x))))

namespace Strategy {
  // Functions which will be shifted to Utils later

  inline bool getIntersection(const pair<float, float> &range1, const pair<float, float> &range2, pair<float, float> &result) {
    result = pair<float, float>(max(range1.first, range2.first), min(range1.second, range2.second));
    return result.first < result.second;
  }
  
  inline void adjustRange(float &theta1, float &theta2) {
    if(fabs(theta1-theta2) > PI) {
      if(fabs(theta1) > fabs(theta2)) {
        if(theta1 > 0) {
          theta1 -= 2*PI;
        } else {
          theta1 += 2*PI;
        }
      } else {
        if(theta2 > 0) {
          theta2 -= 2*PI;
        } else {
          theta2 += 2*PI;
        }
      }
    }
    if(theta1 > theta2)
      swap(theta1, theta2);
  }
  
  bool getIntersection(const vector<pair<float, float> > &obstacles, pair<float, float> &targetRange, 
                        pair<float, float> &result) {
    float start = obstacles.end()->second;
    start -= 2*PI;
    vector<pair<float, float> >::const_iterator it_cur  = obstacles.begin();
    pair<float, float> last_best(0,0); float last_max = 0;
    while(it_cur != obstacles.end()) {
      float end = it_cur->first;
      if(end - start > 0.01) {
        pair<float, float> intersection_res;
        if(getIntersection(targetRange, pair<float, float> (start, end), intersection_res)) {
          float delta = intersection_res.second - intersection_res.first;
          if(delta > last_max) {
            last_max  = delta;
            last_best = intersection_res;
          }
        }
      }
      start = it_cur->second;
      it_cur++;
    }
    float end = obstacles.begin()->first + 2*PI;
    if(end - start > 0.01) {
      pair<float, float> intersection_res;
      if(getIntersection(targetRange, pair<float, float> (start, end), intersection_res)) {
        float delta = intersection_res.second - intersection_res.first;
        if(delta > last_max) {
          last_max  = delta;
          last_best = intersection_res;
        }
      }
    }
    if(last_max < 0.01)
      return false;
    result = last_best;
    return true;
  }
  
  bool Tactic::getOppGoalClearance(const vector<pair<float, float> > &obstacles, const Vector2D<int> &center, 
                            pair<float, float> &result) const {
    Vector2D<int> end1(ForwardX(HALF_FIELD_MAXX), OUR_GOAL_MAXY);
    Vector2D<int> end2(ForwardX(HALF_FIELD_MAXX), OUR_GOAL_MINY);
    float theta1 = Vector2D<int>::angle(end1, center);
    float theta2 = Vector2D<int>::angle(end2, center);
    adjustRange(theta1, theta2);
    pair<float, float> goalrange(theta1, theta2);
    return getIntersection(obstacles, goalrange, result);
  }


  void Tactic::addsegment(vector<pair<float, float> > &obs, const Vector2D<int> &end1, 
                          const Vector2D<int> &end2, const Vector2D<int> &center) const {
    float theta1 = Vector2D<int>::angle(end1, center);
    float theta2 = Vector2D<int>::angle(end2, center);
    if(fabs(theta1 - theta2) > PI) {
      if(theta1 > theta2) {
        obs.push_back(pair<float, float> (theta1, PI));
        obs.push_back(pair<float, float> (-PI, theta2));
      } else {
        obs.push_back(pair<float, float> (theta2, PI));
        obs.push_back(pair<float, float> (-PI, theta1));
      }
    } else {
      if(theta1 > theta2) {
        obs.push_back(pair<float, float> (theta2, theta1));
      } else {
        obs.push_back(pair<float, float> (theta1, theta2));
      }
    }
  }
  
  void Tactic::getFullAngularClearance(vector<pair<float, float> > &result, const int radius, const Vector2D<int> &center) const {
    // TODO: Some clearance gap needs to be added but how much?
    vector<pair<float, float> > obstacles;
    // adding obstacles for our bots
    for(int i = 0; i < HomeTeam::SIZE; i++) {
      if(i == botID) continue;
      if(Vector2D<int>::dist(state->homePos[i], center) >= radius) continue;
      float angle = Vector2D<int>::angle(state->homePos[i], center);
      Vector2D<int> end1(state->homePos[i].x + BOT_RADIUS*sin(angle), state->homePos[i].y - BOT_RADIUS*cos(angle));
      Vector2D<int> end2(state->homePos[i].x - BOT_RADIUS*sin(angle), state->homePos[i].y + BOT_RADIUS*cos(angle));
      float theta1 = Vector2D<int>::angle(end1, center);
      float theta2 = Vector2D<int>::angle(end2, center);
      if(theta1 > theta2) {
        obstacles.push_back(pair<float, float>(-PI, theta2));
        obstacles.push_back(pair<float, float>(theta1, PI));
      }else {
        obstacles.push_back(pair<float, float>(theta1, theta2));
      }
    }
    
    // adding obstacles for opp bots
    for(int i = 0; i < AwayTeam::SIZE; i++) {
      if(Vector2D<int>::dist(state->awayPos[i], state->homePos[botID]) > radius) continue;
      float angle = Vector2D<int>::angle(state->awayPos[i], state->homePos[botID]);
      Vector2D<int> end1(state->awayPos[i].x + BOT_RADIUS*sin(angle), state->awayPos[i].y - BOT_RADIUS*cos(angle));
      Vector2D<int> end2(state->awayPos[i].x - BOT_RADIUS*sin(angle), state->awayPos[i].y + BOT_RADIUS*cos(angle));
      float theta1 = Vector2D<int>::angle(end1, state->homePos[botID]);
      float theta2 = Vector2D<int>::angle(end2, state->homePos[botID]);
      if(theta1 > theta2) {
        obstacles.push_back(pair<float, float>(-PI, theta2));
        obstacles.push_back(pair<float, float>(theta1, PI));
      }else {
        obstacles.push_back(pair<float, float>(theta1, theta2));
      }
    }
    // adding obstacles for our goal
    Vector2D<int> ourg1(-HALF_FIELD_MAXX, OUR_GOAL_MINY);
    Vector2D<int> ourg2(-HALF_FIELD_MAXX - GOAL_DEPTH, OUR_GOAL_MINY);
    Vector2D<int> ourg3(-HALF_FIELD_MAXX - GOAL_DEPTH, OUR_GOAL_MAXY);
    Vector2D<int> ourg4(-HALF_FIELD_MAXX, OUR_GOAL_MAXY);
    if(Vector2D<int>::dist((ourg1+ourg2)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, ourg1, ourg2, state->homePos[botID]);
    if(Vector2D<int>::dist((ourg2+ourg3)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, ourg2, ourg3, state->homePos[botID]);
    if(Vector2D<int>::dist((ourg3+ourg4)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, ourg3, ourg4, state->homePos[botID]);

    // adding obstacles for opp goal
    Vector2D<int> oppg1(HALF_FIELD_MAXX, OUR_GOAL_MINY);
    Vector2D<int> oppg2(HALF_FIELD_MAXX + GOAL_DEPTH, OUR_GOAL_MINY);
    Vector2D<int> oppg3(HALF_FIELD_MAXX + GOAL_DEPTH, OUR_GOAL_MAXY);
    Vector2D<int> oppg4(HALF_FIELD_MAXX, OUR_GOAL_MAXY);
    if(Vector2D<int>::dist((oppg1+oppg2)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, oppg1, oppg2, state->homePos[botID]);
    if(Vector2D<int>::dist((oppg2+oppg3)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, oppg2, oppg3, state->homePos[botID]);
    if(Vector2D<int>::dist((oppg3+oppg4)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, oppg3, oppg4, state->homePos[botID]);
    
    sort(obstacles.begin(), obstacles.end());
    
    float start = -PI, end = -PI - 1;
    vector<pair<float, float> >::iterator it = obstacles.begin();
    while(it != obstacles.end()) {
      if(it->first > end) {
        if(end > start) {
          result.push_back(pair<float, float> (start, end));
        }
        start = it->first;
      }
      end = max(end, it->second);
      it++;
    }
    result.push_back(pair<float, float> (start, end));
  }
  
  // List the stateless decisive skill calling functions here.


  void Tactic::gotoPoint(int x, int y, bool align, float finalSlope, float finalVelocity) {
    sID = SkillSet::GoToPoint;
    sParam.GoToPointP.x                       = x ;
    sParam.GoToPointP.y                       = y ;
    sParam.GoToPointP.align                   = align;
    sParam.GoToPointP.finalslope              = finalSlope ;
    sParam.GoToPointP.finalVelocity           = finalVelocity;
    sParam.GoToPointP.bot_point_dist_penalty  = SkillSet::Bot_Point_dis_penalty_l2;
    sParam.GoToPointP.angle_diff_penalty      = SkillSet::angle_penalty_l2;
      
    // Execute the selected skill
    skillSet->executeSkill(sID, sParam);
  }
  
  void Tactic::gotoPointExact(int x, int y, bool align, float finalSlope, float finalVelocity) {
    sID = SkillSet::GoToPoint;
    sParam.GoToPointP.x                       = x ;
    sParam.GoToPointP.y                       = y ;
    sParam.GoToPointP.align                   = align;
    sParam.GoToPointP.finalslope              = finalSlope ;
    sParam.GoToPointP.finalVelocity           = finalVelocity;
    sParam.GoToPointP.bot_point_dist_penalty  = SkillSet::Bot_Point_dis_penalty_l1;
    sParam.GoToPointP.angle_diff_penalty      = SkillSet::angle_penalty_l1;
      
    // Execute the selected skill
    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::turnToAngle(const float finalSlope, const int maxOmega) {
    sID = SkillSet::TurnToAngle;
    sParam.TurnToAngleP.finalslope = finalSlope;
    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::stopBot() {
    sID = SkillSet::Stop;
    skillSet->executeSkill(sID, sParam);
  }
}