#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "playonplansql.h"

#include <QDebug>

#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QMouseEvent>
#include <QPoint>
#include <QMenu>
#include <QVBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    geometryIni();

    currentVPMode = PlayOff;
    playOn = new playon(this);
    playOn->setLabel(ui->field);
    playOn->setWidget(ui->tab);
    playOn->setStatusBar(ui->statusBar);
    playOn->setTabWidget(ui->tabWidget);
    playOnCurrentPlan = 0;
    playOnCreateActions();
    playOnNew = false;
    connect(playOn, SIGNAL(updateTags(QString)), this, SLOT(updateTags(QString)));

    playOff = new playoff(this);
    playOff->setLabel(ui->field);
    playOff->setWidget(ui->POtabWidget);
    playOff->setLineEdits(ui->POTBPosX, ui->POTBPosY, ui->POTBPosAng, ui->POTBPosTol);
    playOff->setStatusBar(ui->statusBar);
    playOff->setAgentSizeCB(ui->comboBox_2);
    playOffCurrentPlan = 0;
    playOffCreateActions();
    playOffNew = false;
    playOffSpinInit = true;
    connect(playOff, SIGNAL(updateTags(QString)), this, SLOT(updateTags(QString)));

    tagDialog = new tags(this);
    connect(tagDialog, SIGNAL(tagExit(QString)), this, SLOT(tagDialogExit(QString)));

    if(currentVPMode == PlayOn) {
        ui->menuBar->actions().at(0)->menu()->actions().at(0)->setChecked(true);
        ui->menuBar->actions().at(0)->menu()->actions().at(1)->setChecked(false);
        currentVPMode = PlayOn;
        playOn->enableSQL();
        playOff->disableSQL();
        initVPMode();
        ui->newBtn->setVisible(true);
        playOn->enable();
    }
    else {
        ui->menuBar->actions().at(0)->menu()->actions().at(0)->setChecked(false);
        ui->menuBar->actions().at(0)->menu()->actions().at(1)->setChecked(true);
        currentVPMode = PlayOff;
        playOn->disableSQL();
        playOff->enableSQL();
        initVPMode();
        ui->newBtn->setVisible(true);
        playOn->disable();
        if (playOffSpinInit) {
            ui->spinBox->setMaximum(0);
            playOffSpinInit = false;
        }
    }
}

//this allows user to have cross platform availability with same style
//basicalluy ignores the OS margin, paddings and etc.
void MainWindow::geometryIni()
{
    QRect fieldRect(5, 5, 859, 655), tabWidgetRect(874, 5, 401, 655);
    ui->field->setGeometry(fieldRect);

    //playon tab widget
    ui->tabWidget->setGeometry(tabWidgetRect);
    //playoff tab widget
    ui->POtabWidget->setGeometry(tabWidgetRect);

    //////////
    QRect browseBtnRect(5, 665, 100, 25);
    QRect spinRect(110, 665, 70, 25);
    QRect addBtnRect(185, 665, 100, 25);
    QRect resetBtnRect(290, 665, 100, 25);
    QRect saveBtnRect(395, 665, 100, 25);
    QRect removeBtnRect(500, 665, 100, 25);
    QRect newBtnRect(605, 665, 100, 25);
    QRect tagsBtnRect(765, 665, 100, 25);
    ui->browseBtn->setGeometry(browseBtnRect);
    ui->spinBox->setGeometry(spinRect);
    ui->addBtn->setGeometry(addBtnRect);
    ui->resetBtn->setGeometry(resetBtnRect);
    ui->saveBtn->setGeometry(saveBtnRect);
    ui->removeBtn->setGeometry(removeBtnRect);
    ui->newBtn->setGeometry(newBtnRect);
    ui->tagsBtn->setGeometry(tagsBtnRect);
    ////////
    QRect ballLabelRect(874, 662, 400, 33);
    ui->ballLabel->setGeometry(ballLabelRect);
    ui->ballLabel->setStyleSheet("QLabel {background-color: orange; color: #fff; font-weight: bold;}");
    ui->ballLabel->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    geometryIni();
}

//mouse press event
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint tempPos;
    tempPos = ui->centralWidget->mapFromParent(event->pos());
    tempPos = ui->field->mapFromParent(tempPos);

    if (ui->field->underMouse()) {
        if ((currentVPMode == PlayOn && !playOnNew) ||
                (currentVPMode == PlayOff && !playOffNew)) {
            QMessageBox::warning(this, ">>NEW BUTTON<<","You have to make a new plan!", QMessageBox::Ok);
            return;
        }
    }
    //PlayOn Mode
    if (currentVPMode == PlayOn) {
        playOn->mousePressed(event, tempPos);
    }
    //PlayOff mode
    else if (currentVPMode == PlayOff) {
        playOff->mousePressed(event, tempPos);
    }
}

//mouse release event
//this is mainly for deactiving acrtived event in Mouse Press Event
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint tempPos;
    tempPos = ui->centralWidget->mapFromParent(event->pos());
    tempPos = ui->field->mapFromParent(tempPos);
    if (currentVPMode == PlayOn) {
        playOn->mouseReleased(event, tempPos);
    }
    else if (currentVPMode == PlayOff) {
        playOff->mouseReleased(event, tempPos);
    }
}

//Mouse Move event
//this is added to have better animation
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint tempPos;
    tempPos = ui->centralWidget->mapFromParent(event->pos());
    tempPos = ui->field->mapFromParent(tempPos);
    if (currentVPMode == PlayOn) {
        playOn->mouseMoved(event, tempPos);
    }
    else if (currentVPMode == PlayOff) {
        playOff->mouseMoved(event, tempPos);
    }
}

//Key Press Event
//Mainly for using shortcuts
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (currentVPMode == PlayOn) {
        if (!playOn->isInBallMode()) {
            switch (event->key()) {
            case Qt::Key_W:
                playOnActivePrevAgent();
                break;
            case Qt::Key_S:
                playOnActiveNextAgent();
                break;
            case Qt::Key_A:
                playOnActivePrevPriority();
                break;
            case Qt::Key_D:
                playOnActiveNextPriority();
                break;
            case Qt::Key_Q:
                playOnActiveA();
                break;
            case Qt::Key_E:
                playOnActiveB();
                break;
            }
        }
        switch(event->key()) {
        case Qt::Key_O:
        case Qt::Key_Up:
            ui->spinBox->setValue(ui->spinBox->value() + 1);
            break;
        case Qt::Key_L:
        case Qt::Key_Down:
            ui->spinBox->setValue(ui->spinBox->value() - 1);
            break;
        case Qt::Key_K:
        case Qt::Key_Left:

            break;
        case Qt::Key_Semicolon:
        case Qt::Key_Right:

            break;
        case Qt::Key_B:

            break;
        }
    }
    else if (currentVPMode == PlayOff) {
        if (event->modifiers() != Qt::ControlModifier) {
            switch (event->key()) {
            case Qt::Key_W:
            case Qt::Key_Up:
                if(ui->spinBox->value() + 1 <= ui->spinBox->maximum() )
                    ui->spinBox->setValue(ui->spinBox->value() + 1);
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                if(ui->spinBox->value() - 1 >= 0 )
                    ui->spinBox->setValue(ui->spinBox->value() - 1);
                break;
            }
        }
        else if (event->modifiers() == Qt::ControlModifier) {
            switch(event->key()) {
            case Qt::Key_O:
                on_browseBtn_clicked();
                break;
            case Qt::Key_S:
                on_saveBtn_clicked();
                break;
            }
        }
    }
}

//This fucntion creates actions for contex menu (aka right click menu)
void MainWindow::playOnCreateActions()
{
    playOnPassSkill[0] = new QAction(tr("&Pass"), this);
    playOnPassSkill[0]->setStatusTip(tr("Set point A then B to send pass. Offensive"));
    connect(playOnPassSkill[0], SIGNAL(triggered()), this, SLOT(playOnActivePassO()));

    playOnPassSkill[1] = new QAction(tr("&Pass"), this);
    playOnPassSkill[1]->setStatusTip(tr("Set point A then B to send pass. Defensive"));
    connect(playOnPassSkill[1], SIGNAL(triggered()), this, SLOT(playOnActivePassD()));

    playOnMoveSkill[0] = new QAction(tr("&Move"), this);
    playOnMoveSkill[0]->setStatusTip(tr("Set point A then B to move. Offensive"));
    connect(playOnMoveSkill[0], SIGNAL(triggered()), this, SLOT(playOnActiveMoveO()));

    playOnMoveSkill[1] = new QAction(tr("&Move"), this);
    playOnMoveSkill[1]->setStatusTip(tr("Set point A then B to move. Defensive"));
    connect(playOnMoveSkill[1], SIGNAL(triggered()), this, SLOT(playOnActiveMoveD()));

    playOnKickSkill[0] = new QAction(tr("&Kick"), this);
    playOnKickSkill[0]->setStatusTip(tr("Set point A then B to kick. Defensive"));
    connect(playOnKickSkill[0], SIGNAL(triggered()), this, SLOT(playOnActiveKickO()));

    playOnKickSkill[1] = new QAction(tr("&Kick"), this);
    playOnKickSkill[1]->setStatusTip(tr("Set point A then B to kick. Defensive"));
    connect(playOnKickSkill[1], SIGNAL(triggered()), this, SLOT(playOnActiveKickD()));

    playOnChipSkill[0] = new QAction(tr("&Chip"), this);
    playOnChipSkill[0]->setStatusTip(tr("Set point A then B to chip. Offensive"));
    connect(playOnChipSkill[0], SIGNAL(triggered()), this, SLOT(playOnActiveChipO()));

    playOnChipSkill[1] = new QAction(tr("&Chip"), this);
    playOnChipSkill[1]->setStatusTip(tr("Set point A then B to chip. Defensive"));
    connect(playOnChipSkill[1], SIGNAL(triggered()), this, SLOT(playOnActiveChipD()));

    playOnMarkSkill[0] = new QAction(tr("&Mark"), this);
    playOnMarkSkill[0]->setStatusTip(tr("Set a region to Mark. Offensive"));
    connect(playOnMarkSkill[0], SIGNAL(triggered()), this, SLOT(playOnActiveMarkO()));

    playOnMarkSkill[1] = new QAction(tr("&Mark"), this);
    playOnMarkSkill[1]->setStatusTip(tr("Set a region to Mark. Defensive"));
    connect(playOnMarkSkill[1], SIGNAL(triggered()), this, SLOT(playOnActiveMarkD()));

    playOnOneTouchSkill = new QAction(tr("&On Touch"), this);
    playOnOneTouchSkill->setStatusTip(tr("Set point A then B to One Touch. Offensive"));
    connect(playOnOneTouchSkill, SIGNAL(triggered()), this, SLOT(playOnActiveOneTouch()));

    playOnCatchBallSkill = new QAction(tr("&Catch Ball"), this);
    playOnCatchBallSkill->setStatusTip(tr("No need to select a region"));
    connect(playOnCatchBallSkill, SIGNAL(triggered()), this, SLOT(playOnActiveCatchBall()));

    playOnReceivePass = new QAction(tr("&Receive Pass"), this);
    playOnReceivePass->setStatusTip(tr("Set point A to receive pass (B point in pass skill)"));
    connect(playOnReceivePass, SIGNAL(triggered()), this, SLOT(playOnActiveReceivePass()));

    playOnShotSkill = new QAction(tr("&Shot"), this);
    playOnShotSkill->setStatusTip(tr("Set point to shot, B point is Goal"));
    connect(playOnShotSkill, SIGNAL(triggered()), this, SLOT(playOnActiveShot()));

    playOnChipToGoalSkill = new QAction(tr("&Chip to Goal"), this);
    playOnChipToGoalSkill->setStatusTip(tr("Set point to chip, B point is behind opponent Defenders"));
    connect(playOnChipToGoalSkill, SIGNAL(triggered()), this, SLOT(playOnActiveChipToGoal()));

    playOnPrevAgent = new QAction(tr("&Previous Agent"), this);
    playOnPrevAgent->setStatusTip(tr("Previous Agent"));
    connect(playOnPrevAgent, SIGNAL(triggered()), this, SLOT(playOnActivePrevAgent()));

    playOnNextAgent = new QAction(tr("&Next Agent"), this);
    playOnNextAgent->setStatusTip(tr("Next Agent"));
    connect(playOnNextAgent, SIGNAL(triggered()), this, SLOT(playOnActiveNextAgent()));

    playOnPrevPriority = new QAction(tr("&Previous Priority"), this);
    playOnPrevPriority->setStatusTip(tr("Previous Priority"));
    connect(playOnPrevPriority, SIGNAL(triggered()), this, SLOT(playOnActivePrevPriority()));

    playOnNextPriority = new QAction(tr("&Next Priority"), this);
    playOnNextPriority->setStatusTip(tr("Next Priority"));
    connect(playOnNextPriority, SIGNAL(triggered()), this, SLOT(playOnActiveNextPriority()));

    playOnPrevPriority->setEnabled(false);
}

//contex menu event/ it will be called if right click pressed
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (currentVPMode == PlayOn) {
        if (ui->field->underMouse() && !playOn->isInBallMode()) {
            QMenu *offMenu, *defMenu;
            QMenu menu(this);
            offMenu = menu.addMenu(tr("&Offensive"));
            offMenu->addAction(playOnMoveSkill[0]);
            offMenu->addAction(playOnPassSkill[0]);
            offMenu->addAction(playOnKickSkill[0]);
            offMenu->addAction(playOnChipSkill[0]);
            offMenu->addAction(playOnMarkSkill[0]);

            defMenu = menu.addMenu(tr("&Defensive"));
            defMenu->addAction(playOnMoveSkill[1]);
            defMenu->addAction(playOnPassSkill[1]);
            defMenu->addAction(playOnKickSkill[1]);
            defMenu->addAction(playOnChipSkill[1]);
            defMenu->addAction(playOnMarkSkill[1]);

            menu.addAction(playOnReceivePass);
            menu.addAction(playOnOneTouchSkill);
            menu.addAction(playOnShotSkill);
            menu.addAction(playOnChipToGoalSkill);
            menu.addAction(playOnCatchBallSkill);
            menu.addSeparator();
            menu.addAction(playOnPrevAgent);
            menu.addAction(playOnNextAgent);
            menu.addSeparator();
            menu.addAction(playOnPrevPriority);
            menu.addAction(playOnNextPriority);

            menu.exec(event->globalPos());
        }
    }
    else {
        if (ui->field->underMouse()) {
            QMenu menu(this);
            menu.addAction(playOffMove);
            menu.addSeparator();
            menu.addAction(playOffPass);
            menu.addAction(playOffReceivePass);
            menu.addAction(playOffShotToGoal);
            menu.addAction(playOffChipToGoal);
            menu.addAction(playOffOneTouch);

            menu.exec(event->globalPos());
        }
    }
}

//Slots for contex menu
//start
void MainWindow::playOnActivePassO()
{
    playOn->setPlan(PassOffensive);
}

void MainWindow::playOnActivePassD()
{
    playOn->setPlan(PassDefensive);
}

void MainWindow::playOnActiveMoveO()
{
    playOn->setPlan(MoveOffensive);
}

void MainWindow::playOnActiveMoveD()
{
    playOn->setPlan(MoveDefensive);
}

void MainWindow::playOnActiveKickO()
{
    playOn->setPlan(KickOffensive);
}

void MainWindow::playOnActiveKickD()
{
    playOn->setPlan(KickDefensive);
}

void MainWindow::playOnActiveChipO()
{
    playOn->setPlan(ChipOffensive);
}

void MainWindow::playOnActiveChipD()
{
    playOn->setPlan(ChipDefensive);
}

void MainWindow::playOnActiveMarkO()
{
    playOn->setPlan(MarkOffensive);
}

void MainWindow::playOnActiveMarkD()
{
    playOn->setPlan(MarkDefensive);
}

void MainWindow::playOnActiveOneTouch()
{
    playOn->setPlan(OneTouch);
}

void MainWindow::playOnActiveCatchBall()
{
    playOn->setPlan(CatchBall);
}

void MainWindow::playOnActiveReceivePass()
{
    playOn->setPlan(ReceivePass);
}

void MainWindow::playOnActiveShot()
{
    playOn->setPlan(Shot);
}

void MainWindow::playOnActiveChipToGoal()
{
    playOn->setPlan(ChipToGoal);
}
//end

//Contex menu next agent slot
void MainWindow::playOnActivePrevAgent()
{
    qDebug()<<"Prev Agent";
    //    currentAgent--;
    //    if(currentAgent < 0) currentAgent = 0;
    //    if(currentPlan->getAgentSkill(currentAgent + 1, currentPriority + 1) == None)
    //        currentPriority = currentPlan->getPrioritySize(currentAgent + 1);
    //    if(currentAgent == 0)
    //        prevAgent->setEnabled(false);
    //    else
    //        prevAgent->setEnabled(true);
    //    if(mode == _1AGENT)
    //        nextAgent->setEnabled(false);
    //    else
    //        nextAgent->setEnabled(true);
    //    setSkillColors(currentAgent, currentPriority, aOrB);
    //    blinkCurrentAgent = true;
    //    showAgentArea(aSelectedArea[currentAgent][aOrB]);

    //    if(aOrB == 1)
    //        activeA();
}

//contex menu previous agent slot
void MainWindow::playOnActiveNextAgent()
{
    qDebug()<<"Next Agent";
    //    currentAgent++;
    //    enableContexAgent();
    //    if(currentPlan->getAgentSkill(currentAgent + 1, currentPriority + 1) == None)
    //        currentPriority = currentPlan->getPrioritySize(currentAgent + 1);

    //    setSkillColors(currentAgent, currentPriority, aOrB);
    //    blinkCurrentAgent = true;
    //    showAgentArea(aSelectedArea[currentAgent][aOrB]);

    //    if(aOrB == 1)
    //        activeA();
}

//Contex menu previous priority slot
void MainWindow::playOnActivePrevPriority()
{
    qDebug()<<"Prev Priority";
    //    currentPriority--;
    //    if(currentPriority < 0) currentPriority = 0;
    //    if(currentPriority == 0)
    //        prevPriority->setEnabled(false);
    //    else
    //        prevPriority->setEnabled(true);
    //    nextPriority->setEnabled(true);

    //    setSkillColors(currentAgent, currentPriority, aOrB);
    //    blinkCurrentAgent = true;
    //    showAgentArea(aSelectedArea[currentAgent][aOrB]);
}

//Contex menu next priority slot
void MainWindow::playOnActiveNextPriority()
{
    qDebug()<<"Next Priority";
    //    if(currentPlan->getAgentSkill(currentAgent + 1, currentPriority + 1) == None)
    //        return;
    //    currentPriority++;
    //    if(currentPriority > 3) currentPriority = 3;
    //    if(currentPriority == 3)
    //        nextPriority->setEnabled(false);
    //    else
    //        nextPriority->setEnabled(true);
    //    prevPriority->setEnabled(true);

    //    setSkillColors(currentAgent, currentPriority, aOrB);
    //    blinkCurrentAgent = true;
    //    showAgentArea(aSelectedArea[currentAgent][aOrB]);
}

void MainWindow::playOnActiveA()
{

}

void MainWindow::playOnActiveB()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Add/Apply/New button event
void MainWindow::on_addBtn_clicked()
{
    if (currentVPMode == PlayOn) {
        playOn->apply(playOnCurrentPlan);
    }
    else if (currentVPMode == PlayOff) {
        playOff->apply(playOffCurrentPlan);
    }

}

//Browse button event
void MainWindow::on_browseBtn_clicked()
{
    QDir currentDir;
    if (currentVPMode == PlayOn) {
        QSettings settings(settingAddress, QSettings::IniFormat);
        QString tempDir = QFileDialog::getOpenFileName(this, tr("Open SQL"),
                                                       settings.value(DEFAULT_DIR_KEY).toString(), tr("SQL Files (*.db3 *.db);;All Files (*.*)"));
        if (tempDir.length() > 2) {
            settings.setValue(DEFAULT_DIR_KEY, currentDir.absoluteFilePath(tempDir));
            playOnOpenFileDir = tempDir;
            int tempCnt = playOn->loadPlan(playOnOpenFileDir);
            ui->spinBox->setEnabled(true);
            if (tempCnt > 0) {
                playOn->choosePlan(0);
                playOnNew = true;
            }
            ui->spinBox->setMaximum(tempCnt-1);
            playOnCurrentPlan = 0;
            on_spinBox_valueChanged("");
            //sets plan directory in window title
            setWindowTitle("Visual Planner - "+playOnOpenFileDir);
        }
    }
    else if (currentVPMode == PlayOff) {
        QSettings settings(settingAddress, QSettings::IniFormat);
        QString tempDir = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                       settings.value(DEFAULT_DIR_KEY).toString(),
                                                       tr("Json Files (*.json);;SQL Files (*.db3 *.db);;All Files (*.*)"));
        if (tempDir.length() > 2) {
            settings.setValue(DEFAULT_DIR_KEY, currentDir.absoluteFilePath(tempDir));
            playOffopenFileDir = tempDir;
            playOff->cleanPlans();
            ui->spinBox->setEnabled(true);
            int tempCnt ;
            if (tempDir.endsWith(QString("json"))) {
                tempCnt = playOff->loadPlanJson(playOffopenFileDir);
            }
            else {
                tempCnt = playOff->loadPlan(playOffopenFileDir);
            }
            //            int tempCnt = playOff->loadPlan(playOffopenFileDir);
            //            int tempCnt = playOff->loadPlanJson(playOffopenFileDir);
            if (tempCnt > 0) {
                playOff->choosePlan(0);
                playOffNew = true;
            }
            ui->spinBox->setMaximum(tempCnt-1);
            playOffCurrentPlan = 0;
            on_spinBox_valueChanged("");
            //sets plan directory in window title
            setWindowTitle("Visual Planner - "+playOffopenFileDir);
        }
    }
}

//save button event
void MainWindow::on_saveBtn_clicked()
{
    QDir currentDir;
    QSettings settings(settingAddress, QSettings::IniFormat);
    if(currentVPMode == PlayOn)
    {
        QMessageBox::StandardButton reply;
        if(playOnOpenFileDir.length() < 2)
        {
            reply = QMessageBox::question(this, "Save File", "Do you want to save?",
                                          QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel)
                return;
        }
        else
        {
            reply = QMessageBox::question(this, "Save File", "Do you want to save?",
                                          QMessageBox::Save|QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel)
                return;
        }

        if(reply == QMessageBox::Yes)
        {
            //removes the plan file with same name
            QFile::remove(playOffopenFileDir);
        }
        if(playOnOpenFileDir.length() < 2 || reply == QMessageBox::Save)
        {
            QString tempDir;
            tempDir = QFileDialog::getSaveFileName(this, tr("Save plan file"),
                                                   settings.value(DEFAULT_DIR_KEY).toString(),
                                                   tr("SQL Files (*.db3 *.db);;All Files (*.*)"));
            if(tempDir.length() > 2)
                playOnSaveFileDir = tempDir;
            else
                return;
        }
        else
            playOnSaveFileDir = playOnOpenFileDir;

        playOn->savePlan(playOnSaveFileDir);

        playOnOpenFileDir = playOnSaveFileDir;

        settings.setValue(DEFAULT_DIR_KEY, currentDir.absoluteFilePath(playOnOpenFileDir));

        //sets plan directory in window title
        setWindowTitle("Visual Planner - "+playOnSaveFileDir);

    }
    else if(currentVPMode == PlayOff)
    {
        QMessageBox::StandardButton reply;
        if(playOffopenFileDir.length() < 2)
        {
            reply = QMessageBox::question(this, "Save File", "Do you want to save?",
                                          QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel)
                return;
        }
        else
        {
            reply = QMessageBox::question(this, "Save File", "Do you want to save?",
                                          QMessageBox::Save|QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel)
                return;
        }

        if(reply == QMessageBox::Yes)
        {
            //removes the plan file with same name
            QFile::remove(playOffopenFileDir);
        }
        if(playOffopenFileDir.length() < 2 || reply == QMessageBox::Save)
        {
            QString tempDir;
            tempDir = QFileDialog::getSaveFileName(this, tr("Save plan file"),
                                                   settings.value(DEFAULT_DIR_KEY).toString(),
                                                   tr("Json Files (*.json);;SQL Files (*.db3 *.db);;All Files (*.*)"));
            if(tempDir.length() > 2)
                playOffsaveFileDir = tempDir;
            else
                return;
        }
        else
            playOffsaveFileDir = playOffopenFileDir;

        if (playOffsaveFileDir.endsWith(QString("json"))) {
            //playOff->savePlan(playOffsaveFileDir);
            playOff->savePlanJson(playOffsaveFileDir);
        }
        else {
            playOff->savePlan(playOffsaveFileDir);
        }
        playOffopenFileDir = playOffsaveFileDir;

        settings.setValue(DEFAULT_DIR_KEY, currentDir.absoluteFilePath(playOffopenFileDir));

        //sets plan directory in window title
        setWindowTitle("Visual Planner - "+playOffsaveFileDir);
    }
}

//Reset button event
void MainWindow::on_resetBtn_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset Plan", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if( currentVPMode == PlayOn) {
            playOn->reset();
        }
        else if (currentVPMode == PlayOff) {
            playOff->reset();
        }
    }
}


//Plan explorer event
void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{
    int tempSpin = ui->spinBox->value();
    if (currentVPMode == PlayOn) {
        playOnCurrentPlan = tempSpin;
        playOn->choosePlan(tempSpin);
        if (playOnCurrentPlan < playOn->getPlanSize() && playOn->getPlanSize() > 0) {
            ui->removeBtn->setDisabled(false);
        }
        else {
            ui->removeBtn->setDisabled(true);
        }
    }
    else if (currentVPMode == PlayOff) {
        playOffCurrentPlan = tempSpin;
        playOff->choosePlan(tempSpin);
        if (playOffCurrentPlan < playOff->getPlanSize() - 1 && playOff->getPlanSize() > 0) {
            ui->removeBtn->setDisabled(false);
        }
        else {
            ui->removeBtn->setDisabled(true);
        }
    }
}

//Removes plan
void MainWindow::on_removeBtn_clicked()
{
    if (currentVPMode == PlayOn) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Remove Plan", "Are you sure?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;

        playOn->removePlan(playOnCurrentPlan);
        ui->spinBox->setValue(0);
        ui->spinBox->setMaximum(playOn->getPlanSize()-1);
        playOnCurrentPlan = 0;
        on_spinBox_valueChanged("");
    }
    else if (currentVPMode == PlayOff) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Remove Plan", "Are you sure?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;

        playOff->removePlan(playOffCurrentPlan);
        ui->spinBox->setValue(0);
        ui->spinBox->setMaximum(playOff->getPlanSize()-1);
        playOffCurrentPlan = 0;
        on_spinBox_valueChanged("");
    }
}

//Menu event for PlayOn
//Shortcut is Ctrl+N
void MainWindow::on_actionPlayOn_triggered()
{
    ui->menuBar->actions().at(0)->menu()->actions().at(0)->setChecked(true);
    ui->menuBar->actions().at(0)->menu()->actions().at(1)->setChecked(false);
    currentVPMode = PlayOn;
    playOn->enableSQL();
    playOff->disableSQL();
    initVPMode();
    ui->newBtn->setVisible(true);
    playOn->enable();
}

//Menu event for PlayOff
//Shortcut is Ctrl+M
void MainWindow::on_actionPlayOff_triggered()
{
    ui->menuBar->actions().at(0)->menu()->actions().at(0)->setChecked(false);
    ui->menuBar->actions().at(0)->menu()->actions().at(1)->setChecked(true);
    currentVPMode = PlayOff;
    playOn->disableSQL();
    playOff->enableSQL();
    initVPMode();
    ui->newBtn->setVisible(true);
    playOn->disable();
    if (playOffSpinInit) {
        ui->spinBox->setMaximum(0);
        playOffSpinInit = false;
    }
}

//Initilize Visual Planner modes
void MainWindow::initVPMode()
{
    if (currentVPMode == PlayOn) {
        ui->tabWidget->setVisible(true);
        ui->ballLabel->setVisible(true);
        ui->POtabWidget->setVisible(false);
        playOn->initPainting();
    }
    else if (currentVPMode == PlayOff) {
        ui->tabWidget->setVisible(false);
        ui->ballLabel->setVisible(false);
        ui->POtabWidget->setVisible(true);
        playOff->draw();
    }
}

void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    playOff->setAgentSize(index + 1);
    if (playOff->getCurrentAgent() > index) {
        playOff->setCurrentAgent(index);
    }
    playOff->POSetSelectedDisplayLabel(playOff->getDisplayMode());
    playOff->POSetSelectedCurrentAgentLabel(playOff->getCurrentAgent());
}

//PlayOff geometry QLineEdit events
void MainWindow::on_POBtn_pressed()
{
    playOff->setShowAllFlag(true);
}

void MainWindow::on_POBtn_released()
{
    playOff->setShowAllFlag(false);
}

void MainWindow::on_POTBPosX_returnPressed()
{
    playOff->setGeomX(ui->POTBPosX->text().toInt());
}

void MainWindow::on_POTBPosY_returnPressed()
{
    playOff->setGeomY(ui->POTBPosY->text().toInt());
}

void MainWindow::on_POTBPosAng_returnPressed()
{
    int tempAng = ui->POTBPosAng->text().toInt();
    if (tempAng > 180) {
        tempAng = 180;
    }
    if (tempAng < -180) {
        tempAng = -180;
    }
    ui->POTBPosAng->setText(QString::number(tempAng));
    playOff->setGeomAngle(tempAng);
}

void MainWindow::on_POTBPosTol_returnPressed()
{
    playOff->setGeomTolerance(ui->POTBPosTol->text().toInt());
}

//Creates contex menu for PlayOff
void MainWindow::playOffCreateActions()
{
    playOffPass = new QAction(tr("&Pass"), this);
    playOffPass->setStatusTip(tr("Select the target agent"));
    connect(playOffPass, SIGNAL(triggered()), this, SLOT(playOffActivePass()));

    playOffReceivePass = new QAction(tr("&Receive Pass"), this);
    playOffReceivePass->setStatusTip(tr("Select receive pass agent"));
    connect(playOffReceivePass, SIGNAL(triggered()),this,SLOT(playOffActiveReceivePass()));

    playOffShotToGoal = new QAction(tr("&Shot to Goal"), this);
    playOffShotToGoal->setStatusTip(tr("Select shoter agent"));
    connect(playOffShotToGoal, SIGNAL(triggered()),this,SLOT(playOffActiveShotToGoal()));

    playOffChipToGoal = new QAction(tr("&Chip to Goal"), this);
    playOffChipToGoal->setStatusTip(tr("Select chipper agent"));
    connect(playOffChipToGoal, SIGNAL(triggered()),this,SLOT(playOffActiveChipToGoal()));

    playOffOneTouch = new QAction(tr("&One Touch"), this);
    playOffOneTouch->setStatusTip(tr("Select one toucher"));
    connect(playOffOneTouch, SIGNAL(triggered()),this,SLOT(playOffActiveOneTouch()));

    playOffMove = new QAction(tr("&Move"), this);
    playOffMove->setStatusTip(tr("Move"));
    connect(playOffMove, SIGNAL(triggered()),this,SLOT(playOffActiveMove()));
}

//PlayOff contex menu slots
//start
void MainWindow::playOffActivePass()
{
    playOff->setSkill(PassSkill);
}

void MainWindow::playOffActiveReceivePass()
{
    playOff->setSkill(ReceivePassSkill);
}

void MainWindow::playOffActiveShotToGoal()
{
    playOff->setSkill(ShotToGoalSkill);
}

void MainWindow::playOffActiveChipToGoal()
{
    playOff->setSkill(ChipToGoalSkill);
}

void MainWindow::playOffActiveOneTouch()
{
    playOff->setSkill(OneTouchSkill);
}

void MainWindow::playOffActiveMove()
{
    playOff->setSkill(MoveSkill);
}
//end

//New button fot PlayOff
void MainWindow::on_newBtn_clicked()
{
    if (currentVPMode == PlayOn) {
        ui->spinBox->setMaximum(playOn->getPlanSize());
        ui->spinBox->setValue(playOn->getPlanSize());
        ui->spinBox->setEnabled(true);
        on_spinBox_valueChanged("");
        playOnNew = true;
    }
    else if (currentVPMode == PlayOff) {
        ui->spinBox->setMaximum(playOff->getPlanSize());
        ui->spinBox->setValue(playOff->getPlanSize());
        ui->spinBox->setEnabled(true);
        on_spinBox_valueChanged("");
        playOff->clean();
        playOffNew = true;
    }
}

void MainWindow::on_tagsBtn_clicked()
{
    if ((currentVPMode == PlayOn && !playOnNew) ||
            (currentVPMode == PlayOff && !playOffNew)) {
        QMessageBox::warning(this, ">>NEW BUTTON<<","You have to make a new plan!", QMessageBox::Ok);
        return;
    }
    tagDialog->show();
}

void MainWindow::tagDialogExit(QString str)
{
    if (currentVPMode == PlayOn) {
        playOn->setTags(str);
        playOn->apply(playOnCurrentPlan);
    }
    else if (currentVPMode == PlayOff) {
        playOff->setTags(str);
        playOff->apply(playOffCurrentPlan);
    }
}

void MainWindow::updateTags(QString str)
{
    tagDialog->loadTagsByString(str);
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    if (currentVPMode == PlayOff) {
        playOff->setChance(arg1);
        playOff->apply(playOffCurrentPlan);
    }
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    if (currentVPMode == PlayOff) {
        playOff->setLastDist(arg1);
        playOff->apply(playOffCurrentPlan);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_Send_clicked()
{
    playOff->pb->set_id(1);
    playOff->pb->set_apiversion(API_VERSION);
    //PlanBook* _pb = playOff->pb;
    for(int i{};i< playOff->myPlan->planList.size();i++)
        playOff->writeproto(playOff->pb , i);
    Server s{};

        s.send(playOff->pb);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
