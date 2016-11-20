#ifndef BASE
#define BASE

#define BASE

#include <QString>
#include <QRect>
#include <QPoint>
#include <QResource>

enum PlannerMode{
    BALL = 0,
    _1AGENT = 1,
    _2AGENT = 2,
    _3AGENT = 3,
    _4AGENT = 4,
    _5AGENT = 5
};

struct LabelMode {
    int agent;
    int priority;
    int part;
};

struct Robot {
    int x, y;
    QString label;
    int agent;
    int aOrB;
};

enum Skills {
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
};

enum EndPolicy {
    NoPolicy = 0,
    Cycle = 1,
    ExactAgent = 2,
    AllAgents = 3,
    ExactDisturb = 4
};

struct Agent {
    int A, B;
    Skills skill[4];
    int pSize;
};

struct AgentPlan {
    int planId;
    int ball;
    EndPolicy endMode;
    int endPolicy;
    int possession;
    int agentsSize;
    QString tags;
    Agent agents[5];
};

struct PlayOnPlan {
    int planId;
    int ball;
    int agentSize;
    QString tags;
};

#define _RobotAngRad 25
#define _MAX_ROBOT_COUNT 6
#define _CENTRALWIDGET_V_OFFSET 25

enum PlayOffSkills {
    NoSkill = 0,
    PassSkill = 1,
    ReceivePassSkill = 2,
    ShotToGoalSkill = 3,
    ChipToGoalSkill = 4,
    OneTouchSkill = 5,
    MoveSkill = 6,
    ReceivePassIASkill = 7
};

struct AgentAndIndex {
    int agent;
    int index;
};

struct RobotAttr {
    int index;
    int agent;
    int skillNum;
    bool isAng;
};

struct PlayOffRobot {
    int x, y;
    QString label;
    int agent;
    double angle;
    QRect angRect;
    int tolerance;
    bool temp;
    RobotAttr target;
    PlayOffSkills skill[3];
    int skillData[3][2];
    int skillSize;
    bool IAMode[3];
};

struct RobotGeometry {
    QPoint pos;
    double angle;
    int tolerance;
};

enum POMODE {
    KICKOFF = 1,
    DIRECT = 2,
    INDIRECT = 3
};

struct POInitPos {
    int ballX, ballY;
    int AgentX[6], AgentY[6];
};


struct Vector2D{
    Vector2D(const double _x, const double _y){
        x = _x;
        y = _y;
    }

    double x;
    double y;
};

struct Vector2I{
    Vector2I(const int _x, const int _y){
        x = _x;
        y = _y;
    }

    int x;
    int y;
};

#define API_VERSION 1.2

#endif // BASE

