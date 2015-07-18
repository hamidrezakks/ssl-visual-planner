#ifndef PLAYOFF_H
#define PLAYOFF_H

#include "masterplay.h"

#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QMessageBox>

#define ClassProperty(skill,type,name,local,chflag) \
        public: inline type get##name() {return local;} \
        public: inline skill* set##name(type val) {local = val;chflag = true;return this;} \
        protected: type local


struct POPlanStruct {
    QList<playOffRobot> AgentPlan[6];
    POMODE planMode;
    int agentSize;
    POInitPos initPos;
};

struct distAndId {
    double dist;
    int id;
};


struct POOwnerReceive {
    int ballOwnerAgent;
    int ballOwnerIndex;
    int receiveAgent;
    int receiveIndex;
    POffSkills skill;
};

struct kkTimeAndIndex {
    long time;
    int index;
    int agent;
    POffSkills skill;
};

class CRolePlayOff{
private:
    bool updated;
    QList<int> kkMarkAgents;

public:

    CRolePlayOff();

    void execute();



    CSkillKick *kickSkill;
    CSkillGotoPoint *gotoPointSkill;
    CSkillReceivePass *receivePassSkill;
    CSkillKickOneTouch *oneTouchSkill;
    CSkillGotoPointAvoid *gotoPointAvoidSkill;

    ClassProperty(CRolePlayOff, int, AgentID, agentID, updated);
    ClassProperty(CRolePlayOff, CAgent*, Agent, agent, updated);
    ClassProperty(CRolePlayOff, kkSkill, SelectedSkill, selectedSkill, updated);
    ClassProperty(CRolePlayOff, Vector2D, Target, target, updated);
    ClassProperty(CRolePlayOff, Vector2D, TargetDir, targetDir, updated);
    ClassProperty(CRolePlayOff, Vector2D, TargetVel, targetVel, updated);
    ClassProperty(CRolePlayOff, bool, AvoidPenaltyArea, avoidPenaltyArea, updated);
    ClassProperty(CRolePlayOff, bool, AvoidCenterCircle, avoidCenterCircle, updated);
    ClassProperty(CRolePlayOff, double, Tolerance, tolerance, updated);
    ClassProperty(CRolePlayOff, bool, Chip, chip, updated);
    ClassProperty(CRolePlayOff, int, KickSpeed, kickSpeed, updated);
    ClassProperty(CRolePlayOff, bool, Slow, slow, updated);
    ClassProperty(CRolePlayOff, float, ReceiveRadius, receiveRadius, updated);
    ClassProperty(CRolePlayOff, Vector2D, WaitPos, waitPos, updated);
    ClassProperty(CRolePlayOff, double, MaxVelocity, maxVelocity, updated);
    ClassProperty(CRolePlayOff, bool, Intercept, intercept, updated);
public:
    void setUpdated(bool _updated);
    bool getUpdated();
protected:
};


class CPlayOff : public CMasterPlay {
public:
    CPlayOff();
    ~CPlayOff();

    void execute_0();
    void execute_1();
    void execute_2();
    void execute_3();
    void execute_4();
    void execute_5();
    void execute_6();
    void init(QList <int> _agents , QMap<QString , EditData*> *_editData);

    bool firstTime;


private:

    void globalExecute(int agentCnt);

    ///////////////////////////////////////////////////////////////
    /////////////////////////MAHI POSITIONING//////////////////////
    ///////////////////////////////////////////////////////////////
    void findPasser();
    void findReceiver();
    void findBestPosForPass(Vector2D _pos, Circle2D _circle, bool forceChip, int ID);
    void findFirstValidPos(Vector2D Position, double escapeRad, int ID);
    Vector2D findValidPos(Vector2D Position, double escapeRad, int ID);
    void mahiInsertObstacleToList(Circle2D circle, int ID);
    bool isPathClear(Vector2D _pos1, Vector2D _pos2, double _radius, double treshold);



    void posReset();
    void posExecute();
    void setAgentSize(int _agentSize);
    bool isPlanEnd();
    bool isPassDone(int mahiAgent);

    positioningAgent positionAgent[6];
    QList<passerStruct> passStructList;
    QList<receiverStruct> receiverStructList;
    QList<passAndReceive> passAndReceiveList;

    bool kkTaskDoneOneTouchEnterFlag;
    bool kkTaskDoneOneTouchEnterFlag1;
    int agentSize;
    Vector2D pos4Pass;
    bool doChip;
    bool ballEnteredKickerFlag;
    bool ballEnteredKickerChipFlag;
    bool passReceivedFlag;
    bool isPassDoneflag;
    bool isFirstTime[6];

    /////////////////////////////////////////////////////////////////
    ////////////////////////////////////////KK SQL & MATCHIN'////////
    /////////////////////////////////////////////////////////////////
    void setPlanDir(QString directory);
    void loadSQL();
    int loadPlan();
    int getPlanSize();
    Vector2D convertPos(int _x, int _y);
    void loadEachPlan(POPlanStruct &_plan, QString _name);
    bool loadSQLtoStruct(QSqlQuery _query, int _rIndex, playOffRobot &temp);
    void getMatchedPlan(POMODE _mode, int agentSize, bool _rand = true);
    void getPassTimeline(POPlanStruct &tCurrentPlan, QList<POOwnerReceive> &tList);
    void copyStructToStruct(POPlanStruct &empty,const POPlanStruct &full);


    QList<POPlanStruct> planListKickOff;
    QList<POPlanStruct> planListDirect;
    QList<POPlanStruct> planListIndirect;
    QList<POOwnerReceive> ownerReceiveList;

    QString directory;
    void reset();
    QSqlDatabase kkPOPlanSQL;
    double radLimit;
    /////////////////////////////////////////////////////////////////////
    /////////////////////////MAHI PLANNER////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    void mainPlanner(int tAgentSize);
    POMODE getPlayOffMode();
    void assignID();
    int insertActiveAgentsToList();
    void getCostRec(double costArr[][6], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId = 0);
    int kkGetIndex(kkValue &value, int cIndex);
    void assignTasks();
    void fillRolesProperties();
    void findPasserIndex();
    void initPosition(QList<positioningArg> _posArg[]);
    void assignTask(int agentID,POffSkills agentSkill);
    bool chipOrNot(int passerID,int ReceiverID,int ReceiverState);
    bool kkIsPathToOppGoaliClear(Vector2D tPoint);
    bool kkCheckIntersectWithAgents(Segment2D tSeg);
    Vector2D getGoalTarget(int shoterID,int shoterState);
    double getMaxVel(int agentID,int agentState);
    Vector2D getMoveTarget(int agentID,int agentState);
    void checkEndState();
    bool isTaskDone(int agentID);
    bool isKickDone(CAgent* _agent, int agentID);
    bool isReceiveDone(CAgent* _agent);
    bool isMoveDone(int agentID);
    bool isPassInCurrentState();
    bool isReceiveInCurrentState();
    void findCurrentPassReceiver();
    void terminateReceiverTasks();
    void passManager();
    bool isBallMoved();


    long lastDecideTime;
    Vector2D lastBallPos;
    bool decidePlan;
    int kkAgentsID[6];
    QList<positioningArg> positionArg[6];

    POPlanStruct currentPlan;
    QList<CAgent*> activeAgents;
    QList<kkRobot> agentList;
    CRolePlayOff *roleAgent[6];

    int lastAgentCount;
    bool isBallNearRobot;
    int firstPasserID;
    /////////////////////////////////////////
    /////////////////////////////////////////
    /////////////////////////////////////////

};


#endif // CPLAYOFF_H
