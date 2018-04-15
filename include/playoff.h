#ifndef PLAYOFF_H
#define PLAYOFF_H


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
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include <base.h>
#include "playoffplansql.h"
#include "ul.h"

struct TimeAndIndex {
    long time;
    int index;
    int agent;
};

class playoff : public QWidget
{
    Q_OBJECT
public:
    explicit playoff(QWidget *parent = 0);
    ~playoff();

    enum toolMode {
        TMOVE   = 1,
        TDELETE = 2,
        TSELECT = 3,
        TCOPY   = 4,
        TPASTE  = 5
    };

    void setLabel(QLabel *tLabel);
    void setWidget(QWidget *tWidget);
    void setLineEdits(QLineEdit *_posX,
                      QLineEdit *_posY,
                      QLineEdit *_posAng,
                      QLineEdit *_posTol);

    void setStatusBar(QStatusBar *_statusBar);
    void setAgentSizeCB(QComboBox *_comboBox);

    void setMaxEffectiveCB(QComboBox * _maxEffective);
    void setMinNeededCB(QComboBox * _minNeeded);
    void setLastDistDSB(QDoubleSpinBox * _lastDistDSB);
    void setChanceSB(QSpinBox * _chanceSB);

    void mousePressed(QMouseEvent *event, QPoint tempPos);
    void mouseMoved(QMouseEvent *event, QPoint tempPos);
    void mouseReleased(QMouseEvent *event, QPoint tempPos);

    void mousePressedOnField(QMouseEvent* _event, QPoint _pos);
    void mousePressedOnTabWidget(QMouseEvent* _event);

    void draw();

    void reset();
    void placeRobot(QPoint pos, int tAgent, bool tTemp = false);
    void moveRobot(QPoint pos, int tAgent, int tIndex, bool tTemp = false);
    void moveSelectedRobots(QList<AgentAndIndex> &tList, QPoint tOffset);
    void turnRobotAng(QPoint pos, int tAgent, int tIndex);
    void removeRobot(int tAgent, int tIndex);
    void removeSelectedRobots(QList<AgentAndIndex> &tList);

    bool containPoint(QPoint point, QPoint pos, int r);
    RobotAttr getRobot(QPoint pos, bool passExp = false, QPoint *_base = new QPoint );
    void setDisplayMode(int tMode, bool noDraw = false);
    int getDisplayMode();
    void setCurrentAgent(int tAgent);
    int getCurrentAgent();
    void setAgentSize(int tSize);
    int getAgentSize();
    int getPlanSize();

    void setShowAllFlag(bool tFlag);

    RobotGeometry getRobotGeom(int tAgent, int tIndex);
    void setGeomX(int tX);
    void setGeomY(int tY);
    void setGeomAngle(double tAng);
    void setGeomTolerance(int tTol);
    void setCurrentSkillNum(int tNum);
    int getCurrentSkillNum();
    void setCurrentSkillSize(int tSize);
    int getCurrentSkillSize();

    void setSkill(PlayOffSkills tSkill, int targetAgent = -1, int targetIndex = -1);

    PlayOffSkills getSkill(int tSkillNum);
    PlayOffSkills getSkill(int tSkillNum, int &targetAgent, int &targetIndex);

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

    void setTags(QString str);
    inline void setChance (unsigned int _chance) {
        chance = _chance;
    }
    inline void setLastDist(double _lastDist) {
        lastDist = _lastDist;
    }
    inline void setMaxEffective(double _MaxEff) {
        maxEff = static_cast<int>(_MaxEff);
    }
    inline void setMinNeeded(double _MinNeed) {
        minNeed= static_cast<int>(_MinNeed);
    }
     playOffPlanSQL *myPlan;
private:
    QLabel *fieldLabel;
    QPixmap *fieldPix;
    QWidget *POWidget;
    QStatusBar *statusBar;
    QComboBox *agentSizeCB;

    QComboBox *maxEffectiveCB, *minNeededCB;

    QDoubleSpinBox *lastDistDSB;

    QSpinBox *chanceSB;

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

    QList<AgentAndIndex> getSelectedAgents(int filter = -1);

    QList<AgentAndIndex> toMoveAgentList;
    QList<QPoint> toMoveAgentFirstPointList;
    void fillFirstPointsList(QList<AgentAndIndex> &tList);

    void drawRobot(QPainter &painter, int x, int y, QString label, int agent, bool selected = true, bool blink = false);
    void drawRobots(QPainter &painter, int tRobotIndex, bool selected = true);

    QList<PlayOffRobot> robots[_MAX_ROBOT_COUNT];

    QList<PlayOffRobot> unsavedPlan[_MAX_ROBOT_COUNT];
    planMData unsavedMPlan;

    RobotAttr currentRobot;
    int displayMode;

    int currentAgent;

    bool showAll;
    int agentSize;

    int maxEff, minNeed;

    bool passFlag;
    ////////
    RobotAttr POCurrentRobot;
    RobotAttr passReceiver;
    bool POFieldSelected;
    QLabel *PODisplayModeLabel[_MAX_ROBOT_COUNT + 2];
    QLabel *POCurrentAgentLabel[_MAX_ROBOT_COUNT + 1];
    QLabel *POSkills[3];
    QLabel *POTools[6];
    QLabel *POMode[4];
    QLineEdit *POTiming[6];
    QLabel *POTimingLable[6];
    QLabel *POPassTarget[3];
    QLabel *POReceiveIA[3];
    void POinitLables();
    QRect getRobotAngRect(const PlayOffRobot &_robot);

    void insertSkillData(int _row, int _col, int data, bool noupdate);

    void POOpenSkill(int index, bool temp = true);

    void POPaintSkill();

    void writeJSON( QJsonObject &json, QString dir) const;
    void writePlanJSON( QJsonObject &json, int index) const;
    void writePosJSON( QJsonObject &json, const QList<PlayOffRobot> &index) const;
    void writeSkillJSON(QJsonObject &json, const PlayOffRobot &index) const;

    QString getStrFromPlayOffSkillEnum(PlayOffSkills _enum) const;
    PlayOffSkills getPlayoffSkillEnumFromStr(QString _skillName) const;

    Vector2D convertPos(Vector2I _input) const;
    Vector2I convertPosInverse(Vector2D _input) const;

    //playOffPlanSQL *myPlan;      we added this comment

    //copy & paste
    void POCopy(int filter);
    void POPaste();

    QList<PlayOffRobot> copyRobotList[_MAX_ROBOT_COUNT];

    QPoint currentBase;

    int currentId;

    QString tags;
    unsigned int chance;
    double lastDist;

    //sql side
public:
    POInitPos getInitPos();

    void disableSQL();
    void enableSQL();

    void savePlan(QString directory);
    int loadPlan(QString directory);

    void choosePlan(int index);

        /*Developed by mohammad Raziei*/
    bool savePlanJson(QString directory);
    int loadPlanJson(QString directory);
    void readJSON(const QJsonObject& playBook);


public slots:

    void POLineEdit0_0(QString str = "", bool noupdate = false);
    void POLineEdit0_1(QString str = "", bool noupdate = false);

    void POLineEdit1_0(QString str = "", bool noupdate = false);
    void POLineEdit1_1(QString str = "", bool noupdate = false);

    void POLineEdit2_0(QString str = "", bool noupdate = false);
    void POLineEdit2_1(QString str = "", bool noupdate = false);


signals:
    void updateTags(QString str);


};


#endif // PLAYOFF_H
