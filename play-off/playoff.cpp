#include "plays/playoff.h"

CRolePlayOff::CRolePlayOff() {
    for(int i = 0;i < 6;i++) {
        gotoPointAvoidSkill = new CSkillGotoPointAvoid(NULL);
        gotoPointSkill = new CSkillGotoPoint(NULL);
        kickSkill = new CSkillKick(NULL);
        oneTouchSkill = new CSkillKickOneTouch(NULL);
        receivePassSkill = new CSkillReceivePass(NULL);
    }
    updated = true;
}

void CRolePlayOff::execute() {
    if(selectedSkill == SkillGotopointAvoid) {
        if(updated) {
            gotoPointAvoidSkill->setFinalPos(target);
            gotoPointAvoidSkill->setAvoidPenaltyArea(avoidPenaltyArea);
            gotoPointAvoidSkill->setMaxVelocity(maxVelocity);
            gotoPointAvoidSkill->setFinalDir(targetDir);
            gotoPointAvoidSkill->setAgent(agent);
            updated = false;
        }
        gotoPointAvoidSkill->execute();
    }
    if(selectedSkill == SkillGotopoint) {
        if(updated) {
            gotoPointSkill->setFinalPos(target);
            gotoPointSkill->setMaxVelocity(maxVelocity);
            gotoPointSkill->setFinalDir(targetDir);
            gotoPointSkill->setAgent(agent);
            updated = false;
        }
        gotoPointSkill->execute();
    }
    if(selectedSkill == SkillKick) {
        if(updated) {
            kickSkill->setTarget(target);
            kickSkill->setAvoidPenaltyArea(avoidPenaltyArea);
            kickSkill->setInterceptMode(intercept);
            kickSkill->setKickSpeed(kickSpeed);
            kickSkill->setChip(chip);
            kickSkill->setAgent(agent);
            updated = false;
        }
        kickSkill->execute();
    }
    if(selectedSkill == SkillReceivePass) {
        if(updated) {
            receivePassSkill->setTarget(target);
            receivePassSkill->setAvoidOppPenaltyArea(avoidPenaltyArea);
            receivePassSkill->setReceiveRadius(receiveRadius);
            receivePassSkill->setAgent(agent);
            updated = false;
        }
        //        receivePassSkill->execute();
    }
    if(selectedSkill == SkillOneTouch) {
        if(updated) {
            oneTouchSkill->setTarget(target);
            //oneTouchSkill->setChip(chip);
            oneTouchSkill->setWaitPos(waitPos);
            //oneTouchSkill->setAvoidPenaltyArea(avoidPenaltyArea);
            oneTouchSkill->setKickSpeed(kickSpeed);
            oneTouchSkill->setAgent(agent);
            updated = false;
        }
        oneTouchSkill->execute();
    }
    if(selectedSkill == SkillMark) {
        kkMarkAgents.append(agent->id());
    }
}

void CRolePlayOff::setUpdated(bool _updated) {
    updated = _updated;
}
bool CRolePlayOff::getUpdated() {
    return updated;
}

CPlayOff::CPlayOff()
{
    radLimit = 10.5;
    directory = QString::fromStdString(policy()->KKPlayOff_KKPOPlanSQL());
    loadSQL();
    loadPlan();
    decidePlan = true;
    firstTime = true;
    agentSize = 1;
    for(int i = 0;i < agentSize;i++) positionAgent[i].stateNumber = 0;
    ballEnteredKickerFlag = false;
    ballEnteredKickerChipFlag = false;
    passReceivedFlag = false;
    isPassDoneflag = false;
    for(int i = 0;i < 6;i++) {
        isFirstTime[i] = true;
        roleAgent[i] = new CRolePlayOff();
    }
    isBallNearRobot = false;
}

CPlayOff::~CPlayOff()
{

}

void CPlayOff::setPlanDir(QString directory)
{
    directory = QString::fromStdString(policy()->KKPlayOff_KKPOPlanSQL());
}

void CPlayOff::loadSQL()
{
    kkPOPlanSQL = QSqlDatabase::addDatabase("QSQLITE");
    kkPOPlanSQL.setDatabaseName(directory);
    //kkPOPlanSQL = QSqlDatabase::database("playoff");
    if (!kkPOPlanSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
                              "Unable to establish a database connection.\n"\
                              "This example needs SQLite support. Please read "\
                              "the Qt SQL driver documentation for information how "\
                              "to build it.\n\n"\
                              "Click Cancel to exit.", QMessageBox::Cancel);
    }
    qDebug()<<"PlayOff SQL Connected!";
}

Vector2D CPlayOff::convertPos(int _x, int _y)
{
    _x -= 429;
    _y -= 328;

    double tempX = double(_x)/404.5;
    double tempY = -double(_y)/304;

    tempX = tempX*(_FIELD_WIDTH/2);
    tempY = tempY*(_FIELD_HEIGHT/2);
    return Vector2D(tempX, tempY);
}

int CPlayOff::loadPlan()
{
    //kkPOPlanSQL = QSqlDatabase::database("playoff");
    QSqlQuery squery;
    squery.exec("SELECT * FROM poplanlist ORDER BY id ASC");
    planListKickOff.clear();
    planListDirect.clear();
    planListIndirect.clear();
    while(squery.next())
    {
        POPlanStruct tempPlan;
        tempPlan.planMode = POMODE(squery.value(2).toInt());
        tempPlan.agentSize = squery.value(3).toInt();
        loadEachPlan(tempPlan, squery.value(1).toString());
        if(squery.value(4).toString() != "na")
        {
            tempPlan.initPos.ball = convertPos(squery.value(4).toString().split("|").at(0).toInt(),
                                               squery.value(4).toString().split("|").at(1).toInt());
        }
        else
        {
            tempPlan.initPos.ball = Vector2D(100, 100);
        }

        for(int i = 0; i < 6; i++)
        {
            if(squery.value(5 + i).toString() != "na")
            {
                tempPlan.initPos.Agent[i] = convertPos(squery.value(5 + i).toString().split("|").at(0).toInt(),
                                                       squery.value(5 + i).toString().split("|").at(1).toInt());
            }
            else
            {
                tempPlan.initPos.Agent[i] = Vector2D(100, 100);
            }
        }
        if(tempPlan.planMode == KICKOFF)
            planListKickOff.append(tempPlan);
        else if(tempPlan.planMode == DIRECT)
            planListDirect.append(tempPlan);
        else if(tempPlan.planMode == INDIRECT)
            planListIndirect.append(tempPlan);
        debug(QString("%1").arg(int(tempPlan.planMode)), D_KK);
    }
    qDebug()<<"PlayOff SQL Loaded!";
    return planListKickOff.count() + planListDirect.count() + planListIndirect.count();

}

void CPlayOff::loadEachPlan(POPlanStruct &_plan, QString _name)
{
    QSqlQuery squery;
    squery.exec("SELECT * FROM "+_name+" ORDER BY id ASC");
    playOffRobot tempStep;
    while(squery.next())
    {
        for(int i = 0; i < 6; i++)
        {
            if(loadSQLtoStruct(squery, i, tempStep))
                _plan.AgentPlan[i].append(tempStep);
        }
    }
}

bool CPlayOff::loadSQLtoStruct(QSqlQuery _query, int _rIndex, playOffRobot &temp)
{
    if(_query.value(_rIndex*7 + 1).toString() == "na")
        return false;
    //playOffRobot temp;
    temp.pos = convertPos(_query.value(_rIndex*7 + 1).toString().split("|").at(0).toInt(),
                          _query.value(_rIndex*7 + 1).toString().split("|").at(1).toInt());
    temp.angle = AngleDeg(_query.value(_rIndex*7 + 1).toString().split("|").at(2).toInt());
    temp.tolerance = _query.value(_rIndex*7 + 2).toDouble()/100;
    temp.skillSize = _query.value(_rIndex*7 + 3).toInt();

    for(int i = 0; i < 3; i++)
    {
        if(_query.value(_rIndex*7 + 4).toString() != "na")
        {
            temp.skill[i] = POffSkills(_query.value(_rIndex*7 + 4 + i).toString().split("|").at(0).toInt());
            temp.skillData[i][0] = _query.value(_rIndex*7 + 4 + i).toString().split("|").at(1).toInt();
            temp.skillData[i][1] = _query.value(_rIndex*7 + 4 + i).toString().split("|").at(2).toInt();
        }
        else
        {
            temp.skill[i] = NoSkill;
            temp.skillData[i][0] = 1000;
            temp.skillData[i][1] = 1000;
        }
    }

    temp.targetAgent = _query.value(_rIndex*7 + 7).toString().split("|").at(0).toInt();
    temp.targetIndex = _query.value(_rIndex*7 + 7).toString().split("|").at(1).toInt();

    return true;
}

void CPlayOff::getMatchedPlan(POMODE _mode, int agentSize, bool _rand)
{
    QList<POPlanStruct> planList;
    POPlanStruct tempPlanStruct;
    if(_mode == KICKOFF) {
        for(int i =0 ;i < planListKickOff.size();i++) {
            copyStructToStruct(tempPlanStruct,planListKickOff.at(i));
            planList.append(tempPlanStruct);
        }

    }
    else if(_mode == DIRECT)
    {
        for(int i =0 ;i < planListDirect.size();i++) {
            copyStructToStruct(tempPlanStruct,planListDirect.at(i));
            planList.append(tempPlanStruct);
        }

    }
    else if(_mode == INDIRECT)
    {
        for(int i =0 ;i < planListIndirect.size();i++) {
            copyStructToStruct(tempPlanStruct,planListIndirect.at(i));
            planList.append(tempPlanStruct);
        }

    }

    Vector2D tempBallPos = wm->ball->pos;
    QList<distAndId> tempBallDist;
    distAndId temp;
    double min = 1000.0;
    int tempId = 0;
    for(int i = 0; i < planList.count(); i++)
    {
        if(planList.at(i).agentSize != agentSize)
        {
            continue;
        }
        temp.id = i;
        temp.dist = planList.at(i).initPos.ball.dist(tempBallPos);
        if(temp.dist < min)
        {
            min = temp.dist;
            tempId = i;
        }
        tempBallDist.append(temp);
    }
    debug(QString("%1, %2").arg(planList.at(0).AgentPlan[1].size()).arg(planList.count()), D_KK);

    QList<POPlanStruct> selectedPlans;
    int cnt = 0;
    for(int i = 0; i < tempBallDist.count(); i++)
    {
        if(tempBallDist.at(i).dist <= radLimit)
        {
            //            selectedPlans.append(planList.at(tempBallDist.at(i).id));
            copyStructToStruct(tempPlanStruct,planList.at(tempBallDist.at(i).id));
            selectedPlans.append(tempPlanStruct);

        }
    }

    if(selectedPlans.isEmpty())
    {
        copyStructToStruct(tempPlanStruct,planList.at(0));
        selectedPlans.append(tempPlanStruct);
        debug("check",D_KK);
    }

    debug(QString("%1, %2,").arg(selectedPlans.count()).arg(selectedPlans.at(0).AgentPlan[1].size()), D_KK);

    for(int i = 0; i < 6; i++)
    {
        currentPlan.AgentPlan[i].clear();
    }
    if(selectedPlans.size() > 0 && _rand)
    {
        int selectedID = rand()%selectedPlans.size();
        //        for(int i = 0; i < 6;i++) {
        //            for(int j = 0;j < selectedPlans.at(selectedID).AgentPlan[i].size();j++)
        //                currentPlan.AgentPlan[i].append(selectedPlans.at(selectedID).AgentPlan[i].at(j));
        //            currentPlan.initPos.Agent[i] = selectedPlans.at(selectedID).initPos.Agent[i];
        //        }
        //        currentPlan.agentSize = selectedPlans.at(selectedID).agentSize;
        //        currentPlan.initPos.ball = selectedPlans.at(selectedID).initPos.ball;
        //        currentPlan.planMode = selectedPlans.at(selectedID).planMode;
        //        selectedPlans.at(selectedID);
        copyStructToStruct(currentPlan,selectedPlans.at(selectedID));
        debug("check",D_KK);

    }
    else
    {
        //        for(int i = 0; i < 6;i++)
        //        {
        //            for(int j = 0; j < currentPlan.AgentPlan[i].count(); j++)
        //            {
        //                for(int j = 0;j < planList.at(tempId).AgentPlan[i].size();j++)
        //                    currentPlan.AgentPlan[i].append(planList.at(tempId).AgentPlan[i].at(j));
        //            }
        //            currentPlan.agentSize = planList.at(tempId).agentSize;
        //            currentPlan.initPos = planList.at(tempId).initPos;
        //            currentPlan.planMode = planList.at(tempId).planMode;
        //        }
        copyStructToStruct(currentPlan,planList.at(tempId));

        //        return planList.at(tempId);
    }

    debug(QString("%1, %2").arg(currentPlan.AgentPlan[1].size()).arg(currentPlan.agentSize), D_KK);

}

void CPlayOff::copyStructToStruct(POPlanStruct &empty,const POPlanStruct &full) {
    empty.planMode = full.planMode;
    empty.agentSize = full.agentSize;
    empty.initPos = full.initPos;
    for(int i = 0;i < 6;i++) {
        empty.AgentPlan[i].clear();
        for(int j = 0;j < full.AgentPlan[i].size();j++) {
            empty.AgentPlan[i].append(full.AgentPlan[i].at(j));
        }
    }
}

void CPlayOff::getPassTimeline(POPlanStruct &tCurrentPlan, QList<POOwnerReceive> &tList)
{
    //QList<POOwnerReceive> tempPaRList;
    tList.clear();
    QList<kkTimeAndIndex> temp;
    kkTimeAndIndex tempStruct;

    tempStruct.agent = -1;
    tempStruct.index = -1;
    tempStruct.time = 0;
    tempStruct.skill = NoSkill;

    for(int i = 0; i < tCurrentPlan.agentSize; i++)
    {
        tempStruct.agent = -1;
        tempStruct.index = -1;
        tempStruct.time = 0;
        tempStruct.skill = NoSkill;
        for(int j = 0; j < tCurrentPlan.AgentPlan[i].count(); j++)
        {
            for(int k = 0; k < tCurrentPlan.AgentPlan[i].at(j).skillSize; k++)
            {
                if(tCurrentPlan.AgentPlan[i].at(j).skill[k] == PassSkill/* ||
                                tCurrentPlan.AgentPlan[i].at(j).skill[k] == ShotToGoalSkill ||
                                tCurrentPlan.AgentPlan[i].at(j).skill[k] == ChipToGoalSkill ||
                                tCurrentPlan.AgentPlan[i].at(j).skill[k] == OneTouchSkill*/)
                {
                    tempStruct.skill = tCurrentPlan.AgentPlan[i].at(j).skill[k];
                    tempStruct.agent = i;
                    tempStruct.index = j;
                    temp.append(tempStruct);

                    tempStruct.agent = -1;
                    tempStruct.index = -1;
                    //tempStruct.time = 0;
                    tempStruct.skill = NoSkill;
                }
                else
                {
                    tempStruct.time += tCurrentPlan.AgentPlan[i].at(j).skillData[k][0];
                    tempStruct.time += tCurrentPlan.AgentPlan[i].at(j).skillData[k][1];
                }
            }
        }
    }

    for(int i = 0; i < temp.count(); i++)
    {
        for(int j = i; j < temp.count(); j++)
        {
            if(temp.at(i).time > temp.at(j).time)
            {
                temp.swap(i, j);
            }
        }
    }

    POOwnerReceive tempOaR;
    for(int i = 0; i < temp.count(); i++)
    {
        tempOaR.ballOwnerAgent = temp.at(i).agent;
        tempOaR.ballOwnerIndex = temp.at(i).index;
        tempOaR.receiveAgent = tCurrentPlan.AgentPlan[tempOaR.ballOwnerAgent].at(tempOaR.ballOwnerIndex).targetAgent;
        tempOaR.receiveIndex = tCurrentPlan.AgentPlan[tempOaR.ballOwnerAgent].at(tempOaR.ballOwnerIndex).targetIndex;
        tempOaR.skill = temp.at(i).skill;
        tList.append(tempOaR);
    }
}

void CPlayOff::globalExecute(int agentCnt) {
    playOnFlag = false;
    if(!firstTime) {
        if(lastAgentCount != agentCnt) {
            playOnFlag = true;
            decidePlan = true;
        }
    }
    else {
        decidePlan = true;
        firstTime = false;
        lastBallPos = wm->ball->pos;
    }
    lastAgentCount = agentCnt;
    mainPlanner(agentCnt);
}

bool CPlayOff::isBallMoved() {
    if(wm->ball->pos.dist(lastBallPos) > 0.05) return true;
    else return false;
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
void CPlayOff::mainPlanner(int tAgentSize) {

    if(decidePlan) {
        reset();
        setAgentSize(tAgentSize);
        lastDecideTime = knowledge->getCurrentKKTime();
        appendRemainingsAgents(activeAgents);
        //        currentPlan =
        getMatchedPlan(getPlayOffMode(),tAgentSize,false);//var types
        //        debug(QString("mode: %1, %2").arg(getPlayOffMode()).arg(knowledge->getGameState()),D_KK);
        insertActiveAgentsToList();
        assignID();
        assignTasks();
        initPosition(positionArg);
        decidePlan = false;
        getPassTimeline(currentPlan, ownerReceiveList);
        for(int i = 0;i < currentPlan.agentSize;i++)
        {
            debug(QString("%1 -> %2").arg(i).arg(kkAgentsID[i]),D_KK);
        }
    }
    debug(QString("receiverID %1, receiver State %2 ").arg(ownerReceiveList.at(0).receiveAgent).arg(ownerReceiveList.at(0).receiveIndex),D_KK);
    for(int i = 0;i < positionAgent[0].positionArg.size();i++)
        draw(positionAgent[0].positionArg.at(i).staticPos);

    fillRolesProperties();
    //            passManager();
    posExecute();
    checkEndState();
    //        if((positioning->isPlanEnd()) || (knowledge->getCurrentKKTime() - lastDecideTime > 60000)) {
    //        executedFlag = true;
    //        decidePlan = true;
    //    }
}
///////////////PassManager///////////////////
void CPlayOff::passManager() {
    if(isPassInCurrentState() && !isReceiveInCurrentState()) {
        terminateReceiverTasks();
    }
}

///////////////////////////////////////////////
bool CPlayOff::isPassInCurrentState() {
    for(int i = 0;i < currentPlan.agentSize;i++) {
        if(positionAgent[i].positionArg.at(positionAgent[i].stateNumber).staticSkill == PassSkill) {
            return true;
        }
    }
    return false;
}

bool CPlayOff::isReceiveInCurrentState() {
    for(int i = 0;i < currentPlan.agentSize;i++) {
        if(positionAgent[i].positionArg.at(positionAgent[i].stateNumber).staticSkill == ReceivePassSkill) {
            return true;
        }
    }
    return false;
}

void CPlayOff::terminateReceiverTasks() {
    isFirstTime[ownerReceiveList.at(0).receiveIndex] = true;
    positionAgent[ownerReceiveList.at(0).receiveIndex].stateNumber = ownerReceiveList.at(0).receiveIndex;
}

///////////////////////////////////////////
void CPlayOff::checkEndState() {
    for(int i = 0;i < agentSize;i++) {
        if(isTaskDone(i)) {
            debug("check",D_KK);
            if(positionAgent[i].stateNumber < positionAgent[i].positionArg.size() - 1)
            {
                positionAgent[i].stateNumber++;
                isFirstTime[i] = true;
            }
            else {
                markAgents.append(knowledge->getAgent(kkAgentsID[i]));
            }
        }
    }
}

bool CPlayOff::isTaskDone(int agentID){
    CAgent* tAgent;
    tAgent = knowledge->getAgent(kkAgentsID[agentID]);
    debug("check",D_KK);

    if(roleAgent[agentID]->getSelectedSkill() == SkillGotopointAvoid) {

        if(isMoveDone(agentID)) return true;
    }
    else if(roleAgent[agentID]->getSelectedSkill() == SkillKick) {
        if(isKickDone(tAgent,agentID)) return true;
    }
    else if(roleAgent[agentID]->getSelectedSkill() == SkillReceivePass) {
        if(isReceiveDone(tAgent))  {
            ownerReceiveList.removeFirst();
            return true;
        }
    }
    else if(roleAgent[agentID]->getSelectedSkill() == SkillOneTouch) {

        if(isKickDone(tAgent,agentID))  return true;
    }
    else {
        debug("kk",D_KK);

        return false;
    }
}



////////////////////////////////////////////
///////////////////////////////////////////
bool CPlayOff::isMoveDone(int agentID) {
    long tempDiffTime;
    tempDiffTime = knowledge->getCurrentKKTime() - positionAgent[agentID].mahiLastTime;
    if(tempDiffTime > positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticWait + positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticTime)
        return true;
    return false;
}

///////////////////////////////////////////
bool CPlayOff::isReceiveDone(CAgent *_agent) {
    if(Circle2D(_agent->pos() + _agent->dir().norm()*0.08,0.08).contains(wm->ball->pos)) return true;
    else return false;
}

///////////////////////////////////////////
bool CPlayOff::isKickDone(CAgent *_agent,int agentID) {
    bool isBallInCorrectWay;
    Vector2D sol1,sol2;
    if(Circle2D(roleAgent[agentID]->getTarget(),0.5).intersection(Ray2D(wm->ball->pos,wm->ball->pos + wm->ball->vel),&sol1,&sol2)) {
        isBallInCorrectWay = true;
    }
    else isBallInCorrectWay = false;
    if(Circle2D(_agent->pos(),0.5).contains(wm->ball->pos)) {
        isBallNearRobot = true;
    }
    else if(isBallNearRobot && isBallInCorrectWay) {
        isBallNearRobot = false;
        return true;
    }

    return false;

}

void CPlayOff::posExecute() {
    for(int i = 0;i < agentSize;i++) {
        if(isFirstTime[i]) {
            positionAgent[i].mahiLastTime = knowledge->getCurrentKKTime();
            isFirstTime[i] = false;
            //                    fillRolesProperties();
            //            roleAgent[i]->setUpdated(false);
            //            roleAgent[i]->updated = false;
        }
        roleAgent[i]->execute();
    }
}



///////////////////////////////////////////////
//////////////////////////////////////////////
void CPlayOff::fillRolesProperties(){
    for(int i = 0;i < agentSize;i++) {
        debug(QString("agent : %1").arg(kkAgentsID[i]),D_KK);
        roleAgent[i]->setAgent(knowledge->getAgent(kkAgentsID[i]));
        assignTask(i,positionAgent[i].positionArg.at(positionAgent[i].stateNumber).staticSkill);
    }
}

void CPlayOff::assignTask(int agentID, POffSkills agentSkill) {
    switch(agentSkill) {
    case NoSkill:
        break;
    case PassSkill:
        roleAgent[agentID]->setAvoidCenterCircle(false);
        roleAgent[agentID]->setAvoidPenaltyArea(true);
        roleAgent[agentID]->setChip(chipOrNot(agentID,positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).PassToId,positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).PassToState));
        roleAgent[agentID]->setKickSpeed(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticTime/25);
        roleAgent[agentID]->setTarget(positionAgent[positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).PassToId].positionArg.at(positionAgent[positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).PassToId].stateNumber).staticPos);
        roleAgent[agentID]->setIntercept(false);
        roleAgent[agentID]->setSelectedSkill(SkillKick);
        break;
    case ReceivePassSkill:
        roleAgent[agentID]->setAvoidPenaltyArea(true);
        roleAgent[agentID]->setTarget(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticPos);
        roleAgent[agentID]->setReceiveRadius(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticEscapeRadius);
        roleAgent[agentID]->setSelectedSkill(SkillReceivePass);
        break;
    case ShotToGoalSkill:
        roleAgent[agentID]->setAvoidCenterCircle(false);
        roleAgent[agentID]->setAvoidPenaltyArea(true);
        roleAgent[agentID]->setChip(false);
        roleAgent[agentID]->setKickSpeed(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticTime);
        roleAgent[agentID]->setTarget(getGoalTarget(agentID,positionAgent[agentID].stateNumber));
        roleAgent[agentID]->setIntercept(false);
        roleAgent[agentID]->setSelectedSkill(SkillKick);
        break;
    case ChipToGoalSkill:
        roleAgent[agentID]->setAvoidCenterCircle(false);
        roleAgent[agentID]->setAvoidPenaltyArea(true);
        roleAgent[agentID]->setChip(true);
        roleAgent[agentID]->setKickSpeed(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticTime);
        roleAgent[agentID]->setTarget(getGoalTarget(agentID,positionAgent[agentID].stateNumber));
        roleAgent[agentID]->setIntercept(false);
        roleAgent[agentID]->setSelectedSkill(SkillKick);
        break;
    case OneTouchSkill:
//        roleAgent[agentID]->setAvoidPenaltyArea(true);
//        roleAgent[agentID]->setChip(false);
        roleAgent[agentID]->setWaitPos(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticPos);
        roleAgent[agentID]->setKickSpeed(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticTime);
        roleAgent[agentID]->setTarget(/*getGoalTarget(agentID,positionAgent[agentID].stateNumber)*/wm->field->oppGoal());
        roleAgent[agentID]->setSelectedSkill(SkillOneTouch);
        break;
    case MoveSkill:
        roleAgent[agentID]->setAvoidPenaltyArea(true);
        roleAgent[agentID]->setTargetDir(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticAng);
        roleAgent[agentID]->setMaxVelocity(getMaxVel(agentID,positionAgent[agentID].stateNumber));
        roleAgent[agentID]->setTarget(getMoveTarget(agentID,positionAgent[agentID].stateNumber));
        roleAgent[agentID]->setSelectedSkill(SkillGotopointAvoid);
        break;
    }
}

Vector2D CPlayOff::getMoveTarget(int agentID,int agentState) {
    Vector2D tempTarget,finalTarget,position;
    double escapeRad;
    int oppCnt = 0;
    bool posFound;
    escapeRad = positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticEscapeRadius;
    position = positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticPos;
    finalTarget = position;
    for(double dist=0.0 ; dist<=escapeRad ; dist+=0.2 ) {
        for(double ang=-180.0 ; ang<=180.0 ; ang+=60.0 ) {
            tempTarget = position + Vector2D::polar2vector(dist,ang);
            for(int i = 0; i < wm->opp.activeAgentsCount();i++) {
                if(Circle2D(wm->opp.active(i)->pos,0.02).contains(tempTarget)) oppCnt++;
            }
            if(!oppCnt) {
                finalTarget = tempTarget;
                posFound = true;
                break;
            }
        }
        if(posFound) break;
    }

    return finalTarget;
}

double CPlayOff::getMaxVel(int agentID,int agentState) {
    double tempVel,tDist;
    Vector2D tAgentPos;
    tAgentPos = knowledge->getAgent(kkAgentsID[agentID])->pos();
    tDist = tAgentPos.dist(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticPos);
    tempVel = tDist/positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticTime;
    if(tempVel >= 4) tempVel = 4;
    else if(tempVel <= 1.5) tempVel = 1.5;
    return tempVel;
}

Vector2D CPlayOff::getGoalTarget(int shoterID,int shoterState) {
    Vector2D tempPos;
    tempPos.assign(wm->field->oppGoal().x,-_GOAL_WIDTH/2 + double(positionAgent[shoterID].positionArg.at(shoterState).staticWait)/1000.0);
}

bool CPlayOff::kkIsPathToOppGoaliClear(Vector2D tPoint)
{
    Segment2D segToOppGoali[5];
    Vector2D tempPos = tPoint;
    int tCnt = 0;
    int intersectCnt = 0;
    for(double y = wm->field->oppGoalR().y; y <= wm->field->oppGoalL().y; y += _GOAL_WIDTH/4.01)
    {
        segToOppGoali[tCnt].assign(tempPos, Vector2D(wm->field->oppGoalR().x, y));
        if(kkCheckIntersectWithAgents(segToOppGoali[tCnt])) {
            intersectCnt++;
            //            segToOppGoali[tCnt]
        }
        draw(segToOppGoali[tCnt], QColor(Qt::green));
        tCnt++;
    }
    if(intersectCnt < 5)
        return true;
    else
        return false;
}

bool CPlayOff::kkCheckIntersectWithAgents(Segment2D tSeg)
{
    Vector2D sol1, sol2;
    for(int i = 0; i < wm->our.activeAgentsCount(); i++)
        if(Circle2D(wm->our.active(i)->pos, 0.10).intersection(tSeg, &sol1, &sol2) > 0)
            return true;

    for(int i = 0; i < wm->opp.activeAgentsCount(); i++)
        if(Circle2D(wm->opp.active(i)->pos, 0.10).intersection(tSeg, &sol1, &sol2) > 0)
            return true;

    return false;
}

bool CPlayOff::chipOrNot(int passerID,int ReceiverID, int ReceiverState){
    if(positionAgent[passerID].positionArg.at(positionAgent[passerID].stateNumber).staticTime < 0) return false;
    else if(positionAgent[passerID].positionArg.at(positionAgent[passerID].stateNumber).staticWait < 0) return true;
    else return isPathClear(wm->ball->pos,positionAgent[ReceiverID].positionArg.at(ReceiverState).staticPos,0.5,0.1);
}

bool CPlayOff::isPathClear(Vector2D _pos1,Vector2D _pos2, double _radius,double treshold) {
    Vector2D sol1,sol2,sol3;
    Line2D _path(_pos1,_pos2);
    Polygon2D _poly;
    Circle2D(_pos2,_radius + treshold).intersection(_path.perpendicular(_pos2),&sol1,&sol2);
    _poly.addVertex(sol1);
    sol3 = sol1;
    _poly.addVertex(sol2);
    Circle2D(_pos1,CRobot::robot_radius_new + treshold).intersection(_path.perpendicular(_pos1),&sol1,&sol2);
    _poly.addVertex(sol1);
    _poly.addVertex(sol2);
    _poly.addVertex(sol3);

    for(int i = 0;i < wm->opp.activeAgentsCount();i++) {
        if(_poly.contains(wm->opp.active(1)->pos)) return false;
    }
    return true;
}

void CPlayOff::setAgentSize(int _agentSize){
    agentSize = _agentSize;
}

void CPlayOff::assignTasks()
{
    positioningArg tempPosArg;
    for(int i = 0; i < currentPlan.agentSize;i++)
    {
        positionArg[i].clear();
        for(int j = 0;j < currentPlan.AgentPlan[i].size();j++)
        {
            for(int k = 0;k < currentPlan.AgentPlan[i].at(j).skillSize;k++)
            {
                tempPosArg.staticPos = currentPlan.AgentPlan[i].at(j).pos;
                tempPosArg.staticAng = Vector2D::polar2vector(1,currentPlan.AgentPlan[i].at(j).angle);
                tempPosArg.staticEscapeRadius = currentPlan.AgentPlan[i].at(j).tolerance;
                tempPosArg.staticSkill = currentPlan.AgentPlan[i].at(j).skill[k];
                tempPosArg.PassToId = currentPlan.AgentPlan[i].at(j).targetAgent;
                tempPosArg.PassToState = currentPlan.AgentPlan[i].at(j).targetIndex;
                tempPosArg.staticWait = currentPlan.AgentPlan[i].at(j).skillData[k][0]/5;
                tempPosArg.staticTime = currentPlan.AgentPlan[i].at(j).skillData[k][1]/5;
                tempPosArg.currentIndex = j;
                positionArg[i].append(tempPosArg);
            }
        }
    }
}

void CPlayOff::findPasserIndex() {
    firstPasserID = -1;
    debug(QString("current size : %1 , agentplansize %2").arg(currentPlan.agentSize).arg(currentPlan.AgentPlan[0].size()),D_KK);
    for(int i = 0;i < currentPlan.agentSize;i++) {
        if(currentPlan.AgentPlan[i].size())
        {
            if(currentPlan.initPos.ball != Vector2D(100,100))
            {
                if(currentPlan.initPos.Agent[i] == Vector2D(100,100))
                {
                    firstPasserID = i;
                }
            }
        }
    }
}

void CPlayOff::assignID() {
    double distPoint[6][6];
    double minDist = 1000;
    int passerID = -1;
    kkValue tempValue;
    QList<kkValue> tempListValue;
    Vector2D tempBallPos = wm->ball->pos;

    for(int i = 0;i < 6;i++)
        tempValue.IDs[i] = -1;
    tempValue.value = 0;
    tempValue.agentSize = 6;

    findPasserIndex();

    debug(QString("x : %1,y : %2").arg(currentPlan.initPos.ball.x).arg(currentPlan.initPos.ball.y),D_KK);

    if(firstPasserID != -1) {
        for(int i = 0;i < activeAgents.size();i++) {
            if(tempBallPos.dist(agentList.at(i).pos) < minDist) {
                minDist = tempBallPos.dist(agentList.at(i).pos);
                passerID = i;
            }
        }
    }
    debug(QString("skill %1").arg(currentPlan.AgentPlan[0].at(0).skill[1]),D_KK);
    debug(QString("passer : %1,PasserID : %2").arg(firstPasserID).arg(passerID),D_KK);


    QList<kkRobot> tempList1, tempList2;
    kkRobot tempAgent;
    for(int i = 0; i < currentPlan.agentSize; i++)
    {
        if(firstPasserID == i)
            continue;
        tempAgent = agentList.at(i);
        tempList1.append(tempAgent);
    }
    for(int i = 0; i < agentList.count(); i++)
    {
        if(passerID == i && firstPasserID != -1)
            continue;
        tempAgent = agentList.at(i);
        tempList2.append(tempAgent);
    }


    for(int i = 0; i < tempList1.count(); i++)
        for(int j = 0; j < tempList2.count(); j++)
        {
            distPoint[i][j] = tempList1.at(i).pos.dist(tempList2.at(j).pos);
        }

    getCostRec(distPoint, tempList1.count(), tempListValue, tempValue, tempList1.count(), 0);

    double minValue = 10000;
    int bestMatchIndex = 0;
    for(int i = 0; i < tempListValue.length(); i++)
    {
        if(tempListValue.at(i).value < minValue)
        {
            minValue = tempListValue.at(i).value;
            bestMatchIndex = i;
        }
    }

    if(firstPasserID != -1)
    {
        debug(QString("AgentSize: %1,").arg(currentPlan.agentSize),D_KK);
        for(int i = 0; i < currentPlan.agentSize; i++)
        {
            if(i == firstPasserID)
                kkAgentsID[i] = agentList.at(passerID).id;
            else if( i < firstPasserID)
                kkAgentsID[i] = tempList2.at(tempListValue.at(bestMatchIndex).IDs[i]).id;
            else if( i > firstPasserID)
                kkAgentsID[i] = tempList2.at(tempListValue.at(bestMatchIndex).IDs[i-1]).id;
        }
    }
    else
    {
        for(int i = 0; i < tempList1.count(); i++)
        {
            kkAgentsID[i] = agentList.at(tempListValue.at(bestMatchIndex).IDs[i]).id;
        }
    }
}

void CPlayOff::getCostRec(double costArr[][6], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId)
{
    if(size <= 1) {
        int tIndex = kkGetIndex(value, 0);
        value.IDs[tIndex] = aId;
        value.value += costArr[tIndex][aId];
        valueList.append(value);
        return;
    }
    for(int i = 0; i < size; i++) {
        kkValue tValue = value;
        int tIndex = kkGetIndex(tValue, i);
        tValue.IDs[tIndex] = aId;
        tValue.value += costArr[tIndex][aId];
        getCostRec(costArr, arrSize, valueList, tValue, size - 1, aId + 1);
    }
}

int CPlayOff::kkGetIndex(kkValue &value, int cIndex)
{
    int cnt = 0;
    for(int i = 0; i < value.agentSize; i++)
    {
        if(value.IDs[i] == -1)
        {
            if(cnt == cIndex)
                return i;
            cnt++;
        }
    }
}

int CPlayOff::insertActiveAgentsToList()
{
    int activeAgentsCount = activeAgents.length();
    agentList.clear();
    kkRobot tempRobot;
    for(int i = 0; i < activeAgents.length(); i++)
    {
        tempRobot.pos = activeAgents.at(i)->pos();
        tempRobot.vel = activeAgents.at(i)->vel();
        tempRobot.dir = activeAgents.at(i)->dir();
        tempRobot.id  = activeAgents.at(i)->id();
        agentList.append(tempRobot);
    }
    for(int i = activeAgentsCount; i < 6; i++)
    {
        tempRobot.pos = Vector2D(0,0);
        tempRobot.vel = Vector2D(0,0);
        tempRobot.dir = Vector2D(0,0);
        tempRobot.id = -1;
        agentList.append(tempRobot);
    }
    return activeAgentsCount;
}

POMODE CPlayOff::getPlayOffMode() {
    if(knowledge->getGameState() == knowledge->OurKickOff || knowledge->getGameMode() == knowledge->OurKickOff) return KICKOFF;
    else if(knowledge->getGameState() == knowledge->OurIndirectKick || knowledge->getGameMode() == knowledge->OurIndirectKick) return INDIRECT;
    else if(knowledge->getGameState() == knowledge->OurDirectKick || knowledge->getGameMode() == knowledge->OurDirectKick) return DIRECT;
}

void CPlayOff::initPosition(QList<positioningArg> _posArg[]){
    for(int i = 0;i < agentSize;i++) {
        positionAgent[i].positionArg.clear();
        for(int j = 0;j < _posArg[i].size();j++) {
            positionAgent[i].positionArg.append(_posArg[i].at(j));
        }
    }
}

void CPlayOff::reset(){
    executedCycles = 0;
    activeAgents.clear();
    markAgents.clear();
    for(int i = 0;i < 6;i++) {
        positionAgent[i].stateNumber = 0;
        isFirstTime[i] = 0;
    }

}

void CPlayOff::init(QList<int> _agents , QMap<QString , EditData*> *_editData){
    setAgentsID(_agents);
    setEditData(_editData);
    initMaster();

    if( knowledge->getLastPlayExecuted() != TheirDirectPlay ){
        reset();
    }
    knowledge->setLastPlayExecuted(TheirDirectPlay);
}

void CPlayOff::execute_0(){
    globalExecute(0);
}

void CPlayOff::execute_1(){
    globalExecute(1);
}

void CPlayOff::execute_2(){
    globalExecute(2);
}

void CPlayOff::execute_3(){
    globalExecute(3);
}

void CPlayOff::execute_4(){
    globalExecute(4);
}

void CPlayOff::execute_5(){
    globalExecute(5);
}

void CPlayOff::execute_6(){
    globalExecute(6);
}

