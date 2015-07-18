#include "kkagentplanclass.h"

#include <QString>
#include <QList>
#include <QStringList>

kkAgentPlanClass::kkAgentPlanClass(int agentSize, int id)
{
    if(agentSize < 1) agentSize = 1;
    if(agentSize > 5) agentSize = 5;
    agentsSize = agentSize;

    ball = -1;

    if(id < 0) id = 0;
    planId = id;

    possession = 0;

    comment = "";

    for(int i = 0; i < 5; i++)
    {
        agents[i].A = -1;
        agents[i].B = -1;
        agents[i].skill[0] = agents[i].skill[1] = agents[i].skill[2] = agents[i].skill[3] = None;
        agents[i].pSize = 0;
    }
}

void kkAgentPlanClass::clear(int agentSize)
{
    for(int i = 0; i < 5; i++)
    {
        agents[i].A = -1;
        agents[i].B = -1;
        agents[i].skill[0] = agents[i].skill[1] = agents[i].skill[2] = agents[i].skill[3] = None;
        agents[i].pSize = 0;
    }
    possession = 0;
    endMode =  Cycle;
    endPolicy = -1;
    ball = -1;
    agentsSize = agentSize;
    planId = -1;
    comment = "";
}

bool kkAgentPlanClass::setAgentSize(int agentSize)
{
    if(agentSize < 1) return false;
    if(agentSize > 5) return false;
    agentsSize = agentSize;
    return true;
}

bool kkAgentPlanClass::setAgentSkill(int agent, int priority, int a, int b, PSkills skill)
{
    if(agent > agentsSize || agent < 1) return false;
    if(priority > 4 || priority < 1) return false;
    /*if(a > 10 || a < 1) return false;
    if(b > 10 || b < 1) return false;*/
    agent--;
    priority--;
    if(skill == None && agents[agent].pSize > 0 && priority < 3)
    {
         for(int i = 0; i < agents[agent].pSize; i++)
         {
              if(i >= priority)
                  agents[agent].skill[i] = agents[agent].skill[i + 1];
         }
         agents[agent].pSize--;
         return true;
    }
    agents[agent].A = a;
    agents[agent].B = b;
    agents[agent].skill[priority] = skill;
    if(skill != None)
        agents[agent].pSize = priority + 1;
    return true;
}

bool kkAgentPlanClass::setPlanPoints(int agent, int a, int b)
{
    if(agent > agentsSize || agent < 1) return false;
    agents[agent - 1].A = a;
    agents[agent - 1].B = b;
    return true;
}

bool kkAgentPlanClass::setBallPos(int ballPos)
{
    if(ballPos < 1) return false;
    if(ballPos > 6) return false;
    ball = ballPos;
    return true;
}

void kkAgentPlanClass::setEndPolicy(PEndPolicy mode, int policy)
{
    endMode = mode;
    if(mode == 0)
    {
        if(policy < 100) policy = 100;
    }
    endPolicy = policy;
}

bool kkAgentPlanClass::setPlanId(int id)
{
    if(id < 0) return false;
    planId = id;
    return true;
}

bool kkAgentPlanClass::setPossession(int poss)
{
    if(poss < 0 || poss > 2) return false;
    possession = poss;
    return true;
}

void kkAgentPlanClass::setComment(QString cmt)
{
    comment = cmt;
}

int kkAgentPlanClass::getAgentSize()
{
    return agentsSize;
}

int kkAgentPlanClass::getAgentA(int agent)
{
    if(agent < 1) agent = 1;
    if(agent > 6) agent = 6;
    return agents[agent - 1].A;
}

int kkAgentPlanClass::getAgentB(int agent)
{
    if(agent < 1) agent = 1;
    if(agent > 6) agent = 6;
    return agents[agent - 1].B;
}

PSkills kkAgentPlanClass::getAgentSkill(int agent, int priority)
{
    if(agent < 1) agent = 1;
    if(agent > 6) agent = 6;
    if(priority < 1) priority = 1;
    if(priority > 4) priority = 4;
    return agents[agent - 1].skill[priority - 1];
}

int kkAgentPlanClass::getPrioritySize(int agent)
{
    if(agent < 1) agent = 1;
    if(agent > 6) agent = 6;
    return agents[agent - 1].pSize;
}

int kkAgentPlanClass::getBallPos()
{
    return ball;
}

PEndPolicy kkAgentPlanClass::getEndPolicyMode()
{
    return endMode;
}

int kkAgentPlanClass::getEndPolicyValue()
{
    return endPolicy;
}

int kkAgentPlanClass::getId()
{
    return planId;
}

int kkAgentPlanClass::getPossession()
{
    return possession;
}

QString kkAgentPlanClass::getComment()
{
    /*QString tempText = comment;
    QStringList tempTextList = tempText.split("*spc*");
    tempText = tempTextList.join("\n*");*/
    return comment;
}

bool kkAgentPlanClass::isExecutable(QString &error)
{
    error = "";
    QList<PSkills> twoPointSkills;
    twoPointSkills.append(MoveOffensive);
    twoPointSkills.append(MoveDefensive);
    twoPointSkills.append(PassOffensive);
    twoPointSkills.append(PassDefensive);
    twoPointSkills.append(KickOffensive);
    twoPointSkills.append(KickDefensive);
    twoPointSkills.append(ChipOffensive);
    twoPointSkills.append(ChipDefensive);
    twoPointSkills.append(MarkOffensive);
    twoPointSkills.append(MarkDefensive);
    bool hasPass = false;
    int passCnt = 0;
    if(ball == -1)
    {
        error += "You should choose ball position!";
        return false;
    }
    for(int i = 0; i < agentsSize; i++)
    {
        if(agents[i].skill[0] == None)
        {
            error += "You should choose a skill for agent:"+QString::number(i + 1)+"!";
            return false;
        }
        if(agents[i].A == -1)
        {
            error += "You should choose Point A for agent:"+QString::number(i + 1)+"!";
            return false;
        }
        for(int j = 0; j < 4; j++)
        {
            if(twoPointSkills.contains(agents[i].skill[j]))
            {
                 if(agents[i].A == -1 || agents[i].B == -1)
                 {
                      error += "You should choose both points, for agent:"+QString::number(i + 1)+"!";
                      return false;
                 }
            }
        }
    }
    int passId = 0;
    int hasOneTouch = false;
    for(int i = 0; i < agentsSize; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            if(agents[i].skill[j] == PassOffensive ||
               agents[i].skill[j] == PassDefensive)
            {
                hasPass = true;
                passId = i;
                passCnt++;
            }
            if(agents[i].skill[j] == OneTouch ||
               agents[i].skill[j] == ReceivePass)
            {
                hasOneTouch = true;
            }
        }
    }
    if(hasPass && passCnt > 1)
    {
        error += "You should not have two pass skills!";
        return false;
    }
    bool hasReceive = false, hasReceiveT = false;
    int receiveId = 0;
    int tCnt = 0;
    if(hasPass)
    {
        for(int i = 0; i < agentsSize; i++)
        {
            hasReceiveT = false;
            for(int j = 0; j < 4; j++)
            {
                if(agents[i].skill[j] == ReceivePass ||
                   agents[i].skill[j] == OneTouch)
                {
                    if(j == 0)
                    {
                        hasReceive = true;
                        receiveId = i;
                    }
                    else if(!hasReceive && j != 1)
                    {
                         error += "Receive/One-touch should be in first"\
                                  " two priorities! Agent:"+QString::number(i + 1)+""\
                                  "Priority:"+QString::number(j)+"!";
                         return false;
                    }
                }

            }
            if(hasReceiveT)
                tCnt++;
        }
    }
    if(tCnt > 1)
    {
        error += "You can not have two or more agents for Receiving ball!";
        return false;
    }
    if(hasPass && !hasReceive)
    {
        error += "You should have a Receive or One-Touch skill!";
        return false;
    }
    if(hasPass)
    {
        if(agents[passId].B != agents[receiveId].A)
        {
            error += "Receive/One-Touch A point must be Pass B point!";
            return false;
        }

    }
    if(hasOneTouch && !hasPass)
    {
        error += "You should have a pass skill!";
        return false;
    }
    return true;
}

