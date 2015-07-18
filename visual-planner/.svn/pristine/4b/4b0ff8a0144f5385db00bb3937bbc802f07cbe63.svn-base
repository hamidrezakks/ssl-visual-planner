#include "kkplansql.h"

kkPlanSQL::kkPlanSQL(QString directory)
{
    planSQL = QSqlDatabase::addDatabase("QSQLITE");
    planSQL.setDatabaseName(directory);
    if (!planSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
        "Unable to establish a database connection.\n"\
              "This example needs SQLite support. Please read "\
              "the Qt SQL driver documentation for information how "\
              "to build it.\n\n"\
              "Click Cancel to exit.", QMessageBox::Cancel);
    }
}

bool kkPlanSQL::changeSQLDir(QString directory)
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
}

PEndPolicy kkPlanSQL::getPolicyByIndex(int index)
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

int kkPlanSQL::getIntByEnum(PEndPolicy tEnum)
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

PSkills kkPlanSQL::getPSkillByIndex(int index)
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

int kkPlanSQL::getIntByEnum(PSkills tEnum)
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

QString kkPlanSQL::getCommentForSQL(QString string)
{
    QString tempText = string;
    QStringList tempTextList = tempText.split("\n");
    tempText = tempTextList.join("*spc*");
    return tempText;
}

QString kkPlanSQL::getCommentForPlanner(QString string)
{
    QString tempText = string;
    QStringList tempTextList = tempText.split("*spc*");
    tempText = tempTextList.join("\n");
    return tempText;
}

QString kkPlanSQL::savePlanCommand(QString tableName, kkAgentPlan planStruct, int agentSize, int planId)
{
    QString str = "INSERT INTO "+tableName+" VALUES ( ";
    str += QString::number(planId)+", ";
    str += QString::number(planStruct.ball)+", ";
    str += QString::number(getIntByEnum(planStruct.endMode))+", ";
    str += QString::number(planStruct.endPolicy)+", ";
    str += QString::number(planStruct.possession)+", ";
    str += QString::number(planStruct.agentsSize)+", ";
    str += "'"+getCommentForSQL(planStruct.comment)+"' , ";
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

void kkPlanSQL::savePlan()
{
    QSqlQuery squery;
    if(planSQL.tables().contains("plan1agent"))
        squery.exec("DROP TABLE plan1agent;");
    squery.exec("CREATE TABLE plan1agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, comment varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int )");

    for(int i = 0; i < _1AgentPlan->length(); i++)
    {
        qDebug()<<squery.exec(savePlanCommand("plan1agent", _1AgentPlan->at(i), 1, i));
    }
    /////////////////
    if(planSQL.tables().contains("plan2agent"))
        squery.exec("DROP TABLE plan2agent;");
    squery.exec("CREATE TABLE plan2agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, comment varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int )");

    for(int i = 0; i < _2AgentPlan->length(); i++)
    {
        squery.exec(savePlanCommand("plan2agent", _2AgentPlan->at(i), 2, i));
    }
    ////////////////
    if(planSQL.tables().contains("plan3agent"))
        squery.exec("DROP TABLE plan3agent;");
    squery.exec("CREATE TABLE plan3agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, comment varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int,"\
                " agent3a int, agent3b int, agent3p1 int, agent3p2 int, agent3p3 int, agent3p4 int )");

    for(int i = 0; i < _3AgentPlan->length(); i++)
    {
        squery.exec(savePlanCommand("plan3agent", _3AgentPlan->at(i), 3, i));
    }
    ///////////////
    if(planSQL.tables().contains("plan4agent"))
        squery.exec("DROP TABLE plan4agent;");
    squery.exec("CREATE TABLE plan4agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, comment varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int,"\
                " agent3a int, agent3b int, agent3p1 int, agent3p2 int, agent3p3 int, agent3p4 int,"\
                " agent4a int, agent4b int, agent4p1 int, agent4p2 int, agent4p3 int, agent4p4 int )");

    for(int i = 0; i < _4AgentPlan->length(); i++)
    {
        squery.exec(savePlanCommand("plan4agent", _4AgentPlan->at(i), 4, i));
    }
    ///////////////
    if(planSQL.tables().contains("plan5agent"))
        squery.exec("DROP TABLE plan5agent;");
    squery.exec("CREATE TABLE plan5agent (id int PRIMARY KEY, ball int, endmode int,"\
                " endval int, possession int, agentsize int, comment varchar(1024),"\
                " agent1a int, agent1b int, agent1p1 int, agent1p2 int, agent1p3 int, agent1p4 int,"\
                " agent2a int, agent2b int, agent2p1 int, agent2p2 int, agent2p3 int, agent2p4 int,"\
                " agent3a int, agent3b int, agent3p1 int, agent3p2 int, agent3p3 int, agent3p4 int,"\
                " agent4a int, agent4b int, agent4p1 int, agent4p2 int, agent4p3 int, agent4p4 int,"\
                " agent5a int, agent5b int, agent5p1 int, agent5p2 int, agent5p3 int, agent5p4 int )");

    for(int i = 0; i < _5AgentPlan->length(); i++)
    {
        squery.exec(savePlanCommand("plan5agent", _5AgentPlan->at(i), 5, i));
    }

}

void kkPlanSQL::insertToList(QList<kkAgentPlan> &list, QSqlQuery query, int agentSize)
{
    bool errorCheck = true;
    while(query.next())
    {
        errorCheck = false;
        kkAgentPlan tempPlan;
        tempPlan.planId = query.value(0).toInt();
        tempPlan.ball = query.value(1).toInt();
        tempPlan.endMode = getPolicyByIndex(query.value(2).toInt());
        tempPlan.endPolicy = query.value(3).toInt();
        tempPlan.possession = query.value(4).toInt();
        tempPlan.agentsSize = query.value(5).toInt();
        tempPlan.comment = getCommentForPlanner(query.value(6).toString());
        for(int i = 0; i < agentSize; i++)
        {
            tempPlan.agents[i].A = query.value(7 + i*6).toInt();
            tempPlan.agents[i].B = query.value(8 + i*6).toInt();
            for(int j = 0; j < 4; j++)
            {
                tempPlan.agents[i].skill[j] = getPSkillByIndex(query.value(9 + i*6 + j).toInt());
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
        list.append(tempPlan);
    }
    if(errorCheck)
    {
        qDebug()<<planSQL.databaseName();
        qDebug()<< "SqLite error:" << query.lastError().text() << ", SqLite error code:" << query.lastError().number();
    }
}

void kkPlanSQL::loadPlan(QList<kkAgentPlan> &list, int agentSize)
{
    QSqlQuery query;
    list.clear();
    switch(agentSize)
    {
        default:
        case 1:
            query.exec("SELECT * FROM plan1agent ORDER BY id ASC");
            insertToList(list, query, 1);
        break;
        case 2:
            query.exec("SELECT * FROM plan2agent ORDER BY id ASC");
            insertToList(list, query, 2);
        break;
        case 3:
            query.exec("SELECT * FROM plan3agent ORDER BY id ASC");
            insertToList(list, query, 3);
        break;
        case 4:
            query.exec("SELECT * FROM plan4agent ORDER BY id ASC");
            insertToList(list, query, 4);
        break;
        case 5:
            query.exec("SELECT * FROM plan5agent ORDER BY id ASC");
            insertToList(list, query, 5);
        break;

    }
}

bool kkPlanSQL::addPlan(QList<kkAgentPlan> list, int agentSize)
{
    switch(agentSize)
    {
        default:
        case 1:
            _1AgentPlan = new QList<kkAgentPlan>(list);
        break;
        case 2:
            _2AgentPlan = new QList<kkAgentPlan>(list);
        break;
        case 3:
            _3AgentPlan = new QList<kkAgentPlan>(list);
        break;
        case 4:
            _4AgentPlan = new QList<kkAgentPlan>(list);
        break;
        case 5:
            _5AgentPlan = new QList<kkAgentPlan>(list);
        break;

    }
}

kkPlanSQL::~kkPlanSQL()
{

}

void kkPlanSQL::disableSQL()
{
    planSQL.close();
}

void kkPlanSQL::enableSQL()
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
