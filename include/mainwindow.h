#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <base.h>
#include <agentplanclass.h>
#include <playonplansql.h>

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
#include "playon.h"
#include "tags.h"

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
        PlayOn = 1,
        PlayOff = 2
    };

    VPMode currentVPMode;

    void initVPMode();

    explicit MainWindow(QWidget *parent = 0);

    void geometryIni();
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void contextMenuEvent(QContextMenuEvent *event);

    ~MainWindow();

private:
    playon *playOn;
    void playOnCreateActions();
    QAction *playOnPassSkill[2],
            *playOnMoveSkill[2],
            *playOnKickSkill[2],
            *playOnChipSkill[2],
            *playOnMarkSkill[2],
            *playOnOneTouchSkill,
            *playOnCatchBallSkill,
            *playOnReceivePass,
            *playOnShotSkill,
            *playOnChipToGoalSkill;

    QAction *playOnPrevAgent,
            *playOnNextAgent;

    QAction *playOnPrevPriority,
            *playOnNextPriority;

    QString playOnOpenFileDir,
            playOnSaveFileDir;

    int playOnCurrentPlan;
    bool playOnNew;

    playoff *playOff;
    void playOffCreateActions();
    QAction *playOffPass,
            *playOffReceivePass,
            *playOffShotToGoal,
            *playOffChipToGoal,
            *playOffOneTouch,
            *playOffMove;

    QAction *afterlifeDefense,
            *afterlifeSupport,
            *afterlifePosition,
            *afterlifeGoalie,
            *afterlifeMark;

    QString playOffopenFileDir,
            playOffsaveFileDir;

    int playOffCurrentPlan;
    bool playOffSpinInit;
    bool playOffNew;

    //tags dialog
    tags *tagDialog;

private slots:
    void playOnActivePassO();
    void playOnActivePassD();
    void playOnActiveMoveO();
    void playOnActiveMoveD();
    void playOnActiveKickO();
    void playOnActiveKickD();
    void playOnActiveChipO();
    void playOnActiveChipD();
    void playOnActiveMarkO();
    void playOnActiveMarkD();
    void playOnActiveOneTouch();
    void playOnActiveCatchBall();
    void playOnActiveReceivePass();
    void playOnActiveShot();
    void playOnActiveChipToGoal();
    void playOnActivePrevAgent();
    void playOnActiveNextAgent();
    void playOnActivePrevPriority();
    void playOnActiveNextPriority();
    void playOnActiveA();
    void playOnActiveB();

    void on_addBtn_clicked();

    void on_browseBtn_clicked();

    void on_saveBtn_clicked();

    void on_resetBtn_clicked();

    void on_spinBox_valueChanged(const QString &arg1);

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

    void on_newBtn_clicked();

    void on_tagsBtn_clicked();

    void on_spinBox_2_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_Max_Effective_currentIndexChanged(int arg1);

    void on_Min_Needed_currentIndexChanged(int arg1);

    /* Play off */
    void playOffActivePass();
    void playOffActiveReceivePass();
    void playOffActiveShotToGoal();
    void playOffActiveChipToGoal();
    void playOffActiveOneTouch();
    void playOffActiveMove();
    /* play off */

    /* after life */
    void afterlifeActionDefense();
    void afterlifeActionSupport();
    void afterlifeActionPosition();
    void afterlifeActionGoalie();
    void afterlifeActionMark();

    /* after life */
    void tagDialogExit(QString str);
    void updateTags(QString str);

    void on_Send_clicked();

private:
    Ui::MainWindow *ui;
};

extern bool Gupdate;
extern QString Gdir;

#endif // MAINWINDOW_H
