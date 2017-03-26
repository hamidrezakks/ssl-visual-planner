#include "playonplansql.h"

playOnPlanSQL::playOnPlanSQL(QString directory)
{
//    planSQL = QSqlDatabase::addDatabase("QSQLITE");
//    planSQL.setDatabaseName(directory);
//    if (!planSQL.open()) {
//        QMessageBox::critical(0, "Cannot open database",
//                              "Unable to establish a database connection.\n"\
//                              "This example needs SQLite support. Please read "\
//                              "the Qt SQL driver documentation for information how "\
//                              "to build it.\n\n"\
//                              "Click Cancel to exit.", QMessageBox::Cancel);
//    }
}

bool playOnPlanSQL::changeSQLDir(QString directory)
{
    planSQL.setDatabaseName(directory);
    if (!planSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
                              "Unable to establish a database connection.\n"\
                              "This example needs SQLite support. Please read "\
                              "the Qt SQL driver documentation for information how "\
                              "to build it.\n\n"\
                              "Click Cancel to exit.", QMessageBox::Cancel);
    }
    return true;
}

EndPolicy playOnPlanSQL::getPolicyByIndex(int index)
{
    switch (index) {
    default:
    case 1:
        return Cycle;
    case 2:
        return ExactAgent;
    case 3:
        return AllAgents;
    case 4:
        return ExactDisturb;
    }
}

int playOnPlanSQL::getIntByEnum(EndPolicy tEnum)
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
    case NoPolicy:
    default:
        return 0;
    }
}

Skills playOnPlanSQL::getPSkillByIndex(int index)
{
    switch (index) {
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
    default:
        return None;
    }
}

int playOnPlanSQL::getIntByEnum(Skills tEnum)
{
    switch (tEnum) {
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

QString playOnPlanSQL::getCommentForSQL(QString string)
{
    QString tempText = string;
    QStringList tempTextList = tempText.split("\n");
    tempText = tempTextList.join("*spc*");
    return tempText;
}

QString playOnPlanSQL::getCommentForPlanner(QString string)
{
    QString tempText = string;
    QStringList tempTextList = tempText.split("*spc*");
    tempText = tempTextList.join("\n");
    return tempText;
}

QString playOnPlanSQL::savePlanCommand(QString tableName, AgentPlan planStruct, int agentSize, int planId)
{
    QString str = "INSERT INTO "+tableName+" VALUES ( ";
    str += QString::number(planId)+", ";
    str += QString::number(planStruct.ball)+", ";
    str += QString::number(getIntByEnum(planStruct.endMode))+", ";
    str += QString::number(planStruct.endPolicy)+", ";
    str += QString::number(planStruct.possession)+", ";
    str += QString::number(planStruct.agentsSize)+", ";
    str += "'"+getCommentForSQL(planStruct.tags)+"' , ";
    for(int i = 0; i < agentSize; i++)
    {
        str += QString::number(planStruct.agents[i].A)+", ";
        str += QString::number(planStruct.agents[i].B)+", ";
        for(int j = 0; j < 4; j++)
        {
            if(j == 3 && i == agentSize - 1)
                str += QString::number(getIntByEnum(planStruct.agents[i].skill[j]))+" ";
            else
                str += QString::number(getIntByEnum(planStruct.agents[i].skill[j]))+", ";
        }
    }
    str += " );";
    return str;
}

void playOnPlanSQL::savePlanListInSQL(QSqlQuery &_query)
{
    QString str;
    for (int i = 0; i < playOnPlans.length(); i++) {
        str = "INSERT INTO playonplans VALUES ( ";
        str += QString::number(playOnPlans.at(i).planId)+", ";
        str += QString::number(playOnPlans.at(i).agentSize)+", ";
        str += QString::number(playOnPlans.at(i).ball)+", ";
        str += "'"+playOnPlans.at(i).tags+"' ); ";
        _query.exec(str);
    }
}

void playOnPlanSQL::savePlan()
{
    QSqlQuery squery;
    if(planSQL.tables().contains("playonplans"))
        squery.exec("DROP TABLE playonplans;");
    squery.exec("CREATE TABLE playonplans (id int PRIMARY KEY, agentsize int, ball int, "\
                " tags varchar(1024) )");

    savePlanListInSQL(squery);

    if (planSQL.tables().contains("plan1agent")) {
        squery.exec("DROP TABLE plan1agent;");
    }
    squery.exec("CREATE TABLE plan1agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, tags varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int )");

    if (planSQL.tables().contains("plan2agent")) {
        squery.exec("DROP TABLE plan2agent;");
    }
    squery.exec("CREATE TABLE plan2agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, tags varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int )");

    if (planSQL.tables().contains("plan3agent")) {
        squery.exec("DROP TABLE plan3agent;");
    }
    squery.exec("CREATE TABLE plan3agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, tags varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int,"\
                " agent3a int, agent3b int, agent3p1 int, agent3p2 int, agent3p3 int, agent3p4 int )");

    if (planSQL.tables().contains("plan4agent")) {
        squery.exec("DROP TABLE plan4agent;");
    }
    squery.exec("CREATE TABLE plan4agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, tags varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int,"\
                " agent3a int, agent3b int, agent3p1 int, agent3p2 int, agent3p3 int, agent3p4 int,"\
                " agent4a int, agent4b int, agent4p1 int, agent4p2 int, agent4p3 int, agent4p4 int )");

    if (planSQL.tables().contains("plan5agent")) {
        squery.exec("DROP TABLE plan5agent;");
    }
    squery.exec("CREATE TABLE plan5agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, tags varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int,"\
                " agent3a int, agent3b int, agent3p1 int, agent3p2 int, agent3p3 int, agent3p4 int,"\
                " agent4a int, agent4b int, agent4p1 int, agent4p2 int, agent4p3 int, agent4p4 int,"\
                " agent5a int, agent5b int, agent5p1 int, agent5p2 int, agent5p3 int, agent5p4 int )");

    for (int i = 0; i < playOnPlans.length(); i++) {
        if (playOnPlans.at(i).agentSize == 1) {
            squery.exec(savePlanCommand("plan1agent", agentPlan.at(i), 1, i));
        }
        else if (playOnPlans.at(i).agentSize == 2) {
            squery.exec(savePlanCommand("plan2agent", agentPlan.at(i), 2, i));
        }
        else if (playOnPlans.at(i).agentSize == 3) {
            squery.exec(savePlanCommand("plan3agent", agentPlan.at(i), 3, i));
        }
        else if (playOnPlans.at(i).agentSize == 4) {
            squery.exec(savePlanCommand("plan4agent", agentPlan.at(i), 4, i));
        }
        else if (playOnPlans.at(i).agentSize == 5) {
            squery.exec(savePlanCommand("plan5agent", agentPlan.at(i), 5, i));
        }
    }
}

void playOnPlanSQL::insertToList(QList<AgentPlan> &list, QSqlQuery query, int agentSize)
{
    bool errorCheck = true;
    while (query.next()) {
        errorCheck = false;
        AgentPlan tempPlan;
        tempPlan.planId = query.value(0).toInt();
        tempPlan.ball = query.value(1).toInt();
        tempPlan.endMode = getPolicyByIndex(query.value(2).toInt());
        tempPlan.endPolicy = query.value(3).toInt();
        tempPlan.possession = query.value(4).toInt();
        tempPlan.agentsSize = query.value(5).toInt();
        tempPlan.tags = query.value(6).toString();
        for (int i = 0; i < agentSize; i++) {
            tempPlan.agents[i].A = query.value(7 + i*6).toInt();
            tempPlan.agents[i].B = query.value(8 + i*6).toInt();
            for (int j = 0; j < 4; j++) {
                tempPlan.agents[i].skill[j] = getPSkillByIndex(query.value(9 + i*6 + j).toInt());
            }
        }
        for (int i = agentSize; i < 5; i++) {
            tempPlan.agents[i].A = -1;
            tempPlan.agents[i].B = -1;
            for (int j = 0; j < 4; j++) {
                tempPlan.agents[i].skill[j] = None;
            }
        }
        list.append(tempPlan);
    }
    if (errorCheck) {
        qDebug()<<planSQL.databaseName();
        qDebug()<< "SqLite error:" << query.lastError().text() << ", SqLite error code:" << query.lastError().number();
    }
}

void playOnPlanSQL::loadPlan()
{
    QSqlQuery query;
    playOnPlans.clear();
    agentPlan.clear();

    PlayOnPlan tempPlayOnPlan;
    query.exec("SELECT * FROM playonplans ORDER BY id ASC");
    while (query.next()) {
        tempPlayOnPlan.planId = query.value(0).toInt();
        tempPlayOnPlan.agentSize = query.value(1).toInt();
        tempPlayOnPlan.ball = query.value(2).toInt();
        tempPlayOnPlan.tags = query.value(3).toString();
        playOnPlans.append(tempPlayOnPlan);
    }

    QList<AgentPlan> tempList[5];

    query.exec("SELECT * FROM plan1agent ORDER BY id ASC");
    insertToList(tempList[0], query, 1);

    query.exec("SELECT * FROM plan2agent ORDER BY id ASC");
    insertToList(tempList[1], query, 2);

    query.exec("SELECT * FROM plan3agent ORDER BY id ASC");
    insertToList(tempList[2], query, 3);

    query.exec("SELECT * FROM plan4agent ORDER BY id ASC");
    insertToList(tempList[3], query, 4);

    query.exec("SELECT * FROM plan5agent ORDER BY id ASC");
    insertToList(tempList[4], query, 5);

    int planCouter[5] = {0};
    for (int i = 0; i < playOnPlans.length(); i++) {
        if (playOnPlans.at(i).agentSize == 1) {
            if (planCouter[0] < tempList[0].length()) {
                agentPlan.append(tempList[0].at(planCouter[0]));
                planCouter[0]++;
            }
        }
        else if (playOnPlans.at(i).agentSize == 2) {
            if (planCouter[1] < tempList[1].length()) {
                agentPlan.append(tempList[1].at(planCouter[1]));
                planCouter[1]++;
            }
        }
        else if (playOnPlans.at(i).agentSize == 3) {
            if (planCouter[2] < tempList[2].length()) {
                agentPlan.append(tempList[2].at(planCouter[2]));
                planCouter[2]++;
            }
        }
        else if (playOnPlans.at(i).agentSize == 4) {
            if (planCouter[3] < tempList[3].length()) {
                agentPlan.append(tempList[3].at(planCouter[3]));
                planCouter[3]++;
            }
        }
        else if (playOnPlans.at(i).agentSize == 5) {
            if (planCouter[4] < tempList[4].length()) {
                agentPlan.append(tempList[4].at(planCouter[4]));
                planCouter[4]++;
            }
        }
    }
}

bool playOnPlanSQL::addPlan(int planId, AgentPlan plan, QString &str)
{
    PlayOnPlan tempPlayOffPlan;
    tempPlayOffPlan.planId = planId;
    tempPlayOffPlan.ball = plan.ball;
    tempPlayOffPlan.tags = plan.tags;
    tempPlayOffPlan.agentSize = plan.agentsSize;

    if (planId >= playOnPlans.length()) {
        playOnPlans.append(tempPlayOffPlan);
        agentPlan.append(plan);
        str = "Plan successfully added";
    }
    else {
        playOnPlans.replace(planId, tempPlayOffPlan);
        agentPlan.replace(planId, plan);
        str = "Plan successfully modified";
    }
    return true;
}

void playOnPlanSQL::removePlan(int _index)
{
    if (_index < getPlanSize()) {
        playOnPlans.removeAt(_index);
        agentPlan.removeAt(_index);
    }
}

playOnPlanSQL::~playOnPlanSQL()
{

}

void playOnPlanSQL::disableSQL()
{
    planSQL.close();
}

void playOnPlanSQL::enableSQL()
{
    planSQL = QSqlDatabase::addDatabase("QSQLITE");
    planSQL.setDatabaseName("temp.db");
    if (!planSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
                              "Unable to establish a database connection.\n"\
                              "This example needs SQLite support. Please read "\
                              "the Qt SQL driver documentation for information how "\
                              "to build it.\n\n"\
                              "Click Cancel to exit.", QMessageBox::Cancel);
    }
}

int playOnPlanSQL::getPlanSize()
{
    return playOnPlans.length();
}

AgentPlan playOnPlanSQL::getPlan(int _index)
{
    return agentPlan.at(_index);
}
