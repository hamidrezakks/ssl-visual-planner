#ifndef FORCESTART_H
#define FORCESTART_H

#include "masterplay.h"

#ifndef KK_PLAYON
class CForceStart : public CMasterPlay{
public:
    CForceStart();
    ~CForceStart();
    void execute_0();
    void execute_1();
    void execute_2();
    void execute_3();
    void execute_4();
    void execute_5();
    void execute_6();
    void init(QList <int> _agents , QMap<QString , EditData*> *_editData);

    void onePlayMakeOtherPositions();
private:
    void reset();
};
#else
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QMessageBox>

struct kkSpeeds {
    double pass;
    double kick;
    double chip;
    double shot;
    double ChipToGoal;
};

/*enum PSkills {
    None = 0,
    MoveOffensive = 1,
    MoveDefensive = 2,
    PassOffensive = 3,
    PassDefensive = 4,
    KickOffensive = 5,
    KickDefensive = 6,
    ChipOffensive = 7,
    ChipDefensive = 8,
    MarkOffensive = 9,
    MarkDefensive = 10,
    OneTouch = 11,
    CatchBall = 12,
    ReceivePass = 13,
    Shot = 14,
    ChipToGoal = 15
};*/

enum PEndPolicy {
    Cycle = 1,
    ExactAgent = 2,
    AllAgents = 3,
    ExactDisturb = 4
};

struct kkAgent {
    int A, B;
    PSkills skill[4];
    int pSize;
};

struct kkAgentPlan {
    int planId;
    int ball;
    PEndPolicy endMode;
    int endPolicy;
    int possession;
    int agentsSize;
    //QString comment;
    kkAgent agents[5];
};


struct kk2Vector2D {
    Vector2D vec1;
    Vector2D vec2;
};

struct kk2Segment2D {
    Segment2D seg1;
    Segment2D seg2;
};

enum ballOwner {
    BOur,
    BThem,
    BNone
};

class CForceStart : public CMasterPlay{
public:
    CForceStart();
    ~CForceStart();

    void execute_0();
    void execute_1();
    void execute_2();
    void execute_3();
    void execute_4();
    void execute_5();
    void execute_6();
    void init(QList <int> _agents , QMap<QString , EditData*> *_editData);

    void onePlayMakeOtherPositions();

    void setDecidePlan(bool _check);

    bool abdollaw;
private:
    void reset();

    Rect2D kkARegion[10];
    Rect2D kkBRegion[6];

    bool  executed;
    bool isBallInTheirKick,isBallInOurKick;
    bool theirFlag,ourFlag,noneFlag;
    long theirTime,ourTime,noneTime;
    ballOwner Owner;


    QSqlDatabase kkPlanSQL;

    int kkGetIntByEnum(PEndPolicy tEnum);
    int kkGetIntByEnum(PSkills tEnum);
    PEndPolicy kkGetPolicyByIndex(int index);
    PSkills kkGetPSkillByIndex(int index);

    int rankByBallDist(int rank,Vector2D arr[][3]);

    void kkDrawRect();
    int kkGetArea(Rect2D *rect, int size, Vector2D point);
    bool kkIsInRect(Rect2D rect, Vector2D point, double offset = 0.0);

    void kkInsertToList(QList<kkAgentPlan> **list, QSqlQuery query, int agentSize);
    void kkLoadPlan(QList<kkAgentPlan> **list, int agentSize);

    QList<kkAgentPlan> kkFillQList(QList<kkAgentPlan> *list);
    void kkGetBestPlan(int planBallPos, bool _ballPoss, int agentSize = 1);

    //bool kkIsPlanMatch(QList<kkRobot> aList, kkAgentPlan plan);

    QList<kkAgentPlan> *kk1AgentPlan[6];
    QList<kkAgentPlan> *kk2AgentPlan[6];
    QList<kkAgentPlan> *kk3AgentPlan[6];
    QList<kkAgentPlan> *kk4AgentPlan[6];
    QList<kkAgentPlan> *kk5AgentPlan[6];

    QList<kkRobot> kkAgentList;
    int kkInsertActiveAgentsToList();

    kkAgentPlan kkCurrentPlan;
    kkAgentPlan kkNextPlan;

    kkAgentPlan kkChooseBetweenInappropiatePlan(QList<kkAgentPlan> tList);

    int kkAgentsId[5];
    void kkAssignId();
    void kkNewAssignId();

    ///
    int cntRobotInRegion(Rect2D region);
    ///


    int kkGetIndex(kkValue &value, int cIndex);
    void kkGetCostRec(double costArr[][5], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId = 0);
    void kkGetCostRec2(double costBrr[5], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId = 0);

    Vector2D kkBestMovePos;
    QList<Circle2D> kkMoveObstacle;
    int kkAgentToMove;
    bool kkCheckMovePos(double x, double y);
    void kkAmirSearch(double x, double y, short id, double step);
    void kkAmirSearch(double x, double y, short id, Polygon2D &tPoly, double step);
    void kkAmirNewSearch(double x, double y, short id, double step);
    void kkAmirNewSearch(double x, double y, short id, Polygon2D &tPoly, double step);
    void kkInsertObstacleToList(int rectIndex);
    Vector2D kkFindBestPosForMove(int fromAgent, int regionId, bool offensive);

    QList<Circle2D> kkPassObstacle;
    bool kkHasOverlap(Segment2D seg1, Segment2D seg2);
    kk2Segment2D kkMergeTwoSegments(kk2Segment2D seg1, kk2Segment2D seg2);
    void kkMergeSegments(QList<kk2Segment2D> &segList);
    void kkSortSegmentsByAngle(QList<Segment2D> &segList, Vector2D tBallPos);
    void kkInvertSegments(QList<kk2Segment2D> &segList, Segment2D tSeg, QList<kk2Vector2D> &vecList);
    kk2Segment2D kkBlockedByRobots(Circle2D tRobot, Vector2D tOrigin, Line2D tLine);
    bool kkFindBestRegionsToPass(int regionIndex, double reachTime, Polygon2D &tPoly, int tException = -1, bool regionException = 0);
    bool kkFindBestRegionsToKick(Polygon2D &tPoly, Vector2D passPos);
    void kkInsertPassObstacleToList(double reachTime, Polygon2D mPoly, Segment2D lLine, Segment2D rLine, int tException = -1);
    void kkSortObstacleByAngle(Vector2D tBallPos);
    Vector2D kkFindBestPosForPass(int fromAgent, int regionId, bool offensive, double reachTime, int tException = -1);
    Vector2D kkNewFindBestPosForPass(int fromAgent, int regionId,int regionIdPasser, bool offensive, double reachTime, int tException = -1);
    bool isBallOurs();

    bool kkIsballOurs();

    bool kkCheckAgentSize(int agentSize);

    bool kkCheckIntersectWithAgents(Segment2D tSeg);
    bool kkIsPathToOppGoaliClear(Vector2D tPoint);
    bool kkIsPathToOppGoaliClear();

    bool kkIsPathFromAToBClear(Vector2D tA, Vector2D tB, double tTolerance = 0.20);

    void kkAssignTasks();
    void kkNewAssignTasks();

    bool kkHasReceiveOrOneTouch(int tAgent);
    void kkChooseBestTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill[], int tA, int tB);
    void kkNewChooseBestTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill[], int tA, int tB);

    void kkAssignTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill, int tA, int tB);
    void kkNewAssignTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill, int tA, int tB);

    Vector2D kkPointToPass;
    int kkPassReceiverAgent;

    void kkMainPlanner(int tAgentSize);

    bool kkDoneTaskAgent[5];
    void kkCheckEndPolicy();
    bool kkCheckCycleTimeOut(long tTime);
    long kkExecutedCycle;

    Segment2D kkGetToleranceSeg(Vector2D tAVec, Vector2D tBVec, double tTol);
    bool kkIsAgentTaskDone(int tAgent);
    bool kkIsAgentTaskFailed(int tAgent);
    int kkTaskDonePassCnt;
    int kkTaskDoneKickCnt[5];
    bool kkTaskDoneOneTouchEnterFlag;
    bool kkTaskDoneOneTouchEnterFlag1;
    double kkTaskDoneRadiusTolerance;

    bool kkIsballArriveToPoint(int vel, Vector2D target);

    CRolePlayOn *agentSkill[5];
    bool kkActiveAgent[5];
    QList<CAgent*> kkActiveAgents;
    void kkFixPasserPos();
    Vector2D kkPasserPos;

    void checkOneTouchTask();//clean

    int kkDefaultCycle;

    void kkMoveManager(CAgent *_agent, CRolePlayOn &);

    kkSpeeds currentSpeeds;

    long kkLastTime;

    bool decidePlan;
    int decideCount;
    int ballOursCounter;
    long kkLastTimeDef;
    int lastAgentCount;

private:
    bool ATFballEnteredKickerFlag;
    bool ATFballEnteredKickerChipFlag;
    bool ATFpassReceivedFlag;
    bool FPPballEnteredKickerFlag;

    int kkOppBallPossCnt;
    int kkOurBallPossCnt;
    bool lastBallPossMode;

    QList<int> kkMarkList;

    CSkillGotoPointAvoid *kkMarkGPA[3];
    Vector2D kkMarkPoints[3];
    void kkManToManMark(int id1);
    void kkManToManMarkTwo(int ids[3]);
    void kkManToManMarkThree(int ids[3]);
};
#endif

#endif // FORCESTART_H
