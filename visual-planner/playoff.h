#ifndef PLAYOFF_H
#define PLAYOFF_H

#include "base.h"
#include "kkpoplansql.h"

#include <QLabel>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <math.h>
#include <QWidget>
#include <QLineEdit>
#include <QMouseEvent>
#include <QStatusBar>
#include <QRubberBand>
#include <QComboBox>

#include <QDebug>

struct kkTimeAndIndex {
    long time;
    int index;
    int agent;
    POffSkills skill;
};

class playoff : public QWidget
{
    Q_OBJECT
public:
    explicit playoff(QWidget *parent = 0);
    ~playoff();

    enum toolMode {
        TMOVE = 1,
        TDELETE = 2,
        TSELECT = 3,
        TCOPY = 4,
        TPASTE = 5
    };

    void setLabel(QLabel *tLabel);
    void setWidget(QWidget *tWidget);
    void setLineEdits(QLineEdit *_posX, QLineEdit *_posY, QLineEdit *_posAng, QLineEdit *_posTol);
    void setStatusBar(QStatusBar *_statusBar);
    void setAgentSizeCB(QComboBox *_comboBox);

    void mousePressed(QMouseEvent *event, QPoint tempPos);
    void mouseMoved(QMouseEvent *event, QPoint tempPos);
    void mouseReleased(QMouseEvent *event, QPoint tempPos);

    void draw();

    void reset();
    void placeRobot(QPoint pos, int tAgent, bool tTemp = false);
    void moveRobot(QPoint pos, int tAgent, int tIndex, bool tTemp = false);
    void moveSelectedRobots(QList<agentAndIndex> &tList, QPoint tOffset);
    void turnRobotAng(QPoint pos, int tAgent, int tIndex);
    void removeRobot(int tAgent, int tIndex);
    void removeSelectedRobots(QList<agentAndIndex> &tList);

    bool containPoint(QPoint point, QPoint pos, int r);
    robotAttr getRobot(QPoint pos, bool passExp = false, QPoint *_base = new QPoint );
    void setDisplayMode(int tMode, bool noDraw = false);
    int getDisplayMode();
    void setCurrentAgent(int tAgent);
    int getCurrentAgent();
    void setAgentSize(int tSize);
    int getAgentSize();
    int getPlanSize();

    void setShowAllFlag(bool tFlag);

    robotGeom getRobotGeom(int tAgent, int tIndex);
    void setGeomX(int tX);
    void setGeomY(int tY);
    void setGeomAngle(double tAng);
    void setGeomTolerance(int tTol);
    int getRobotSize(int tAgent);
    void setCurrentSkillNum(int tNum);
    int getCurrentSkillNum();
    void setCurrentSkillSize(int tSize);
    int getCurrentSkillSize();

    void setSkill(POffSkills tSkill, int targetAgent = -1, int targetIndex = -1);
    POffSkills getSkill(int &targetAgent, int &targetIndex);
    POffSkills getSkill();
    POffSkills getSkill(int tSkillNum);
    POffSkills getSkill(int tSkillNum, int &targetAgent, int &targetIndex);

    //////
    void POSetSelectedDisplayLabel(int index);
    void POSetSelectedCurrentAgentLabel(int index);
    void POSetSelectedToolLabel(toolMode tTMode);
    void POSetSelectedModeLabel(POMODE tPOMode);
    void POSetCheckedIA();
    //////
    void apply(int _id);

    void clean();

    void backupP();
    void restoreP();
    bool backup, restore;
    void cleanPlans();
    void removePlan(int index);
private:
    QLabel *fieldLabel;
    QPixmap *fieldPix;
    QWidget *POWidget;
    QStatusBar *statusBar;
    QComboBox *agentSizeCB;

    QLineEdit *POTBPosX, *POTBPosY, *POTBPosAng, *POTBPosTol;

    toolMode currentTool;

    POMODE currentPOMode;

    QRubberBand *PORubberBand;
    bool moveRubberBand;
    bool validRubberBand;
    bool rbStarted;
    QPoint rbBeginPoint;
    QPoint rbEndPoint;
    QPoint baseMove;
    QPoint rbMoveBase, rbMoveChange;

    QList<agentAndIndex> getSelectedAgents(int filter = -1);

    QList<agentAndIndex> toMoveAgentList;
    QList<QPoint> toMoveAgentFirstPointList;
    void fillFirstPointsList(QList<agentAndIndex> &tList);

    void drawRobot(QPainter &painter, int x, int y, QString label, int agent, bool selected = true, bool blink = false);
    void drawRobots(QPainter &painter, int tRobotIndex, bool selected = true);

    QList<playOffRobot> robots[6];

    QList<playOffRobot> unsavedPlan[6];
    planMData unsavedMPlan;

    robotAttr currentRobot;
    int displayMode;

    int currentAgent;

    bool showAll;
    int agentSize;
    int currentSkillNum;

    bool passFlag;
    ////////
    robotAttr POCurrentRobot;
    robotAttr passReceiver;
    bool POFieldSelected;
    QLabel *PODisplayModeLabel[8];
    QLabel *POCurrentAgentLabel[7];
    QLabel *POSkills[3];
    QLabel *POTools[6];
    QLabel *POMode[4];
    QLineEdit *POTiming[6];
    QLabel *POTimingLable[6];
    QLabel *POPassTarget[3];
    QLabel *POReceiveIA[3];
    void POinitLables();

    void insertSkillData(int _row, int _col, int data, bool noupdate);

    void POOpenSkill(int index, bool temp = true);

    void POSetSkill(int tAgent, int tIndex, int tSkillNum, POffSkills tSkill);

    void POPaintSkill();


    kkPOPlanSQL *myPlan;
    //copy & paste
    void POCopy(int filter);
    void POPaste();

    QList<playOffRobot> copyRobotList[6];

    QPoint currentBase;

    //sql side
public:
    POInitPos getInitPos();

    void disableSQL();
    void enableSQL();

    void savePlan(QString directory);
    int loadPlan(QString directory);

    void loadPOPlan(int index);

public slots:

    void POLineEdit0_0(QString str = "", bool noupdate = false);
    void POLineEdit0_1(QString str = "", bool noupdate = false);

    void POLineEdit1_0(QString str = "", bool noupdate = false);
    void POLineEdit1_1(QString str = "", bool noupdate = false);

    void POLineEdit2_0(QString str = "", bool noupdate = false);
    void POLineEdit2_1(QString str = "", bool noupdate = false);

};

#endif // PLAYOFF_H
