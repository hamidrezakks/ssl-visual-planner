#ifndef AGENTPLANCLASS_H
#define AGENTPLANCLASS_H

#include "base.h"

#include <QDebug>
#include <QString>

class AgentPlanClass {
private:
    Agent agents[5];
    EndPolicy endMode;
    int endPolicy;
    int ball;
    int agentsSize;
    int planId;
    int possession;
    QString tags;

public:
    AgentPlanClass(int agentSize = 1, int id = 0);
    void clear(int agentSize = 1);
    bool setAgentSize(int agentSize);
    bool setAgentSkill(int agent, int priority, int a, int b, Skills tSkill);
    bool setPlanPoints(int agent, int a, int b);
    bool setBallPos(int ballPos);
    void setEndPolicy(EndPolicy mode, int policy);
    bool setPlanId(int id);
    bool setPossession(int poss);
    void setTags(QString _tags);

    int getAgentSize();
    int getAgentA(int agent);
    int getAgentB(int agent);
    Skills getAgentSkill(int agent, int priority);
    int getPrioritySize(int agent);
    int getBallPos();
    EndPolicy getEndPolicyMode();
    int getEndPolicyValue();
    int getPossession();
    QString getTags();
    void loadPlanByStruct(AgentPlan _plan);

    int getId();
    bool isExecutable(QString &error);
};

#endif // AGENTPLANCLASS_H
