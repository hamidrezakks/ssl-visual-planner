#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base.h"
#include "kkagentplanclass.h"
#include "kkplansql.h"

#include <QMainWindow>
#include <QPixmap>

#include <QRect>
#include <QPoint>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QDir>

#include <QLineEdit>

#include "playoff.h"

const QString DEFAULT_DIR_KEY("default_dir");

const QString settingAddress(QDir::currentPath()+"/data/setting.ini");

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum VPMode {
        kkPlayOn = 1,
        kkPlayOff = 2
    };

    VPMode currentVPMode;

    void initVPMode();

    explicit MainWindow(QWidget *parent = 0);

    int getIntByEnum(PEndPolicy tEnum);
    int getIntByEnum(PSkills tEnum);
    int getIntByEnum(PlannerMode tEnum);
    PEndPolicy getPolicyByIndex(int index);
    PSkills getPSkillByIndex(int index);

    void geometryIni();
    void resizeEvent(QResizeEvent *);
    void showArea(int id, QRect *rect, int size);
    void showBallArea(int id = -1);
    void showAgentArea(int id = -1);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void contextMenuEvent(QContextMenuEvent *event);
    void changeBallScroll();
    void createActions();
    void drawRobot(QPainter &painter, int x, int y, QString label, int agent, bool selected = false, bool blink = false);
    void drawRobots(QPainter &painter, int id = -1);

    QAction *passSkill[2], *moveSkill[2], *kickSkill[2],
            *chipSkill[2], *markSkill[2],
            *oneTouchSkill, *catchBallSkill,
            *receivePass, *shotSkill, *chipToGoalSkill;
    QAction *prevAgent, *nextAgent;
    QAction *prevPriority, *nextPriority;

    void enableContexAgent();
    void setSkillColor(QLabel **label, int tI, int agent, int priority, int tAOrB);
    void setSkillColors(int agent, int priority, int tAOrB);
    QString getSkillTextByEnum(PSkills skill);
    void setSkillText(QLabel **label, int tI);
    void setSkillTexts();

    int getArea(QRect *rect, int size, QPoint mPos);

    QRect bRect[6];
    bool bAreaSelected;
    int bSelectedArea, bTempArea;

    QRect aRect[10];
    bool aAreaSelected;
    int aSelectedArea[5][2], aTempArea[5][2], currentAgent, currentPriority, aOrB;

    QLabel *_1AgentLable[12];
    QLabel *_2AgentLable[24];
    QLabel *_3AgentLable[36];
    QLabel *_4AgentLable[48];
    QLabel *_5AgentLable[60];

    QLabel *agentLabel[15];

    void setQLabel(QWidget *widget, QLabel **label, QRect rect, int index = 0);
    void setQlabelInTabWidget(QWidget *widget, QLabel **label, QRect rect, int size = 1);
    void setAgentLabel(QWidget *widget, QRect rect, int size);

    kkLabelMode getPressedLabel(QLabel **label, int size);
    kkLabelMode getPressedLabel();

    void setLabelABText(QLabel **label, int size);
    void setLabelABText();

    PlannerMode mode;

    QPixmap *fieldPix;

    QTimer *timer;
    int timerCounter;
    bool blinkCurrentAgent;

    QList<kkAgentPlan> _1agentPlans;
    QList<kkAgentPlan> _2agentPlans;
    QList<kkAgentPlan> _3agentPlans;
    QList<kkAgentPlan> _4agentPlans;
    QList<kkAgentPlan> _5agentPlans;
    int agentPlanCount[5];

    kkAgentPlanClass *currentPlan;

    bool setPlan(PSkills skill);
    bool setPlanPoints();
    bool errorCheck;
    int errorCounter;

    kkPlanSQL *myPlanSql;
    QString currentSQL;
    bool rightClickEmpty;

    void insertPlanToStruct(kkAgentPlan &plan);
    void insertPlanToList(QList<kkAgentPlan> &list, int agentSize);

    void clearPlan(bool resetTab = true);
    int addBtnMode;

    void loadPlan(kkAgentPlan plan);
    void loadPlan(int planId, PlannerMode tMode);
    int currentTab;

    QString openFileDir, saveFileDir;
    QString POopenFileDir, POsaveFileDir;

    /*kkAgentPlan currentTempPlan;*/
    int lastPlan;
    PlannerMode lastMode;

    bool applyEdit();

    int getPlanSize(int agentSize = 1);
    bool tabChanged;

    kkAgentPlan unsavedPlan;
    bool unsavedPlanFlag;

    int currentId;

    void setBrief();

    ~MainWindow();

private:
    playoff *playOff;
    void POCreateActions();
    QAction *POPass, *POReceivePass, *POShotToGoal,
            *POChipToGoal, *POOneTouch, *POMove;

    int POCurrentPlan;
    bool POSpinInit;

private slots:
    void on_tabWidget_currentChanged(int index);

    void activePassO();
    void activePassD();
    void activeMoveO();
    void activeMoveD();
    void activeKickO();
    void activeKickD();
    void activeChipO();
    void activeChipD();
    void activeMarkO();
    void activeMarkD();
    void activeOneTouch();
    void activeCatchBall();
    void activeReceivePass();
    void activeShot();
    void activeChipToGoal();
    void activePrevAgent();
    void activeNextAgent();
    void activePrevPriority();
    void activeNextPriority();
    void activeA();
    void activeB();
    void emptySkill(int agent, int priority);

    void timerSlot();
    void on_addBtn_clicked();

    void on_ballPosCombo_currentIndexChanged(int index);

    void on_ballPossessionCombo_currentIndexChanged(int index);

    void on_endPolicyCombo_currentIndexChanged(int index);

    void on_endPolicyValCombo_currentIndexChanged(int index);

    void on_endPolicyValTextBox_editingFinished();

    void on_browseBtn_clicked();

    void on_saveBtn_clicked();

    void on_resetBtn_clicked();

    void on_spinBox_valueChanged(const QString &arg1);

    void on_commentTextBox_textChanged();

    void on_removeBtn_clicked();

    void on_actionPlayOn_triggered();

    void on_actionPlayOff_triggered();

    void on_comboBox_2_currentIndexChanged(int index);

    void on_POBtn_pressed();

    void on_POBtn_released();

    void on_POTBPosX_returnPressed();

    void on_POTBPosY_returnPressed();

    void on_POTBPosAng_returnPressed();

    void on_POTBPosTol_returnPressed();

    /* Play off */
    void POActivePass();
    void POActiveReceivePass();
    void POActiveShotToGoal();
    void POActiveChipToGoal();
    void POActiveOneTouch();
    void POActiveMove();
    /* play off */

    void on_newBtn_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
