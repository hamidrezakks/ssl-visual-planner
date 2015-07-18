#include "playonrole.h"

CRolePlayOn::CRolePlayOn()
{
    updated = false;
    gotoPointSkill = new CskillNewGotoPoint(agent);
    gotoPointAvoidSkill = new CSkillGotoPointAvoid(agent);
    kickSkill = new CSkillKick(agent);
    receivePassSkill =  new CSkillReceivePass(agent);
    oneTouchSkill = new CSkillKickOneTouch(agent);
}

CRolePlayOn::~CRolePlayOn()
{

}

void CRolePlayOn::setAgent(CAgent *_agent)
{
    agent = _agent;
}

void CRolePlayOn::initTask()
{
    updated = false;
    switch(selectedSkill)
    {
        case SkillGotopoint:
            gotoPointSkill->setAgent(agent);
            gotoPointSkill->setFinalPos(target);
            gotoPointSkill->setFinalDir(targetDir);
            gotoPointSkill->setFinalVel(targetVel);
        break;
        case SkillGotopointAvoid:
            gotoPointAvoidSkill->setAgent(agent);
            gotoPointAvoidSkill->setFinalPos(target);
            gotoPointAvoidSkill->setFinalDir(targetDir);
            gotoPointAvoidSkill->setFinalVel(targetVel);
            gotoPointAvoidSkill->oppRelax(oppRelax);
            gotoPointAvoidSkill->ourRelax(ourRelax);
            gotoPointAvoidSkill->setAvoidCenterCircle(avoidCenterCircle);
            gotoPointAvoidSkill->setAvoidPenaltyArea(avoidPenaltyArea);
            gotoPointAvoidSkill->setMaxVelocity(3.0);
        break;
        case SkillKick:
            kickSkill->setAgent(agent);
            kickSkill->setTarget(target);
            kickSkill->setTolerance(tolerance);
            kickSkill->setChip(chip);
            kickSkill->setSpin(spin);
            kickSkill->setKickSpeed(kickSpeed);
            kickSkill->setAvoidPenaltyArea(avoidPenaltyArea);
            kickSkill->setIsGotoPointAvoid(isGotoPointAvoid);
        break;
        case SkillReceivePass:
            receivePassSkill->setAgent(agent);
            receivePassSkill->setTarget(waitPos);
            receivePassSkill->setSlow(slow);
            receivePassSkill->setReceiveRadius(receiveRadius);
            receivePassSkill->setAvoidOppPenaltyArea(avoidPenaltyArea);
            receivePassSkill->setAvoidOurPenaltyArea(avoidPenaltyArea);
        break;
        case SkillOneTouch:
            oneTouchSkill->setAgent(agent);
            oneTouchSkill->setWaitPos(waitPos);
            oneTouchSkill->setTarget(target);
            oneTouchSkill->setKickSpeed(kickSpeed);
            oneTouchSkill->setChip(chip);
            oneTouchSkill->setAvoidPenaltyArea(avoidPenaltyArea);
        break;
    }
}

void CRolePlayOn::execute()
{
    if(updated)
        initTask();
    if(selectedSkill ==  SkillGotopoint)
    {
            gotoPointSkill->execute();
    }
    else if(selectedSkill == SkillGotopointAvoid)
    {
            gotoPointAvoidSkill->execute();
    }
    else if(selectedSkill == SkillKick)
    {
            kickSkill->execute();
     }
     else if(selectedSkill == SkillReceivePass)
     {
            receivePassSkill->execute();
     }
    else if(selectedSkill == SkillOneTouch)
    {
            oneTouchSkill->execute();
    }
}
