#include "playon.h"

playon::playon(QWidget *parent) :
    QWidget(parent)
{
#if UL_MAC_OSX
    fieldPix = new QPixmap("../../../image/double-sized-field.png");
#else
    fieldPix = new QPixmap("image/double-sized-field.png");
#endif

    bRect[0].setRect(25, 25, 354, 605);
    bRect[1].setRect(379, 25, 455, 177);
    bRect[2].setRect(379, 202, 355, 249);
    bRect[3].setRect(734, 202, 100, 249);
    bRect[4].setRect(379, 451, 455, 179);

    aRect[0].setRect(25, 25, 354, 605);
    aRect[1].setRect(379, 25, 455, 177);
    aRect[2].setRect(379, 202, 355, 249);
    aRect[3].setRect(734, 202, 100, 249);
    aRect[4].setRect(379, 451, 455, 179);

    bAreaSelected = false;
    bTempArea = bSelectedArea = -1;

    for (int i = 0; i < 5; i++) {
        aTempArea[i][0] = aTempArea[i][1] = aSelectedArea[i][0] = aSelectedArea[i][1] = -1;
    }

    aAreaSelected = false;
    timer = new QTimer(parent);
    timer->start(100);
    timerCounter = 0;
    blinkCurrentAgent = true;
    connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));

    currentPlan = new kkAgentPlanClass();
    currentPlan->clear(1);

    myPlan = new playOnPlanSQL("tempplayoff.sql");
    rightClickEmpty = false;
}

playon::~playon()
{
    delete fieldPix;
    delete timer;
    delete currentPlan;
}

void playon::setLabel(QLabel *tLabel)
{
    fieldLabel = tLabel;
}

void playon::setWidget(QWidget *tWidget)
{
    POWidget = tWidget;
    initLabels();
}

void playon::setStatusBar(QStatusBar *_statusBar)
{
    statusBar = _statusBar;
}

void playon::setTabWidget(QTabWidget *_widget)
{
    POTabWidget = _widget;
}

void playon::initLabels()
{
    int tabWidth = POWidget->width();
    tabWidth = 400;
    const int vMargin = 5, hMargin = 10, yStart = 90, boxHeight = 50;
    for (int i = 0; i < 5; i++) {
        playOnEndPolicyLabel[i] = new QLabel(POWidget);
        playOnEndPolicyLabel[i]->setGeometry(QRect(hMargin + ((tabWidth/5)*i), 300, (tabWidth/5), 50));
        playOnEndPolicyLabel[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        playOnEndPolicyLabel[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            playOnEndPolicyLabel[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;font-size: 13px;}");
            playOnEndPolicyLabel[i]->setCursor(Qt::ArrowCursor);
        }
        else {
            playOnEndPolicyLabel[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size: 12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        playOnEndPolicyLabel[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        playOnEndPolicyLabel[i]->setCursor(Qt::PointingHandCursor);
    }
    playOnEndPolicyLabel[0]->setText("End\nPolicy");
    playOnEndPolicyLabel[0]->setGeometry(QRect(10,65, 50, 50));
    playOnEndPolicyLabel[1]->setText("Cycle");
    playOnEndPolicyLabel[1]->setGeometry(QRect(60,65, 48, 50));
    playOnEndPolicyLabel[2]->setText("Exact Agent\nComplete");
    playOnEndPolicyLabel[2]->setGeometry(QRect(108,65, 98, 50));
    playOnEndPolicyLabel[3]->setText("All Agents\nComplete");
    playOnEndPolicyLabel[3]->setGeometry(QRect(206,65, 88, 50));
    playOnEndPolicyLabel[4]->setText("Exact Agent\nDisturbed");
    playOnEndPolicyLabel[4]->setGeometry(QRect(294,65, 96, 50));

    for (int i = 0; i < 6; i++) {
        playOnAgentSize[i] = new QLabel(POWidget);
        playOnAgentSize[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        playOnAgentSize[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            playOnAgentSize[i]->setCursor(Qt::ArrowCursor);
            playOnAgentSize[i]->setGeometry(QRect(hMargin, 10, 50, boxHeight));
            playOnAgentSize[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;font-size:13px;}");
            playOnAgentSize[i]->setText("Agent\nSize");
        }
        else {
            playOnAgentSize[i]->setGeometry(QRect(hMargin + ((tabWidth-(hMargin*2 + 50) )/5)*(i-1) + 50, 10, (tabWidth-(hMargin*2+50))/5, 50));
            playOnAgentSize[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
            playOnAgentSize[i]->setText(QString::number(i));
        }
    }

    for (int i = 0; i < 6; i++) {
        playOnEndPolicyValue[i] = new QLabel(POWidget);
        playOnEndPolicyValue[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        playOnEndPolicyValue[i]->setCursor(Qt::PointingHandCursor);
        if (i == 0) {
            playOnEndPolicyValue[i]->setCursor(Qt::ArrowCursor);
            playOnEndPolicyValue[i]->setGeometry(QRect(hMargin, 120, 50, boxHeight));
            playOnEndPolicyValue[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;font-size:13px;}");
            playOnEndPolicyValue[i]->setText("Value");
        }
        else {
            playOnEndPolicyValue[i]->setGeometry(QRect(hMargin + ((tabWidth-(hMargin*2 + 50) )/5)*(i-1) + 50, 120, (tabWidth-(hMargin*2+50))/5, 50));
            playOnEndPolicyValue[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
            playOnEndPolicyValue[i]->setText(QString::number(i));
        }
    }

    for (int i = 0; i < 3; i++) {
        playOnFieldMode[i] = new QLabel(POWidget);
        playOnFieldMode[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        playOnFieldMode[i]->setCursor(Qt::PointingHandCursor);
        if(i == 0) {
            playOnFieldMode[i]->setCursor(Qt::ArrowCursor);
            playOnFieldMode[i]->setGeometry(QRect(hMargin, 175, 50, boxHeight));
            playOnFieldMode[i]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;font-size:13px;}");
            playOnFieldMode[i]->setText("Ball or\nAgent");
        }
        else {
            playOnFieldMode[i]->setGeometry(QRect(hMargin + ((tabWidth-(hMargin*2 + 50) )/2)*(i-1) + 50, 175, (tabWidth-(hMargin*2+50))/2, 50));
            playOnFieldMode[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }
    playOnFieldMode[1]->setText("Select Ball Region");
    playOnFieldMode[2]->setText("Select Agent Region");

    setQlabelInTabWidget(POWidget, agentLabels, QRect(30,230,365,390),5);

    QRect endPolicyCycleValueLabelRect(60, 120, 330, 50);
    playOnEndPolicyCycleValueLabel = new QLabel(POWidget);
    playOnEndPolicyCycleValueLabel->setGeometry(endPolicyCycleValueLabelRect);
    playOnEndPolicyCycleValueLabel->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;}");

    QRect endPolicyCycleValueRect(65, 125, 320, 40);
    playOnEndPolicyCycleValue = new QLineEdit(POWidget);
    playOnEndPolicyCycleValue->setGeometry(endPolicyCycleValueRect);
    playOnEndPolicyCycleValue->setText("1000");
    playOnEndPolicyCycleValue->setAlignment(Qt::AlignCenter);

    paintAgentLabels();
    paintSettings();
}

//Setting skills button for PalyOn
//For making it more stylish, it has been implimented using QLable
void playon::setQLabel(QWidget *widget, QLabel **label, QRect rect, int index)
{
    //skill lables
    label[index*3] = new QLabel(widget);
    label[index*3]->setGeometry(QRect(rect.left(), rect.top(), rect.width(), (rect.height()*2)/3 ));
    label[index*3]->setText(QString::number(index%4 + 1)+": NA");
    label[index*3]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
    label[index*3]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    label[index*3]->setCursor(Qt::PointingHandCursor);

    //A point lables
    label[index*3 + 1] = new QLabel(widget);
    label[index*3 + 1]->setGeometry(QRect(rect.left(), rect.top() + (rect.height()*2)/3 ,rect.width()/2, rect.height()/3 ));
    label[index*3 + 1]->setText("A:NA");
    label[index*3 + 1]->setStyleSheet("QLabel { background-color : #eec482; color : white; font-weight: bold;} QLabel:HOVER { background-color : #dfa752; }");
    label[index*3 + 1]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    label[index*3 + 1]->setCursor(Qt::PointingHandCursor);

    //B point lables
    label[index*3 + 2] = new QLabel(widget);
    label[index*3 + 2]->setGeometry(QRect(rect.left() + rect.width()/2, rect.top() + (rect.height()*2)/3, rect.width()/2, rect.height()/3 ));
    label[index*3 + 2]->setText("B:NA");
    label[index*3 + 2]->setStyleSheet("QLabel { background-color : #6dad67; color : white; font-weight: bold;} QLabel:HOVER { background-color : #39ad2e; }");
    label[index*3 + 2]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    label[index*3 + 2]->setCursor(Qt::PointingHandCursor);
}

void playon::setQlabelInTabWidget(QWidget *widget, QLabel **label, QRect rect, int size)
{
    int fWidth = rect.width(), fHeight = rect.height(), fX = rect.left(), fY = rect.top();
    int paddingW = 3, paddingH = 3;
    int blockHeight = fHeight/size - paddingH;
    int blockWidth = fWidth / 4 - paddingW;
    QRect tempRect;
    setAgentLabel(widget,QRect(rect.x()-20, rect.y(), 20, blockHeight));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 4; j++) {
            tempRect.setRect(fX + j * (blockWidth + paddingW), fY + i * (blockHeight + paddingH), blockWidth, blockHeight);
            setQLabel(widget, label, tempRect, i * 4 + j);
        }
    }
}

void playon::setAgentLabel(QWidget *widget, QRect rect)
{
    int paddingH = 3;
    for (int i = 0; i < 5; i++) {
        agentLabel[i] = new QLabel(widget);
        agentLabel[i]->setGeometry(QRect(rect.x(), rect.y() + (rect.height()+paddingH) * i, rect.width(), rect.height()-1));
        agentLabel[i]->setText("A\n#\n"+QString::number(i + 1));
        agentLabel[i]->setStyleSheet("QLabel { background-color : #f30c03; color : white; font-weight: bold;}");
        agentLabel[i]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    }
}

void playon::mousePressed(QMouseEvent *event, QPoint tempPos)
{
    if (POTabWidget->underMouse()) {
        QPoint tempPoint;
        tempPoint = POTabWidget->mapToParent(agentLabels[0]->geometry().topLeft());
        tempPoint += QPoint(0, 23);
        if (tempPos.y() < tempPoint.y()) {
            getSelectedSettings();
            paintSettings();
        }
        else {
            if (event->buttons() == Qt::LeftButton) {
                kkSelectedAgent tempSelected = getSelectedAgent();
                if (tempSelected.agent != -1 &&
                    tempSelected.priority != -1 &&
                    tempSelected.part != -1 &&
                    !currentState.isBall) {
                    insertSelectedAgentToState(tempSelected);
                    if(tempSelected.priority != 0) {
                        if (currentPlan->getAgentSkill(currentState.agent + 1, tempSelected.priority) == None &&
                                currentSelected.priority < tempSelected.priority) {
                            return;
                        }
                    }
                    paintAgentLabels();
                    lastState = currentState;
                }
                else {
                    qDebug()<<"out";
                }
            }
            else if (event->buttons() == Qt::RightButton) {
                rightClickEmpty = true;
            }
        }
    }
    else if (fieldLabel->underMouse()) {
        if (event->buttons() == Qt::LeftButton) {
            if (currentState.isBall) {
                bAreaSelected = true;
                bTempArea = getArea(bRect, 5, tempPos);
                if (bSelectedArea != bTempArea) {
                    showBallArea(bTempArea);
                }
            }
            else {//gets agent positions
                aAreaSelected = true;
                aTempArea[currentState.agent][currentState.AorB] = getArea(aRect, 5, tempPos);
                if (aSelectedArea[currentState.agent][currentState.AorB] != aTempArea[currentState.agent][currentState.AorB]) {
                    showAgentArea(aTempArea[currentState.agent][currentState.AorB]);
                }
            }
        }
    }
}

void playon::mouseMoved(QMouseEvent *event, QPoint tempPos)
{
    int tempArea;
    if (fieldLabel->underMouse()) {
        if (bAreaSelected) {
            tempArea = getArea(bRect, 5, tempPos);
            if (tempArea != bTempArea) {
                showBallArea(tempArea);
            }
            bTempArea = tempArea;
        }
        else if (aAreaSelected) {
            tempArea = getArea(aRect, 5, tempPos);
            if (tempArea != aTempArea[currentState.agent][currentState.AorB]) {
                showAgentArea(tempArea);
            }
            aTempArea[currentState.agent][currentState.AorB] = tempArea;
        }
    }
}

void playon::mouseReleased(QMouseEvent *event, QPoint tempPos)
{
    if (fieldLabel->underMouse()) {
        if (bAreaSelected) {
            bSelectedArea = getArea(bRect, 5, tempPos);
            if(bSelectedArea != bTempArea) {
                showBallArea(bSelectedArea);
            }
            currentPlan->setBallPos(bSelectedArea);
        }
        else if (aAreaSelected) {
            aSelectedArea[currentState.agent][currentState.AorB] = getArea(aRect, 5, tempPos);
            showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
            setABPointsLabelText();
            setPlanPoints();
            if (currentState.AorB == 0) {
                activeB();
            }
        }
        bAreaSelected = false;
        aAreaSelected = false;
    }
    else if (POTabWidget->underMouse()) {
        if (rightClickEmpty) {
            kkSelectedAgent tempSelected = getSelectedAgent();
            if(tempSelected.agent != -1 && tempSelected.priority != -1 && tempSelected.part != -1) {
                emptySkill(tempSelected.agent, tempSelected.priority);
            }
            rightClickEmpty = false;
        }
    }
}

kkSelectedAgent playon::getSelectedAgent()
{
    kkSelectedAgent temp;
    for (int i = 0; i < currentState.agentSize; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                if (agentLabels[k + 3*j + 12*i ]->underMouse()) {
                    temp.agent = i;
                    temp.priority = j;
                    temp.part = k;
                    return temp;
                }
            }
        }
    }
    return temp;
}

void playon::paintAgentLabels()
{
    for(int i = 0; i < currentState.agentSize; i++) {
        for(int j = 0; j < 4; j++) {
            if( currentState.agent == i &&
                currentState.priority == j &&
                !currentState.isBall) {
                agentLabels[12*currentState.agent + 3*currentState.priority]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                if (currentState.AorB == 0) {
                    agentLabels[12*currentState.agent + 3*currentState.priority + 1]->setStyleSheet("QLabel { background-color : #0fad00; color : #ffff00; font-weight: bold;} QLabel:HOVER { background-color : #39ad2e; }");
                    agentLabels[12*currentState.agent + 3*currentState.priority + 2]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;} QLabel:HOVER { background-color : #dfa752; }");
                }
                else {
                    agentLabels[12*currentState.agent + 3*currentState.priority + 2]->setStyleSheet("QLabel { background-color : #0fad00; color : #ffff00; font-weight: bold;} QLabel:HOVER { background-color : #39ad2e; }");
                    agentLabels[12*currentState.agent + 3*currentState.priority + 1]->setStyleSheet("QLabel { background-color : #ea8c00; color : white; font-weight: bold;} QLabel:HOVER { background-color : #dfa752; }");
                }
            }
            else {
                agentLabels[i*12 + j*3]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;} QLabel:HOVER { background-color : #2f78b3; }");
                agentLabels[i*12 + j*3 + 1]->setStyleSheet("QLabel { background-color : #eec482; color : white; font-weight: bold;} QLabel:HOVER { background-color : #dfa752; }");
                agentLabels[i*12 + j*3 + 2]->setStyleSheet("QLabel { background-color : #6dad67; color : white; font-weight: bold;} QLabel:HOVER { background-color : #39ad2e; }");
            }
        }
    }
}

void playon::insertSelectedAgentToState(kkSelectedAgent _selected) {
    currentSelected = _selected;
    currentState.agent = _selected.agent;
    currentState.priority = _selected.priority;

    if(currentState.agent == lastState.agent && currentState.priority == lastState.priority) {
        if(_selected.part == 1) {
            currentState.AorB = 0;
        }
        else if(_selected.part == 2) {
            currentState.AorB = 1;
        }
    }
    else {
        if(_selected.part == 2) {
            currentState.AorB = 1;
        }
        else {
            currentState.AorB = 0;
        }
    }
}

void playon::getSelectedSettings() {
    for (int i = 1; i < 6; i++) {
        if (playOnAgentSize[i]->underMouse()) {
            currentState.agentSize = i;
        }
    }

    for (int i = 1; i < 5; i++) {
        if (playOnEndPolicyLabel[i]->underMouse()) {
            currentState.endPolicy = PEndPolicy(i);
        }
    }

    for (int i = 1; i < currentState.agentSize+1; i++) {
        if (playOnEndPolicyValue[i]->underMouse()) {
            currentState.endPolicyValue = i;
        }
    }

    if (playOnFieldMode[1]->underMouse()) {
        currentState.isBall = true;
    }
    else if (playOnFieldMode[2]->underMouse()) {
        currentState.isBall = false;
    }

    currentPlan->setAgentSize(currentState.agentSize);
    if (currentState.endPolicy == Cycle) {
        currentPlan->setEndPolicy(PEndPolicy(currentState.endPolicy), playOnEndPolicyCycleValue->text().toInt());
    }
    else {
        currentPlan->setEndPolicy(PEndPolicy(currentState.endPolicy), currentState.endPolicyValue);
    }

}

void playon::paintSettings() {
    for (int i = 1; i < 6; i++) {
        if (currentState.agentSize == i) {
            playOnAgentSize[i]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        else {
            playOnAgentSize[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }

    for (int i = 1; i < 5; i++) {
        if (currentState.endPolicy == PEndPolicy(i)) {
            playOnEndPolicyLabel[i]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        else {
            playOnEndPolicyLabel[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }

    for (int i = 1; i < 6; i++) {
        if (currentState.endPolicyValue == i) {
            playOnEndPolicyValue[i]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
        else {
            playOnEndPolicyValue[i]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        }
    }


    if (currentState.isBall == true) {
        playOnFieldMode[1]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        playOnFieldMode[2]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        showBallArea(bSelectedArea);
    }
    else {
        playOnFieldMode[2]->setStyleSheet("QLabel { background-color : #0866af; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        playOnFieldMode[1]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: bold;font-size:12px;} QLabel:HOVER { background-color : #2f78b3; }");
        showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
    }

    paintAgentLabels();

    for (int i = 0; i < currentState.agentSize; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                agentLabels[k + 3*j + 12*i]->show();
            }
        }
        agentLabel[i]->show();
    }

    for (int i = currentState.agentSize; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                agentLabels[k + 3*j + 12*i]->hide();
            }
        }
        agentLabel[i]->hide();
    }

    if (currentState.endPolicy == Cycle) {
        for (int i = 1; i < 6; i++) {
            playOnEndPolicyValue[i]->hide();
        }
        playOnEndPolicyCycleValue->show();
        playOnEndPolicyCycleValueLabel->show();
    }
    else {
        for (int i = 1; i < 6; i++) {
            playOnEndPolicyValue[i]->show();
        }
        playOnEndPolicyCycleValue->hide();
        playOnEndPolicyCycleValueLabel->hide();
    }

    if (currentState.agent == -1 && !currentState.isBall) {
        currentState.agent = 0;
        currentState.priority = 0;
        currentState.AorB = 0;
        paintAgentLabels();
    }
    //CHECK ME
    blinkCurrentAgent = true;
}

void playon::drawRobot(QPainter &painter, int x, int y, QString label, int agent, bool selected, bool blink)
{
    QPen robotPen[3];
    QBrush robotBrush;
    QRect robotRect;
    QTextOption robotFont;

    if (blink) {
        robotPen[1].setColor(QColor(239, 154, 42, 64));
    }
    else {
        robotPen[1].setColor(QColor(239, 154, 42));
    }

    robotPen[0].setWidth(2);

    robotBrush.setColor(QColor(57, 103, 128));
    if (selected) {
        robotBrush.setStyle(Qt::SolidPattern);
        robotPen[0].setColor(QColor(32, 79, 105));
    }
    else {
        robotBrush.setStyle(Qt::Dense3Pattern);
        robotPen[0].setColor(QColor(32, 79, 105, 128));
    }

    robotPen[2].setColor(QColor(Qt::white));

    robotFont.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    robotRect.setRect(x + 10, y - 20, 16, 16);
    painter.setPen(robotPen[0]);
    painter.setBrush(robotBrush);
    painter.drawEllipse(robotRect);
    painter.setPen(robotPen[2]);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(robotRect, QString::number(agent), robotFont);

    robotRect.setRect(x - 18, y - 18, 36, 36);

    painter.setPen(robotPen[0]);
    painter.setBrush(robotBrush);
    painter.drawEllipse(robotRect);

    painter.setPen(robotPen[1]);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(robotRect, label, robotFont);

}

//Draws PlayOn robots according to agent size
//It also decide where to draw robots and considers rectabgles layout
void playon::drawRobots(QPainter &painter, int id)
{
    QList<kkRobot> areaAgents[10];
    kkRobot tempRobot;

    for (int i = 0; i < currentState.agentSize; i++) {
        for (int j = 0; j < 2; j++) {
            if (aSelectedArea[i][j] > 0) {
                tempRobot.x = tempRobot.y = -1;
                tempRobot.agent = i + 1;
                if (j == 0) {
                    tempRobot.label = "A";
                    tempRobot.aOrB = 0;
                }
                else {
                    tempRobot.label = "B";
                    tempRobot.aOrB = 1;
                }
                areaAgents[aSelectedArea[i][j] - 1].append(tempRobot);
            }
        }
    }

    int x, y, w = 39, h = 34, hMargin = 5, vMargin = 6;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < areaAgents[i].length(); j++) {
            if ( j == 0) {
                x = aRect[i].x() + w/2 + hMargin;
                y = aRect[i].y() + h/2 + vMargin;
            }
            else {
                x += w + hMargin;
                if (!aRect[i].contains(x, y)) {
                    x = aRect[i].x() + w/2 + hMargin;
                    y += h + vMargin;
                }
            }
            tempRobot.x = x;
            tempRobot.y = y;
            tempRobot.label = areaAgents[i].at(j).label;
            tempRobot.agent = areaAgents[i].at(j).agent;
            tempRobot.aOrB = areaAgents[i].at(j).aOrB;
            areaAgents[i].replace(j, tempRobot);
            drawRobot(painter, areaAgents[i].at(j).x,
                      areaAgents[i].at(j).y, areaAgents[i].at(j).label,
                      areaAgents[i].at(j).agent,
                      (areaAgents[i].at(j).agent - 1 == id && areaAgents[i].at(j).aOrB == currentState.AorB)? true : false,
                      (areaAgents[i].at(j).agent - 1 == id && blinkCurrentAgent)? true : false);
        }
    }
}

int playon::getArea(QRect *rect, int size, QPoint mPos)
{
    for (int i = 0; i < size; i++) {
        if (rect[i].contains(mPos)) {
            return i + 1;
        }
    }
    return -1;
}

void playon::showArea(int id, QRect *rect, int size)
{
    QPixmap temp(859, 655);
    temp = *fieldPix;

    QPainter painter(&temp);

    QPen pen1;
    pen1.setColor(QColor(51, 52, 153));
    pen1.setWidth(2);

    QBrush brush1;
    brush1.setColor(QColor(200, 200, 200, 128));
    brush1.setStyle(Qt::SolidPattern);

    QPen pen2;
    pen2.setColor(QColor(255,255,0));
    pen2.setWidth(2);

    QBrush brush2;
    brush2.setColor(Qt::transparent);
    brush2.setStyle(Qt::SolidPattern);

    QTextOption font1;
    font1.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    painter.setFont( QFont("Arial",36));

    for (int i = 0; i < size; i++) {
        painter.setPen(pen1);
        if (id == i + 1) {
            painter.setBrush(brush1);
        }
        else {
            painter.setBrush(brush2);
        }
        painter.drawRect(rect[i]);
        painter.setPen(pen2);
        painter.drawText(rect[i], QString::number(i + 1),font1);
    }

    if (!currentState.isBall) {
        drawRobots(painter, currentState.agent);
    }

    fieldLabel->setPixmap(temp);

}

void playon::showBallArea(int id)
{
    showArea(id, bRect, 5);
}

//Uses former function for agents
void playon::showAgentArea(int id)
{
    //    id = bSelectedArea;
    showArea(id, aRect, 5);
}

//Actives A point for PlayOn
void playon::activeA()
{
    currentState.AorB = 0;
    paintAgentLabels();
    blinkCurrentAgent = true;
    showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
}

//Actives B point for PlayOn
void playon::activeB()
{
    currentState.AorB = 1;
    paintAgentLabels();
    blinkCurrentAgent = true;
    showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
}

void playon::timerSlot() {
    timerCounter++;
    if (timerCounter > 10) {
        timerCounter = 0;
    }
    if (aAreaSelected && !blinkCurrentAgent) {
        blinkCurrentAgent = true;
        showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
    }
    if (!currentState.isBall && !aAreaSelected) {
        if (timerCounter == 6) {
            blinkCurrentAgent = false;
            showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
        }
        if (timerCounter == 0) {
            blinkCurrentAgent = true;
            showAgentArea(aSelectedArea[currentState.agent][currentState.AorB]);
        }
    }
}

bool playon::setPlan(PSkills skill)
{
    bool temp;
    temp = currentPlan->setAgentSkill(currentState.agent + 1,
                                      currentState.priority + 1,
                                      aSelectedArea[currentState.agent][0],
            aSelectedArea[currentState.agent][1],
            skill);
    setSkillTexts();
    return temp;
}

void playon::setSkillTexts()
{
    for (int i = 0; i < currentState.agentSize; i++) {
        for (int j = 0; j < 4; j++) {
            agentLabels[i*12 + j*3]->setText(QString::number(j + 1)+": "+getSkillTextByEnum(currentPlan->getAgentSkill(i + 1, j + 1)) );
        }
    }
}

bool playon::setPlanPoints()
{
    return currentPlan->setPlanPoints(currentState.agent + 1,
                                      aSelectedArea[currentState.agent][0],
                                      aSelectedArea[currentState.agent][1]);
}

void playon::emptySkill(int agent, int priority)
{
    currentPlan->setAgentSkill(agent + 1,
                               priority + 1,
                               aSelectedArea[agent][0],
            aSelectedArea[priority][1],
            None);
    if (agent == currentState.agent) {
        if (currentPlan->getAgentSkill(currentState.agent + 2, currentState.priority + 2) == None) {
            currentState.priority = currentPlan->getPrioritySize(currentState.agent + 1);
        }
        paintAgentLabels();
    }
    setSkillTexts();
}

QString playon::getSkillTextByEnum(PSkills skill)
{
    switch(skill) {
    default:
    case None:
        return "NA";
    case MoveOffensive:
        return "Move\nOffensive";
    case MoveDefensive:
        return "Move\nDefensive";
    case PassOffensive:
        return "Pass\nOffensive";
    case PassDefensive:
        return "Pass\nDefensive";
    case KickOffensive:
        return "Kick\nOffensive";
    case KickDefensive:
        return "Kick\nDefensive";
    case ChipOffensive:
        return "Chip\nOffensive";
    case ChipDefensive:
        return "Chip\nDefensive";
    case MarkOffensive:
        return "Mark\nOffensive";
    case MarkDefensive:
        return "Mark\nDefensive";
    case OneTouch:
        return "One\nTouch";
    case CatchBall:
        return "Catch\nBall";
    case ReceivePass:
        return "Receive\nPass";
    case Shot:
        return "Shot";
    case ChipToGoal:
        return "Chip to\nGoal";
    }
}

bool playon::isInBallMode()
{
    return currentState.isBall;
}

void playon::disable()
{
    timer->stop();
    blinkCurrentAgent = false;
}

void playon::enable()
{
    timer->start();
}

void playon::setTags(QString str)
{
    currentPlan->setTags(str);
}

void playon::apply(int _id)
{
    QString str;
    statusBar->setStyleSheet("QStatusBar {color: #cc0000;}");
    if (currentPlan->isExecutable(str)) {
        kkAgentPlan tempPlan;
        insertPlanToStruct(tempPlan);
        myPlan->addPlan(_id, tempPlan, str);
        statusBar->setStyleSheet("QStatusBar {color: #00cc00;}");
    }
    statusBar->showMessage(str, 3000);
}

void playon::setABPointsLabelText()
{
    for (int i = 0; i < currentState.agentSize; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 1; k < 3; k++) {
                if(k == 1) {
                    agentLabels[k + j * 3 + i * 12 ]->setText("A:"+((aSelectedArea[i][0] != -1)? QString::number(aSelectedArea[i][0]):"NA"));
                }
                else {
                    agentLabels[k + j * 3 + i * 12 ]->setText("B:"+((aSelectedArea[i][1] != -1)? QString::number(aSelectedArea[i][1]):"NA"));
                }
            }
        }
    }
}

void playon::insertPlanToStruct(kkAgentPlan &plan)
{
    for (int i = 0; i < 5; i++) {
        plan.agents[i].A = currentPlan->getAgentA(i + 1);
        plan.agents[i].B = currentPlan->getAgentB(i + 1);
        plan.agents[i].pSize = currentPlan->getPrioritySize(i + 1);
        for (int j = 0; j < 4; j++) {
            plan.agents[i].skill[j] = currentPlan->getAgentSkill(i + 1, j + 1);
        }
    }
    plan.agentsSize = currentPlan->getAgentSize();
    plan.ball = currentPlan->getBallPos();
    plan.possession = currentPlan->getPossession();
    plan.endMode = currentPlan->getEndPolicyMode();
    plan.endPolicy = currentPlan->getEndPolicyValue();
    plan.planId = currentPlan->getId();
    plan.tags = currentPlan->getTags();
}

void playon::enableSQL()
{
    myPlan->enableSQL();
}

void playon::disableSQL()
{
    myPlan->disableSQL();
}

void playon::savePlan(QString directory)
{
    myPlan->enableSQL();
    myPlan->changeSQLDir(directory);
    myPlan->savePlan();
}

int playon::loadPlan(QString directory)
{
    myPlan->changeSQLDir(directory);
    myPlan->loadPlan();
    return myPlan->getPlanSize();
}

int playon::getPlanSize()
{
    return myPlan->getPlanSize();
}

void playon::choosePlan(int _index)
{
    clearPlan();
    currentPlan->clear();
    if (_index < myPlan->getPlanSize()) {
        currentPlan->loadPlanByStruct(myPlan->getPlan(_index));
        emit updateTags(currentPlan->getTags());
    }
    else {
        emit updateTags("");
    }
    initVarForPainting();
    paintAgentLabels();
    paintSettings();
    setABPointsLabelText();
    setSkillTexts();
    showBallArea(bTempArea);
}

void playon::initVarForPainting()
{
    bTempArea = bSelectedArea = currentPlan->getBallPos();
    for (int i = 0; i < currentPlan->getAgentSize(); i++) {
        aTempArea[i][0] = aSelectedArea[i][0] = currentPlan->getAgentA(i);
        aTempArea[i][1] = aSelectedArea[i][1] = currentPlan->getAgentB(i);
    }
    currentState.endPolicy = currentPlan->getEndPolicyMode();
    currentState.endPolicyValue = currentPlan->getEndPolicyValue();
    if (currentState.endPolicy == Cycle) {
        playOnEndPolicyCycleValue->setText(QString::number(currentState.endPolicyValue));
    }
    currentState.agentSize = currentPlan->getAgentSize();
}

void playon::reset()
{
    currentPlan->clear();
    clearPlan();
    paintAgentLabels();
    paintSettings();
    setSkillTexts();
    showBallArea(bTempArea);
}

void playon::clearPlan()
{
    bAreaSelected = false;
    bTempArea = bSelectedArea = -1;

    for (int i = 0; i < 5; i++) {
        aTempArea[i][0] = aTempArea[i][1] = aSelectedArea[i][0] = aSelectedArea[i][1] = -1;
    }
    aAreaSelected = false;
    currentState.agent = 0;
    currentState.priority = 0;
    currentState.AorB = 0;
    currentState.endPolicy = Cycle;
    currentState.endPolicyValue = 1000;
    currentState.isBall = true;
    currentState.plan = NoSkill;

    currentSelected.agent = -1;
    currentSelected.priority = -1;
    currentSelected.part = -1;

    rightClickEmpty = false;
}

void playon::removePlan(int _index)
{
    myPlan->removePlan(_index);
}

void playon::initPainting()
{
    showBallArea(bTempArea);
    paintAgentLabels();
    paintSettings();
}
