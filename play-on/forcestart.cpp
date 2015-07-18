#include "plays/forcestart.h"

#define LOG(text) debug(text , D_MASOOD , "blue");

#ifndef KK_PLAYON
CForceStart::CForceStart(){
}

CForceStart::~CForceStart(){
}

void CForceStart::reset(){
    position.reset();
    executedCycles = 0;
}

void CForceStart::init(QList<int> _agents , QMap<QString , EditData*> *_editData){
    setAgentsID(_agents);
    setEditData(_editData);
    initMaster();

    if( knowledge->getLastPlayExecuted() != StartPlay ){
        reset();
    }
    knowledge->setLastPlayExecuted(StartPlay);
}

void CForceStart::onePlayMakeOtherPositions(){
    executedCycles++;

    choosePlayMaker();

    playMakeRole.setKickMode(NoMode);
    playMakeRole.setSlow(false);
    playMakeRole.setThrough(true);
    playMakeRole.setNoKick(false);


    positionAgents.clear();
    appendRemainingsAgents(markAgents);

    for( int i=0 ; i<markAgents.size() ; i++ ){
        if( markAgents.at(i)->intention != NULL && markAgents.at(i)->intention->finished() == false && markAgents.at(i)->intention->M_type == "position"){
            positionAgents.append(markAgents.at(i));
            markAgents.removeAt(i);
            i--;
        }
    }

    setFormation("PlayOnOffense");
}

void CForceStart::execute_0(){

}

void CForceStart::execute_1(){
    executedCycles++;

    choosePlayMaker();

    playMakeRole.setKickMode(NoMode);
    playMakeRole.setSlow(true);
    playMakeRole.setThrough(true);
    playMakeRole.setNoKick(false);
}

void CForceStart::execute_2(){
    onePlayMakeOtherPositions();
}

void CForceStart::execute_3(){
    onePlayMakeOtherPositions();
}

void CForceStart::execute_4(){
    onePlayMakeOtherPositions();
}

void CForceStart::execute_5(){
    onePlayMakeOtherPositions();
}

void CForceStart::execute_6(){
    onePlayMakeOtherPositions();
}
#else
CForceStart::CForceStart()
{
    kkARegion[0].assign(-_FIELD_WIDTH/2, _FIELD_HEIGHT/2, _FIELD_WIDTH/2-0.5, _FIELD_HEIGHT);
    kkARegion[1].assign(-0.5, _FIELD_HEIGHT/2, 1, _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);
    kkARegion[2].assign(0.5, _FIELD_HEIGHT/2, _FIELD_WIDTH/2 - (0.5 + _GOAL_RAD), _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);
    kkARegion[3].assign(_FIELD_WIDTH/2 - _GOAL_RAD, _FIELD_HEIGHT/2, _GOAL_RAD, _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);
    kkARegion[4].assign(-0.5, _PENALTY_WIDTH/2, 1, _PENALTY_WIDTH);
    kkARegion[5].assign(0.5, _PENALTY_WIDTH/2, _FIELD_WIDTH/2 - (0.5 + _GOAL_RAD), _PENALTY_WIDTH);
    kkARegion[6].assign(_FIELD_WIDTH/2 - _GOAL_RAD, _PENALTY_WIDTH/2, _GOAL_RAD, _PENALTY_WIDTH);
    kkARegion[7].assign(-0.5, -_PENALTY_WIDTH/2, 1, _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);
    kkARegion[8].assign(0.5, -_PENALTY_WIDTH/2, _FIELD_WIDTH/2 - (0.5 + _GOAL_RAD), _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);
    kkARegion[9].assign(_FIELD_WIDTH/2 - _GOAL_RAD, -_PENALTY_WIDTH/2, _GOAL_RAD, _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);

    kkBRegion[0].assign(-_FIELD_WIDTH/2, _FIELD_HEIGHT/2, _FIELD_WIDTH/2-0.5, _FIELD_HEIGHT);
    kkBRegion[1].assign(-0.5, _FIELD_HEIGHT/2, _FIELD_WIDTH/2 + 0.5, _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);
    kkBRegion[2].assign(-0.5, _PENALTY_WIDTH/2, 1, _PENALTY_WIDTH);
    kkBRegion[3].assign(0.5, _PENALTY_WIDTH/2, _FIELD_WIDTH/2 - (0.5 + _GOAL_RAD), _PENALTY_WIDTH);
    kkBRegion[4].assign(_FIELD_WIDTH/2 - _GOAL_RAD, _PENALTY_WIDTH/2, _GOAL_RAD, _PENALTY_WIDTH);
    kkBRegion[5].assign(-0.5, -_PENALTY_WIDTH/2, _FIELD_WIDTH/2 + 0.5, _FIELD_HEIGHT/2 - (_PENALTY_WIDTH)/2);

    QString tempDir = QString::fromStdString(policy()->KKPlayOn_KKPlanSQL());
    //tempDir = QDir::currentPath()+tempDir;

    kkPlanSQL = QSqlDatabase::addDatabase("QSQLITE");
    kkPlanSQL.setDatabaseName(tempDir);
    if (!kkPlanSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
                              "Unable to establish a database connection.\n"\
                              "This example needs SQLite support. Please read "\
                              "the Qt SQL driver documentation for information how "\
                              "to build it.\n\n"\
                              "Click Cancel to exit.", QMessageBox::Cancel);
    }

    for(int i = 0; i < 6; i++)
    {
        kk1AgentPlan[i] = new QList<kkAgentPlan>();
        kk2AgentPlan[i] = new QList<kkAgentPlan>();
        kk3AgentPlan[i] = new QList<kkAgentPlan>();
        kk4AgentPlan[i] = new QList<kkAgentPlan>();
        kk5AgentPlan[i] = new QList<kkAgentPlan>();
    }


    kkLoadPlan(kk1AgentPlan, 1);
    kkLoadPlan(kk2AgentPlan, 2);
    kkLoadPlan(kk3AgentPlan, 3);
    kkLoadPlan(kk4AgentPlan, 4);
    kkLoadPlan(kk5AgentPlan, 5);

    decidePlan = true;

    for(int i = 0; i < 5; i++)
        kkActiveAgent[i] = false;

    //    for(int i = 0; i < 6; i++)
    //        debug(QString::number(kk2AgentPlan[i]->length()),D_KK);

    currentSpeeds.pass = policy()->KKPlayOn_KKPassSpeed();
    currentSpeeds.kick = policy()->KKPlayOn_KKKickSpeed();
    currentSpeeds.chip = policy()->KKPlayOn_KKChipSpeed();
    currentSpeeds.shot = policy()->KKPlayOn_KKShotSpeed();
    currentSpeeds.ChipToGoal = policy()->KKPlayOn_KKChipToGoalSpeed();

    kkLastTimeDef = 0;

    isBallInTheirKick = false;
    isBallInOurKick = false;
    theirFlag = false;
    ourFlag = false;
    noneFlag = false;
    Owner = BNone;

    ATFballEnteredKickerFlag = false;
    ATFballEnteredKickerChipFlag = false;
    ATFpassReceivedFlag = false;
    FPPballEnteredKickerFlag = false;

    for(int i = 0; i < 5; i++)
        agentSkill[i] = new CRolePlayOn();

    for(int i = 0; i < 3; i++)
        kkMarkGPA[i] = new CSkillGotoPointAvoid(NULL);
}

CForceStart::~CForceStart(){
}

void CForceStart::reset(){
    position.reset();
    executedCycles = 0;
}

void CForceStart::setDecidePlan(bool _check)
{
    decidePlan = _check;
//    debug("Decide Plan: "+QString::fromStdString((_check?"True":"False")),D_KK);
}

void CForceStart::init(QList<int> _agents , QMap<QString , EditData*> *_editData){
    setAgentsID(_agents);
    setEditData(_editData);
    initMaster();

    /*if( knowledge->getLastPlayExecuted() != StartPlay ){
        reset();
    }
    knowledge->setLastPlayExecuted(StartPlay);*/
}

void CForceStart::onePlayMakeOtherPositions(){
    executedCycles++;

    choosePlayMaker();

    playMakeRole.setKickMode(NoMode);
    playMakeRole.setSlow(false);
    playMakeRole.setThrough(true);
    playMakeRole.setNoKick(false);


    positionAgents.clear();
    appendRemainingsAgents(markAgents);

    for( int i=0 ; i<markAgents.size() ; i++ ){
        if( markAgents.at(i)->intention != NULL && markAgents.at(i)->intention->finished() == false && markAgents.at(i)->intention->M_type == "position"){
            positionAgents.append(markAgents.at(i));
            markAgents.removeAt(i);
            i--;
        }
    }

    setFormation("PlayOnOffense");
}

Segment2D CForceStart::kkGetToleranceSeg(Vector2D tAVec, Vector2D tBVec, double tTol)
{
    Line2D tempLine(tAVec, tBVec);
    Line2D tempLineP = tempLine.perpendicular(tBVec);
    Vector2D tSol1, tSol2;
    Circle2D(tBVec, tTol).intersection(tempLineP, &tSol1, &tSol2);
    return Segment2D(tSol1, tSol2);
}
/////////////By Mahi
///
///
///
///

bool CForceStart::kkIsAgentTaskFailed(int tAgent) {

    Vector2D sol1,sol2;
    Vector2D ballPos = wm->ball->pos;
    Vector2D ballVel = wm->ball->vel;
    if(kkIsAgentTaskDone(tAgent)) {
        ATFballEnteredKickerFlag = false;
        ATFballEnteredKickerChipFlag = false;
        ATFpassReceivedFlag = false;
        return false;
    }

    switch(agentSkill[tAgent]->getTask()){


    case MoveOffensive:
    case MoveDefensive:
        break;
    case PassOffensive:
    case PassDefensive:
    case KickOffensive:
    case KickDefensive:
        draw(Circle2D(agentSkill[tAgent]->getTarget(), 0.75),QColor(Qt::cyan));
        draw(Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1),QColor(Qt::cyan));
        draw(Segment2D(ballPos,ballPos + ballVel));
        draw(Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08),QColor(Qt::red));
        if( Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08).contains(ballPos)) {
            ATFballEnteredKickerFlag = true;
        }

        if(!Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1).contains(ballPos) && ATFballEnteredKickerFlag) {
            if(Circle2D(agentSkill[tAgent]->getTarget(),0.75).intersection(Ray2D(ballPos,ballPos + ballVel),&sol1,&sol2)) return false;
            else {
                ATFballEnteredKickerFlag = false;
                ATFballEnteredKickerChipFlag = false;
                ATFpassReceivedFlag = false;
                return true;
            }
        }
        else return false;
        break;
    case ChipOffensive:
    case ChipDefensive:
        draw(Circle2D(agentSkill[tAgent]->getTarget(), 0.5),QColor(Qt::cyan));
        draw(Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1),QColor(Qt::cyan));
        draw(Segment2D(ballPos,ballPos + ballVel));
        draw(Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08),QColor(Qt::red));
        if( Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08).contains(ballPos)) {
            ATFballEnteredKickerChipFlag = true;
        }


        if(!Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1).contains(ballPos) && ATFballEnteredKickerChipFlag) {
            if(Circle2D(agentSkill[tAgent]->getTarget(),0.5).intersection(Ray2D(ballPos,ballPos + ballVel),&sol1,&sol2)) return false;
            else {
                ATFballEnteredKickerFlag = false;
                ATFballEnteredKickerChipFlag = false;
                ATFpassReceivedFlag = false;
                return true;
            }
        }
        else return false;
        break;
    case OneTouch:
        draw(Circle2D(agentSkill[tAgent]->getTarget(), 0.5),QColor(Qt::cyan));
        draw(Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1),QColor(Qt::cyan));
        draw(Segment2D(ballPos,ballPos + ballVel));
        if(Circle2D(agentSkill[tAgent]->getWaitPos(),1).contains(ballPos)) {
            ATFpassReceivedFlag = true;

        }
        if(ATFpassReceivedFlag && !Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1.2).contains(ballPos)){
            ATFballEnteredKickerFlag = false;
            ATFballEnteredKickerChipFlag = false;
            ATFpassReceivedFlag = false;
            return true;
        }
        draw(Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08),QColor(Qt::red));
        if( Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08).contains(ballPos)) {
            ATFballEnteredKickerChipFlag = true;
        }

        if(!Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1).contains(ballPos) && ATFballEnteredKickerChipFlag) {
            if(Circle2D(agentSkill[tAgent]->getTarget(),0.5).intersection(Ray2D(ballPos,ballPos + ballVel),&sol1,&sol2)) return false;
            else {
                ATFballEnteredKickerFlag = false;
                ATFballEnteredKickerChipFlag = false;
                return true;
            }
        }
        else return false;
        break;
    case CatchBall:
        break;
    case ReceivePass:
        if(Circle2D(agentSkill[tAgent]->getTarget(),1).contains(ballPos)) {
            ATFpassReceivedFlag = true;

        }
        if(ATFpassReceivedFlag && !Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1.2).contains(ballPos)){
            ATFpassReceivedFlag = false;
            return true;
        }
        break;
    case Shot:
        draw(Circle2D(agentSkill[tAgent]->getTarget(), 0.5),QColor(Qt::cyan));
        draw(Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1),QColor(Qt::cyan));
        draw(Segment2D(ballPos,ballPos + ballVel));
        draw(Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08),QColor(Qt::red));
        if( Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08).contains(ballPos)) {
            ATFballEnteredKickerFlag = true;
        }

        if(!Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1).contains(ballPos) && ATFballEnteredKickerFlag) {
            if(Circle2D(agentSkill[tAgent]->getTarget(),0.5).intersection(Ray2D(ballPos,ballPos + ballVel),&sol1,&sol2)) return false;
            else {
                ATFballEnteredKickerFlag = false;
                ATFballEnteredKickerChipFlag = false;
                ATFpassReceivedFlag = false;
                return true;
            }
        }
        else return false;
        break;
    case ChipToGoal:
        draw(Circle2D(agentSkill[tAgent]->getTarget(), 0.5),QColor(Qt::cyan));
        draw(Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1),QColor(Qt::cyan));
        draw(Segment2D(ballPos,ballPos + ballVel));
        draw(Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08),QColor(Qt::red));
        if( Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08).contains(ballPos)) {
            ATFballEnteredKickerFlag = true;
        }
        if(!Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(),1).contains(ballPos) && ATFballEnteredKickerFlag) {
            if(Circle2D(agentSkill[tAgent]->getTarget(),0.5).intersection(Ray2D(ballPos,ballPos + ballVel),&sol1,&sol2)) return false;
            else {
                ATFballEnteredKickerFlag = false;
                ATFballEnteredKickerChipFlag = false;
                ATFpassReceivedFlag = false;
                return true;
            }
        }
        else return false;
        break;
    default:
        return false;
        break;
    }
}





///
///
///
bool CForceStart::kkIsAgentTaskDone(int tAgent)
{
    Vector2D ballPos = wm->ball->pos;
    Vector2D ballVel = wm->ball->vel;
    switch(agentSkill[tAgent]->getTask())
    {
    case MoveOffensive:
    case MoveDefensive:
        if(Circle2D(agentSkill[tAgent]->getTarget(), 0.20).contains(knowledge->getAgent(agentsID[tAgent])->pos()))
            return true;
        else
            return false;
        break;
    case PassOffensive:
    case PassDefensive:
    case KickOffensive:
    case KickDefensive:
        draw(Circle2D(agentSkill[tAgent]->getTarget(), 0.5),QColor(Qt::cyan));
        if(Circle2D(agentSkill[tAgent]->getTarget(), .5).contains(wm->ball->pos))
            return true;
        return false;
        break;
    case ChipOffensive:
    case ChipDefensive:
        if(Circle2D(agentSkill[tAgent]->getTarget(), 0.8).contains(wm->ball->pos))
            return true;
        return false;
        break;
    case OneTouch:
        if(Circle2D(agentSkill[tAgent]->getWaitPos(), 0.5 + kkTaskDoneRadiusTolerance).contains(wm->ball->pos)) {
            kkTaskDoneOneTouchEnterFlag = true;
            kkTaskDoneRadiusTolerance = 0.3;
            // debug("1st True",D_MAHI);
        }
        else {
            kkTaskDoneOneTouchEnterFlag = false;
            kkTaskDoneRadiusTolerance = 0.0;
        }
        if(Circle2D((knowledge->getAgent(kkAgentsId[tAgent])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[tAgent])->pos(),0.08).contains(ballPos)) {
            //   debug("2nd True",D_MAHI);

            kkTaskDoneOneTouchEnterFlag1 = true;
        }

        if(kkTaskDoneOneTouchEnterFlag && kkTaskDoneOneTouchEnterFlag1) {
            if((Circle2D(agentSkill[tAgent]->getWaitPos(),0.5 + kkTaskDoneRadiusTolerance).contains(ballPos))) {
                //      debug("3rd True",D_MAHI);
                kkTaskDoneOneTouchEnterFlag1 = false;
                return true;
            }

            //                        debug("2ed True",D_MAHI);
            //            Line2D tempLinePass(wm->ball->pos, wm->ball->pos + wm->ball->vel.norm()*10);
            //            draw(kkGetToleranceSeg(agentSkill[tAgent]->getWaitPos(),
            //                                   agentSkill[tAgent]->getTarget(),
            //                                   1.5),QColor(Qt::cyan));
            //            if(kkGetToleranceSeg(agentSkill[tAgent]->getWaitPos(),
            //                                 agentSkill[tAgent]->getTarget(),
            //                                 1.5).intersection(tempLinePass).valid() )
            //                if(!Circle2D(agentSkill[tAgent]->getWaitPos(), 0.5).contains(wm->ball->pos) ) {
            //                    debug("3nd True",D_MAHI);
            //                    return true;
            //                }
        }
        else return false;
        break;
    case CatchBall:
        if( Circle2D(wm->ball->pos, 0.2).contains(knowledge->getAgent(kkAgentsId[tAgent])->pos()) )
            return true;
        return false;
        break;
    case ReceivePass:
        if( Circle2D(knowledge->getAgent(kkAgentsId[tAgent])->pos(), 0.2).contains(wm->ball->pos) )
            return true;
        return false;
        break;
    case Shot:
    case ChipToGoal:
        if( wm->field->isInOppPenaltyArea(wm->ball->pos) )
            return true;
        return false;
        break;
    case MarkDefensive:
        break;
    case MarkOffensive:
        break;


    default:
        return false;
        break;
    }
}

bool CForceStart::kkCheckCycleTimeOut(long tTime)
{
    if(tTime < knowledge->getCurrentKKTime() - kkLastTime)
        return true;
    return false;
}

void CForceStart::kkCheckEndPolicy()
{
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        if(!kkDoneTaskAgent[i])
            kkDoneTaskAgent[i] = kkIsAgentTaskDone(i);
    }
    int tempCCnt = 0;
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
        if(kkDoneTaskAgent[i])
            tempCCnt++;
    if(tempCCnt >= kkCurrentPlan.agentsSize)
    {
        decidePlan = true;
        debug(QString("Allagent"),D_KK);
        return;
    }

   if(kkCurrentPlan.endMode == Cycle)
    {
        if( kkCheckCycleTimeOut(kkCurrentPlan.endPolicy) )
        {
            decidePlan = true;
            debug(QString("Cycle"),D_KK);
            return;
        }
    }
    else if(kkCurrentPlan.endMode == ExactAgent)
    {
        if( kkIsAgentTaskDone(kkCurrentPlan.endPolicy) )
        {
            decidePlan = true;
            debug(QString("ExactAgent"),D_KK);
            return;
        }
    }
    else if(kkCurrentPlan.endMode == AllAgents)
    {
        for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
        {
            if(!kkDoneTaskAgent[i])
                kkDoneTaskAgent[i] = kkIsAgentTaskDone(i);
        }
        int tempCCnt = 0;
        for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
            if(kkDoneTaskAgent[i])
                tempCCnt++;
        if(tempCCnt >= kkCurrentPlan.agentsSize) {
            decidePlan = true;
            debug(QString("Allagent"),D_KK);
            return;
        }
    }
    else if (kkCurrentPlan.endMode == ExactDisturb)
    {
        if(kkIsAgentTaskFailed(kkCurrentPlan.endPolicy))
        {
            decidePlan = true;
            debug(QString("ExactDisturb"),D_KK);
            return;
        }

    }
    if(kkDefaultCycle < knowledge->getCurrentKKTime() - kkLastTime)
    {
        decidePlan = true;
        debug(QString("First"),D_KK);
        return;
    }
    if(knowledge->getCurrentKKTime() - kkLastTimeDef> 100) {
        decidePlan = true;
        debug(QString("Second"),D_KK);
    }
    kkExecutedCycle++;
}

bool CForceStart::isBallOurs() {


        for(int i = 0;i < wm->opp.activeAgentsCount();i++) {
            if(Circle2D(wm->opp.active(i)->pos + wm->opp.active(i)->dir.norm()*0.08,0.08).contains(wm->ball->pos)) {
                debug(QString("INOUR"),D_KK);
                isBallInTheirKick = true;
                ourTime = 0;
                noneTime = 0;
                if(theirTime > 2) {
                    Owner = BThem;
                }
                theirTime++;
            }
            else {
                isBallInTheirKick = false;
            }
        }


        for(int i = 0;i < wm->our.activeAgentsCount();i++) {
            if(Circle2D(wm->our.active(i)->pos + wm->our.active(i)->dir.norm()*0.08,0.08).contains(wm->ball->pos)) {
                debug(QString("INOUR"),D_KK);
                isBallInOurKick = true;
                theirTime = 0;
                noneTime = 0;
                if(ourTime > 2) {
                    Owner = BOur;
                }
                ourTime++;
            }
            else {
                isBallInOurKick = false;
            }
        }

        if(!isBallInTheirKick && !isBallInOurKick) {
            debug(QString("NONE %1").arg(noneTime),D_KK);
            ourTime = 0;
            theirTime = 0;
            if(noneTime > 500) {
                debug(QString("NONE"),D_KK);

                Owner = BNone;
            }
            noneTime++;
        }

        if(Owner == BOur) return true;
        else return false;

}

void CForceStart::kkFixPasserPos()
{
    if(decidePlan) FPPballEnteredKickerFlag = false;
    for(int i = 0;i < kkCurrentPlan.agentsSize;i++) {
        if(agentSkill[i]->getTask() == PassOffensive ||
                agentSkill[i]->getTask() == PassDefensive) {
            draw(Circle2D((knowledge->getAgent(kkAgentsId[agentSkill[i]->getAgentID()])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[agentSkill[i]->getAgentID()])->pos(),0.08),QColor(Qt::red));
            if(Circle2D((knowledge->getAgent(kkAgentsId[agentSkill[i]->getAgentID()])->dir()).norm()*0.09  + knowledge->getAgent(kkAgentsId[agentSkill[i]->getAgentID()])->pos(),0.08).contains(wm->ball->pos)) {
                FPPballEnteredKickerFlag = true;
                debug(QString("4"),D_KK);
            }

            if(!Circle2D(knowledge->getAgent(kkAgentsId[agentSkill[i]->getAgentID()])->pos(),0.3).contains(wm->ball->pos) && FPPballEnteredKickerFlag && !kkIsAgentTaskFailed(i)) {
                agentSkill[i]->setTarget(knowledge->getAgent(kkAgentsId[agentSkill[i]->getAgentID()])->pos());
                agentSkill[i]->setTargetDir(wm->ball->pos);
                agentSkill[i]->setTargetVel(Vector2D(0, 0));
                agentSkill[i]->setSelectedSkill(SkillGotopoint);
                FPPballEnteredKickerFlag = false;
                debug(QString("SAGSAG"),D_KK);
            }
            debug(QString("%1").arg(agentSkill[i]->getAgentID()),D_KK);

        }
        //debug(QString("2"),D_MAHI);

    }
    //debug(QString("1"),D_MAHI);


    //    static bool flag = true;
    //    Vector2D tempVec;
    //    int receiverID;
    //    bool isOneTouch;
    //    if(decidePlan) flag = true;
    //    for(int i = 0;(i < kkCurrentPlan.agentsSize) && flag;i++) {
    //            if(agentSkill[i].getTask() == ReceivePass){
    //                receiverID = i;
    //                isOneTouch = false;
    //                flag = false;
    //            }
    //            else if(agentSkill[i].getTask() == OneTouch) {
    //                receiverID = i;
    //                isOneTouch = true;
    //                flag = false;
    //            }
    //    }
    //    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    //    {

    //        if(agentSkill[i].getTask() == PassOffensive ||
    //                agentSkill[i].getTask() == PassDefensive)
    //        {
    //            if(!Circle2D(knowledge->getAgent(i)->pos(),0.1).contains(wm->ball->pos) && !kkIsAgentTaskFailed(i)) {
    //                if(agentSkill[receiverID].getTask() == ReceivePass) {
    //                    debug("Done",D_MAHI);
    //                    agentSkill[i].setTarget(kkPasserPos);
    //                    agentSkill[i].setTargetDir(wm->ball->pos);
    //                    agentSkill[i].setTargetVel(Vector2D(0, 0));
    //                    agentSkill[i].setSelectedSkill(SkillGotopoint);
    //                }
    //                else if(agentSkill[receiverID].getTask() == OneTouch) {
    //                    debug("Done",D_MAHI);
    //                    agentSkill[i].setTarget(kkPasserPos);
    //                    agentSkill[i].setTargetDir(wm->ball->pos);
    //                    agentSkill[i].setTargetVel(Vector2D(0, 0));
    //                    agentSkill[i].setSelectedSkill(SkillGotopoint);
    //                }
    //            }


    //        }
    //        if(agentSkill[receiverID].getTask() == OneTouch) {
    //            if(Circle2D(knowledge->getAgent(receiverID)->pos(),2).contains(knowledge->getAgent(i)->pos())) kkPasserPos = knowledge->getAgent(i)->pos();
    //            else kkPasserPos = wm->ball->pos - Vector2D(-5,0);
    //        }
    //        else if(agentSkill[receiverID].getTask() == ReceivePass) {
    //            if(knowledge->getAgent(i)->pos().x < _FIELD_WIDTH/2 - 1) tempVec = Vector2D(1,0);
    //            else tempVec = Vector2D(0,0);
    //            if(Circle2D(knowledge->getAgent(receiverID)->pos(),2).contains(knowledge->getAgent(i)->pos())) kkPasserPos = knowledge->getAgent(i)->pos();
    //            else kkPasserPos = knowledge->getAgent(i)->pos() + tempVec;
    //        }
    //    }
}

void CForceStart::checkOneTouchTask()
{
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        if(agentSkill[i]->getTask() == OneTouch)
        {
            if(kkIsAgentTaskDone(i))
            {
                debug("OneTouch Done",D_KK);
            }
        }
    }
}

bool CForceStart::kkCheckAgentSize(int agentSize) {
    if(agentSize == kkActiveAgents.size()) return true;
    else return false;
}

void CForceStart::kkMainPlanner(int tAgentSize)
{
    //debug(QString("is ball ours : %1").arg(kkIsballOurs()),D_KK);
    bool tempBallPoss = kkIsballOurs();
    if(decidePlan)
    {
        currentSpeeds.pass = policy()->KKPlayOn_KKPassSpeed();
        currentSpeeds.kick = policy()->KKPlayOn_KKKickSpeed();
        currentSpeeds.chip = policy()->KKPlayOn_KKChipSpeed();
        currentSpeeds.shot = policy()->KKPlayOn_KKShotSpeed();
        currentSpeeds.ChipToGoal = policy()->KKPlayOn_KKChipToGoalSpeed();

        decideCount++;

        kkLastTime = knowledge->getCurrentKKTime();
        kkActiveAgents.clear();
        for(int i = 0; i < 5; i++)
            kkDoneTaskAgent[i] = false;
        appendRemainingsAgents(kkActiveAgents);
        int tempBall = kkGetArea(kkBRegion, 6, wm->ball->pos);
        kkGetBestPlan(tempBall, tempBallPoss, tAgentSize);

        markAgents.clear();
        kkMarkList.clear();
        kkNewAssignTasks();
        decidePlan = false;
        kkDefaultCycle = policy()->KKPlayOn_KKDefaultCycle();
        kkExecutedCycle = 0;
        kkTaskDonePassCnt = 0;
        for(int i = 0; i < 5; i++)
            kkTaskDoneKickCnt[i] = 0;
        kkTaskDoneOneTouchEnterFlag = false;
        kkTaskDoneRadiusTolerance = 0.0;
        debug(QString("decide Plan"),D_KK);
    }

    kkFixPasserPos();

    checkOneTouchTask();


    if(kkActiveAgent[0]) {
        //kkMoveManager(knowledge->getAgent(kkAgentsId[0]),agentSkill[0]);
        agentSkill[0]->execute();
    }
    if(kkActiveAgent[1]) {
//            kkMoveManager(knowledge->getAgent(kkAgentsId[1]),agentSkill[1]);
        agentSkill[1]->execute();
    }
    if(kkActiveAgent[2]) {
//            kkMoveManager(knowledge->getAgent(kkAgentsId[2]),agentSkill[2]);
        agentSkill[2]->execute();
    }
    if(kkActiveAgent[3]) {
//            kkMoveManager(knowledge->getAgent(kkAgentsId[3]),agentSkill[3]);
        agentSkill[3]->execute();
    }
    if(kkActiveAgent[4]) {
//            kkMoveManager(knowledge->getAgent(kkAgentsId[4]),agentSkill[4]);
        agentSkill[4]->execute();
    }
    if(!kkMarkList.isEmpty())
    {
        int tempId[3];
        for(int i = 0; i < 3 || i < kkMarkList.count(); i++)
        {
            tempId[i] = kkMarkList.at(i);
        }
        if(kkMarkList.count() == 1)
            kkManToManMark(tempId[0]);
        else if(kkMarkList.count() == 2)
            kkManToManMarkTwo(tempId);
        else if(kkMarkList.count() == 3)
            kkManToManMarkThree(tempId);
    }

    kkCheckEndPolicy();
    kkLastTimeDef = knowledge->getCurrentKKTime();
//    }

}
void CForceStart::execute_0(){
    draw("KKPLayOn agent(s): 0", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 0) decidePlan = true;
    lastAgentCount = 0;
}

void CForceStart::execute_1(){
    draw("KKPLayOn agent(s): 1", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 1) decidePlan = true;
    lastAgentCount = 1;
    kkMainPlanner(1);
    // debug(QString::number(kk3AgentPlan[1]->length()), D_KK);
}

void CForceStart::execute_2(){
    draw("KKPLayOn agent(s): 2", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 2) decidePlan = true;
    lastAgentCount = 2;
    kkMainPlanner(2);
}

void CForceStart::execute_3(){
    draw("KKPLayOn agent(s): 3", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 3) decidePlan = true;
    lastAgentCount = 3;
    kkMainPlanner(3);
}

void CForceStart::execute_4(){
    draw("KKPLayOn agent(s): 4", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 4) decidePlan = true;
    lastAgentCount = 4;
    kkMainPlanner(4);
}

void CForceStart::execute_5(){
    draw("KKPLayOn agent(s): 5", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 5) decidePlan = true;
    lastAgentCount = 5;
    kkMainPlanner(5);
}

void CForceStart::execute_6(){
    draw("KKPLayOn agent(s): 6", Vector2D(_FIELD_WIDTH/2 - 1.5, _FIELD_HEIGHT/2 + 0.15), QColor(0, 51, 102));
    if(lastAgentCount != 5) decidePlan = true;
    lastAgentCount = 5;
    kkMainPlanner(5);
}

PEndPolicy CForceStart::kkGetPolicyByIndex(int index)
{
    switch(index)
    {
    default:
    case 1:
        return Cycle;
        break;
    case 2:
        return ExactAgent;
        break;
    case 3:
        return AllAgents;
        break;
    case 4:
        return ExactDisturb;
        break;
    }
}

int CForceStart::kkGetIntByEnum(PEndPolicy tEnum)
{
    switch (tEnum)
    {
    case Cycle:
        return 1;
    case ExactAgent:
        return 2;
    case AllAgents:
        return 3;
    case ExactDisturb:
        return 4;
    }
}

PSkills CForceStart::kkGetPSkillByIndex(int index)
{
    switch (index)
    {
    case 0:
        return None;
    case 1:
        return MoveOffensive;
    case 2:
        return MoveDefensive;
    case 3:
        return PassOffensive;
    case 4:
        return PassDefensive;
    case 5:
        return KickOffensive;
    case 6:
        return KickDefensive;
    case 7:
        return ChipOffensive;
    case 8:
        return ChipDefensive;
    case 9:
        return MarkOffensive;
    case 10:
        return MarkDefensive;
    case 11:
        return OneTouch;
    case 12:
        return CatchBall;
    case 13:
        return ReceivePass;
    case 14:
        return Shot;
    case 15:
        return ChipToGoal;
    }
}

int CForceStart::kkGetIntByEnum(PSkills tEnum)
{
    switch (tEnum)
    {
    case None:
        return 0;
    case MoveOffensive:
        return 1;
    case MoveDefensive:
        return 2;
    case PassOffensive:
        return 3;
    case PassDefensive:
        return 4;
    case KickOffensive:
        return 5;
    case KickDefensive:
        return 6;
    case ChipOffensive:
        return 7;
    case ChipDefensive:
        return 8;
    case MarkOffensive:
        return 9;
    case MarkDefensive:
        return 10;
    case OneTouch:
        return 11;
    case CatchBall:
        return 12;
    case ReceivePass:
        return 13;
    case Shot:
        return 14;
    case ChipToGoal:
        return 15;
    }
}

void CForceStart::kkDrawRect()
{
    for(int i =0; i < 10; i++)
        draw(kkARegion[i], QColor(255, 0, 0));

    for(int i = 0; i < 6; i++)
        draw(kkBRegion[i], QColor(255, 255, 0, 128));
}

void CForceStart::kkInsertToList(QList<kkAgentPlan> **list, QSqlQuery query, int agentSize)
{
    bool errorCheck = true;
    while(query.next())
    {
        errorCheck = false;
        kkAgentPlan tempPlan;
        tempPlan.planId = query.value(0).toInt();
        tempPlan.ball = query.value(1).toInt();
        tempPlan.endMode = kkGetPolicyByIndex(query.value(2).toInt());
        tempPlan.endPolicy = query.value(3).toInt();
        tempPlan.possession = query.value(4).toInt();
        tempPlan.agentsSize = query.value(5).toInt();
        //tempPlan.comment = getCommentForPlanner(query.value(6).toString());
        for(int i = 0; i < agentSize; i++)
        {
            tempPlan.agents[i].A = query.value(7 + i*6).toInt();
            tempPlan.agents[i].B = query.value(8 + i*6).toInt();
            for(int j = 0; j < 4; j++)
            {
                tempPlan.agents[i].skill[j] = kkGetPSkillByIndex(query.value(9 + i*6 + j).toInt());
            }
        }
        for(int i = agentSize; i < 5; i++)
        {
            tempPlan.agents[i].A = -1;
            tempPlan.agents[i].B = -1;
            for(int j = 0; j < 4; j++)
            {
                tempPlan.agents[i].skill[j] = None;
            }
        }
        switch(tempPlan.ball)
        {
        default:
        case 1:
            list[0]->append(tempPlan);
            break;
        case 2:
            list[1]->append(tempPlan);
            break;
        case 3:
            list[2]->append(tempPlan);
            break;
        case 4:
            list[3]->append(tempPlan);
            break;
        case 5:
            list[4]->append(tempPlan);
            break;
        case 6:
            list[5]->append(tempPlan);
            break;
        }
    }
    if(errorCheck)
    {
        qDebug()<<kkPlanSQL.databaseName();
        qDebug()<< "SqLite error:" << query.lastError().text() << ", SqLite error code:" << query.lastError().number();
    }
}

void CForceStart::kkLoadPlan(QList<kkAgentPlan> **list, int agentSize)
{
    QSqlQuery query;
    switch(agentSize)
    {
    default:
    case 1:
        query.exec("SELECT * FROM plan1agent ORDER BY id ASC");
        kkInsertToList(list, query, 1);
        break;
    case 2:
        query.exec("SELECT * FROM plan2agent ORDER BY id ASC");
        kkInsertToList(list, query, 2);
        break;
    case 3:
        query.exec("SELECT * FROM plan3agent ORDER BY id ASC");
        kkInsertToList(list, query, 3);
        break;
    case 4:
        query.exec("SELECT * FROM plan4agent ORDER BY id ASC");
        kkInsertToList(list, query, 4);
        break;
    case 5:
        query.exec("SELECT * FROM plan5agent ORDER BY id ASC");
        kkInsertToList(list, query, 5);
        break;

    }
}

int CForceStart::kkGetArea(Rect2D *rect, int size, Vector2D point)
{
    for(int i = 0; i < size; i++)
    {
        if(rect[i].contains(point))
            return i + 1;
    }
    return -1;
}

bool CForceStart::kkIsInRect(Rect2D rect, Vector2D point, double offset)
{
    Rect2D tempRect;
    tempRect.assign(rect.left() - offset, rect.top() + offset, rect.size().length() + offset*2, rect.size().width() + offset*2);
    return tempRect.contains(point);
}

QList<kkAgentPlan> CForceStart::kkFillQList(QList<kkAgentPlan> *list)
{
    QList<kkAgentPlan> tempList;
    for(int i = 0; i < list->length(); i++)
    {
        tempList.append(list->at(i));
    }
    return tempList;
}

//bool CForceStart::kkIsPlanMatch(QList<mahiRobot> aList, kkAgentPlan plan)
//{
//    int tempRegion;
//    int regionCnt[10], agentCnt[10];
//    for(int i = 0; i < 10; i++)
//    {
//        regionCnt[i] = 0;
//        agentCnt[i] = 0;
//    }

//    for(int i = 0; i < plan.agentsSize; i++)
//    {
//        tempRegion = plan.agents[i].A - 1;
//        if(tempRegion < 0) tempRegion = 0;
//        if(tempRegion > 9) tempRegion = 9;
//        regionCnt[tempRegion]++;
//    }

//    for(int i = 0; i < aList.length(); i++)
//    {
//        if(aList.at(i).id != -1)
//        {
//            tempRegion = kkGetArea(kkARegion, 10, aList.at(i).pos) - 1;
//            if(tempRegion >= 0 && tempRegion <= 9)
//                agentCnt[tempRegion]++;
//        }
//    }

//    for(int i = 0; i < 10; i++)
//    {
//        if(agentCnt[i] != regionCnt[i])
//            return false;
//    }

//    return true;
//}

int CForceStart::kkInsertActiveAgentsToList()
{
    //int activeAgentsCount = wm->our.activeAgentsCount();
    int activeAgentsCount = kkActiveAgents.length();
    kkAgentList.clear();
    kkRobot tempRobot;
    for(int i = 0; i < kkActiveAgents.length(); i++)
    {
        tempRobot.pos = kkActiveAgents.at(i)->pos();
        tempRobot.vel = kkActiveAgents.at(i)->vel();
        tempRobot.dir = kkActiveAgents.at(i)->dir();
        tempRobot.id = kkActiveAgents.at(i)->id();
        kkAgentList.append(tempRobot);
    }
    for(int i = activeAgentsCount; i < 5; i++)
    {
        tempRobot.pos = Vector2D(0,0);
        tempRobot.vel = Vector2D(0,0);
        tempRobot.dir = Vector2D(0,0);
        tempRobot.id = -1;
        kkAgentList.append(tempRobot);
    }
    return activeAgentsCount;
}

void CForceStart::kkGetBestPlan(int planBallPos, bool _ballPoss, int agentSize)
{
    QList<kkAgentPlan> tempPlan;
    planBallPos--;
    if(planBallPos < 0) planBallPos = 0;
    if(planBallPos > 5) planBallPos = 5;
    switch(agentSize) {
    default:
    case 1:
        tempPlan = kkFillQList(kk1AgentPlan[planBallPos]);
        break;
    case 2:
        tempPlan = kkFillQList(kk2AgentPlan[planBallPos]);
        break;
    case 3:
        tempPlan = kkFillQList(kk3AgentPlan[planBallPos]);
        break;
    case 4:
        tempPlan = kkFillQList(kk4AgentPlan[planBallPos]);
        break;
    case 5:
        tempPlan = kkFillQList(kk5AgentPlan[planBallPos]);
        break;
    }

    if(!_ballPoss)
        tempPlan.clear();

    QList<int> selectedPlans;

    debug(QString("%1").arg(planBallPos), D_KK);
    kkInsertActiveAgentsToList();
    for(int i = 0; i < tempPlan.length(); i++)
    {
        //        if(kkIsPlanMatch(kkAgentList, tempPlan.at(i)))
        //            selectedPlans.append(i);
    }
    debug(QString("Matched plan(s): %1/%2 agentSize: %3").arg(selectedPlans.length()).arg(tempPlan.length()).arg(agentSize), D_KK);

    if(!tempPlan.isEmpty())
    {
        if(selectedPlans.length() == 1)
        {
            kkCurrentPlan = tempPlan.at(selectedPlans.at(0));
        }
        else if(selectedPlans.length() > 1)
        {
            QList<kkAgentPlan> tempExtraPlan;
            for(int i = 0; i < selectedPlans.length(); i++)
                tempExtraPlan.append(tempPlan.at(selectedPlans.at(i)));
            kkCurrentPlan = kkChooseBetweenInappropiatePlan(tempExtraPlan);
        }
        else if(selectedPlans.length() == 0 && tempPlan.length() != 0)
        {
            kkCurrentPlan = kkChooseBetweenInappropiatePlan(tempPlan);
        }
    }
    else if(agentSize != 0)
    {
        kkCurrentPlan.agentsSize = agentSize;
        kkCurrentPlan.ball = planBallPos;
        kkCurrentPlan.endMode = ExactAgent;
        kkCurrentPlan.endPolicy = 0;
        kkCurrentPlan.agents[0].A = 6;
        kkCurrentPlan.agents[0].A = 7;
        kkCurrentPlan.agents[0].skill[0] = Shot;
        kkCurrentPlan.agents[0].pSize = 1;

        for(int i = 1; i < agentSize; i++)
        {
            kkCurrentPlan.agents[i].A = 6;
            kkCurrentPlan.agents[i].A = 6;
            kkCurrentPlan.agents[i].skill[0] = MarkOffensive;
            kkCurrentPlan.agents[i].pSize = 1;
        }
    }


    debug(QString("%1, %2, %3").arg(selectedPlans.length()).arg(agentSize).arg(tempPlan.length()), D_KK);

//    kkAssignId();
    kkNewAssignId();
}

kkAgentPlan CForceStart::kkChooseBetweenInappropiatePlan(QList<kkAgentPlan> tList)
{
    double distPoint[5][5];
    QList<double> tempImpList;
    for(int k = 0; k < tList.length(); k++)
    {
        for(int i = 0; i < 5; i++)
            for(int j = 0; j < 5; j++)
                distPoint[i][j] = 10000;

        for(int i = 0; i < tList.at(k).agentsSize; i++)
        {
            for(int j = 0; j < kkAgentList.length(); j++)
            {
                distPoint[i][j] = kkARegion[tList.at(k).agents[i].A - 1].center().dist(kkAgentList.at(j).pos);
            }
        }

        kkValue tempValue;
        for(int i = 0; i < 5; i++)
            tempValue.IDs[i] = -1;
        tempValue.value = 0;
        tempValue.agentSize = 3;

        QList<kkValue> tempListValue;
        kkGetCostRec(distPoint, tList.at(k).agentsSize, tempListValue, tempValue, tList.at(k).agentsSize, 0);

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
        tempImpList.append(tempListValue.at(bestMatchIndex).value);
    }

    double tempMinValue = 10000;
    int tempBestMatchIndex = 0;
    for(int i = 0; i < tempImpList.length(); i++)
    {
        if(tempImpList.at(i) < tempMinValue)
        {
            tempMinValue = tempImpList.at(i);
            tempBestMatchIndex = i;
        }
    }
    debug("KK:"+QString::number(tempBestMatchIndex),D_KK);

    return tList.at(tempBestMatchIndex);
}


int CForceStart::cntRobotInRegion(Rect2D region) {
    int tCount = 0;
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++) {
        if(region.contains(kkARegion[kkCurrentPlan.agents[i].A - 1].center())){
            tCount++;
        }
    }
    return tCount;
}

void CForceStart::kkNewAssignId() {

    QList<PSkills> ballDependenceSkills;
    QList<PSkills> twoPointSkills;
    ballDependenceSkills.append(ChipDefensive);
    ballDependenceSkills.append(ChipOffensive);
    ballDependenceSkills.append(PassOffensive);
    ballDependenceSkills.append(PassDefensive);
    ballDependenceSkills.append(KickOffensive);
    ballDependenceSkills.append(KickDefensive);
    ballDependenceSkills.append(Shot);
    ballDependenceSkills.append(ChipToGoal);
    ballDependenceSkills.append(CatchBall);
    twoPointSkills.append(MoveDefensive);
    twoPointSkills.append(MoveOffensive);
    twoPointSkills.append(MarkDefensive);
    twoPointSkills.append(MarkOffensive);
    QList<int> ballDependenceSkill;
    QList<int> ballNotDependenceSkill;
    QList<int> twoPointSkill;
    QList<int> onePointSkill;

    for(int i = 0; i < kkCurrentPlan.agentsSize; i++){
        if(ballDependenceSkills.contains(kkCurrentPlan.agents[i].skill[0])) ballDependenceSkill.append(i);
        else ballNotDependenceSkill.append(i);
        if(twoPointSkills.contains(kkCurrentPlan.agents[i].skill[0])) twoPointSkill.append(i);
        else onePointSkill.append(i);
    }

    double distPoint[5][5];
    double distPoint2[5][5];
    double distPointBall[5];

    kkValue tempValue;
    for(int i = 0; i < 5; i++)
        tempValue.IDs[i] = -1;

    tempValue.value = 0;
    tempValue.agentSize = 5;

    for(int i = 0; i < 5; i++){
        distPointBall[i] = 10000;
        for(int j = 0; j < 5; j++){
            distPoint[i][j] = 10000;
            distPoint2[i][j] = 10000;
        }
    }

    QList<kkValue> tempListValue;

    if((ballDependenceSkill.size()) && cntRobotInRegion(kkARegion[kkCurrentPlan.agents[ballDependenceSkill.at(0)].A - 1]) > 1) {
        for(int i = 0;i<kkCurrentPlan.agentsSize;i++) {
            distPointBall[i] = wm->ball->pos.dist(kkAgentList.at(i).pos);
        }
        kkGetCostRec2(distPointBall,kkCurrentPlan.agentsSize, tempListValue, tempValue, kkCurrentPlan.agentsSize, 0);
    }
    else if ((twoPointSkill.size()) && cntRobotInRegion(kkARegion[kkCurrentPlan.agents[twoPointSkill.at(0)].B - 1]) > 1) {
        for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
            for(int j = 0; j < kkAgentList.length(); j++)
                distPoint2[i][j] = kkARegion[kkCurrentPlan.agents[i].B - 1].center().dist(kkAgentList.at(j).pos);

        kkGetCostRec(distPoint2, kkCurrentPlan.agentsSize, tempListValue, tempValue, kkCurrentPlan.agentsSize, 0);

    }
    else {
        for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
            for(int j = 0; j < kkAgentList.length(); j++)
                distPoint[i][j] = kkARegion[kkCurrentPlan.agents[i].A - 1].center().dist(kkAgentList.at(j).pos);

        kkGetCostRec(distPoint, kkCurrentPlan.agentsSize, tempListValue, tempValue, kkCurrentPlan.agentsSize, 0);

        /*for(int i = 0; i < tempListValue.length(); i++)
        {
            debug(QString(" %1, %2, %3 val:%4").arg(tempListValue.at(i).IDs[0])
                                                .arg(tempListValue.at(i).IDs[1])
                                                .arg(tempListValue.at(i).IDs[2])
                                                .arg(tempListValue.at(i).value), 0);
        }*/
    }

    double minValue = 10000;
    int bestMatchIndex = 0;
    for(int i = 0; i < tempListValue.length(); i++)
    {
        if(tempListValue.at(i).value < minValue)
        {
            minValue = tempListValue.at(i).value;
            bestMatchIndex = i;
        }
        //debug(QString::number(tempListValue.at(i).value), D_KK);
    }

    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
        kkAgentsId[i] = kkAgentList.at(tempListValue.at(bestMatchIndex).IDs[i]).id;


}

void CForceStart::kkAssignId()
{
    //    QList<PSkills> twoPointSkills;
    //    twoPointSkills.append(MoveOffensive);
    //    twoPointSkills.append(MoveDefensive);
    //    twoPointSkills.append(PassOffensive);
    //    twoPointSkills.append(PassDefensive);
    //    twoPointSkills.append(KickOffensive);
    //    twoPointSkills.append(KickDefensive);
    //    twoPointSkills.append(ChipOffensive);
    //    twoPointSkills.append(ChipDefensive);
    //    twoPointSkills.append(MarkOffensive);
    //    twoPointSkills.append(MarkDefensive);
    //    QList<int> twoPointSkill;
    //    QList<int> onePointSkill;
    //    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    //    {
    //        if(twoPointSkills.contains(kkCurrentPlan.agents[i].skill[0]))
    //            twoPointSkill.append(i);
    //        else
    //            onePointSkill.append(i);
    //    }

    double distPoint[5][5];

    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++)
            distPoint[i][j] = 10000;

    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        for(int j = 0; j < kkAgentList.length(); j++)
        {
            distPoint[i][j] = kkARegion[kkCurrentPlan.agents[i].A - 1].center().dist(kkAgentList.at(j).pos);
        }
    }

    kkValue tempValue;
    for(int i = 0; i < 5; i++)
        tempValue.IDs[i] = -1;
    tempValue.value = 0;
    tempValue.agentSize = 3;

    QList<kkValue> tempListValue;
    kkGetCostRec(distPoint, kkCurrentPlan.agentsSize, tempListValue, tempValue, kkCurrentPlan.agentsSize, 0);

    /*for(int i = 0; i < tempListValue.length(); i++)
    {
        debug(QString(" %1, %2, %3 val:%4").arg(tempListValue.at(i).IDs[0])
                                            .arg(tempListValue.at(i).IDs[1])
                                            .arg(tempListValue.at(i).IDs[2])
                                            .arg(tempListValue.at(i).value), 0);
    }*/

    double minValue = 10000;
    int bestMatchIndex = 0;
    for(int i = 0; i < tempListValue.length(); i++)
    {
        if(tempListValue.at(i).value < minValue)
        {
            minValue = tempListValue.at(i).value;
            bestMatchIndex = i;
        }
        //debug(QString::number(tempListValue.at(i).value), D_KK);
    }

    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
        kkAgentsId[i] = kkAgentList.at(tempListValue.at(bestMatchIndex).IDs[i]).id;
}


int CForceStart::kkGetIndex(kkValue &value, int cIndex)
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

void CForceStart::kkGetCostRec(double costArr[][5], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId)
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
        kkGetCostRec(costArr, arrSize, valueList, tValue, size - 1, aId + 1);
    }
}

void CForceStart::kkGetCostRec2(double costBrr[5], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId)
{
    if(size <= 1) {
        int tIndex = kkGetIndex(value, 0);
        value.IDs[tIndex] = aId;
        value.value += costBrr[aId];
        valueList.append(value);
        return;
    }
    for(int i = 0; i < size; i++) {
        kkValue tValue = value;
        int tIndex = kkGetIndex(tValue, i);
        tValue.IDs[tIndex] = aId;
        tValue.value += costBrr[aId];
        //debug(QString::number(tValue.IDs[0])+QString::number(tValue.IDs[1])+QString::number(tValue.IDs[2]),0,QColor(Qt::blue));
        kkGetCostRec2(costBrr, arrSize, valueList, tValue, size - 1, aId + 1);
    }
}

bool CForceStart::kkCheckMovePos(double x, double y)
{
    Circle2D tempCircle(Vector2D(x, y), 0.15);
    Vector2D sol1, sol2;
    for(int i = 0; i < kkMoveObstacle.length(); i++)
    {
        if(tempCircle.intersection(kkMoveObstacle.at(i), &sol1, &sol2) > 0 || tempCircle.contains(kkMoveObstacle.at(i).center()))
            return false;
    }
    kkBestMovePos.assign(x, y);
    return true;
}

void CForceStart::kkAmirSearch(double x, double y, short id, double step)
{
    double tempDist[4];
    tempDist[0] = Vector2D(x, y).dist(kkARegion[id].topLeft());
    tempDist[1] = Vector2D(x, y).dist(kkARegion[id].topRight());
    tempDist[2] = Vector2D(x, y).dist(kkARegion[id].bottomRight());
    tempDist[3] = Vector2D(x, y).dist(kkARegion[id].bottomLeft());

    double maxRadius = 0;
    for(int i = 0; i < 4; i ++)
    {
        if(tempDist[i] > maxRadius)
            maxRadius = tempDist[i];
    }

    double xx, yy;

    for(double r = 0; r <= maxRadius; r += step)
    {
        for(double theta = 0; theta <= 2*_PI; theta += step/xx)
        {
            xx = r*cos(theta) + x;
            yy = r*sin(theta) + y;
            if(id == 5)
            {
                if(kkARegion[id].contains(Vector2D(xx, yy)) && !wm->field->isInOppPenaltyArea(Vector2D(xx, yy)))
                {
                    if(kkCheckMovePos(xx, yy))
                        return;
                }
            }
            else
            {
                if(kkARegion[id].contains(Vector2D(xx, yy)))
                {
                    if(kkCheckMovePos(xx, yy))
                        return;
                }
            }
        }
    }
}

void CForceStart::kkAmirSearch(double x, double y, short id, Polygon2D &tPoly, double step)
{
    double tempDist[4];
    tempDist[0] = Vector2D(x, y).dist(kkARegion[id].topLeft());
    tempDist[1] = Vector2D(x, y).dist(kkARegion[id].topRight());
    tempDist[2] = Vector2D(x, y).dist(kkARegion[id].bottomRight());
    tempDist[3] = Vector2D(x, y).dist(kkARegion[id].bottomLeft());

    double maxRadius = 0;
    for(int i = 0; i < 4; i ++)
    {
        if(tempDist[i] > maxRadius)
            maxRadius = tempDist[i];
    }

    double xx, yy;

    for(double r = 0; r <= maxRadius; r += step)
    {
        for(double theta = 0; theta <= 2*_PI; theta += step/xx)
        {
            xx = r*cos(theta) + x;
            yy = r*sin(theta) + y;
            if(id == 5)
            {
                if(kkARegion[id].contains(Vector2D(xx, yy)) && tPoly.contains(Vector2D(xx, yy)) && !wm->field->isInOppPenaltyArea(Vector2D(xx, yy)))
                {
                    if(kkCheckMovePos(xx, yy))
                        return;
                }
            }
            else
            {
                if(kkARegion[id].contains(Vector2D(xx, yy)) && tPoly.contains(Vector2D(xx, yy)))
                {
                    if(kkCheckMovePos(xx, yy))
                        return;
                }
            }
        }
    }
}
void CForceStart::kkAmirNewSearch(double x, double y, short id, Polygon2D &tPoly, double step)
{
    double tempDist[4];
    tempDist[0] = Vector2D(x, y).dist(kkARegion[id].topLeft());
    tempDist[1] = Vector2D(x, y).dist(kkARegion[id].topRight());
    tempDist[2] = Vector2D(x, y).dist(kkARegion[id].bottomRight());
    tempDist[3] = Vector2D(x, y).dist(kkARegion[id].bottomLeft());

    Polygon2D tempPoly;

    bool canKickToGoal;


    double maxRadius = 0;
    for(int i = 0; i < 4; i ++)
    {
        if(tempDist[i] > maxRadius)
            maxRadius = tempDist[i];
    }

    double xx, yy;

    for(double r = 0; r <= maxRadius; r += step)
    {
        for(double theta = 0; theta <= 2*_PI; theta += step/xx)
        {
            xx = r*cos(theta) + x;
            yy = r*sin(theta) + y;
            canKickToGoal = kkFindBestRegionsToKick(tempPoly,Vector2D(xx, yy));
            if(canKickToGoal) {
                if(id == 5)
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)) && tPoly.contains(Vector2D(xx, yy)) && canKickToGoal && !wm->field->isInOppPenaltyArea(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
                else
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)) && tPoly.contains(Vector2D(xx, yy)) && tempPoly.contains(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
            }
            else {
                if(id == 5)
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)) && tPoly.contains(Vector2D(xx, yy)) && !wm->field->isInOppPenaltyArea(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
                else
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)) && tPoly.contains(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
            }
        }
    }
}

void CForceStart::kkAmirNewSearch(double x, double y, short id, double step)
{
    double tempDist[4];
    tempDist[0] = Vector2D(x, y).dist(kkARegion[id].topLeft());
    tempDist[1] = Vector2D(x, y).dist(kkARegion[id].topRight());
    tempDist[2] = Vector2D(x, y).dist(kkARegion[id].bottomRight());
    tempDist[3] = Vector2D(x, y).dist(kkARegion[id].bottomLeft());

    Polygon2D tempPoly;
    bool canKickToGoal;

    double maxRadius = 0;
    for(int i = 0; i < 4; i ++)
    {
        if(tempDist[i] > maxRadius)
            maxRadius = tempDist[i];
    }

    double xx, yy;

    for(double r = 0; r <= maxRadius; r += step)
    {
        for(double theta = 0; theta <= 2*_PI; theta += step/xx)
        {
            xx = r*cos(theta) + x;
            yy = r*sin(theta) + y;
            canKickToGoal = kkFindBestRegionsToKick(tempPoly,Vector2D(xx, yy));

            if(canKickToGoal) {
                if(id == 5)
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)) && !wm->field->isInOppPenaltyArea(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
                else
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
            }
            else {
                if(id == 5)
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)) && !wm->field->isInOppPenaltyArea(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
                else
                {
                    if(kkARegion[id].contains(Vector2D(xx, yy)))
                    {
                        if(kkCheckMovePos(xx, yy))
                            return;
                    }
                }
            }
        }
    }
}

void CForceStart::kkInsertObstacleToList(int rectIndex)
{
    kkMoveObstacle.clear();
    int cnt = 0;
    for(int i = 0; i < wm->our.activeAgentsCount(); i++)
    {
        if(kkARegion[rectIndex].contains(wm->our.active(i)->pos))
            kkMoveObstacle.append(Circle2D(wm->our.active(i)->pos, 0.12));
        cnt++;
    }
    for(int i = 0; i < wm->opp.activeAgentsCount(); i++)
    {
        if(kkARegion[rectIndex].contains(wm->opp.active(i)->pos))
            kkMoveObstacle.append(Circle2D(wm->opp.active(i)->pos, 0.12));
        cnt++;
    }
    //debug(QString::number(cnt), 0);
}

Vector2D CForceStart::kkFindBestPosForMove(int fromAgent, int regionId, bool offensive)
{
    regionId--;
    if(regionId < 0) regionId = 0;
    if(regionId > 9) regionId = 9;
    kkInsertObstacleToList(regionId);
    Rect2D offRect, defRect;
    offRect.assign(kkARegion[regionId].center().x, kkARegion[regionId].top(), kkARegion[regionId].size().length()/2, kkARegion[regionId].size().width());
    defRect.assign(kkARegion[regionId].left(), kkARegion[regionId].top(), kkARegion[regionId].size().length()/2, kkARegion[regionId].size().width());
    if(offensive)
    {
        kkBestMovePos = Vector2D(offRect.center());
        //        kkAmirNewSearch(kkBestMovePos.x, kkBestMovePos.y,regionId, 0.08);
        kkAmirSearch(kkBestMovePos.x, kkBestMovePos.y, regionId, 0.08);
    }
    else
    {
        kkBestMovePos = Vector2D(defRect.center());
        // kkAmirNewSearch(kkBestMovePos.x, kkBestMovePos.y,regionId, 0.08);
        kkAmirSearch(kkBestMovePos.x, kkBestMovePos.y, regionId, 0.08);
    }
    return kkBestMovePos;
}

bool CForceStart::kkFindBestRegionsToPass(int regionIndex, double reachTime, Polygon2D &tPoly, int tException, bool regionException)
{
    if(regionException) {
        return true;
    }
    Vector2D tempBallPos = wm->ball->pos;
    Vector2D rectVertice[4];
    rectVertice[0] = kkARegion[regionIndex].topLeft();
    rectVertice[1] = kkARegion[regionIndex].topRight();
    rectVertice[2] = kkARegion[regionIndex].bottomRight();
    rectVertice[3] = kkARegion[regionIndex].bottomLeft();

    double verticeAngles[4][4];
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            verticeAngles[i][j] = 0.0;

    for(int i = 0; i < 4; i++)
    {
        for(int j = i; j < 4; j++)
        {
            verticeAngles[i][j] = Vector2D::angleOf(rectVertice[i], tempBallPos, rectVertice[j]).abs();
        }
    }

    double tempMaxAngle = 0.0;
    int aPoint = 0, bPoint = 0, farthestPoint;
    for(int i = 0; i < 4; i ++)
    {
        for(int j = i; j < 4; j++)
        {
            if(verticeAngles[i][j] > tempMaxAngle)
            {
                tempMaxAngle = verticeAngles[i][j];
                aPoint = i;
                bPoint = j;
            }
        }
    }

    double maxLegth = 0;
    for(int i = 0; i < 4; i++)
    {
        if(tempBallPos.dist(rectVertice[i]) > maxLegth)
        {
            if(aPoint == i || bPoint == i)
                continue;
            maxLegth = tempBallPos.dist(rectVertice[i]);
            farthestPoint = i;
        }
    }

    //Segment2D widestView(rectVertice[aPoint], rectVertice[bPoint]);
    Line2D tempLine(rectVertice[farthestPoint], (rectVertice[aPoint] - rectVertice[bPoint]).dir());
    Line2D tempRLine(tempBallPos, rectVertice[aPoint]);
    Line2D tempLLine(tempBallPos, rectVertice[bPoint]);
    Vector2D tempRPoint, tempLPoint;
    tempRPoint = tempLine.intersection(tempRLine);
    tempLPoint = tempLine.intersection(tempLLine);

    Polygon2D tempPoly;
    tempPoly.addVertex(tempLPoint);
    tempPoly.addVertex(tempRPoint);
    tempPoly.addVertex(tempBallPos);
    tempPoly.addVertex(tempLPoint);

    kkInsertPassObstacleToList(reachTime, tempPoly, Segment2D(tempBallPos, tempLPoint), Segment2D(tempBallPos, tempRPoint), tException);
    //sortObstacleByAngle(tempBallPos);

    QList<kk2Segment2D> blockedSegs;
    for(int i = 0; i < kkPassObstacle.length(); i++)
        blockedSegs.append(kkBlockedByRobots(kkPassObstacle.at(i), tempBallPos, tempLine));

    /*for(int i = 0; i < blockedSegs.length(); i++)
        draw(blockedSegs.at(i).seg1, QColor(Qt::cyan));*/
    kkMergeSegments(blockedSegs);
    QList<kk2Vector2D> vectorList;
    kkInvertSegments(blockedSegs, Segment2D(tempRPoint, tempLPoint), vectorList);

    tPoly.addVertex(tempBallPos);
    bool firstDraw = true;
    for(int i = 0; i < vectorList.length()/2; i++)
    {
        if(!firstDraw)
        {
            tPoly.addVertex(vectorList.at(i*2 - 1).vec2);
            tPoly.addVertex(vectorList.at(i*2).vec2);
        }
        tPoly.addVertex(vectorList.at(i*2).vec1);
        tPoly.addVertex(vectorList.at(i*2 + 1).vec1);
        firstDraw = false;
    }
    tPoly.addVertex(tempBallPos);
    draw(tPoly, QColor(255, 0, 0, 32), true);
    return ~firstDraw;
}

bool CForceStart::kkFindBestRegionsToKick(Polygon2D &tPoly, Vector2D passPos)
{
    Vector2D rectVertice[4];
    rectVertice[0] = Vector2D(_FIELD_WIDTH,_GOAL_WIDTH/2);//kkARegion[regionIndex].topLeft();
    rectVertice[1] = Vector2D(_FIELD_WIDTH+_GOAL_DEPTH,_GOAL_WIDTH/2);//kkARegion[regionIndex].topRight();
    rectVertice[2] = Vector2D(_FIELD_WIDTH+_GOAL_DEPTH,-_GOAL_WIDTH/2);//kkARegion[regionIndex].bottomRight();
    rectVertice[3] = Vector2D(_FIELD_WIDTH,-_GOAL_WIDTH/2);//_kkARegion[regionIndex].bottomLeft();

    double verticeAngles[4][4];
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            verticeAngles[i][j] = 0.0;

    for(int i = 0; i < 4; i++)
    {
        for(int j = i; j < 4; j++)
        {
            verticeAngles[i][j] = Vector2D::angleOf(rectVertice[i], passPos, rectVertice[j]).abs();
        }
    }

    double tempMaxAngle = 0.0;
    int aPoint = 0, bPoint = 0, farthestPoint;
    for(int i = 0; i < 4; i ++)
    {
        for(int j = i; j < 4; j++)
        {
            if(verticeAngles[i][j] > tempMaxAngle)
            {
                tempMaxAngle = verticeAngles[i][j];
                aPoint = i;
                bPoint = j;
            }
        }
    }

    double maxLegth = 0;
    for(int i = 0; i < 4; i++)
    {
        if(passPos.dist(rectVertice[i]) > maxLegth)
        {
            if(aPoint == i || bPoint == i)
                continue;
            maxLegth = passPos.dist(rectVertice[i]);
            farthestPoint = i;
        }
    }

    //Segment2D widestView(rectVertice[aPoint], rectVertice[bPoint]);
    Line2D tempLine(rectVertice[farthestPoint], (rectVertice[aPoint] - rectVertice[bPoint]).dir());
    Line2D tempRLine(passPos, rectVertice[aPoint]);
    Line2D tempLLine(passPos, rectVertice[bPoint]);
    Vector2D tempRPoint, tempLPoint;
    tempRPoint = tempLine.intersection(tempRLine);
    tempLPoint = tempLine.intersection(tempLLine);

    Polygon2D tempPoly;
    tempPoly.addVertex(tempLPoint);
    tempPoly.addVertex(tempRPoint);
    tempPoly.addVertex(passPos);
    tempPoly.addVertex(tempLPoint);

    kkInsertPassObstacleToList(.0, tempPoly, Segment2D(passPos, tempLPoint), Segment2D(passPos, tempRPoint), -1);
    //sortObstacleByAngle(passPos);

    QList<kk2Segment2D> blockedSegs;
    for(int i = 0; i < kkPassObstacle.length(); i++)
        blockedSegs.append(kkBlockedByRobots(kkPassObstacle.at(i), passPos, tempLine));

    /*for(int i = 0; i < blockedSegs.length(); i++)
        draw(blockedSegs.at(i).seg1, QColor(Qt::cyan));*/
    kkMergeSegments(blockedSegs);
    QList<kk2Vector2D> vectorList;
    kkInvertSegments(blockedSegs, Segment2D(tempRPoint, tempLPoint), vectorList);

    tPoly.addVertex(passPos);
    bool firstDraw = true;
    for(int i = 0; i < vectorList.length()/2; i++)
    {
        if(!firstDraw)
        {
            tPoly.addVertex(vectorList.at(i*2 - 1).vec2);
            tPoly.addVertex(vectorList.at(i*2).vec2);
        }
        tPoly.addVertex(vectorList.at(i*2).vec1);
        tPoly.addVertex(vectorList.at(i*2 + 1).vec1);
        firstDraw = false;
    }
    tPoly.addVertex(passPos);
    draw(tPoly, QColor(255, 0, 0, 32), true);
    return ~firstDraw;
}


bool CForceStart::kkHasOverlap(Segment2D seg1, Segment2D seg2)
{
    if(seg1.contains(seg2.a()) ||
            seg1.contains(seg2.b()) ||
            seg2.contains(seg1.a()) ||
            seg2.contains(seg1.b()))
        return true;
    return false;
}

kk2Segment2D CForceStart::kkMergeTwoSegments(kk2Segment2D seg1, kk2Segment2D seg2)
{
    kk2Segment2D temp2Seg;
    Vector2D vertice[4];
    vertice[0] = seg1.seg1.a();
    vertice[1] = seg1.seg1.b();
    vertice[2] = seg2.seg1.a();
    vertice[3] = seg2.seg1.b();

    Vector2D tangVertice[4];
    tangVertice[0] = seg1.seg2.a();
    tangVertice[1] = seg1.seg2.b();
    tangVertice[2] = seg2.seg2.a();
    tangVertice[3] = seg2.seg2.b();

    double distance[4][4];
    for(int i = 0; i < 4; i++)
        for(int j = i; j < 4; j++)
            distance[i][j] = vertice[i].dist(vertice[j]);

    double maxDist = 0.0;
    int tA = 0, tB = 3;

    for(int i = 0; i < 4; i++)
    {
        for(int j = i; j < 4; j++)
        {
            if(distance[i][j] > maxDist && i != j)
            {
                maxDist = distance[i][j];
                tA = i;
                tB = j;
            }
        }
    }
    temp2Seg.seg1 = Segment2D(vertice[tA], vertice[tB]);
    temp2Seg.seg2 = Segment2D(tangVertice[tA], tangVertice[tB]);
    return temp2Seg;
}

void CForceStart::kkMergeSegments(QList<kk2Segment2D> &segList)
{
    if(segList.length() <= 0)
        return;
    bool check = false;
    for(int i = 0; i < segList.length(); i++)
    {
        for(int j = i; j < segList.length(); j++)
        {
            if(kkHasOverlap(segList.at(i).seg1, segList.at(j).seg1) && i != j)
            {
                segList.replace(i ,kkMergeTwoSegments(segList.at(i), segList.at(j)));
                segList.removeAt(j);
                check = true;
                break;
            }
        }
        if(check) break;
    }

    if(check)
        kkMergeSegments(segList);
}

void CForceStart::kkInvertSegments(QList<kk2Segment2D> &segList, Segment2D tSeg, QList<kk2Vector2D> &vecList)
{
    kk2Vector2D temp2Vec;
    if(segList.length() <= 0)
    {
        temp2Vec.vec1 = tSeg.a();
        temp2Vec.vec2 = tSeg.a();
        vecList.append(temp2Vec);
        temp2Vec.vec1 = tSeg.b();
        temp2Vec.vec2 = tSeg.b();
        vecList.append(temp2Vec);
        return;
    }
    QList<double> distList;
    QList<int> idList;
    QList<kk2Vector2D> vectorList;
    AngleDeg tempDegree = (tSeg.a() - tSeg.b()).dir();
    for(int i = 0; i < segList.length(); i++)
    {
        if(tempDegree == (tSeg.a() -  segList.at(i).seg1.a()).dir() )
            distList.append(tSeg.a().dist(segList.at(i).seg1.a()));
        else
            distList.append(-tSeg.a().dist(segList.at(i).seg1.a()));
        temp2Vec.vec1 = segList.at(i).seg1.a();
        temp2Vec.vec2 = segList.at(i).seg2.a();
        vectorList.append(temp2Vec);
        idList.append(i*2);

        if(tempDegree == (tSeg.a() -  segList.at(i).seg1.b()).dir())
            distList.append(tSeg.a().dist(segList.at(i).seg1.b()));
        else
            distList.append(-tSeg.a().dist(segList.at(i).seg1.b()));
        temp2Vec.vec1 = segList.at(i).seg1.b();
        temp2Vec.vec2 = segList.at(i).seg2.b();
        vectorList.append(temp2Vec);
        idList.append(i*2 + 1);
    }

    double temp;
    int tempID;
    for(int i = 0; i < distList.length(); i++)
    {
        for(int j = i; j < distList.length(); j++)
        {
            if(distList.at(i) > distList.at(j))
            {
                temp = distList.at(j);
                distList.replace(j, distList.at(i));
                distList.replace(i, temp);

                tempID = idList.at(j);
                idList.replace(j, idList.at(i));
                idList.replace(i, tempID);
            }
        }
    }

    for(int i = 0; i < idList.length(); i++)
    {
        temp2Vec.vec1 = vectorList.at(idList.at(i)).vec1;
        temp2Vec.vec2 = vectorList.at(idList.at(i)).vec2;
        vecList.append(temp2Vec);
    }

    bool checkA = false, checkB = false;

    if(vecList.length() >= 2)
    {
        if(Segment2D(vecList.at(0).vec1, vecList.at(1).vec1).contains(tSeg.a()))
            checkA = true;

        if(Segment2D(vecList.at(vecList.length() - 2).vec1, vecList.at(vecList.length() - 1).vec1).contains(tSeg.b()))
            checkB = true;
    }

    if(checkA)
        vecList.removeAt(0);
    else
    {
        temp2Vec.vec1 = tSeg.a();
        temp2Vec.vec2 = tSeg.a();
        vecList.prepend(temp2Vec);
    }

    if(checkB)
        vecList.removeLast();
    else
    {
        temp2Vec.vec1 = tSeg.b();
        temp2Vec.vec2 = tSeg.b();
        vecList.append(temp2Vec);
    }
}

kk2Segment2D CForceStart::kkBlockedByRobots(Circle2D tRobot, Vector2D tOrigin, Line2D tLine)
{
    kk2Segment2D temp2Seg;
    Vector2D tangentPointR, tangentPointL;
    tRobot.tangent(tOrigin, &tangentPointR, &tangentPointL);
    Line2D tempRLine(tOrigin, tangentPointR);
    Line2D tempLLine(tOrigin, tangentPointL);
    Vector2D tempRPoint, tempLPoint;
    tempRPoint = tLine.intersection(tempRLine);
    tempLPoint = tLine.intersection(tempLLine);
    temp2Seg.seg1 = Segment2D(tempLPoint, tempRPoint);
    temp2Seg.seg2 = Segment2D(tangentPointL, tangentPointR);
    return temp2Seg;
}

void CForceStart::kkInsertPassObstacleToList(double reachTime, Polygon2D mPoly, Segment2D lLine, Segment2D rLine, int tException)
{
    kkPassObstacle.clear();
    Circle2D oppCircle;
    double addedRadius;
    Vector2D sol1, sol2;
    for(int i = 0; i < wm->our.activeAgentsCount(); i++)
    {
        if(tException != -1)
            if(wm->our.active(i)->id == kkAgentsId[tException])
                continue;
        oppCircle.assign(wm->our.active(i)->pos, 0.12);
        if(mPoly.contains(wm->our.active(i)->pos))
        {
            kkPassObstacle.append(oppCircle);
        }
        else
        {
            if(oppCircle.intersection(lLine, &sol1, &sol2) > 0 || oppCircle.intersection(rLine, &sol1, &sol2))
            {
                kkPassObstacle.append(oppCircle);
            }
        }
    }
    for(int i = 0; i < wm->opp.activeAgentsCount(); i++)
    {
        addedRadius = wm->opp.active(i)->vel.length()*reachTime;
        oppCircle.assign(wm->opp.active(i)->pos, 0.15 + addedRadius);
        if(mPoly.contains(wm->opp.active(i)->pos))
        {
            kkPassObstacle.append(oppCircle);
        }
        else
        {
            if(oppCircle.intersection(lLine, &sol1, &sol2) > 0 || oppCircle.intersection(rLine, &sol1, &sol2))
            {
                kkPassObstacle.append(oppCircle);
            }
        }
    }
}

void CForceStart::kkSortObstacleByAngle(Vector2D tBallPos)
{
    QList<double> agentAngles;
    QList<int> agentIDs;
    for(int i = 0; i < kkPassObstacle.length(); i++)
    {
        agentAngles.append((kkPassObstacle.at(i).center() - tBallPos).th().degree());
        agentIDs.append(i);
    }

    double temp;
    int tempID;
    for(int i = 0; i < agentAngles.length(); i++)
    {
        for(int j = i; j < agentAngles.length(); j++)
        {
            if(agentAngles.at(i) > agentAngles.at(j))
            {
                temp = agentAngles.at(j);
                agentAngles.replace(j, agentAngles.at(i));
                agentAngles.replace(i, temp);

                tempID = agentIDs.at(j);
                agentIDs.replace(j, agentIDs.at(i));
                agentIDs.replace(i, tempID);
            }
        }
    }

    QList<Circle2D> tempObstacle;
    for(int i = 0; i < agentIDs.length(); i++)
        tempObstacle.append(kkPassObstacle.at(agentIDs.at(i)));

    kkPassObstacle.clear();
    for(int i = 0; i < agentIDs.length(); i++)
        kkPassObstacle.append(tempObstacle.at(i));
}

Vector2D CForceStart::kkFindBestPosForPass(int fromAgent, int regionId, bool offensive, double reachTime, int tException)
{
    regionId--;
    if(regionId < 0) regionId = 0;
    if(regionId > 9) regionId = 9;
    kkInsertObstacleToList(regionId);
    Rect2D offRect, defRect;
    offRect.assign(kkARegion[regionId].center().x, kkARegion[regionId].top(), kkARegion[regionId].size().length()/2, kkARegion[regionId].size().width());
    defRect.assign(kkARegion[regionId].left(), kkARegion[regionId].top(), kkARegion[regionId].size().length()/2, kkARegion[regionId].size().width());

    Polygon2D tempPoly;
    bool tBool = false;
    bool canKick = kkFindBestRegionsToPass(regionId, reachTime, tempPoly, tException, tBool);

    if(offensive)
        kkBestMovePos = Vector2D(offRect.center());
    else
        kkBestMovePos = Vector2D(defRect.center());
    if(canKick)
        kkAmirSearch(kkBestMovePos.x, kkBestMovePos.y, regionId, tempPoly, 0.08);
    //kkAmirNewSearch(kkBestMovePos.x, kkBestMovePos.y,regionId,tempPoly, 0.08);
    else
        kkAmirSearch(kkBestMovePos.x, kkBestMovePos.y, regionId, 0.08);
    // kkAmirNewSearch(kkBestMovePos.x, kkBestMovePos.y,regionId, 0.08);
    return kkBestMovePos;
}

Vector2D CForceStart::kkNewFindBestPosForPass(int fromAgent, int regionId, int regionIdPasser, bool offensive, double reachTime, int tException)
{
    regionId--;
    bool hardcore = 1;
    if(regionId < 0) regionId = 0;
    if(regionId > 9) regionId = 9;
    kkInsertObstacleToList(regionId);
    Rect2D offRect, defRect;
    offRect.assign(kkARegion[regionId].center().x, kkARegion[regionId].top(), kkARegion[regionId].size().length()/2, kkARegion[regionId].size().width());
    defRect.assign(kkARegion[regionId].left(), kkARegion[regionId].top(), kkARegion[regionId].size().length()/2, kkARegion[regionId].size().width());
    Vector2D pointInRect[3][3];
    Polygon2D tempPoly;
    bool tBool = false;
    if(regionId == 0) tBool = true;
    bool canKick = kkFindBestRegionsToPass(regionId, reachTime, tempPoly, tException, tBool);

    pointInRect[0][0] = (kkARegion[regionId].center() + kkARegion[regionId].topLeft())/2;
    pointInRect[0][1] = (kkARegion[regionId].center() + Vector2D(kkARegion[regionId].left(),kkARegion[regionId].center().y))/2;
    pointInRect[0][2] = (kkARegion[regionId].center() + kkARegion[regionId].bottomLeft())/2;
    pointInRect[1][0] = (kkARegion[regionId].center() + Vector2D(kkARegion[regionId].center().x,kkARegion[regionId].top()))/2;
    pointInRect[1][1] =  kkARegion[regionId].center()/2;
    pointInRect[1][2] = (kkARegion[regionId].center() + Vector2D(kkARegion[regionId].center().x,kkARegion[regionId].bottom()))/2;
    pointInRect[2][0] = (kkARegion[regionId].center() + Vector2D(kkARegion[regionId].topRight()))/2;
    pointInRect[2][1] = (kkARegion[regionId].center() + Vector2D(kkARegion[regionId].right(),kkARegion[regionId].center().y))/2;
    pointInRect[2][2] = (kkARegion[regionId].center() + Vector2D(kkARegion[regionId].bottomRight()))/2;


    if(((((regionId == 2 || regionId == 3)  && hardcore) || ((regionId == 7 || regionId == 8 || regionId == 9) && !hardcore)) && regionId != 0) &&  !offensive)
        kkBestMovePos = pointInRect[0][0];
    else if(((((regionId == 1 || regionId == 2 || regionId == 3)  && !hardcore) || ((regionId == 8 || regionId == 9) && hardcore)) && regionId != 0) &&  !offensive)
        kkBestMovePos = pointInRect[0][2];
    else if(!offensive && !hardcore && (regionId != 0 && regionId != 1 && regionId != 4 && regionId != 7))
        kkBestMovePos = pointInRect[0][1];
    else if((hardcore && offensive) && (regionId == 7 || regionId == 8) && (regionId != 3 && regionId != 6 && regionId != 9))
        kkBestMovePos = pointInRect[2][0];
    else if(hardcore && offensive && (regionId == 1 || regionId == 2) && (regionId != 3 && regionId != 6 && regionId != 9))
        kkBestMovePos = pointInRect[2][2];
    else if((offensive && !hardcore) && (regionId != 3 && regionId != 6 && regionId != 9))
        kkBestMovePos = pointInRect[2][1];
    else if(((regionId == 1) && !offensive && hardcore) || (regionId == 9 && hardcore && offensive))
        kkBestMovePos = pointInRect[1][0];
    else if((regionId == 7) && !offensive && hardcore || (regionId == 3 && hardcore && offensive))
        kkBestMovePos = pointInRect[1][2];
    else if((offensive && !hardcore && (regionId == 3 || regionId == 6 || regionId == 9)) || (!offensive && !hardcore && (regionId == 1 || regionId == 4 || regionId == 7)))
        kkBestMovePos = pointInRect[1][1];
    else if(hardcore && (regionId == 0 || regionId == 4 || regionId == 5))
        kkBestMovePos = pointInRect[2][rankByBallDist(1,pointInRect)];
    else if(offensive)
        kkBestMovePos = Vector2D(offRect.center());
    else
        kkBestMovePos = Vector2D(defRect.center());

    if((regionId == regionIdPasser) && offensive) {
        kkBestMovePos = pointInRect[2][rankByBallDist(2,pointInRect)];
    }

    if(regionId == 0) return kkBestMovePos;

    if(canKick)
        kkAmirSearch(kkBestMovePos.x, kkBestMovePos.y, regionId, tempPoly, 0.08);
    //kkAmirNewSearch(kkBestMovePos.x, kkBestMovePos.y,regionId,tempPoly, 0.08);
    else
        kkAmirSearch(kkBestMovePos.x, kkBestMovePos.y, regionId, 0.08);
    // kkAmirNewSearch(kkBestMovePos.x, kkBestMovePos.y,regionId, 0.08);
    return kkBestMovePos;
}

bool CForceStart::kkCheckIntersectWithAgents(Segment2D tSeg)
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

bool CForceStart::kkIsPathToOppGoaliClear(Vector2D tPoint)
{
    Segment2D segToOppGoali[5];
    Vector2D tempMousePos = tPoint;
    int tCnt = 0;
    int intersectCnt = 0;
    for(double y = wm->field->oppGoalR().y; y <= wm->field->oppGoalL().y; y += _GOAL_WIDTH/4.01)
    {
        segToOppGoali[tCnt].assign(tempMousePos, Vector2D(wm->field->oppGoalR().x, y));
        if(kkCheckIntersectWithAgents(segToOppGoali[tCnt]))
            intersectCnt++;
        draw(segToOppGoali[tCnt], QColor(Qt::green));
        tCnt++;
    }
    if(intersectCnt < 5)
        return true;
    else
        return false;
}

bool CForceStart::kkIsPathToOppGoaliClear()
{
    return kkIsPathToOppGoaliClear(wm->ball->pos);
}

bool CForceStart::kkIsPathFromAToBClear(Vector2D tA, Vector2D tB, double tTolerance)
{
    Line2D tempLine(tA, tB);
    Line2D perpenLine = tempLine.perpendicular(tB);
    Circle2D tempCircle(tB, tTolerance);
    Vector2D sol1, sol2;
    tempCircle.intersection(perpenLine, &sol1, &sol2);

    int intersectCnt = 0;

    if(kkCheckIntersectWithAgents(Segment2D(tA, tB)))
        intersectCnt++;

    draw(Segment2D(tA, tB),QColor(Qt::cyan));

    if(kkCheckIntersectWithAgents(Segment2D(tA, sol1)))
        intersectCnt++;

    draw(Segment2D(tA, sol1),QColor(Qt::cyan));

    if(kkCheckIntersectWithAgents(Segment2D(tA, sol2)))
        intersectCnt++;

    draw(Segment2D(tA, sol2),QColor(Qt::cyan));

    if(intersectCnt < 3)
        return true;
    else
        return false;
}

void CForceStart::kkAssignTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill, int tA, int tB)
{
    //agentTask[tAgent]->setAgent(soccer->agents[agentsId[tAgent]]);

    debug(QString::number(kkGetIntByEnum(tSkill)), 0);
    Vector2D tempA, tempB;
    kkActiveAgent[tAgent] = true;
    tRole->setIsActive(true);
    tRole->setAgent(knowledge->getAgent(kkAgentsId[tAgent]));
    tRole->setAgentID(tAgent);
    tRole->setTask(tSkill);
    switch(tSkill)
    {
    case None:
        break;
    case MoveOffensive:
        tempB = kkFindBestPosForMove(tAgent, tB, true);
        tRole->setTarget(tempB);
        tRole->setTargetVel(wm->ball->pos);
        tRole->setTargetVel(Vector2D(0, 0));
        tRole->setSelectedSkill(SkillGotopointAvoid);
        break;
    case MoveDefensive:
        tempB = kkFindBestPosForMove(tAgent, tB, false);
        tRole->setTarget(tempB);
        tRole->setTargetVel(wm->ball->pos);
        tRole->setTargetVel(Vector2D(0, 0));
        tRole->setSelectedSkill(SkillGotopointAvoid);
        break;
    case PassOffensive:
        tempB = kkFindBestPosForPass(tAgent, tB, true, 1, kkPassReceiverAgent);
        kkPointToPass = tempB;
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.1);
        if(kkIsPathFromAToBClear(tempA, tempB, 0.05))
        {
            tRole->setChip(false);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(6);
            else
                tRole->setKickSpeed(currentSpeeds.pass);
        }
        else
        {
            tRole->setChip(true);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(10);
            else
                tRole->setKickSpeed(currentSpeeds.chip);
        }
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(true);
        tRole->setSelectedSkill(SkillKick);
        break;
    case PassDefensive:
        tempB = kkFindBestPosForPass(tAgent, tB, false, 0, kkPassReceiverAgent);
        kkPointToPass = tempB;
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.1);
        if(kkIsPathFromAToBClear(tempA, tempB, 0.05))
        {
            tRole->setChip(false);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(6);
            else
                tRole->setKickSpeed(currentSpeeds.pass);
        }
        else
        {
            tRole->setChip(true);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(10);
            else
                tRole->setKickSpeed(currentSpeeds.chip);
        }
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(true);
        tRole->setSelectedSkill(SkillKick);
        break;
    case KickOffensive:
        tempB = kkFindBestPosForPass(tAgent, tB, true, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.kick);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case KickDefensive:
        tempB = kkFindBestPosForPass(tAgent, tB, false, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.kick);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ChipOffensive:
        tempB = kkFindBestPosForPass(tAgent, tB, true, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(true);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.chip);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ChipDefensive:
        tempB = kkFindBestPosForPass(tAgent, tB, false, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(true);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.chip);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case MarkOffensive:
        tRole->setSelectedSkill(SkillMark);
        break;
    case MarkDefensive:
        tRole->setSelectedSkill(SkillMark);
        break;
    case CatchBall:
        tempB = wm->field->oppGoal();
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        tRole->setKickSpeed(0);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(false);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case Shot:
        tempB = wm->field->oppGoal();
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(10);
        else
            tRole->setKickSpeed(currentSpeeds.shot);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(false);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ChipToGoal:
        tempB = wm->field->oppGoal();
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(true);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(10);
        else
            tRole->setKickSpeed(currentSpeeds.ChipToGoal);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(false);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ReceivePass:
        tempA = kkPointToPass;
        tRole->setWaitPos(tempA);
        tRole->setSlow(false);
        tRole->setReceiveRadius(1.75);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setSelectedSkill(SkillReceivePass);
        break;
    case OneTouch:
        tempA = kkPointToPass - Vector2D(0.1, 0);
        tempB = wm->field->oppGoal();
        tRole->setWaitPos(tempA);
        tRole->setTarget(tempB);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(10);
        else
            tRole->setKickSpeed(currentSpeeds.chip);
        tRole->setChip(false);
        tRole->setAvoidPenaltyArea(true);
        tRole->setSelectedSkill(SkillOneTouch);
        break;
    }

}

void CForceStart::kkNewAssignTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill, int tA, int tB)
{
    //agentTask[tAgent]->setAgent(soccer->agents[agentsId[tAgent]]);

    debug(QString::number(kkGetIntByEnum(tSkill)), 0);
    Vector2D tempA, tempB;
    kkActiveAgent[tAgent] = true;
    tRole->setIsActive(true);
    tRole->setAgentID(tAgent);
    tRole->setTask(tSkill);
    tRole->setAgent(knowledge->getAgent(kkAgentsId[tAgent]));
    switch(tSkill)
    {
    case None:
        break;
    case MoveOffensive:
        tempB = kkFindBestPosForMove(tAgent, tB, true);
        tRole->setTarget(tempB);
        tRole->setTargetVel(wm->ball->pos);
        tRole->setTargetVel(Vector2D(0, 0));
        tRole->setSelectedSkill(SkillGotopointAvoid);
        break;
    case MoveDefensive:
        tempB = kkFindBestPosForMove(tAgent, tB, false);
        tRole->setTarget(tempB);
        tRole->setTargetVel(wm->ball->pos);
        tRole->setTargetVel(Vector2D(0, 0));
        tRole->setSelectedSkill(SkillGotopointAvoid);
        break;
    case PassOffensive:
        tempB = kkNewFindBestPosForPass(tAgent, tB, true, 1, kkPassReceiverAgent);
        kkPointToPass = tempB;
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.1);
        if(kkIsPathFromAToBClear(tempA, tempB, 0.05))
        {
            tRole->setChip(false);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(6);
            else
                tRole->setKickSpeed(currentSpeeds.pass);
        }
        else
        {
            tRole->setChip(true);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(10);
            else
                tRole->setKickSpeed(currentSpeeds.chip);
        }
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(true);
        tRole->setSelectedSkill(SkillKick);
        break;
    case PassDefensive:
        tempB = kkNewFindBestPosForPass(tAgent, tB, false, 0, kkPassReceiverAgent);
        kkPointToPass = tempB;
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.1);
        if(kkIsPathFromAToBClear(tempA, tempB, 0.05))
        {
            tRole->setChip(false);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(6);
            else
                tRole->setKickSpeed(currentSpeeds.pass);
        }
        else
        {
            tRole->setChip(true);
            if(wm->getIsSimulMode())
                tRole->setKickSpeed(10);
            else
                tRole->setKickSpeed(currentSpeeds.chip);
        }
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(true);
        tRole->setSelectedSkill(SkillKick);
        break;
    case KickOffensive:
        tempB = kkNewFindBestPosForPass(tAgent, tB, tA, true, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.kick);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case KickDefensive:
        tempB = kkNewFindBestPosForPass(tAgent, tB, tA, false, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.kick);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ChipOffensive:
        tempB = kkNewFindBestPosForPass(tAgent, tB, tA, true, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(true);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.chip);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ChipDefensive:
        tempB = kkNewFindBestPosForPass(tAgent, tB, tA, false, 0.0);
        tempA = wm->ball->pos;
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(true);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(8);
        else
            tRole->setKickSpeed(currentSpeeds.chip);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(true);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case MarkOffensive:
        //tRole->setSelectedSkill(SkillMark);
        //markAgents.append(knowledge->getAgent(kkAgentsId[tAgent]));
        kkMarkList.append(knowledge->getAgent(kkAgentsId[tAgent])->id());
        break;
    case MarkDefensive:
        //tRole->setSelectedSkill(SkillMark);
        //markAgents.append(knowledge->getAgent(kkAgentsId[tAgent]));
        kkMarkList.append(knowledge->getAgent(kkAgentsId[tAgent])->id());
        break;
    case CatchBall:
        tempB = wm->field->oppGoal();
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        tRole->setKickSpeed(0);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(false);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case Shot:
        tempB = wm->field->oppGoal();
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(false);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(10);
        else
            tRole->setKickSpeed(currentSpeeds.shot);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(false);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ChipToGoal:
        tempB = wm->field->oppGoal();
        tRole->setTarget(tempB);
        tRole->setTolerance(0.2);
        tRole->setChip(true);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(10);
        else
            tRole->setKickSpeed(currentSpeeds.ChipToGoal);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setIsGotoPointAvoid(false);
        tRole->setSlow(false);
        tRole->setSelectedSkill(SkillKick);
        break;
    case ReceivePass:
        tempA = kkPointToPass;
        tRole->setWaitPos(tempA);
        tRole->setSlow(false);
        tRole->setReceiveRadius(1.75);
        tRole->setAvoidPenaltyArea(true);
        tRole->setAvoidCenterCircle(false);
        tRole->setSelectedSkill(SkillReceivePass);
        break;
    case OneTouch:
        tempA = kkPointToPass - Vector2D(0.1, 0);
        if(knowledge->getAgent(kkAgentsId[tAgent])->pos().y >= 0)
            tempB = wm->field->oppGoal() + Vector2D(0, 0.4);
        else
            tempB = wm->field->oppGoal() - Vector2D(0, 0.4);
        tRole->setWaitPos(tempA);
        tRole->setTarget(tempB);
        if(wm->getIsSimulMode())
            tRole->setKickSpeed(10);
        else
            tRole->setKickSpeed(currentSpeeds.shot);
        tRole->setChip(false);
        tRole->setAvoidPenaltyArea(true);
        tRole->setSelectedSkill(SkillOneTouch);
        break;
    }

}

bool CForceStart::kkHasReceiveOrOneTouch(int tAgent)
{
    for(int i = 0; i < 4; i++)
    {
        if(kkCurrentPlan.agents[tAgent].skill[i] != None)
        {
            if(kkCurrentPlan.agents[tAgent].skill[i] == ReceivePass ||
                    kkCurrentPlan.agents[tAgent].skill[i] == OneTouch )
                return true;
        }
    }
    return false;
}

void CForceStart::kkChooseBestTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill[], int tA, int tB)
{
    int tempSkillSize = 0;
    for(int i = 0; i < 4; i++)
        if(tSkill[i] != None)
            tempSkillSize++;
    if(tempSkillSize == 1)
    {
        kkAssignTask(tAgent, tRole, tSkill[0], tA, tB);
        return;
    }
    Vector2D tempA, tempB;
    for(int i = 0; i < tempSkillSize; i++)
    {
        if(i == tempSkillSize - 1)
        {
            kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
        }
        switch(tSkill[i])
        {
        case PassOffensive:
            tempA = kkFindBestPosForPass(tAgent, tB, true, 0, kkPassReceiverAgent);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.10))
            {
                kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case PassDefensive:
            tempA = kkFindBestPosForPass(tAgent, tB, false, 0, kkPassReceiverAgent);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.10))
            {
                kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case KickOffensive:
            tempA = kkFindBestPosForPass(tAgent, tB, true, 0);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.15))
            {
                kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case KickDefensive:
            tempA = kkFindBestPosForPass(tAgent, tB, false, 0);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.15))
            {
                kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case OneTouch:
            tempB = kkPointToPass;
            if(kkIsPathToOppGoaliClear(tempB))
            {
                kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case Shot:
            if(kkIsPathToOppGoaliClear())
            {
                kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case MarkDefensive:
            kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
            break;
        case MarkOffensive:
            kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
            break;

        default:
            kkAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
            break;
        }
    }
}

void CForceStart::kkNewChooseBestTask(int tAgent, CRolePlayOn *tRole, PSkills tSkill[], int tA, int tB)
{
    int tempSkillSize = 0;
    for(int i = 0; i < 4; i++)
        if(tSkill[i] != None)
            tempSkillSize++;
    if(tempSkillSize == 1)
    {
        kkNewAssignTask(tAgent, tRole, tSkill[0], tA, tB);
        return;
    }
    Vector2D tempA, tempB;
    for(int i = 0; i < tempSkillSize; i++)
    {
        if(i == tempSkillSize - 1)
        {
            kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
        }
        switch(tSkill[i])
        {
        case PassOffensive:
            tempA = kkNewFindBestPosForPass(tAgent, tB, true, 0, kkPassReceiverAgent);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.10))
            {
                kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case PassDefensive:
            tempA = kkNewFindBestPosForPass(tAgent, tB, false, 0, kkPassReceiverAgent);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.10))
            {
                kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case KickOffensive:
            tempA = kkNewFindBestPosForPass(tAgent, tB, tA, true, 0);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.15))
            {
                kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case KickDefensive:
            tempA = kkNewFindBestPosForPass(tAgent, tB, tA, false, 0);
            tempB = wm->ball->pos;
            if(kkIsPathFromAToBClear(tempA, tempB, 0.15))
            {
                kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case OneTouch:
            tempB = kkPointToPass;
            if(kkIsPathToOppGoaliClear(tempB))
            {
                kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case Shot:
            if(kkIsPathToOppGoaliClear())
            {
                kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
                return;
            }
            break;
        case MarkDefensive:
            kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
            break;
        case MarkOffensive:
            kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
            break;

        default:
            kkNewAssignTask(tAgent, tRole, tSkill[i], tA, tB);
            return;
            break;
        }
    }
}


void CForceStart::kkAssignTasks()
{
    for(int i = 0; i < 5; i++)
        kkActiveAgent[i] = false;
    int receiveIndex = -1;
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        if(kkHasReceiveOrOneTouch(i))
        {
            kkPassReceiverAgent = i;
            break;
        }
    }
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        if(!kkHasReceiveOrOneTouch(i))
        {
            switch(i)
            {
            case 0:
                kkChooseBestTask(i,
                                 agentSkill[0],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 1:
                kkChooseBestTask(i,
                                 agentSkill[1],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 2:
                kkChooseBestTask(i,
                                 agentSkill[2],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 3:
                kkChooseBestTask(i,
                                 agentSkill[3],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 4:
                kkChooseBestTask(i,
                                 agentSkill[4],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            }
        }
        else
            receiveIndex = i;
    }
    if(receiveIndex != -1)
    {
        int i = receiveIndex;
        switch(i)
        {
        case 0:
            kkChooseBestTask(i,
                             agentSkill[0],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 1:
            kkChooseBestTask(i,
                             agentSkill[1],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 2:
            kkChooseBestTask(i,
                             agentSkill[2],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 3:
            kkChooseBestTask(i,
                             agentSkill[3],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 4:
            kkChooseBestTask(i,
                             agentSkill[4],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        }
    }
}

void CForceStart::kkNewAssignTasks()
{
    for(int i = 0; i < 5; i++)
        kkActiveAgent[i] = false;
    int receiveIndex = -1;
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        if(kkHasReceiveOrOneTouch(i))
        {
            kkPassReceiverAgent = i;
            break;
        }
    }
    for(int i = 0; i < kkCurrentPlan.agentsSize; i++)
    {
        if(!kkHasReceiveOrOneTouch(i))
        {
            switch(i)
            {
            case 0:
                kkNewChooseBestTask(i,
                                    agentSkill[0],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 1:
                kkNewChooseBestTask(i,
                                    agentSkill[1],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 2:
                kkNewChooseBestTask(i,
                                    agentSkill[2],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 3:
                kkNewChooseBestTask(i,
                                    agentSkill[3],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            case 4:
                kkNewChooseBestTask(i,
                                    agentSkill[4],
                        kkCurrentPlan.agents[i].skill,
                        kkCurrentPlan.agents[i].A,
                        kkCurrentPlan.agents[i].B);
                break;
            }
        }
        else
            receiveIndex = i;
    }
    if(receiveIndex != -1)
    {
        int i = receiveIndex;
        switch(i)
        {
        case 0:
            kkNewChooseBestTask(i,
                                agentSkill[0],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 1:
            kkNewChooseBestTask(i,
                                agentSkill[1],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 2:
            kkNewChooseBestTask(i,
                                agentSkill[2],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 3:
            kkNewChooseBestTask(i,
                                agentSkill[3],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        case 4:
            kkNewChooseBestTask(i,
                                agentSkill[4],
                    kkCurrentPlan.agents[i].skill,
                    kkCurrentPlan.agents[i].A,
                    kkCurrentPlan.agents[i].B);
            break;
        }
    }
}

void CForceStart::kkMoveManager(CAgent *_agent,CRolePlayOn &_skill)
{
    if(_skill.getSelectedSkill() == SkillGotopoint) {
        CSkillGotoPoint gotoPointSkill(_agent);
        gotoPointSkill.setFinalPos(_skill.getTarget());
        gotoPointSkill.setFinalDir(_skill.getTargetDir());
        gotoPointSkill.setFinalVel(_skill.getTargetVel());
        gotoPointSkill.execute();


    }
    else if(_skill.getSelectedSkill() == SkillGotopointAvoid) {
        CSkillGotoPointAvoid gotoPointAvoidSkill(_agent);
        gotoPointAvoidSkill.setFinalPos(_skill.getTarget());
        gotoPointAvoidSkill.setFinalDir(_skill.getTargetDir());
        gotoPointAvoidSkill.setFinalVel(_skill.getTargetVel());
        gotoPointAvoidSkill.setAvoidCenterCircle(_skill.getAvoidCenterCircle());
        gotoPointAvoidSkill.setAvoidPenaltyArea(_skill.getAvoidPenaltyArea());
        gotoPointAvoidSkill.setMaxVelocity(3.0);

        gotoPointAvoidSkill.execute();

    }
    else if(_skill.getSelectedSkill() == SkillKick){
//        debug("KICK",D_KK);
        CSkillKick kickSkill(_agent);
        kickSkill.setTarget(_skill.getTarget());
        //        kickSkill.setTolerance(_skill.getTolerance());
        //        kickSkill.setChip(_skill.getChip());
        //        kickSkill.setSpin(_skill.getSpin());
        kickSkill.setKickSpeed(_skill.getKickSpeed());
        kickSkill.setAvoidPenaltyArea(_skill.getAvoidPenaltyArea());
        kickSkill.setIsGotoPointAvoid(_skill.getIsGotoPointAvoid());
//                kickSkill.setTarget((wm->field->ourGoal()+wm->field->ourGoalL())/2);
        kickSkill.setTolerance(0.5);
        kickSkill.setChip(false);
        kickSkill.setSpin(0);
//                kickSkill.setKickSpeed(6);
        kickSkill.setAvoidPenaltyArea(true);
        kickSkill.setIsGotoPointAvoid(true);
        kickSkill.setInterceptMode(false);
        kickSkill.setSlow(false);
        kickSkill.execute();

    }
    else if(_skill.getSelectedSkill() == SkillReceivePass){
        CSkillReceivePass receivePassSkill(_agent);
        receivePassSkill.setTarget(_skill.getWaitPos());
        receivePassSkill.setSlow(_skill.getSlow());
        receivePassSkill.setReceiveRadius(_skill.getReceiveRadius());
        receivePassSkill.setAvoidOppPenaltyArea(_skill.getAvoidPenaltyArea());
        receivePassSkill.setAvoidOurPenaltyArea(_skill.getAvoidPenaltyArea());

        receivePassSkill.execute();

    }
    else if(_skill.getSelectedSkill() == SkillOneTouch){
        CSkillKickOneTouch oneTouchSkill(_agent);
        oneTouchSkill.setWaitPos(_skill.getWaitPos());
        oneTouchSkill.setTarget(_skill.getTarget());
        oneTouchSkill.setKickSpeed(_skill.getKickSpeed());
        oneTouchSkill.setChip(_skill.getChip());
        oneTouchSkill.setAvoidPenaltyArea(_skill.getAvoidPenaltyArea());
        oneTouchSkill.execute();

    }
        else if(_skill.getSelectedSkill() == SkillMark)
        {
//            debug("MARK APPEND",D_KK);
            markAgents.append(_agent);
        }
//    debug(QString("MARK APPEND %1").arg(markAgents.size()),D_KK);


}
int CForceStart::rankByBallDist(int rank, Vector2D arr[][3]){
    double tmpDist [3] = {.0,.0,.0};
    int tmpRank[3];
    for(int i = 0;i < 3;i++) {
        tmpDist[i] = wm->ball->pos.dist(arr[2][i]);
    }
    if(tmpDist[0] > tmpDist[1] && tmpDist[0] > tmpDist[2]) {
        tmpRank[0] = 0;
        if(tmpDist[1] > tmpDist[2]) {
            tmpRank[1] = 1;
            tmpRank[2] = 2;
        }
        else {
            tmpRank[1] = 2;
            tmpRank[2] = 1;
        }
    }
    else if(tmpDist[1] > tmpDist[0] && tmpDist[1] > tmpDist[2]) {
        tmpRank[0] = 1;
        if(tmpDist[0] > tmpDist[2]) {
            tmpRank[1] = 0;
            tmpRank[2] = 2;
        }
        else {
            tmpRank[1] = 2;
            tmpRank[2] = 0;
        }
    }
    else {
        tmpRank[0] = 2;
        if(tmpDist[0] > tmpDist[1]) {
            tmpRank[1] = 0;
            tmpRank[2] = 1;
        }
        else {
            tmpRank[1] = 1;
            tmpRank[2] = 0;
        }

    }
    return tmpRank[rank];
}

bool CForceStart::kkIsballOurs()
{
    Vector2D tempKickerPos;
    double tempX, tempY;
    bool possCheck = false;
    for(int i = 0; i < wm->opp.activeAgentsCount(); i++)
    {
        tempX = wm->opp.active(i)->pos.x + 0.095*cos(wm->opp.active(i)->dir.th().radian());
        tempY = wm->opp.active(i)->pos.y + 0.095*sin(wm->opp.active(i)->dir.th().radian());
        tempKickerPos = Vector2D(tempX, tempY);
        if(Circle2D(tempKickerPos, 0.1).contains(wm->ball->pos))
        {
            kkOppBallPossCnt++;
            possCheck = true;
        }
    }
    if(!possCheck)
        kkOppBallPossCnt = 0;

    possCheck = false;
    for(int i = 0; i < wm->our.activeAgentsCount(); i++)
    {
        tempX = wm->our.active(i)->pos.x + 0.095*cos(wm->our.active(i)->dir.th().radian());
        tempY = wm->our.active(i)->pos.y + 0.095*sin(wm->our.active(i)->dir.th().radian());
        tempKickerPos = Vector2D(tempX, tempY);
        if(Circle2D(tempKickerPos, 0.2).contains(wm->ball->pos))
        {
            kkOurBallPossCnt++;
            possCheck = true;
        }
    }
    if(!possCheck)
        kkOurBallPossCnt = 0;

    if(kkOppBallPossCnt > 25)
    {
        kkOurBallPossCnt = 0;
        lastBallPossMode = false;
    }

    if(kkOurBallPossCnt > 25)
    {
        kkOppBallPossCnt = 0;
        lastBallPossMode = true;
    }

    return lastBallPossMode;
}

void CForceStart::kkManToManMarkTwo(int ids[]){

    Vector2D ballpos = wm->ball->pos;
  Vector2D ballPos = wm->ball->pos;
  Vector2D OppPos[6] , defPos[2] , defNewPos[2];
  defPos[0].assign(0,0);
  defPos[1].assign(0,0);
  defNewPos[0].assign(0,0);
  defNewPos[1].assign(0,0);
  double OppTh[6];
  double OppDistToBall[6];
  double OppDistToGoalie[6];
  double OppW[6][3];
  double minDis=_GOAL_RAD+(_GOAL_WIDTH*1/3), maxDis=_FIELD_WIDTH;

  double KTh=1,KDB=15,KDG=25;// bayad dasti dorst she
  int j,i;
  for(i=0,j=0;i < wm->opp.activeAgentsCount();i++)
  {
    if(/*wm->opp[i]->markedByDefense !=1 && */wm->opp.active(i)->markedByMark !=1 )
    {
      OppPos[j]=wm->opp.active(i)->pos;
      OppTh[j]=1-(Vector2D::angleBetween(OppPos[j],wm->field->ourGoal()).degree()/110);
      OppDistToBall[j]=1/(1+abs(OppPos[j].dist(ballPos)-1));
      OppDistToGoalie[j]=1-((OppPos[j].dist(wm->field->ourGoal())-minDis)/(maxDis-minDis));
      OppW[j][0]=(OppTh[j]*KTh+OppDistToBall[j]*KDB+OppDistToGoalie[j]*KDG)/(KTh+KDB+KDG);
      OppW[j][1]=j;
      if(Circle2D(OppPos[j],0.50).contains(ballPos)==1) OppW[j][0]=0; //ooni ke toop o dare
      OppW[j][2]=i;// id opp

      j++;

    }
  }
  int tmp;


  for(int i=1;i<j;i++){
    for(int k=1;k<=j-i;k++){
      double temp[3];

      if(OppW[k][0]>OppW[k-1][0]){
        temp[0]=OppW[k][0];
        temp[1]=OppW[k][1];
        temp[2]=OppW[k][2];
        OppW[k][0]=OppW[k-1][0];
        OppW[k][1]=OppW[k-1][1];
        OppW[k][2]=OppW[k-1][2];
        OppW[k-1][0]=temp[0];
        OppW[k-1][1]=temp[1];
        OppW[k-1][2]=temp[2];
      }
    }
  }


  Vector2D vctmp[2];
  Circle2D(OppPos[(int)OppW[0][1]],0.40).intersection(Line2D(OppPos[(int)OppW[0][1]],wm->field->ourGoal()),&vctmp[0],&vctmp[1]);
  if(wm->field->ourGoal().dist(vctmp[0])<wm->field->ourGoal().dist(vctmp[1]))
    defPos[0]=vctmp[0];
  else
    defPos[0]=vctmp[1];

  Circle2D(OppPos[(int)OppW[1][1]],0.40).intersection(Line2D(OppPos[(int)OppW[1][1]],wm->field->ourGoal()),&vctmp[0],&vctmp[1]);
  if(wm->field->ourGoal().dist(vctmp[0])<wm->field->ourGoal().dist(vctmp[1]))
    defPos[1]=vctmp[0];
  else
    defPos[1]=vctmp[1];

  if(defPos[0].x>0){
    defPos[0]=Line2D::intersection(Line2D(wm->field->ourGoal(),defPos[0]),Line2D(Vector2D(0,0),Vector2D(0,1)));
  }
  if(defPos[1].x>0){
    defPos[1]=Line2D::intersection(Line2D(wm->field->ourGoal(),defPos[1]),Line2D(Vector2D(0,0),Vector2D(0,1)));
  }



  double dst[3][3]={0};

  for(i=0;i<2;i++){
    for(j=0;j<2;j++){
      dst[i][j]=kkMarkPoints[i].dist(defPos[j]);
    }
  }

  int jj,ii,iii,jjj;
  tmp=1000;
  for(i=0;i<2;i++){

    for(j=0;j<2;j++){
      if(dst[i][j]<tmp){
        tmp=dst[i][j];
        defNewPos[i]=defPos[j];
        jj=j;
        ii=i;
      }
    }
  }

  tmp=1000;
  for(i=0;i<2;i++){

    if(i!=ii)
      for(j=0;j<2;j++){
        if(j!=jj)
          if(dst[i][j]<tmp){
            tmp=dst[i][j];
            defNewPos[i]=defPos[j];

          }
      }
  }

  draw(Circle2D(defPos[0],0.07),QColor(Qt::red),1);
  draw(Circle2D(defPos[1],0.07),QColor(Qt::red),1);

//  defensePoints[id1]=defPos[0];
//  defensePoints[id2]=defPos[1];

  for(int i = 0; i < 3; i++)
  {
      kkMarkPoints[i] = defNewPos[i];
      kkMarkGPA[i]->setAgent(knowledge->getAgent(ids[i]));
      kkMarkGPA[i]->init(defNewPos[i], ballPos - knowledge->getAgent(ids[i])->pos());
      kkMarkGPA[i]->execute();
  }

//  defensePoints[id1]=defNewPos[0];
//  defensePoints[id2]=defNewPos[1];


}

void CForceStart::kkManToManMarkThree(int ids[]){
    Vector2D ballPos = wm->ball->pos;
  Vector2D OppPos[6] , defPos[3] , tempPos[3] , defNewPos[3];
  defPos[0].assign(0,0);
  defPos[1].assign(0,0);
  defPos[2].assign(0,0);
  defNewPos[0].assign(0,0);
  defNewPos[1].assign(0,0);
  defNewPos[2].assign(0,0);
  double OppTh[6];
  double OppDistToBall[6];
  double OppDistToGoalie[6];
  double OppW[6][3];

  double minDis=_GOAL_RAD+(_GOAL_WIDTH*1/3), maxDis=_FIELD_WIDTH;

  double KTh=1,KDB=15,KDG=25;// bayad dasti dorst she
  int j=0;
  int i =0;
  for(i = 0; i < wm->opp.activeAgentsCount(); i++)
  {
    if(/*wm->opp[i]->markedByDefense !=1 && */wm->opp.active(i)->markedByMark !=1 )
    {
      OppPos[j]= wm->opp.active(i)->pos;
      OppTh[j]=1-(Vector2D::angleBetween(OppPos[j],wm->field->ourGoal()).degree()/110);
      OppDistToBall[j]=1/(1+abs(OppPos[j].dist(ballPos)-1));
      OppDistToGoalie[j]=1-((OppPos[j].dist(wm->field->ourGoal())-minDis)/(maxDis-minDis));
      OppW[j][0]=(OppTh[j]*KTh+OppDistToBall[j]*KDB+OppDistToGoalie[j]*KDG)/(KTh+KDB+KDG);
      OppW[j][1]=j;
      if(Circle2D(OppPos[j],0.50).contains(ballPos)==1) OppW[j][0]=0; //ooni ke toop o dare
      OppW[j][2]=i;// id opp

      j++;

    }
  }


  draw(QString::number(wm->opp[2]->markedByDefense),Vector2D(0,_FIELD_HEIGHT/2+0.1));
  for(int i=1;i<j;i++){
    for(int k=1;k<=j-i;k++){
      double temp[3];

      if(OppW[k][0]>OppW[k-1][0]){
        temp[0]=OppW[k][0];
        temp[1]=OppW[k][1];
        temp[2]=OppW[k][2];
        OppW[k][0]=OppW[k-1][0];
        OppW[k][1]=OppW[k-1][1];
        OppW[k][2]=OppW[k-1][2];
        OppW[k-1][0]=temp[0];
        OppW[k-1][1]=temp[1];
        OppW[k-1][2]=temp[2];
      }
    }
  }



  Vector2D vctmp[2];

    Circle2D(OppPos[(int)OppW[0][1]],0.40).intersection(Line2D(OppPos[(int)OppW[0][1]],wm->field->ourGoal()),&vctmp[0],&vctmp[1]);
    if(wm->field->ourGoal().dist(vctmp[0])<wm->field->ourGoal().dist(vctmp[1]))
      defPos[0]=vctmp[0];
    else
      defPos[0]=vctmp[1];



    Circle2D(OppPos[(int)OppW[1][1]],0.40).intersection(Line2D(OppPos[(int)OppW[1][1]],wm->field->ourGoal()),&vctmp[0],&vctmp[1]);
    if(wm->field->ourGoal().dist(vctmp[0])<wm->field->ourGoal().dist(vctmp[1]))
      defPos[1]=vctmp[0];
    else
      defPos[1]=vctmp[1];



    Circle2D(OppPos[(int)OppW[2][1]],0.40).intersection(Line2D(OppPos[(int)OppW[2][1]],wm->field->ourGoal()),&vctmp[0],&vctmp[1]);
    if(wm->field->ourGoal().dist(vctmp[0])<wm->field->ourGoal().dist(vctmp[1]))
      defPos[2]=vctmp[0];
    else
      defPos[2]=vctmp[1];


    if(defPos[0].x>0){
      defPos[0]=Line2D::intersection(Line2D(wm->field->ourGoal(),defPos[0]),Line2D(Vector2D(0,0),Vector2D(0,1)));
    }
    if(defPos[1].x>0){
      defPos[1]=Line2D::intersection(Line2D(wm->field->ourGoal(),defPos[1]),Line2D(Vector2D(0,0),Vector2D(0,1)));
      if(Circle2D(defPos[1],0.09).contains(defPos[0])){
        defPos[1].x-=0.2;
      }
    }
    if(defPos[2].x>0){
      defPos[2]=Line2D::intersection(Line2D(wm->field->ourGoal(),defPos[2]),Line2D(Vector2D(0,0),Vector2D(0,1)));
      if(Circle2D(defPos[2],0.09).contains(defPos[0])){
        defPos[2].x-=0.2;
      }
      if(Circle2D(defPos[2],0.09).contains(defPos[1])){
        defPos[2].x-=0.2;
      }
    }

    draw(Circle2D(wm->field->ourGoal()/*-Vector2D(0.18,0)*/,minDis),0);
    if(Circle2D(wm->field->ourGoal()/*-Vector2D(0.18,0)*/,minDis).contains(defPos[0]))
    {
      Circle2D(OppPos[(int)OppW[0][1]],0.40).intersection(Line2D(OppPos[(int)OppW[0][1]],ballPos),&vctmp[0],&vctmp[1]);
      if(ballPos.dist(vctmp[0])<ballPos.dist(vctmp[1]))
        defPos[0]=vctmp[0];
      else
        defPos[0]=vctmp[1];
    }

    if(Circle2D(wm->field->ourGoal()/*-Vector2D(0.18,0)*/,minDis).contains(defPos[1]))
    {
      Circle2D(OppPos[(int)OppW[1][1]],0.40).intersection(Line2D(OppPos[(int)OppW[1][1]],ballPos),&vctmp[0],&vctmp[1]);
      if(ballPos.dist(vctmp[0])<ballPos.dist(vctmp[1]))
        defPos[1]=vctmp[0];
      else
        defPos[1]=vctmp[1];
    }

    if(Circle2D(wm->field->ourGoal()/*-Vector2D(0.18,0)*/,minDis).contains(defPos[2]))
    {
      Circle2D(OppPos[(int)OppW[2][1]],0.40).intersection(Line2D(OppPos[(int)OppW[2][1]],ballPos),&vctmp[0],&vctmp[1]);
      if(ballPos.dist(vctmp[0])<ballPos.dist(vctmp[1]))
        defPos[2]=vctmp[0];
      else
        defPos[2]=vctmp[1];
    }

    wm->opp[OppW[0][2]]->markedByDefense=1;
    wm->opp[OppW[1][2]]->markedByDefense=1;
    wm->opp[OppW[2][2]]->markedByDefense=1;


  int tmp;
  double dst[3][3]={0};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
      dst[i][j]=kkMarkPoints[i].dist(defPos[j]);
    }
  }

  int jj,ii,iii,jjj;
  tmp=1000;
  for(i=0;i<3;i++){

    for(j=0;j<3;j++){
      if(dst[i][j]<tmp){
        tmp=dst[i][j];
        defNewPos[i]=defPos[j];
        jj=j;
        ii=i;
      }
    }
  }

  tmp=1000;
  for(i=0;i<3;i++){

    if(i!=ii)
      for(j=0;j<3;j++){
        if(j!=jj)
          if(dst[i][j]<tmp){
            tmp=dst[i][j];
            defNewPos[i]=defPos[j];
            iii=i;
            jjj=j;
          }
      }
  }

  tmp=1000;
  for(i=0;i<3;i++){

    if(i!=ii && i!=iii)
      for(j=0;j<3;j++){
        if(j!=jj && j!=jjj)
          if(dst[i][j]<tmp){
            tmp=dst[i][j];
            defNewPos[i]=defPos[j];
          }
      }
  }



//  draw(Circle2D(defPos[0],0.07),QColor(Qt::red),1);
//  draw(Circle2D(defPos[1],0.07),QColor(Qt::red),1);
//  draw(Circle2D(defPos[2],0.07),QColor(Qt::red),1);
//  defensePoints[id1]=defPos[0];
//  defensePoints[id2]=defPos[1];
//  defensePoints[id3]=defPos[2];
  draw(Circle2D(defNewPos[0],0.07),QColor(Qt::red),1);
  draw(Circle2D(defNewPos[1],0.07),QColor(Qt::red),1);
  draw(Circle2D(defNewPos[2],0.07),QColor(Qt::red),1);
  /*defensePoints[id1]=defNewPos[0];
  defensePoints[id2]=defNewPos[1];
  defensePoints[id3]=defNewPos[2];*/
  for(int i = 0; i < 2; i++)
  {
      kkMarkPoints[i] = defNewPos[i];
      kkMarkGPA[i]->setAgent(knowledge->getAgent(ids[i]));
      kkMarkGPA[i]->init(defPos[i], ballPos - knowledge->getAgent(ids[i])->pos());
      kkMarkGPA[i]->execute();
  }
}

void CForceStart::kkManToManMark(int id){

  Vector2D ballPos = wm->ball->pos;
  Vector2D OppPos[6] , defPos[3];
//  defPos.assign(0,0);
  double OppTh[6];
  double OppDistToBall[6];
  double OppDistToGoalie[6];
  double OppW[6][3];
  double minDis=_GOAL_RAD+(_GOAL_WIDTH*2/3), maxDis=_FIELD_WIDTH/2;

  double KTh=1,KDB=9,KDG=3;// bayad dasti dorst she
  int j,i;
  for(i=0,j=0;i<wm->opp.activeAgentsCount();i++)
  {

    if(/*wm->opp.active(i)->markedByDefense !=1 && wm->opp.active(i)->markedByMark !=1*/ 1 )
    {
      OppPos[j]=wm->opp.active(i)->pos;
      OppTh[j]=1-(Vector2D::angleBetween(OppPos[j],wm->field->ourGoal()).degree()/110);
      OppDistToBall[j]=1/(1+abs(OppPos[j].dist(ballPos)-1));
      OppDistToGoalie[j]=((OppPos[j].dist(wm->field->ourGoal())-minDis)/(maxDis-minDis));
      OppW[j][0]=(OppTh[j]*KTh+OppDistToBall[j]*KDB+OppDistToGoalie[j]*KDG)/(KTh+KDB+KDG);
      OppW[j][1]=j;
      if(Circle2D(OppPos[j],0.50).contains(ballPos)==1) OppW[j][0]=0; //ooni ke toop o dare
      OppW[j][2]=i;
      j++;
    }
  }

  for(int i=1;i<j;i++){
    for(int k=1;k<=j-i;k++){
      double temp[3];

      if(OppW[k][0]>OppW[k-1][0]){
        temp[0]=OppW[k][0];
        temp[1]=OppW[k][1];
        temp[2]=OppW[k][2];
        OppW[k][0]=OppW[k-1][0];
        OppW[k][1]=OppW[k-1][1];
        OppW[k][2]=OppW[k-1][2];
        OppW[k-1][0]=temp[0];
        OppW[k-1][1]=temp[1];
        OppW[k-1][2]=temp[2];
      }
    }
  }

  Vector2D vctmp[2];

    Circle2D(OppPos[(int)OppW[0][1]],0.40).intersection(Line2D(OppPos[(int)OppW[0][1]],wm->field->ourGoal()),&vctmp[0],&vctmp[1]);
    if(wm->field->ourGoal().dist(vctmp[0])<wm->field->ourGoal().dist(vctmp[1]))
      defPos[0]=vctmp[0];
    else
      defPos[0]=vctmp[1];

    if(defPos[0].x>0){
      defPos[0]=Line2D::intersection(Line2D(wm->field->ourGoal(),defPos[0]),Line2D(Vector2D(0,0),Vector2D(0,1)));
    }

//  wm->opp.active(OppW[0][2])->markedByDefense=1;
  //defensePoints[id1]=defPos[0];
    for(int i = 0; i < 1; i++)
    {
        kkMarkPoints[i] = defPos[i];
        kkMarkGPA[i]->setAgent(knowledge->getAgent(id));
        kkMarkGPA[i]->init(defPos[i], ballPos - knowledge->getAgent(id)->pos());
        kkMarkGPA[i]->execute();
    }
}

#endif
