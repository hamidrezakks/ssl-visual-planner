#include "playoff.h"

playoff::playoff(QWidget *parent) :
    QWidget(parent)
{
    fieldPix = new QPixmap(":images/Field.png");
    currentRobot.agent = -1;
    currentRobot.index = -1;
    currentRobot.skillNum = 0;
    displayMode = -1;
    currentAgent = 0;
    showAll = false;
    agentSize = 1;
    currentSkillNum = 0;

    POFieldSelected = false;

    currentTool = TMOVE;
    currentPOMode = INDIRECT;

    PORubberBand = new QRubberBand(QRubberBand::Rectangle, parent);

    rbBeginPoint = rbEndPoint = QPoint(0, 0);
    validRubberBand = false;
    moveRubberBand = false;
    rbStarted = false;

    QPalette pal;
    pal.setBrush(QPalette::Highlight, QBrush(Qt::blue, Qt::Dense4Pattern));
    PORubberBand->setPalette(pal);
    passFlag = false;
    myPlan = new playOffPlanSQL();
    backup = false;
    restore = false;

    chance   = 1;
    lastDist = 1.5;

}

playoff::~playoff()
{

}

void playoff::setLabel(QLabel *tLabel)
{
    fieldLabel = tLabel;
}

void playoff::setWidget(QWidget *tWidget)
{
    POWidget = tWidget;
    POinitLables();
    POSetSelectedDisplayLabel(getDisplayMode());
    POSetSelectedCurrentAgentLabel(getCurrentAgent());
}

void playoff::setLineEdits(QLineEdit *_posX, QLineEdit *_posY, QLineEdit *_posAng, QLineEdit *_posTol)
{
    POTBPosX = _posX;
    POTBPosY = _posY;
    POTBPosAng = _posAng;
    POTBPosTol = _posTol;
}

void playoff::setStatusBar(QStatusBar *_statusBar)
{
    statusBar = _statusBar;
}

void playoff::setAgentSizeCB(QComboBox *_comboBox)
{
    agentSizeCB = _comboBox;
}

void playoff::mousePressed(QMouseEvent *event, QPoint tempPos)
{
    if (fieldLabel->underMouse()) {
        if (currentTool == TMOVE) {
            if (event->buttons() == Qt::LeftButton) {
                if (passFlag) {
                    passReceiver = getRobot(tempPos, true);
                    POFieldSelected = true;
                }
                else if (validRubberBand) {
                    if(QRect(rbBeginPoint, rbEndPoint).normalized().contains(event->pos())) {
                        baseMove = event->pos();
                        moveRubberBand = true;
                        rbMoveBase = PORubberBand->geometry().topLeft();
                        toMoveAgentList.clear();
                        toMoveAgentList = getSelectedAgents(displayMode);
                        fillFirstPointsList(toMoveAgentList);
                        //minor bug
                        currentRobot.agent = -1;
                        currentRobot.index = -1;
                        currentRobot.skillNum = 0;
                    }
                    else {
                        rbBeginPoint = rbEndPoint = QPoint(0, 0);
                        PORubberBand->hide();
                        PORubberBand->setGeometry(QRect(rbBeginPoint, rbEndPoint).normalized());
                        validRubberBand = false;
                    }
                }
                else {
                    POFieldSelected = true;
                    POCurrentRobot = getRobot(tempPos, false, &currentBase);
                    if(!POCurrentRobot.isAng) {
                        if(POCurrentRobot.agent == -1) {
                            placeRobot(tempPos, getCurrentAgent(), true);
                        }
                        else {
                            setCurrentAgent(POCurrentRobot.agent);
                            POSetSelectedCurrentAgentLabel(POCurrentRobot.agent);
                            if(robots[POCurrentRobot.agent].at(POCurrentRobot.index).skillSize <= currentRobot.skillNum) {
                                currentRobot.skillNum = 0;
                            }
                        }
                    }
                    else {
                        turnRobotAng(tempPos, POCurrentRobot.agent, POCurrentRobot.index);
                    }
                }
            }
            else if (event->buttons() == Qt::MidButton) {
                POCurrentRobot = getRobot(tempPos);
                if(POCurrentRobot.agent != -1) {
                    removeRobot(POCurrentRobot.agent, POCurrentRobot.index);
                }
                POCurrentRobot.agent= -2;
                POCurrentRobot.index = -2;
                currentRobot.index = robots[currentAgent].count()-1;
                currentRobot.agent = currentAgent;
                currentRobot.skillNum = 0;
                currentRobot.isAng = false;
            }
        }
        else if (currentTool == TDELETE) {
            if (event->buttons() == Qt::LeftButton) {
                if (validRubberBand) {
                    if (QRect(rbBeginPoint, rbEndPoint).normalized().contains(event->pos())) {
                        QList<agentAndIndex> tempList;
                        tempList = getSelectedAgents(displayMode);
                        removeSelectedRobots(tempList);
                    }
                    /////////
                    rbBeginPoint = rbEndPoint = QPoint(0, 0);
                    PORubberBand->hide();
                    PORubberBand->setGeometry(QRect(rbBeginPoint, rbEndPoint).normalized());
                    validRubberBand = false;
                    /////////
                    currentRobot.index = robots[currentAgent].count()-1;
                    currentRobot.agent = currentAgent;
                    currentRobot.skillNum = 0;
                    currentRobot.isAng = false;
                }
                else {
                    POCurrentRobot = getRobot(tempPos);
                    if (POCurrentRobot.agent != -1) {
                        removeRobot(POCurrentRobot.agent, POCurrentRobot.index);
                    }
                    POCurrentRobot.agent= -2;
                    POCurrentRobot.index = -2;
                    currentRobot.index = robots[currentAgent].count()-1;
                    currentRobot.agent = currentAgent;
                    currentRobot.skillNum = 0;
                    currentRobot.isAng = false;
                }
            }
        }
        else if (currentTool == TSELECT) {
            if (validRubberBand) {
                if (QRect(rbBeginPoint, rbEndPoint).normalized().contains(event->pos())) {
                    baseMove = event->pos();
                    moveRubberBand = true;
                    rbMoveBase = PORubberBand->geometry().topLeft();
                }
                else {
                    validRubberBand = false;
                    rbBeginPoint = event->pos();
                    rbStarted = true;
                    PORubberBand->setGeometry(QRect(rbBeginPoint, event->pos()).normalized());
                }
            }
            else {
                validRubberBand = false;
                rbBeginPoint = event->pos();
                rbStarted = true;
                PORubberBand->setGeometry(QRect(rbBeginPoint, event->pos()).normalized());
                PORubberBand->show();
            }
        }
    }
    if (POWidget->underMouse()) {
        for (int i = 0; i < 3; i++) {
            if (POPassTarget[i]->underMouse()) {
                passFlag = true;
                setCurrentSkillNum(i);
                statusBar->setStyleSheet("QStatusBar {color: #2f78b3;}");
                statusBar->showMessage("Select the target agent to pass", 3000);
                for (int j = 0; j < 3; j++) {
                    POPassTarget[j]->setStyleSheet("QLabel { background-color : #0866af; color : #ff8c00; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
                }
            }
            if (POReceiveIA[i]->underMouse()) {
                playOffRobot tempIAStruct;
                if (robots[currentRobot.agent].at(currentRobot.index).IAMode[i]) {
                    tempIAStruct = robots[currentRobot.agent].at(currentRobot.index);
                    tempIAStruct.IAMode[i] = false;
                    robots[currentRobot.agent].replace(currentRobot.index, tempIAStruct);
                    POSetCheckedIA();
                }
                else {
                    tempIAStruct = robots[currentRobot.agent].at(currentRobot.index);
                    tempIAStruct.IAMode[i] = true;
                    robots[currentRobot.agent].replace(currentRobot.index, tempIAStruct);
                    POSetCheckedIA();
                }
            }
        }
        for (int i = 0 ; i < getAgentSize()+1; i++) {
            if (PODisplayModeLabel[i+1]->underMouse()) {
                setDisplayMode(i-1);
                POSetSelectedDisplayLabel(i-1);
            }
        }

        for (int i = 0 ; i < getAgentSize(); i++) {
            if(POCurrentAgentLabel[i+1]->underMouse()) {
                setCurrentAgent(i);
                POSetSelectedCurrentAgentLabel(i);
            }
        }

        for (int i = 1; i < 4; i++) {
            if(POTools[i]->underMouse()) {
                POSetSelectedToolLabel(toolMode(i));
                currentTool = toolMode(i);
            }
        }

        for (int i = 1; i < 4; i++) {
            if (POMode[i]->underMouse()) {
                POSetSelectedModeLabel(POMODE(i));
                currentPOMode = POMODE(i);
            }
        }

        for (int i = 4; i < 6; i++) {
            if (POTools[i]->underMouse()) {
                if (i == 4) {
                    POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wcopy.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;}");
                    POCopy(displayMode);
                }
                else if (i == 5) {
                    POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wpaste.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;}");
                    POPaste();
                }
            }
        }

        if (event->buttons() == Qt::LeftButton) {
            if (POSkills[1]->text() == "+" && !POSkills[2]->isVisible()) {
                if (POSkills[1]->underMouse()) {
                    POOpenSkill(2);
                }
            }
            else if (POSkills[2]->text() == "+") {
                if (POSkills[2]->underMouse()) {
                    POOpenSkill(3);
                }
            }

            for (int i = 0; i < 3; i++) {
                if (POSkills[i]->underMouse()) {
                    setCurrentSkillNum(i);
                }
            }

        }
        else if (event->buttons() == Qt::RightButton) {
            if (POSkills[1]->text() != "+" && POSkills[2]->text() != "+") {
                if (POSkills[2]->underMouse()) {
                    robotAttr tempAttr = currentRobot;
                    currentRobot.skillNum = 2;
                    setSkill(NoSkill);
                    POOpenSkill(2);
                    currentRobot = tempAttr;
                }
            }
            else if (POSkills[1]->text() != "+" && POSkills[2]->text() == "+") {
                if (POSkills[1]->underMouse()) {
                    robotAttr tempAttr = currentRobot;
                    currentRobot.skillNum = 1;
                    setSkill(NoSkill);
                    POOpenSkill(1);
                    currentRobot = tempAttr;
                }
            }
        }
        POPaintSkill();
    }
}

void playoff::mouseReleased(QMouseEvent *event, QPoint tempPos)
{
    if (POFieldSelected) {
        if (passFlag) {
            if (passReceiver.agent != -1 && passReceiver.index != -1) {
                if (passReceiver.agent == currentRobot.agent) {
                    statusBar->setStyleSheet("QStatusBar {color: red;}");
                    statusBar->showMessage("You can't choose current Robot!!!", 3000);
                }
                else {
                    setSkill(PassSkill, passReceiver.agent, passReceiver.index);
                    robotAttr temp = currentRobot;
                    currentRobot.agent = passReceiver.agent;
                    currentRobot.index = passReceiver.index;
                    currentRobot.skillNum = 1;
                    setSkill(ReceivePassSkill);

                    setCurrentSkillSize(2);
                    currentRobot = temp;
                    statusBar->setStyleSheet("QStatusBar {color: green;}");
                    statusBar->showMessage("Target selected!", 3000);
                }
            }
            for (int i = 0; i < 3; i++) {
                POPassTarget[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : #fff; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
            }
            draw();
            passFlag = false;
        }
        else {
            if (!POCurrentRobot.isAng) {
                if (POCurrentRobot.index == -1) {
                    placeRobot(tempPos, getCurrentAgent());
                    currentRobot.index = robots[currentAgent].count()-1;
                    currentRobot.agent = currentAgent;
                    currentRobot.skillNum = 0;
                    currentRobot.isAng = false;
                }
                else if (POCurrentRobot.index >= 0) {
                    moveRobot(tempPos + currentBase, POCurrentRobot.agent, POCurrentRobot.index);
                }
            }
            else {
                turnRobotAng(tempPos, POCurrentRobot.agent, POCurrentRobot.index);
            }
        }

        robotGeom tempGeom;
        if (POCurrentRobot.index != -1) {
            tempGeom = getRobotGeom(POCurrentRobot.agent, POCurrentRobot.index);
            POTBPosX->setText(QString::number(tempGeom.pos.x()));
            POTBPosY->setText(QString::number(tempGeom.pos.y()));
            POTBPosAng->setText(QString::number((int)tempGeom.angle));
            POTBPosTol->setText(QString::number(tempGeom.tolerance));
        }
    }
    if(rbStarted) {
        if (validRubberBand) {
            QPoint tempPoint = event->pos();
            if (tempPoint.x() > fieldLabel->x() + fieldLabel->width()) {
                tempPoint.setX(fieldLabel->x() + fieldLabel->width());
            }
            if (tempPoint.y() > fieldLabel->y() + fieldLabel->height() + _CenteralWidget_v_offset) {
                tempPoint.setY(fieldLabel->y() + fieldLabel->height() + _CenteralWidget_v_offset);
            }
            if (tempPoint.x() < fieldLabel->x()) {
                tempPoint.setX(fieldLabel->x());
            }
            if (tempPoint.y() < fieldLabel->y() + _CenteralWidget_v_offset) {
                tempPoint.setY(fieldLabel->y() + _CenteralWidget_v_offset);
            }
            rbEndPoint = tempPoint;
        }
        else {
            rbBeginPoint = rbEndPoint = QPoint(0, 0);
            PORubberBand->hide();
        }
        PORubberBand->setGeometry(QRect(rbBeginPoint, rbEndPoint).normalized());
        rbStarted = false;
    }
    else if (moveRubberBand) {
        rbBeginPoint = PORubberBand->geometry().topLeft();
        rbEndPoint = PORubberBand->geometry().bottomRight();
        moveRubberBand = false;

        if(currentTool == TMOVE) {
            toMoveAgentList.clear();
        }
    }
    POPaintSkill();
    POFieldSelected = false;
    POTools[4]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wcopy.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
    POTools[5]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wpaste.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
}

void playoff::mouseMoved(QMouseEvent *event, QPoint tempPos)
{
    if (POFieldSelected) {
        statusBar->showMessage(QString("%1, %2").arg(tempPos.x()).arg(tempPos.y()),1000);
        if (!POCurrentRobot.isAng) {
            if (POCurrentRobot.index == -1) {
                placeRobot(tempPos, getCurrentAgent(), true);
            }
            else if (POCurrentRobot.index >= 0) {
                moveRobot(tempPos + currentBase, POCurrentRobot.agent, POCurrentRobot.index, true);
            }
        }
        else {
            turnRobotAng(tempPos, POCurrentRobot.agent, POCurrentRobot.index);
        }

        robotGeom tempGeom;
        if (POCurrentRobot.index != -1) {
            tempGeom = getRobotGeom(POCurrentRobot.agent, POCurrentRobot.index);
            if (!POCurrentRobot.isAng) {
                POTBPosX->setText(QString::number(tempPos.x()-25));
                POTBPosY->setText(QString::number(tempPos.y()-25));
            }
            else {
                POTBPosX->setText(QString::number(tempGeom.pos.x()));
                POTBPosY->setText(QString::number(tempGeom.pos.y()));
            }
            POTBPosAng->setText(QString::number((int)tempGeom.angle));
            POTBPosTol->setText(QString::number(tempGeom.tolerance));
        }
    }
    if (rbStarted) {
        QPoint tempPoint = event->pos();
        if (tempPoint.x() > fieldLabel->x() + fieldLabel->width()) {
            tempPoint.setX(fieldLabel->x() + fieldLabel->width());
        }
        if (tempPoint.y() > fieldLabel->y() + fieldLabel->height() + _CenteralWidget_v_offset) {
            tempPoint.setY(fieldLabel->y() + fieldLabel->height() + _CenteralWidget_v_offset);
        }
        if (tempPoint.x() < fieldLabel->x()) {
            tempPoint.setX(fieldLabel->x());
        }
        if (tempPoint.y() < fieldLabel->y() + _CenteralWidget_v_offset) {
            tempPoint.setY(fieldLabel->y() + _CenteralWidget_v_offset);

        }
        if(containPoint(rbBeginPoint, tempPoint, 10)) {
            validRubberBand = false;
            PORubberBand->hide();
        }
        else {
            validRubberBand = true;
            PORubberBand->show();
        }
        PORubberBand->setGeometry(QRect(rbBeginPoint, tempPoint).normalized());
    }
    else if (moveRubberBand) {
        QPoint tempPoint = event->pos();
        QRect tempRect(rbBeginPoint, rbEndPoint);
        QPoint tempMove = tempPoint-baseMove;
        tempRect = tempRect.normalized();
        tempRect.moveTo(tempRect.topLeft()+tempMove);
        if(tempRect.topLeft().x() < fieldLabel->x()) {
            tempRect.moveLeft(fieldLabel->x());
        }
        if (tempRect.bottomRight().x() > fieldLabel->x() + fieldLabel->width()) {
            tempRect.moveRight(fieldLabel->x() + fieldLabel->width());
        }
        if (tempRect.topLeft().y() < fieldLabel->y() + _CenteralWidget_v_offset) {
            tempRect.moveTop(fieldLabel->y() + _CenteralWidget_v_offset);
        }
        if (tempRect.bottomRight().y() > fieldLabel->y() + fieldLabel->height() + _CenteralWidget_v_offset) {
            tempRect.moveBottom(fieldLabel->y() + fieldLabel->height() + _CenteralWidget_v_offset);
        }
        PORubberBand->setGeometry(tempRect);
        rbMoveChange = PORubberBand->geometry().topLeft() - rbMoveBase;
        if (currentTool == TMOVE && !toMoveAgentList.isEmpty()) {
            moveSelectedRobots(toMoveAgentList, rbMoveChange);
        }
    }
}

void playoff::reset()
{
    for (int i = 0; i < 6; i++) {
        robots[i].clear();
    }
    currentRobot.agent = -1;
    currentRobot.index = -1;
    draw();
}

void playoff::draw()
{
    QPixmap tempPix(859, 655);
    tempPix = *fieldPix;
    QPainter painter(&tempPix);

    if (displayMode == 0 || displayMode == -1) {
        drawRobots(painter, 0);
    }
    else {
        drawRobots(painter, 0, false);
    }
    if (displayMode == 1 || displayMode == -1) {
        drawRobots(painter, 1);
    }
    else {
        drawRobots(painter, 1, false);
    }
    if (displayMode == 2 || displayMode == -1) {
        drawRobots(painter, 2);
    }
    else {
        drawRobots(painter, 2, false);
    }
    if (displayMode == 3 || displayMode == -1) {
        drawRobots(painter, 3);
    }
    else {
        drawRobots(painter, 3, false);
    }
    if (displayMode == 4 || displayMode == -1) {
        drawRobots(painter, 4);
    }
    else {
        drawRobots(painter, 4, false);
    }
    if (displayMode == 5 || displayMode == -1) {
        drawRobots(painter, 5);
    }
    else {
        drawRobots(painter, 5, false);
    }
    fieldLabel->setPixmap(tempPix);
}

void playoff::drawRobot(QPainter &painter, int x, int y, QString label, int agent, bool selected, bool blink)
{
    QPen robotPen[3];
    QBrush robotBrush;
    QRect robotRect;
    QTextOption robotFont;

    if (selected) {
        if(blink) {
            robotPen[1].setColor(QColor(250, 194, 128, 64));
        }
        else {
            robotPen[1].setColor(QColor(250, 194, 128));
        }
        robotPen[0].setWidth(2);
        robotBrush.setColor(QColor(57, 103, 128));
        robotBrush.setStyle(Qt::SolidPattern);
        robotPen[0].setColor(QColor(32, 79, 105, 64));
        robotPen[2].setColor(QColor(Qt::white));
    }
    else {
        robotPen[1].setColor(QColor(250, 194, 128));
        robotPen[0].setWidth(2);
        robotBrush.setColor(QColor(57, 103, 128, 64));
        robotBrush.setStyle(Qt::SolidPattern);
        robotPen[0].setColor(QColor(32, 79, 105, 64));
        robotPen[2].setColor(QColor(255, 255, 255, 64));
    }
    robotFont.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    robotRect.setRect(x - 9, y - 9, 18, 18);
    painter.setPen(robotPen[0]);
    painter.setBrush(robotBrush);
    painter.drawEllipse(robotRect);
    painter.setPen(robotPen[1]);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(robotRect, label, robotFont);

    robotRect.setRect(x + 5, y - 10, 9, 9);
    painter.setPen(robotPen[0]);
    painter.setBrush(robotBrush);
    painter.drawEllipse(robotRect);
    painter.setPen(robotPen[2]);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(robotRect, QString::number(agent+1), robotFont);
}

void playoff::drawRobots(QPainter &painter, int tRobotIndex, bool selected)
{
    QPen tempPen;
    tempPen.setWidth(1);

    QPen tempBPen;
    tempBPen.setWidth(3);

    if (tRobotIndex == displayMode || displayMode == -1) {
        tempBPen.setColor(QColor(68, 132, 127));
    }
    else {
        tempBPen.setColor(QColor(68, 132, 127, 64));
    }

    if (tRobotIndex == displayMode || displayMode == -1) {
        tempPen.setColor(QColor(243, 223, 28));
    }
    else {
        tempPen.setColor(QColor(243, 223, 28, 64));
    }

    if (tRobotIndex == 0) {
        if (tRobotIndex == displayMode || displayMode == -1) {
            tempPen.setColor(QColor(243, 223, 28));
        }
        else {
            tempPen.setColor(QColor(243, 223, 28, 64));
        }
    }
    else if (tRobotIndex == 1) {
        if (tRobotIndex == displayMode || displayMode == -1) {
            tempPen.setColor(QColor(191, 25, 34));
        }
        else {
            tempPen.setColor(QColor(191, 25, 34, 64));
        }
    }
    else if (tRobotIndex == 2) {
        if (tRobotIndex == displayMode || displayMode == -1) {
            tempPen.setColor(QColor(25, 191, 148));
        }
        else {
            tempPen.setColor(QColor(25, 191, 148, 64));
        }
    }
    else if (tRobotIndex == 3) {
        if (tRobotIndex == displayMode || displayMode == -1) {
            tempPen.setColor(QColor(255, 131, 29));
        }
        else {
            tempPen.setColor(QColor(255, 131, 29, 64));
        }
    }
    else if (tRobotIndex == 4) {
        if (tRobotIndex == displayMode || displayMode == -1) {
            tempPen.setColor(QColor(231, 106, 80));
        }
        else {
            tempPen.setColor(QColor(231, 106, 80, 64));
        }
    }
    else if (tRobotIndex == 5) {
        if (tRobotIndex == displayMode || displayMode == -1) {
            tempPen.setColor(QColor(9, 106, 80));
        }
        else {
            tempPen.setColor(QColor(9, 106, 80, 64));
        }
    }
    //    SolidLine,
    //    DashLine,
    //    DotLine,
    //    DashDotLine,
    //    DashDotDotLine,
    //    CustomDashLine
    tempPen.setStyle(Qt::DashLine);

    for (int i = 1; i < robots[tRobotIndex].length(); i++) {
        painter.setPen(tempBPen);
        painter.drawLine(QPoint(robots[tRobotIndex].at(i - 1).x, robots[tRobotIndex].at(i - 1).y),
                         QPoint(robots[tRobotIndex].at(i).x, robots[tRobotIndex].at(i).y));
        painter.setPen(tempPen);
        painter.drawLine(QPoint(robots[tRobotIndex].at(i - 1).x, robots[tRobotIndex].at(i - 1).y),
                         QPoint(robots[tRobotIndex].at(i).x, robots[tRobotIndex].at(i).y));
    }

    if (currentRobot.agent != -1 && (displayMode == currentRobot.agent || displayMode == -1)  && !showAll) {
        if (robots[tRobotIndex].length()) {
            tempPen.setWidth(2);
            tempPen.setColor(QColor(209, 26, 28, 64));

            QRect tempCircle(QPoint(robots[currentRobot.agent].at(currentRobot.index).x, robots[currentRobot.agent].at(currentRobot.index).y)-QPoint(25, 25),
                    QSize(50, 50));
            painter.setPen(tempPen);
            painter.drawArc(tempCircle, 0, 360*16);
            QLine tempAngle;
            tempAngle.setP1(QPoint(robots[currentRobot.agent].at(currentRobot.index).x, robots[currentRobot.agent].at(currentRobot.index).y));
            tempAngle.setP2(QPoint(_RobotAngRad*cos( ( robots[currentRobot.agent].at(currentRobot.index).angle/360 )*2*3.1415 ) + robots[currentRobot.agent].at(currentRobot.index).x,
                    _RobotAngRad*sin( ( robots[currentRobot.agent].at(currentRobot.index).angle/360 )*2*3.1415 ) + robots[currentRobot.agent].at(currentRobot.index).y));
            tempPen.setColor(QColor(209, 26, 28));
            painter.setPen(tempPen);
            painter.drawLine(tempAngle);

            painter.drawEllipse(robots[currentRobot.agent].at(currentRobot.index).angRect);

            if (robots[currentRobot.agent].at(currentRobot.index).tolerance > 0) {
                tempPen.setWidth(1);
                tempPen.setColor(QColor(Qt::cyan));
                tempPen.setStyle(Qt::SolidLine);
                painter.setPen(tempPen);
                QPoint tempPoint(robots[currentRobot.agent].at(currentRobot.index).tolerance,
                        robots[currentRobot.agent].at(currentRobot.index).tolerance);
                QRect tempTol(QPoint(robots[currentRobot.agent].at(currentRobot.index).x, robots[currentRobot.agent].at(currentRobot.index).y)-tempPoint,
                        QSize(robots[currentRobot.agent].at(currentRobot.index).tolerance*2,
                        robots[currentRobot.agent].at(currentRobot.index).tolerance*2));
                painter.drawArc(tempTol,0,360*16);
            }
        }
    }
    else if (showAll) {
        tempPen.setStyle(Qt::SolidLine);
        for (int i = 0; i < robots[tRobotIndex].length(); i++) {
            tempPen.setWidth(2);
            tempPen.setColor(QColor(209, 26, 28, 64));

            QRect tempCircle(QPoint(robots[tRobotIndex].at(i).x, robots[tRobotIndex].at(i).y)-QPoint(25, 25),
                             QSize(50, 50));
            painter.setPen(tempPen);
            painter.drawArc(tempCircle, 0, 360*16);
            QLine tempAngle;
            tempAngle.setP1(QPoint(robots[tRobotIndex].at(i).x, robots[tRobotIndex].at(i).y));
            tempAngle.setP2(QPoint(_RobotAngRad*cos( ( robots[tRobotIndex].at(i).angle/360 )*2*3.1415 ) + robots[tRobotIndex].at(i).x,
                                   _RobotAngRad*sin( ( robots[tRobotIndex].at(i).angle/360 )*2*3.1415 ) + robots[tRobotIndex].at(i).y));
            tempPen.setColor(QColor(209, 26, 28));
            painter.setPen(tempPen);
            painter.drawLine(tempAngle);
        }
    }

    double tempArrowW = 10.0;
    double tempAng;
    QPolygon tempPoly;
    QPoint tempP1;
    QPoint tempP2;

    tempPen.setWidth(1);
    tempPen.setColor(QColor(32, 79, 105, 128));
    tempPen.setStyle(Qt::SolidLine);
    painter.setPen(tempPen);
    QBrush arrowBrush;
    arrowBrush.setColor(QColor(32, 79, 105, 128));
    arrowBrush.setStyle(Qt::SolidPattern);
    painter.setBrush(arrowBrush);
    tempPoly.clear();

    for (int i = 0; i < robots[tRobotIndex].length(); i++) {
        if (robots[tRobotIndex].at(i).target.agent != -1 && robots[tRobotIndex].at(i).target.index != -1 ) {
            if (robots[robots[tRobotIndex].at(i).target.agent].count() - 1 >= robots[tRobotIndex].at(i).target.index) {
                for (int j = 0; j < 3; j++) {
                    if (robots[tRobotIndex].at(i).skill[j] == PassSkill) {
                        tempP1.setX(robots[tRobotIndex].at(i).x);
                        tempP1.setY(robots[tRobotIndex].at(i).y);
                        tempP2.setX(robots[robots[tRobotIndex].at(i).target.agent].at(robots[tRobotIndex].at(i).target.index).x);
                        tempP2.setY(robots[robots[tRobotIndex].at(i).target.agent].at(robots[tRobotIndex].at(i).target.index).y);

                        tempAng = atan2(tempP2.y() - tempP1.y(),
                                        tempP2.x() - tempP1.x());
                        tempPoly.append(tempP2);
                        tempPoly.append(QPoint(tempP1.x() + cos(tempAng-1.5707)*tempArrowW, tempP1.y() + sin(tempAng-1.5707)*tempArrowW));
                        tempPoly.append(QPoint(tempP1.x() + cos(tempAng)*tempArrowW*3, tempP1.y() + sin(tempAng)*tempArrowW*3));
                        tempPoly.append(QPoint(tempP1.x() + cos(tempAng+1.5707)*tempArrowW, tempP1.y() + sin(tempAng+1.5707)*tempArrowW));
                        tempPoly.append(tempP2);
                        painter.drawPolygon(tempPoly);
                    }
                }
            }
        }
    }


    tempPen.setWidth(1);
    tempPen.setColor(QColor(165, 17, 19, 100));
    tempPen.setStyle(Qt::SolidLine);
    painter.setPen(tempPen);
    arrowBrush.setColor(QColor(165, 17, 19, 100));
    arrowBrush.setStyle(Qt::SolidPattern);
    painter.setBrush(arrowBrush);
    tempPoly.clear();

    for (int i = 0; i < robots[tRobotIndex].length(); i++) {
        for (int j = 0; j < 3; j++) {
            if (robots[tRobotIndex].at(i).skill[j] == ShotToGoalSkill) {
                tempP1.setX(robots[tRobotIndex].at(i).x);
                tempP1.setY(robots[tRobotIndex].at(i).y);
                tempP2.setX(834);
                tempP2.setY(377 - (100*(double(robots[tRobotIndex].at(i).skillData[j][1])/1000)));

                tempAng = atan2(tempP2.y() - tempP1.y(),
                                tempP2.x() - tempP1.x());

                tempPoly.append(tempP2);
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng-1.5707)*tempArrowW, tempP1.y() + sin(tempAng-1.5707)*tempArrowW));
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng)*tempArrowW*3, tempP1.y() + sin(tempAng)*tempArrowW*3));
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng+1.5707)*tempArrowW, tempP1.y() + sin(tempAng+1.5707)*tempArrowW));
                tempPoly.append(tempP2);
                painter.drawPolygon(tempPoly);
            }
        }
    }

    tempPen.setWidth(1);
    tempPen.setColor(QColor(248, 245, 42, 100));
    tempPen.setStyle(Qt::SolidLine);
    painter.setPen(tempPen);
    arrowBrush.setColor(QColor(248, 245, 42, 100));
    arrowBrush.setStyle(Qt::SolidPattern);
    painter.setBrush(arrowBrush);
    tempPoly.clear();

    for (int i = 0; i < robots[tRobotIndex].length(); i++) {
        for (int j = 0; j < 3; j++) {
            if (robots[tRobotIndex].at(i).skill[j] == ChipToGoalSkill) {
                tempP1.setX(robots[tRobotIndex].at(i).x);
                tempP1.setY(robots[tRobotIndex].at(i).y);
                tempP2.setX(834);
                tempP2.setY(377 - (100*(double(robots[tRobotIndex].at(i).skillData[j][1])/1000)));

                tempAng = atan2(tempP2.y() - tempP1.y(),
                                tempP2.x() - tempP1.x());

                tempPoly.append(tempP2);
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng-1.5707)*tempArrowW, tempP1.y() + sin(tempAng-1.5707)*tempArrowW));
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng)*tempArrowW*3, tempP1.y() + sin(tempAng)*tempArrowW*3));
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng+1.5707)*tempArrowW, tempP1.y() + sin(tempAng+1.5707)*tempArrowW));
                tempPoly.append(tempP2);
                painter.drawPolygon(tempPoly);
            }
        }
    }

    tempPen.setWidth(1);
    tempPen.setColor(QColor(234, 99, 11, 190));
    tempPen.setStyle(Qt::SolidLine);
    painter.setPen(tempPen);
    arrowBrush.setColor(QColor(234, 99, 11, 190));
    arrowBrush.setStyle(Qt::SolidPattern);
    painter.setBrush(arrowBrush);
    tempPoly.clear();

    for (int i = 0; i < robots[tRobotIndex].length(); i++) {
        for (int j = 0; j < 3; j++) {
            if (robots[tRobotIndex].at(i).skill[j] == OneTouchSkill) {
                tempP1.setX(robots[tRobotIndex].at(i).x);
                tempP1.setY(robots[tRobotIndex].at(i).y);
                tempP2.setX(834);
                tempP2.setY(377 - (100*(double(robots[tRobotIndex].at(i).skillData[j][1])/1000)));

                tempAng = atan2(tempP2.y() - tempP1.y(),
                                tempP2.x() - tempP1.x());

                tempPoly.append(tempP2);
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng-1.5707)*tempArrowW, tempP1.y() + sin(tempAng-1.5707)*tempArrowW));
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng)*tempArrowW*3, tempP1.y() + sin(tempAng)*tempArrowW*3));
                tempPoly.append(QPoint(tempP1.x() + cos(tempAng+1.5707)*tempArrowW, tempP1.y() + sin(tempAng+1.5707)*tempArrowW));
                tempPoly.append(tempP2);
                painter.drawPolygon(tempPoly);
            }
        }
    }

    for (int i = 0; i < robots[tRobotIndex].length(); i++) {
        drawRobot(painter,
                  robots[tRobotIndex].at(i).x,
                  robots[tRobotIndex].at(i).y,
                  /*robots[tRobotIndex].at(i).label,*/QString::number(i+1),
                  robots[tRobotIndex].at(i).agent,
                  selected);
    }
}

void playoff::placeRobot(QPoint pos, int tAgent, bool tTemp)
{
    if (pos.x() < 0) {
        pos.setX(0);
    }
    if (pos.x() > fieldLabel->width()-1) {
        pos.setX(fieldLabel->width()-1);
    }
    if (pos.y() < 0) {
        pos.setY(0);
    }
    if (pos.y() > fieldLabel->height()-1) {
        pos.setY(fieldLabel->height()-1);
    }
    for (int i = 0; i < _MAX_ROBOT_COUNT; i++) {
        for (int j = 0; j < robots[i].length(); j++) {
            if (robots[i].at(j).temp) {
                robots[i].removeAt(j);
            }
        }
    }
    playOffRobot tempRobot;
    tempRobot.x = pos.x();
    tempRobot.y = pos.y();
    //tempRobot.label = "A";
    tempRobot.agent = tAgent;
    tempRobot.angle = 0;
    tempRobot.angRect = QRect(QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)-QPoint(4,4),
                              QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)+QPoint(4,4));
    tempRobot.temp = tTemp;
    tempRobot.tolerance = 0;
    tempRobot.skillSize = 1;
    tempRobot.skill[0] = MoveSkill;
    tempRobot.skill[1] = NoSkill;
    tempRobot.skill[2] = NoSkill;
    tempRobot.target.agent = -1;
    tempRobot.target.index = -1;
    tempRobot.skillData[0][0] = tempRobot.skillData[0][1] = -37;
    tempRobot.skillData[1][0] = tempRobot.skillData[1][1] = -37;
    tempRobot.skillData[2][0] = tempRobot.skillData[2][1] = -37;
    tempRobot.IAMode[0] = false;
    tempRobot.IAMode[1] = false;
    tempRobot.IAMode[2] = false;
    robots[tAgent].append(tempRobot);

    draw();
}

void playoff::moveRobot(QPoint pos, int tAgent, int tIndex, bool tTemp)
{
    if (pos.x() < 0) {
        pos.setX(0);
    }
    if (pos.x() > fieldLabel->width()-1) {
        pos.setX(fieldLabel->width()-1);
    }
    if (pos.y() < 0) {
        pos.setY(0);
    }
    if (pos.y() > fieldLabel->height()-1) {
        pos.setY(fieldLabel->height()-1);
    }
    playOffRobot tempRobot;
    tempRobot = robots[tAgent].at(tIndex);
    tempRobot.x = pos.x();
    tempRobot.y = pos.y();
    tempRobot.angRect = QRect(QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)-QPoint(4,4),
                              QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)+QPoint(4,4));
    tempRobot.temp = tTemp;
    robots[tAgent].replace(tIndex, tempRobot);

    draw();
}

void playoff::moveSelectedRobots(QList<agentAndIndex> &tList, QPoint tOffset)
{
    playOffRobot tempRobot;
    for (int i = 0; i < tList.length(); i++) {
        tempRobot = robots[tList.at(i).agent].at(tList.at(i).index);
        tempRobot.x = toMoveAgentFirstPointList.at(i).x() + tOffset.x();
        tempRobot.y = toMoveAgentFirstPointList.at(i).y() + tOffset.y();
        robots[tList.at(i).agent].replace(tList.at(i).index, tempRobot);
    }
    draw();
}

void playoff::turnRobotAng(QPoint pos, int tAgent, int tIndex)
{
    playOffRobot tempRobot;
    tempRobot = robots[tAgent].at(tIndex);
    double tempAng = atan2(pos.y() - tempRobot.y, pos.x() - tempRobot.x);
    tempRobot.angle = tempAng*(360/(2*3.1415));
    tempRobot.angRect = QRect(QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)-QPoint(4,4),
                              QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)+QPoint(4,4));
    robots[tAgent].replace(tIndex, tempRobot);
    draw();
}

void playoff::removeRobot(int tAgent, int tIndex)
{
    robots[tAgent].removeAt(tIndex);
    currentRobot.agent = -1;
    currentRobot.index = -1;
    draw();
}

void playoff::removeSelectedRobots(QList<agentAndIndex> &tList)
{
    for (int i = tList.length() - 1; i >= 0; i--) {
        robots[tList.at(i).agent].removeAt(tList.at(i).index);
    }
    currentRobot.agent = -1;
    currentRobot.index = -1;
    draw();
}

bool playoff::containPoint(QPoint point, QPoint pos, int r)
{
    if (sqrt( (pos.x() - point.x())*(pos.x() - point.x()) + (pos.y() - point.y())*(pos.y() - point.y()) ) <= r) {
        return true;
    }
    else {
        return false;
    }
}

robotAttr playoff::getRobot(QPoint pos, bool passExp, QPoint *_base)
{
    robotAttr tempRobot;
    for (int i = 0; i < 6; i++) {
        if (i == displayMode || displayMode == -1) {
            for (int j = 0; j < robots[i].length(); j++) {
                if(containPoint(pos, QPoint(robots[i].at(j).x, robots[i].at(j).y), 9)) {
                    tempRobot.agent = i;
                    tempRobot.index = j;
                    tempRobot.isAng = false;
                    if (currentRobot.skillNum < 3 && currentRobot.skillNum >= 0) {
                        tempRobot.skillNum = currentRobot.skillNum;
                    }
                    else {
                        tempRobot.skillNum = 0;
                    }
                    if (!passExp) {
                        currentRobot = tempRobot;
                    }
                    _base->setX(robots[i].at(j).x - pos.x());
                    _base->setY(robots[i].at(j).y - pos.y());
                    return tempRobot;
                }
            }
        }
    }

    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        if (robots[currentRobot.agent].at(currentRobot.index).angRect.contains(pos)) {
            tempRobot.agent = currentRobot.agent;
            tempRobot.index = currentRobot.index;
            tempRobot.isAng = true;
            if (currentRobot.skillNum < 3 && currentRobot.skillNum >= 0) {
                tempRobot.skillNum = currentRobot.skillNum;
            }
            else {
                tempRobot.skillNum = 0;
            }
            if (!passExp) {
                currentRobot = tempRobot;
            }
            return tempRobot;
        }
    }

    tempRobot.agent = -1;
    tempRobot.index = -1;
    tempRobot.isAng = false;
    tempRobot.skillNum = 0;
    if (!passExp) {
        currentRobot = tempRobot;
    }
    return tempRobot;
}

void playoff::setDisplayMode(int tMode, bool noDraw)
{
    if (tMode < -1) {
        tMode = -1;
    }
    if (tMode > 5 ) {
        tMode = 5;
    }
    displayMode = tMode;
    if (!noDraw) {
        draw();
    }
}

int playoff::getDisplayMode()
{
    return displayMode;
}

void playoff::setCurrentAgent(int tAgent)
{
    if (tAgent < 0) {
        tAgent = 0;
    }
    if (tAgent > 5) {
        tAgent = 5;
    }
    currentAgent = tAgent;
}

int playoff::getCurrentAgent()
{
    return currentAgent;
}

void playoff::setAgentSize(int tSize)
{
    if (tSize < 1) {
        tSize = 1;
    }
    if (tSize > 6) {
        tSize = 6;
    }
    agentSize = tSize;
}

int playoff::getAgentSize()
{
    return agentSize;
}

void playoff::setShowAllFlag(bool tFlag)
{
    showAll = tFlag;
    draw();
}

robotGeom playoff::getRobotGeom(int tAgent, int tIndex)
{
    if (tAgent < 0) {
        tAgent = 0;
    }
    if (tAgent > 5) {
        tAgent = 5;
    }
    if (tIndex < 0) {
        tIndex = 0;
    }
    if (tIndex > robots[tAgent].length() - 1) {
        tIndex = robots[tAgent].length() - 1;
    }
    robotGeom tempGeom;
    tempGeom.pos = QPoint(robots[tAgent].at(tIndex).x, robots[tAgent].at(tIndex).y)-QPoint(25,25);
    tempGeom.angle = robots[tAgent].at(tIndex).angle;
    tempGeom.tolerance = robots[tAgent].at(tIndex).tolerance;
    return tempGeom;
}

int playoff::getRobotSize(int tAgent)
{
    if (tAgent < 0) {
        tAgent = 0;
    }
    if (tAgent > 5) {
        tAgent = 5;
    }
    return robots[tAgent].length();
}

void playoff::setGeomX(int tX)
{
    QPoint tempPos;
    tempPos.setY(robots[currentRobot.agent].at(currentRobot.index).y);
    tempPos.setX(tX+25);
    moveRobot(tempPos, currentRobot.agent, currentRobot.index);
}

void playoff::setGeomY(int tY)
{
    QPoint tempPos;
    tempPos.setX(robots[currentRobot.agent].at(currentRobot.index).x);
    tempPos.setY(tY+25);
    moveRobot(tempPos, currentRobot.agent, currentRobot.index);
}

void playoff::setGeomAngle(double tAng)
{
    playOffRobot tempRobot;
    tempRobot = robots[currentRobot.agent].at(currentRobot.index);
    tempRobot.angle = tAng;
    tempRobot.angRect = QRect(QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)-QPoint(4,4),
                              QPoint(_RobotAngRad*cos( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.x,
                                     _RobotAngRad*sin( ( tempRobot.angle/360 )*2*3.1415 ) + tempRobot.y)+QPoint(4,4));
    robots[currentRobot.agent].replace(currentRobot.index, tempRobot);
    draw();
}

void playoff::setGeomTolerance(int tTol)
{
    playOffRobot tempRobot;
    tempRobot = robots[currentRobot.agent].at(currentRobot.index);
    tempRobot.tolerance = tTol;
    robots[currentRobot.agent].replace(currentRobot.index, tempRobot);
    draw();
}

void playoff::setCurrentSkillNum(int tNum)
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        if (tNum < 0) {
            tNum = 0;
        }
        if (tNum > robots[currentRobot.agent].at(currentRobot.index).skillSize - 1) {
            tNum = robots[currentRobot.agent].at(currentRobot.index).skillSize - 1;
        }
        currentRobot.skillNum = tNum;
    }
}

int playoff::getCurrentSkillNum()
{
    return currentRobot.skillNum;
}

void playoff::setCurrentSkillSize(int tSize)
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        playOffRobot tempRobot;
        tempRobot = robots[currentRobot.agent].at(currentRobot.index);
        tempRobot.skillSize = tSize;
        robots[currentRobot.agent].replace(currentRobot.index, tempRobot);
    }
}

int playoff::getCurrentSkillSize()
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        return robots[currentRobot.agent].at(currentRobot.index).skillSize;
    }
    return 1;
}

void playoff::setSkill(playOffSkills tSkill, int targetAgent, int targetIndex)
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        if (robots[currentRobot.agent].count()-1 >= currentRobot.index) {
            if(tSkill != MoveSkill) {
                if(currentRobot.skillNum == 0)
                {
                    POOpenSkill(2);
                    setCurrentSkillNum(1);
                }
            }
            playOffRobot tempRobot;
            tempRobot = robots[currentRobot.agent].at(currentRobot.index);
            if (targetAgent != -1 && targetIndex != -1) {
                tempRobot.target.agent = targetAgent;
                tempRobot.target.index = targetIndex;
            }
            else if (tSkill != PassSkill) {
                tempRobot.target.agent = -1;
                tempRobot.target.index = -1;
            }
            if(tSkill == PassSkill) {
                passFlag = true;
                statusBar->setStyleSheet("QStatusBar {color: #2f78b3;}");
                statusBar->showMessage("Select the target agent to pass", 3000);
                for(int j = 0; j < 3; j++) {
                    POPassTarget[j]->setStyleSheet("QLabel { background-color : #0866af; color : #ff8c00; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
                }
            }
            tempRobot.skill[currentRobot.skillNum] = tSkill;
            robots[currentRobot.agent].replace(currentRobot.index, tempRobot);
            POPaintSkill();
            draw();
        }
    }
}

playOffSkills playoff::getSkill(int &targetAgent, int &targetIndex)
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        targetAgent = robots[currentRobot.agent].at(currentRobot.index).target.agent;
        targetIndex = robots[currentRobot.agent].at(currentRobot.index).target.index;
        return robots[currentRobot.agent].at(currentRobot.index).skill[currentRobot.skillNum];
    }
    return NoSkill;
}

playOffSkills playoff::getSkill()
{
    int tX, tY;
    return getSkill(tX, tY);
}

playOffSkills playoff::getSkill(int tSkillNum, int &targetAgent, int &targetIndex)
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        targetAgent = robots[currentRobot.agent].at(currentRobot.index).target.agent;
        targetIndex = robots[currentRobot.agent].at(currentRobot.index).target.index;
        return robots[currentRobot.agent].at(currentRobot.index).skill[tSkillNum];
    }
    return NoSkill;
}

playOffSkills playoff::getSkill(int tSkillNum)
{
    int tX, tY;
    return getSkill(tSkillNum, tX, tY);
}

void playoff::POinitLables()
{
    int frameHeight = POWidget->height(), frameWidth = POWidget->width();
    const int vMargin = 5, hMargin = 10, yStart = 90, boxHeight = 50;
    for (int i = 0; i < 8; i++) {
        PODisplayModeLabel[i] = new QLabel(POWidget);
        PODisplayModeLabel[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        PODisplayModeLabel[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            PODisplayModeLabel[i]->setCursor(Qt::ArrowCursor);
            PODisplayModeLabel[i]->setGeometry(QRect(hMargin, vMargin + yStart, 70, boxHeight));
            PODisplayModeLabel[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;}");
            PODisplayModeLabel[i]->setText("Display\nMode");
        }
        else {
            PODisplayModeLabel[i]->setGeometry(QRect(hMargin + ((frameWidth-(hMargin*2 + 70) )/7)*(i-1) + 70, vMargin + yStart, (frameWidth-(hMargin*2+70))/7, 50));
            PODisplayModeLabel[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            if (i == 1) {
                PODisplayModeLabel[i]->setText("All");
            }
            else {
                PODisplayModeLabel[i]->setText(QString::number(i - 1));
            }
        }
    }

    for (int i = 0; i < 7; i++) {
        POCurrentAgentLabel[i] = new QLabel(POWidget);
        POCurrentAgentLabel[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        POCurrentAgentLabel[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            POCurrentAgentLabel[i]->setCursor(Qt::ArrowCursor);
            POCurrentAgentLabel[i]->setGeometry(QRect(hMargin, vMargin*2 + yStart + boxHeight, 70, boxHeight));
            POCurrentAgentLabel[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;}");
            POCurrentAgentLabel[i]->setText("Current\nAgent");
        }
        else {
            POCurrentAgentLabel[i]->setGeometry(QRect(hMargin + ((frameWidth-(hMargin*2 + 70) )/6)*(i-1) + 70, vMargin*2 + yStart + boxHeight , (frameWidth-(hMargin*2+70))/6, 50));
            POCurrentAgentLabel[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            POCurrentAgentLabel[i]->setText(QString::number(i));
        }
    }

    for (int i = 0; i < 3; i++) {
        POSkills[i] = new QLabel(POWidget);
        POSkills[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        POSkills[i]->setCursor(Qt::PointingHandCursor);
        POSkills[i]->setText("+");
        POSkills[i]->setGeometry(QRect(hMargin, (vMargin + 50)*i + 245, 50, 50));
        POSkills[i]->setStyleSheet("QLabel { background-color : #ddd; color : #999; font-weight: bold; font-size: 16px;} QLabel:HOVER { background-color: #bbb; color: #777;}");
        POSkills[i]->setVisible(false);
        for(int j = 0; j < 2; j++) {
            POTiming[i*2 + j] = new QLineEdit(POWidget);
            POTiming[i*2 + j]->setGeometry(QRect(hMargin*2 + (85+hMargin)*j + 20 + frameWidth/2, (vMargin + 50)*i + 245 + vMargin*2, 45, 50 - vMargin*4));
            POTiming[i*2 + j]->setText("1000");
            POTiming[i*2 + j]->setVisible(false);

            POTimingLable[i*2 + j] = new QLabel(POWidget);
            POTimingLable[i*2 + j]->setStyleSheet("QLabel { background: transparent; color : #999; font-weight: bold; font-size: 10px;}");
            POTimingLable[i*2 + j]->setGeometry(QRect(hMargin*2 + (75+hMargin)*j + frameWidth/2, (vMargin + 50)*i + 245 + vMargin, 50, 50 - vMargin*2));
            POTimingLable[i*2 + j]->setVisible(false);
        }
        POTimingLable[i*2]->setGeometry(QRect(hMargin*2 + frameWidth/2 - 40, (vMargin + 50)*i + 245 + vMargin, 55, 50 - vMargin*2));
        POTimingLable[i*2 + 1]->setGeometry(QRect(hMargin*2 + frameWidth/2 + 70, (vMargin + 50)*i + 245 + vMargin, 55, 50 - vMargin*2));

        POTimingLable[i*2]->setText("Duration\n(ms)");
        POTimingLable[i*2 + 1]->setText("Wait\n(ms)");

        POPassTarget[i] = new QLabel(POWidget);
        POPassTarget[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        POPassTarget[i]->setCursor(Qt::PointingHandCursor);
        POPassTarget[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : #fff; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
        POPassTarget[i]->setGeometry(QRect(hMargin*2 + frameWidth/2 - 95, (vMargin + 50)*i + 245 + vMargin*2, 45, 30));
        POPassTarget[i]->setText("Target");
        POPassTarget[i]->setVisible(false);

        POReceiveIA[i] = new QLabel(POWidget);
        POReceiveIA[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        POReceiveIA[i]->setCursor(Qt::PointingHandCursor);
        POReceiveIA[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : #fff; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
        POReceiveIA[i]->setGeometry(QRect(hMargin*2 + frameWidth/2 - 85, (vMargin + 50)*i + 245 + vMargin*2, 35, 30));
        POReceiveIA[i]->setToolTip("Ignore Angle");
        POReceiveIA[i]->setText("IA");
        POReceiveIA[i]->setVisible(false);

    }
    POSkills[0]->setVisible(true);
    POOpenSkill(1);

    POPaintSkill();

    connect(POTiming[0],SIGNAL(textEdited(QString)),this,SLOT(POLineEdit0_0()));
    connect(POTiming[1],SIGNAL(textEdited(QString)),this,SLOT(POLineEdit0_1()));
    connect(POTiming[2],SIGNAL(textEdited(QString)),this,SLOT(POLineEdit1_0()));
    connect(POTiming[3],SIGNAL(textEdited(QString)),this,SLOT(POLineEdit1_1()));
    connect(POTiming[4],SIGNAL(textEdited(QString)),this,SLOT(POLineEdit2_0()));
    connect(POTiming[5],SIGNAL(textEdited(QString)),this,SLOT(POLineEdit2_1()));

    // tools
    int toolVOffset = 500;
    for (int i = 0; i < 6; i++) {
        POTools[i] = new QLabel(POWidget);
        POTools[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        POTools[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            POTools[i]->setCursor(Qt::ArrowCursor);
            POTools[i]->setGeometry(QRect(hMargin, vMargin + yStart + toolVOffset, 70, boxHeight));
            POTools[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;}");
            POTools[i]->setText("Tool\nMode");
        }
        else {
            POTools[i]->setGeometry(QRect(hMargin + ((frameWidth-(hMargin*2 + 70) )/5)*(i-1) + 70, vMargin + yStart + toolVOffset, (frameWidth-(hMargin*2+70))/5, 50));
            if (i == 1) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wmove.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                POTools[i]->setToolTip("Add/Move");
            }
            else if (i == 2) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wdel.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                POTools[i]->setToolTip("Remove");
            }
            else if (i == 3) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wsel.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                POTools[i]->setToolTip("Select");
            }
            else if (i == 4) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wcopy.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                POTools[i]->setToolTip("Copy");
            }
            else if (i == 5) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url('btn/wpaste.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                POTools[i]->setToolTip("Paste");
            }
        }
    }
    POSetSelectedToolLabel(currentTool);
    // tools

    //POMode
    toolVOffset = 445;
    for (int i = 0; i < 4; i++) {
        POMode[i] = new QLabel(POWidget);
        POMode[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        POMode[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            POMode[i]->setCursor(Qt::ArrowCursor);
            POMode[i]->setGeometry(QRect(hMargin, vMargin + yStart + toolVOffset, 70, boxHeight));
            POMode[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;}");
            POMode[i]->setText("PlayOff\nMode");
        }
        else {
            POMode[i]->setGeometry(QRect(hMargin + ((frameWidth-(hMargin*2 + 70) )/3)*(i-1) + 70, vMargin + yStart + toolVOffset, (frameWidth-(hMargin*2+70))/3, 50));
            POMode[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            if(i == 1) {
                POMode[i]->setText("Kick-Off");
            }
            else if (i == 2) {
                POMode[i]->setText("Direct");
            }
            else if (i == 3) {
                POMode[i]->setText("Indirect");
            }
        }
    }
    POSetSelectedModeLabel(currentPOMode);
    //POMode
}

void playoff::POSetSelectedDisplayLabel(int index)
{
    for (int i = 1; i < getAgentSize()+2; i++) {
        if(index+2 == i) {
            PODisplayModeLabel[i]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        else {
            PODisplayModeLabel[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }
    for (int i = getAgentSize()+2; i < 8; i++) {
        PODisplayModeLabel[i]->setStyleSheet("QLabel { background-color : #bbb; color : #ccc; font-weight: bold;}");
    }
}

void playoff::POSetSelectedCurrentAgentLabel(int index)
{
    for (int i = 1; i < getAgentSize()+1; i++) {
        if(index+1 == i) {
            POCurrentAgentLabel[i]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        else {
            POCurrentAgentLabel[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }
    for (int i = getAgentSize()+1; i < 7; i++) {
        POCurrentAgentLabel[i]->setStyleSheet("QLabel { background-color : #bbb; color : #ccc; font-weight: bold;}");
    }
}

void playoff::POSetSelectedToolLabel(toolMode tTMode)
{
    for (int i = 1; i < 6; i++) {
        if (toolMode(i) == tTMode) {
            if (i == 1) {
                POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wmove.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 2) {
                POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wdel.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 3) {
                POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wsel.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 4) {
                POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wcopy.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 5) {
                POTools[i]->setStyleSheet("QLabel { background-color : #0866af; background-image: url(':btn/wpaste.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
        }
        else {
            if (i == 1) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wmove.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 2) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wdel.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 3) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wsel.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 4) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wcopy.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
            else if (i == 5) {
                POTools[i]->setStyleSheet("QLabel { background-color : #89a1b5; background-image: url(':btn/wpaste.png'); background-repeat: no-repeat; background-position: center center; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
            }
        }
    }
}

void playoff::POSetSelectedModeLabel(POMODE tPOMode)
{
    for (int i = 1; i < 4; i++) {
        if (POMODE(i) == tPOMode) {
            POMode[i]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        else {
            POMode[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }
}

void playoff::POSetCheckedIA()
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        for (int i = 0; i < 3; i++) {
            if (robots[currentRobot.agent].at(currentRobot.index).IAMode[i]) {
                POReceiveIA[i]->setStyleSheet("QLabel { background-color : #0866af; color : #ff8c00; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
            }
            else {
                POReceiveIA[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : #ffffff; font-weight: bold; font-size: 11px;} QLabel:HOVER { background-color: #2f78b3;}");
            }
        }
    }
}

void playoff::POOpenSkill(int index, bool temp)
{
    int frameHeight = POWidget->height(),
            frameWidth = POWidget->width();

    if(temp) {
        setCurrentSkillSize(index);
    }

    for (int i = 0; i < index; i++) {
        POSkills[i]->setText("   Skill: NA");
        POSkills[i]->setGeometry(QRect(POSkills[i]->geometry().topLeft(), QSize(frameWidth-20-5, 50)));
        POSkills[i]->setStyleSheet("QLabel { background-color : #ddd; color : #999; font-weight: bold; font-size: 16px;}");
        POSkills[i]->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        POSkills[i]->setVisible(true);
        switch(getSkill(i)) {
        case MoveSkill:
            POTiming[i*2]->setText("1000");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Duration\n(ms)");
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(false);
            break;
        case ReceivePassSkill:
            POTiming[i*2]->setText("70");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Radius\n(cm)");
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(true);
            break;
        case ReceivePassIASkill:
            POTiming[i*2]->setText("70");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Radius\n(cm)");
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(true);
            break;
        case PassSkill:
            POTiming[i*2]->setText("850");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Kick-Speed\n-1~1023");
            POPassTarget[i]->setVisible(true);
            POReceiveIA[i]->setVisible(false);
            break;
        case ShotToGoalSkill:
            POTiming[i*2]->setText("850");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Kick-Speed\n0~1023");
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(false);
            break;
        case ChipToGoalSkill:
            POTiming[i*2]->setText("850");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Chip\n0~1023");
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(false);
            break;
        case OneTouchSkill:
            POTiming[i*2]->setText("1023");
            POTiming[i*2]->setVisible(true);
            POTimingLable[i*2]->setVisible(true);
            POTimingLable[i*2]->setText("Kick-Speed\n0~1023");
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(false);
            break;
        case NoSkill:
            POTiming[i*2]->setVisible(false);
            POTimingLable[i*2]->setVisible(false);
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(false);
            break;
        }
        if (getSkill(i) == NoSkill || getSkill(i) == -1) {
            POTiming[i*2+1]->setVisible(false);
            POTimingLable[i*2+1]->setVisible(false);
        }
        else if (getSkill(i) == PassSkill) {
            POTiming[i*2+1]->setText("700");
            POTiming[i*2+1]->setVisible(true);
            //POTimingLable[index*2 + j]->setGeometry(QRect(hMargin*2 + (75+hMargin)*j + frameWidth/2, (vMargin + 50)*i + 245 + vMargin, 30, 50 - vMargin*2));
            POTimingLable[i*2+1]->setVisible(true);
            POTimingLable[i*2+1]->setText("Chip\n-1~1023");
        }
        else if (getSkill(i) == ChipToGoalSkill ||
                getSkill(i) == ShotToGoalSkill ||
                getSkill(i) == OneTouchSkill) {
            POTiming[i*2+1]->setText("700");
            POTiming[i*2+1]->setVisible(true);
            //POTimingLable[index*2 + j]->setGeometry(QRect(hMargin*2 + (75+hMargin)*j + frameWidth/2, (vMargin + 50)*i + 245 + vMargin, 30, 50 - vMargin*2));
            POTimingLable[i*2+1]->setVisible(true);
            POTimingLable[i*2+1]->setText("Y-Goal\n0~100.0%");
        }
        else {
            POTiming[i*2+1]->setText("1000");
            POTiming[i*2+1]->setVisible(true);
            //POTimingLable[index*2 + j]->setGeometry(QRect(hMargin*2 + (75+hMargin)*j + frameWidth/2, (vMargin + 50)*i + 245 + vMargin, 30, 50 - vMargin*2));
            POTimingLable[i*2+1]->setVisible(true);
            POTimingLable[i*2+1]->setText("Wait\n(ms)");
        }
        //        }
    }
    POSetCheckedIA();
    if (index<3) {
        POSkills[index]->setVisible(true);
        POSkills[index]->setText("+");
        POSkills[index]->setGeometry(QRect(POSkills[index]->geometry().topLeft(), QSize(50, 50)));
        POSkills[index]->setStyleSheet("QLabel { background-color : #ddd; color : #999; font-weight: bold; font-size: 16px;} QLabel:HOVER { background-color: #bbb; color: #777;}");
        POSkills[index]->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        for (int j = 0; j < 2; j++) {
            POTiming[index*2 + j]->setVisible(false);
            POTimingLable[index*2 + j]->setVisible(false);
        }
        POPassTarget[index]->setVisible(false);
        POReceiveIA[index]->setVisible(false);
    }
    for (int i = index+1; i < 3; i++) {
        POSkills[i]->setVisible(false);
        for (int j = 0; j < 2; j++) {
            POTiming[i*2 + j]->setVisible(false);
            POTimingLable[i*2 + j]->setVisible(false);
        }
        POPassTarget[index]->setVisible(false);
        POReceiveIA[index]->setVisible(false);
    }
}

void playoff::POSetSkill(int tAgent, int tIndex, int tSkillNum, playOffSkills tSkill)
{

}

void playoff::POPaintSkill()
{
    POOpenSkill(getCurrentSkillSize(), false);
    for(int i = 0; i < 3; i++) {
        if(getCurrentSkillNum() == i) {
            POSkills[i]->setStyleSheet("QLabel { background-color : #ddd; color : #999; font-weight: bold; font-size: 16px; border: 1px solid #555;}");
        }
        else {
            POSkills[i]->setStyleSheet("QLabel { background-color : #ddd; color : #999; font-weight: bold; font-size: 16px;}");
        }
    }

    for (int i = 0; i < 3; i++) {
        switch(getSkill(i)) {
        case MoveSkill:
            POSkills[i]->setText("   Skill: Move");
            break;
        case PassSkill:
            POSkills[i]->setText("   Skill: Pass");
            break;
        case ReceivePassSkill:
            POSkills[i]->setText("   Skill: Receive");
            break;
        case ShotToGoalSkill:
            POSkills[i]->setText("   Skill: Shot");
            break;
        case ChipToGoalSkill:
            POSkills[i]->setText("   Skill: Chip");
            break;
        case OneTouchSkill:
            POSkills[i]->setText("   Skill: OneTouch");
            break;
        case NoSkill:
            POPassTarget[i]->setVisible(false);
            POReceiveIA[i]->setVisible(false);
            POTimingLable[i*2]->setVisible(false);
            POTimingLable[i*2 + 1]->setVisible(false);
            POTiming[i*2]->setVisible(false);
            POTiming[i*2 + 1]->setVisible(false);
            break;
        }
        if (currentRobot.agent != -1 && currentRobot.index != -1) {
            if (robots[currentRobot.agent].count()-1 >= currentRobot.index) {
                if (robots[currentRobot.agent].at(currentRobot.index).skillData[i][0] != -37) {
                    POTiming[2*i]->setText(QString::number(robots[currentRobot.agent].at(currentRobot.index).skillData[i][0]));
                }

                if (robots[currentRobot.agent].at(currentRobot.index).skillData[i][1] != -37) {
                    POTiming[2*i + 1]->setText(QString::number(robots[currentRobot.agent].at(currentRobot.index).skillData[i][1]));
                }
            }
        }
    }
    POLineEdit0_0("", true);
    POLineEdit0_1("", true);
    POLineEdit1_0("", true);
    POLineEdit1_1("", true);
    POLineEdit2_0("", true);
    POLineEdit2_1("", true);
}


QList<agentAndIndex> playoff::getSelectedAgents(int filter)
{
    QPoint tempOffset(fieldLabel->x(), fieldLabel->y() + _CenteralWidget_v_offset);
    QRect tempRect(PORubberBand->geometry().topLeft() - tempOffset,
                   PORubberBand->geometry().bottomRight() - tempOffset);
    QList<agentAndIndex> tempList;
    agentAndIndex tempAAI;


    for (int i = 0; i < 6; i++) {
        if (filter == i || filter == -1) {
            for (int j = 0; j < robots[i].length(); j++) {
                if ( tempRect.contains(QPoint(robots[i].at(j).x, robots[i].at(j).y)) ) {
                    tempAAI.agent = i;
                    tempAAI.index = j;
                    tempList.append(tempAAI);
                }
            }
        }
    }

    return tempList;
}

void playoff::fillFirstPointsList(QList<agentAndIndex> &tList)
{
    toMoveAgentFirstPointList.clear();
    for (int i = 0; i < tList.length(); i++) {
        toMoveAgentFirstPointList.append(QPoint(robots[tList.at(i).agent].at(tList.at(i).index).x,
                                         robots[tList.at(i).agent].at(tList.at(i).index).y));
    }
}

void playoff::POCopy(int filter)
{
    for (int i = 0; i < 6; i++) {
        copyRobotList[i].clear();
    }

    QPoint tempOffset(fieldLabel->x(), fieldLabel->y() + _CenteralWidget_v_offset);
    QRect tempRect(PORubberBand->geometry().topLeft() - tempOffset,
                   PORubberBand->geometry().bottomRight() - tempOffset);

    for (int i = 0; i < 6; i++) {
        if (filter == i || filter == -1) {
            for (int j = 0; j < robots[i].length(); j++) {
                if ( tempRect.contains(QPoint(robots[i].at(j).x, robots[i].at(j).y)) ) {
                    copyRobotList[i].append(robots[i].at(j));
                }
            }
        }
    }
}

void playoff::POPaste()
{
    if (!copyRobotList[5].isEmpty()) {
        agentSizeCB->setCurrentIndex(5);
    }
    else if (!copyRobotList[4].isEmpty() && agentSizeCB->currentIndex() < 4) {
        agentSizeCB->setCurrentIndex(4);
    }
    else if (!copyRobotList[3].isEmpty() && agentSizeCB->currentIndex() < 3) {
        agentSizeCB->setCurrentIndex(3);
    }
    else if (!copyRobotList[2].isEmpty() && agentSizeCB->currentIndex() < 2) {
        agentSizeCB->setCurrentIndex(2);
    }
    else if (!copyRobotList[1].isEmpty() && agentSizeCB->currentIndex() < 1) {
        agentSizeCB->setCurrentIndex(1);
    }
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < copyRobotList[i].length(); j++) {
            robots[i].append(copyRobotList[i].at(j));
        }
    }
    draw();
}

POInitPos playoff::getInitPos()
{
    POInitPos tempInitPos;
    playOffSkills tempSkill;
    tempInitPos.ballX = tempInitPos.ballY = -100;
    for (int i = 0; i < 6; i++) {
        tempInitPos.AgentX[i] = tempInitPos.AgentY[i] = -100;
    }
    kkTimeAndIndex temp[6];
    for (int i = 0; i < 6; i++) {
        temp[i].index = -1;
        temp[i].agent = -1;
        temp[i].time = 0;
    }
    bool flag = false;
    bool flagHasPass = false;

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < robots[i].length(); j++) {
            for (int k = 0; k < robots[i].at(j).skillSize; k++) {
                if (robots[i].at(j).skill[k] == PassSkill) {
                    flagHasPass = true;
                    break;
                }
            }
        }
    }

    if (flagHasPass) {
        for (int i = 0; i < 6; i++) {
            flag = false;
            tempSkill = NoSkill;
            for (int j = 0; j < robots[i].length(); j++) {
                for (int k = 0; k < robots[i].at(j).skillSize; k++) {
                    if (robots[i].at(j).skill[k] == PassSkill) {
                        flag = true;
                        tempSkill = robots[i].at(j).skill[k];
                        break;
                    }
                    else {
                        if (robots[i].at(j).skill[k] == MoveSkill) {
                            temp[i].time += robots[i].at(j).skillData[k][0];
                            temp[i].time += robots[i].at(j).skillData[k][1];
                        }
                    }
                }
                if(flag) {
                    temp[i].index = j;
                    temp[i].agent = i;
                    temp[i].skill = tempSkill;
                    break;
                }
            }
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            flag = false;
            tempSkill = NoSkill;
            for (int j = 0; j < robots[i].length(); j++) {
                for (int k = 0; k < robots[i].at(j).skillSize; k++) {
                    if(robots[i].at(j).skill[k] == PassSkill ||
                            robots[i].at(j).skill[k] == ShotToGoalSkill ||
                            robots[i].at(j).skill[k] == ChipToGoalSkill)
                    {
                        flag = true;
                        tempSkill = robots[i].at(j).skill[k];
                        break;
                    }
                    else {
                        temp[i].time += robots[i].at(j).skillData[k][0];
                        temp[i].time += robots[i].at(j).skillData[k][1];
                    }
                }
                if(flag) {
                    temp[i].index = j;
                    temp[i].agent = i;
                    temp[i].skill = tempSkill;
                    break;
                }
            }
        }
    }
    kkTimeAndIndex tempTaI;
    for (int i = 0; i < 6; i++) {
        for (int j = i; j < 6; j++) {
            if (temp[i].time > temp[j].time) {
                tempTaI = temp[j];
                temp[j] = temp[i];
                temp[i] = tempTaI;
            }
        }
    }
    int selectedIndex = -1;
    for (int i = 0; i < 6; i++) {
        if (temp[i].agent != -1) {
            selectedIndex = i;
            break;
        }
    }
    if (selectedIndex != -1) {
        if (temp[selectedIndex].agent != -1) {
            tempInitPos.ballX = robots[temp[selectedIndex].agent].at(temp[selectedIndex].index).x;
            tempInitPos.ballY = robots[temp[selectedIndex].agent].at(temp[selectedIndex].index).y;
        }
    }
    for (int i = 0; i < agentSize; i++) {
        if (selectedIndex != -1) {
            if (temp[selectedIndex].agent == i) {
                continue;
            }
        }
        if (robots[i].count() > 0) {
            tempInitPos.AgentX[i] = robots[i].at(0).x;
            tempInitPos.AgentY[i] = robots[i].at(0).y;
        }
    }
    return tempInitPos;
}

void playoff::insertSkillData(int _row, int _col, int data, bool noupdate)
{
    if (currentRobot.agent != -1 && currentRobot.index != -1) {
        if (robots[currentRobot.agent].count()-1 >= currentRobot.index) {
            playOffRobot tempRobot;
            tempRobot = robots[currentRobot.agent].at(currentRobot.index);
            if (_col == 1) {
                if(robots[currentRobot.agent].at(currentRobot.index).skill[_row] == OneTouchSkill ||
                        robots[currentRobot.agent].at(currentRobot.index).skill[_row] == ShotToGoalSkill ||
                        robots[currentRobot.agent].at(currentRobot.index).skill[_row] == ChipToGoalSkill) {
                    if (data > 1000) {
                        data = 1000;
                    }
                    if (data < 0) {
                        data = 0;
                    }
                }
            }
            tempRobot.skillData[_row][_col] = data;
            robots[currentRobot.agent].replace(currentRobot.index, tempRobot);
            if (!noupdate) {
                POPaintSkill();
            }
        }
    }
}

void playoff::POLineEdit0_0(QString str, bool noupdate)
{
    insertSkillData(0, 0, POTiming[0]->text().toInt(), noupdate);
}

void playoff::POLineEdit0_1(QString str,bool noupdate)
{
    insertSkillData(0, 1, POTiming[1]->text().toInt(), noupdate);
    draw();
}

void playoff::POLineEdit1_0(QString str,bool noupdate)
{
    insertSkillData(1, 0, POTiming[2]->text().toInt(), noupdate);
}

void playoff::POLineEdit1_1(QString str,bool noupdate)
{
    insertSkillData(1, 1, POTiming[3]->text().toInt(), noupdate);
    draw();
}

void playoff::POLineEdit2_0(QString str,bool noupdate)
{
    insertSkillData(2, 0, POTiming[4]->text().toInt(), noupdate);
}

void playoff::POLineEdit2_1(QString str,bool noupdate)
{
    insertSkillData(2, 1, POTiming[5]->text().toInt(), noupdate);
    draw();
}

void playoff::disableSQL()
{
    myPlan->disableSQL();;
}

void playoff::enableSQL()
{
    myPlan->enableSQL();
}

void playoff::apply(int _id)
{
    POInitPos tempInit;
    tempInit = getInitPos();
    myPlan->addPlan(robots, tempInit, currentPOMode, tags, agentSize, _id, chance, lastDist);
}

void playoff::savePlan(QString directory)
{

    planMData tempMData;
    POInitPos tempInit;
    for (int i = 0; i < myPlan->getPlanSize(); i++) {
        myPlan->insertPlanToQList(robots, tempMData, i);
        tempInit = getInitPos();
        myPlan->addPlan(robots, tempInit, tempMData.planMode, tempMData.tags, tempMData.agentSize, i, tempMData.chance, tempMData.lastDist);
    }
    myPlan->changeSQLDir(directory);
    myPlan->savePlan();
    if (myPlan->getPlanSize() > currentId) {
        choosePlan(currentId);
    }
}

int playoff::loadPlan(QString directory)
{
    myPlan->changeSQLDir(directory);
    int check = myPlan->loadPlan();
    /*for(int i = 0; i < myPlan->getPlanSize(); i++)
    {
        loadPOPlan(i);
        init
    }*/
    return check;
}

void playoff::choosePlan(int index)
{
    currentId = index;
    if(index >= myPlan->getPlanSize()) {
        if(restore) {
            restoreP();
        }
        backup = true;
        updateTags("");
    }
    else {
        restore = true;
        if(backup) {
            backupP();
            backup = false;
        }
        planMData tempMData;
        myPlan->insertPlanToQList(robots, tempMData, index);
        POSetSelectedModeLabel(tempMData.planMode);
        currentPOMode = tempMData.planMode;
        //setAgentSize(tempMData.agentSize);
        agentSizeCB->setCurrentIndex(tempMData.agentSize-1);
        setDisplayMode(-1, true);

        passFlag = false;
        currentRobot.agent = -1;
        currentRobot.index = -1;
        currentRobot.skillNum = 0;
        currentAgent = 0;

        draw();
        POPaintSkill();
        updateTags(tempMData.tags);
    }
}

void playoff::backupP()
{
    unsavedMPlan.agentSize = agentSize;
    unsavedMPlan.planMode = currentPOMode;
    for (int i = 0; i < 6; i++) {
        unsavedPlan[i].clear();
        for (int j = 0; j < robots[i].count(); j++) {
            unsavedPlan[i].append(robots[i].at(j));
        }
    }
}

void playoff::restoreP()
{
    for (int i = 0; i < 6; i++) {
        robots[i].clear();
        for (int j = 0; j < unsavedPlan[i].count(); j++) {
            robots[i].append(unsavedPlan[i].at(j));
        }
    }

    POSetSelectedModeLabel(unsavedMPlan.planMode);
    currentPOMode = unsavedMPlan.planMode;
    //setAgentSize(tempMData.agentSize);
    agentSizeCB->setCurrentIndex(unsavedMPlan.agentSize-1);
    setDisplayMode(-1, true);

    passFlag = false;
    currentRobot.agent = -1;
    currentRobot.index = -1;
    currentRobot.skillNum = 0;
    currentAgent = 0;

    draw();
    POPaintSkill();
}

int playoff::getPlanSize()
{
    return myPlan->getPlanSize();
}

void playoff::clean()
{
    POSetSelectedModeLabel(INDIRECT);
    currentPOMode = INDIRECT;
    agentSizeCB->setCurrentIndex(0);
    setDisplayMode(-1, true);

    passFlag = false;
    currentRobot.agent = -1;
    currentRobot.index = -1;
    currentRobot.skillNum = 0;
    currentAgent = 0;
    for (int i = 0; i < 6; i++) {
        robots[i].clear();
    }
    draw();
    POPaintSkill();
}

void playoff::cleanPlans()
{
    myPlan->cleanPlans();
}

void playoff::removePlan(int index)
{
    myPlan->removePlan(index);
}

void playoff::setTags(QString str)
{
    tags = str;
}

/*
    json save and load functions and other functhions
*/
bool playoff::savePlanJson(QString directory)
{

    planMData tempMData;
    POInitPos tempInit;
    for (int i = 0; i < myPlan->getPlanSize(); i++) {
        myPlan->insertPlanToQList(robots, tempMData, i);
        tempInit = getInitPos();
        myPlan->addPlan(robots, tempInit, tempMData.planMode, tempMData.tags, tempMData.agentSize, i, tempMData.chance, tempMData.lastDist);
    }
    myPlan->changeSQLDir(directory);

    QFile saveJSON(directory);
    if(!saveJSON.open(QIODevice::WriteOnly) )
    {
        qWarning("Couldn't save file.");
        return false;
    }
    QJsonObject object;
    writeJSON(object, directory);
    QJsonDocument saveDocJSON(object);
    saveJSON.write( saveDocJSON.toJson() );
    if (myPlan->getPlanSize() > currentId) {
        choosePlan(currentId);
    }
    saveJSON.close();
    return true;
}

void playoff::writeJSON(QJsonObject &json, QString dir) const
{
    QJsonArray array;
    for (int i = 0 ; i < myPlan->planList.size() ; i++)
    {
        QJsonObject object;
        writePlanJSON(object,i); // fill "object" with each plans
        array.append(object); // add each plan in "array"
    }
    json["apiVersion"] = API_VERSION;
    json["id"] = 1;
    json["plans"] = array; // save in json
}

void playoff::writePlanJSON(QJsonObject &json, int index ) const
{
    switch (myPlan->planList[index].planMode)
    {
    case KICKOFF:
        json["planMode"] = "KICKOFF";
        break;
    case DIRECT:
        json["planMode"] = "DIRECT";
        break;
    case INDIRECT:
        json["planMode"] = "INDIRECT";
        break;
    }
    QJsonArray tagsArray;
    QStringList tagList = myPlan->planList[index].tags.split("|");
    for(const auto& tagCH : tagList)
        tagsArray.append(tagCH);
    json["tags"] = tagsArray;

    QJsonObject tempBallInit;
    Vector2D BallPos = convertPos(Vector2I(myPlan->planList[index].initPos.ballX,
                                           myPlan->planList[index].initPos.ballY));
    tempBallInit["x"] = BallPos.x;
    tempBallInit["y"] = BallPos.y;

    json["ballInitPos"] = tempBallInit;

    QJsonArray agentInitPosArray;
    for(int i = 0; i < myPlan->planList[index].agentSize; i++)
    {
        QJsonObject tempInitPos;
        Vector2D agentPos = convertPos(Vector2I(myPlan->planList[index].initPos.AgentX[i],
                                               myPlan->planList[index].initPos.AgentY[i]));
        tempInitPos["x"] = agentPos.x;
        tempInitPos["y"] = agentPos.y;
        agentInitPosArray.append(tempInitPos);
    }
    json["agentInitPos"] = agentInitPosArray;

    QJsonArray agentsArray;
    for(int i = 0; i < myPlan->planList[index].agentSize; i++)
    {
        QJsonObject object;
        object["ID"] = i;
        writePosJSON(object, myPlan->planList[index].AgentPlan[i] );
        agentsArray.append(object);
    }
    json["agents"] = agentsArray;

    json["chance"]   = static_cast<int>(myPlan->planList[index].chance);
    json["lastDist"] = myPlan->planList[index].lastDist;

}

void playoff::writePosJSON(QJsonObject &json , const QList<playOffRobot> &index) const
{
    QJsonArray positions;
    Q_FOREACH(auto i, index)
    {
        QJsonObject object;
        Vector2D pos = convertPos(Vector2I(i.x, i.y));

        object["pos-x"]     = pos.x;
        object["pos-y"]     = pos.y;
        object["angel"]     = i.angle;
        object["tolerance"] = i.tolerance;
        writeSkillJSON(object, i);
        positions.append(object);
    }
    json["positions"] = positions;
}

void playoff::writeSkillJSON( QJsonObject &json, const playOffRobot &index) const
{
    QJsonArray skillTEMP;
    for(int i = 0; i < index.skillSize ; i++)
    {


        QJsonObject object;
        switch (index.skill[i]) {
        case NoSkill:
            object["name"] = "NoSkill";
            break;
        case PassSkill:
        {
            object["name"] = "PassSkill";
            QJsonObject Target;
            Target["index"] = index.target.index;
            Target["agent"] = index.target.agent;
            object["target"] = Target;
        }
            break;
        case ReceivePassSkill:
            object["name"] = "ReceivePassSkill";
            break;
        case ShotToGoalSkill:
            object["name"] = "ShotToGoalSkill";
            break;
        case ChipToGoalSkill:
            object["name"] = "ChipToGoalSkill";
            break;
        case OneTouchSkill:
            object["name"] = "OneTouchSkill";
            break;
        case MoveSkill:
            object["name"] = "MoveSkill";
            break;
        case ReceivePassIASkill:
            object["name"] = "ReceivePassIASkill";
            break;

        }
        object["primary"] = index.skillData[i][0];
        object["secondary"] = index.skillData[i][1];
        object["flag"] = index.IAMode[i];
        skillTEMP.append(object);
    }

    json["skills"] = skillTEMP;
}

int playoff::loadPlanJson(QString directory)
{
    QFile loadJSON (directory);
    if (!loadJSON.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
        return false;
    }
    QByteArray loadJsonData = loadJSON.readAll();
    QJsonDocument loadJsonDoc (QJsonDocument::fromJson(loadJsonData) );
    loadJSON.close();
    readJSON (loadJsonDoc.object() );


    return myPlan->planList.count();
}

void playoff::readJSON(const QJsonObject &playBook)
{
    if (playBook["apiVersion"].toDouble() != API_VERSION) {
        qWarning() << "Can't Load Plan File Version didn't matched."
                   << "Your Version: " << playBook["apiVersion"].toDouble()
                   << "API Version :" << API_VERSION;
        return;
    }

    myPlan->cleanPlans();
    QJsonArray planList = playBook["plans"].toArray();
    Q_FOREACH(QJsonValue planValue, planList) {

        QJsonObject plan = planValue.toObject();
        planStruct planTEMP; // we want to fill planTEMP with plan

        QString PlanMODE = plan["planMode"].toString();
        if      (PlanMODE == "DIRECT")   planTEMP.planMode = DIRECT;
        else if (PlanMODE == "KICKOFF")  planTEMP.planMode = KICKOFF;
        else if (PlanMODE == "INDIRECT") planTEMP.planMode = INDIRECT;

        QJsonArray tagsArray = plan["tags"].toArray();

        if (tagsArray.size()) {
            planTEMP.tags = tagsArray[0].toString();
            for(int i = 1; i < tagsArray.size(); i++) // fill tags
                planTEMP.tags += "|" + tagsArray[i].toString();
        }

        Vector2I ballPos = convertPosInverse(Vector2D(plan["ballInitPos"].toDouble(),
                                                      plan["ballInitPos"].toDouble()));

        planTEMP.initPos.ballX = ballPos.x;
        planTEMP.initPos.ballY = ballPos.y;

        QJsonArray agentInitPosArray = plan["agentInitPos"].toArray();
        for(int i = 0; i < planTEMP.agentSize; i++) {
            Vector2I agentPos = convertPosInverse(Vector2D(agentInitPosArray.at(i).toObject()["x"].toDouble(),
                                                           agentInitPosArray.at(i).toObject()["y"].toDouble()));
            planTEMP.initPos.AgentX[i] = agentPos.x;
            planTEMP.initPos.AgentY[i] = agentPos.y;
        }

        for(int i = planTEMP.agentSize; i < 6; i++) {
            planTEMP.initPos.AgentX[i] = -100;
            planTEMP.initPos.AgentY[i] = -100;
        }

        QJsonArray agentsJSON = plan["agents"].toArray();
        planTEMP.agentSize = agentsJSON.size();

        int counter = 0;
        Q_FOREACH(QJsonValue agentValue, agentsJSON) {
            QJsonObject agent = agentValue.toObject();
            QJsonArray posListJSON = agent["positions"].toArray();
            Q_FOREACH(QJsonValue positionValue, posListJSON) {
                QJsonObject position = positionValue.toObject();
                playOffRobot posTEMP;
                Vector2I pos = convertPosInverse(Vector2D(position["pos-x"].toDouble(),
                                                          position["pos-y"].toDouble()));
                posTEMP.x         = pos.x;
                posTEMP.y         = pos.y;
                posTEMP.angle     = position["angel"].toDouble();
                posTEMP.tolerance = position["tolerance"].toInt();

                QJsonArray skillsJSON = position["skills"].toArray();
                posTEMP.skillSize = skillsJSON.size();
                for(int i = 0; i < posTEMP.skillSize; i++) {
                    QJsonObject skillJSON = skillsJSON[i].toObject();
                    QString SkillName = skillJSON["name"].toString();
                    if  (SkillName == "NoSkill")
                        posTEMP.skill[i] = NoSkill;
                    else if  (SkillName == "PassSkill" )
                        posTEMP.skill[i] = PassSkill;
                    else if  (SkillName == "ReceivePassSkill" )
                        posTEMP.skill[i] = ReceivePassSkill;
                    else if  (SkillName == "ShotToGoalSkill" )
                        posTEMP.skill[i] = ShotToGoalSkill;
                    else if  (SkillName == "ChipToGoalSkill" )
                        posTEMP.skill[i] = ChipToGoalSkill;
                    else if  (SkillName == "OneTouchSkill" )
                        posTEMP.skill[i] = OneTouchSkill;
                    else if  (SkillName == "MoveSkill" )
                        posTEMP.skill[i] = MoveSkill;
                    else if  (SkillName == "ReceivePassIASkill" )
                            posTEMP.skill[i] = ReceivePassIASkill;

                    posTEMP.IAMode[i]       = skillJSON["flag"].toBool();
                    posTEMP.skillData[i][0] = skillJSON["primary"].toInt();
                    posTEMP.skillData[i][1] = skillJSON["secondary"].toInt();

                    if(posTEMP.skill[i] == PassSkill ) {
                        posTEMP.target.agent = skillJSON["target"].toObject()["agent"].toInt();
                        posTEMP.target.index = skillJSON["target"].toObject()["index"].toInt();

                    }
                    else {
                        posTEMP.target.agent = -1;
                        posTEMP.target.index = -1;
                    }
                }
                for(int i = posTEMP.skillSize; i < 3 ; i++) {
                    posTEMP.skill[i] = NoSkill;
                    posTEMP.skillData[i][0] = 1000;
                    posTEMP.skillData[i][1] = 1000;
                    posTEMP.IAMode[i] = false;
                }
                planTEMP.AgentPlan[counter].append(posTEMP);
            }
            counter++;
        }

        planTEMP.chance   = plan["chance"].toInt();
        planTEMP.lastDist = plan["lastDist"].toDouble();
        myPlan->planList.append(planTEMP);
    }
}

Vector2D playoff::convertPos(Vector2I _input) const
{
    // nowhere Exeption
    if (_input.x == -100) return Vector2D(-100.0000, -100.0000);

    _input.x -= 429;
    _input.y -= 328;

    double tempX =  double(_input.x)/404.5;
    double tempY = -double(_input.y)/304;

    tempX *= (9.000/2);
    tempY *= (6.000/2);
    return Vector2D(tempX, tempY);
}

Vector2I playoff::convertPosInverse(Vector2D _input) const
{

    if (_input.x == -100) return Vector2I(-100, -100);

    _input.x /= (9.000/2);
    _input.y /= (6.000/2);

    int tempX = (int)((_input.x)*404.5);
    int tempY = (int)((_input.y)*304);

    tempX += 429;
    tempY += 328;

    return Vector2I(tempX, tempY);
}
