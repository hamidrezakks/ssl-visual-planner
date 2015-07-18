#ifndef KKAGENTPLANCLASS_H
#define KKAGENTPLANCLASS_H

#include "base.h"

#include <QString>

class kkAgentPlanClass {
private:
    kkAgent agents[5];
    PEndPolicy endMode;
    int endPolicy;
    int ball;
    int agentsSize;
    int planId;
    int possession;
    QString comment;

public:
    kkAgentPlanClass(int agentSize = 1, int id = 0);
    void clear(int agentSize = 1);
    bool setAgentSize(int agentSize);
    bool setAgentSkill(int agent, int priority, int a, int b, PSkills tSkill);
    bool setPlanPoints(int agent, int a, int b);
    bool setBallPos(int ballPos);
    void setEndPolicy(PEndPolicy mode, int policy);
    bool setPlanId(int id);
    bool setPossession(int poss);
    void setComment(QString cmt);

    int getAgentSize();
    int getAgentA(int agent);
    int getAgentB(int agent);
    PSkills getAgentSkill(int agent, int priority);
    int getPrioritySize(int agent);
    int getBallPos();
    PEndPolicy getEndPolicyMode();
    int getEndPolicyValue();
    int getPossession();
    QString getComment();

    int getId();
    bool isExecutable(QString &error);
};

#endif // KKAGENTPLANCLASS_H
