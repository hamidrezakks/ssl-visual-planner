#include "kkpoplansql.h"

kkPOPlanSQL::kkPOPlanSQL(QString directory)
{
    /*planSQL = QSqlDatabase::addDatabase("QSQLITE");
    planSQL.setDatabaseName(directory);
    if (!planSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
        "Unable to establish a database connection.\n"\
              "This example needs SQLite support. Please read "\
              "the Qt SQL driver documentation for information how "\
              "to build it.\n\n"\
              "Click Cancel to exit.", QMessageBox::Cancel);
    }*/
}

bool kkPOPlanSQL::changeSQLDir(QString directory)
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

kkPOPlanSQL::~kkPOPlanSQL()
{

}

void kkPOPlanSQL::savePlan()
{
    QSqlQuery squery;
    /*if(!planSQL.tables().contains("poplanlist"))
        squery.exec("DROP TABLE plan5agent;");*/

    for(int i = 0; i < planSQL.tables().count(); i++)
    {
        qDebug()<<planSQL.tables().at(i);
        squery.exec("DROP TABLE "+planSQL.tables().at(i)+";");
    }

    squery.exec("CREATE TABLE poplanlist (id int PRIMARY KEY, pname varchar(16), pmode int,"\
                " psize int, pball varchar(16), pagent1 varchar(16), pagent2 varchar(16),"\
                " pagent3 varchar(16), pagent4 varchar(16), pagent5 varchar(16), pagent6 varchar(16) )");

    QString tempTableName = "";
    QString tempTableStruct = " (id int PRIMARY KEY ";
    for(int i = 1; i < 7; i++)
    {
        QString tids = QString::number(i);
        tempTableStruct += " , r"+tids+"geo varchar(24), r"+tids+"tol varchar(8), r"+tids+"ssize varchar(8), ";
        tempTableStruct += " r"+tids+"skill1 varchar(24) , r"+tids+"skill2 varchar(24) , r"+tids+"skill3 varchar(24) ,  r"+tids+"ptarget varchar(8) ";
    }
    tempTableStruct += " );";

    QString tempInitStr[7];
    QString tempQStr;
    for(int i = 0; i < planList.count(); i++)
    {
        tempTableName = "plan_"+QString::number(i);
        if(planList.at(i).initPos.ballX == -100 || planList.at(i).initPos.ballY == -100)
            tempInitStr[0] = "na";
        else
            tempInitStr[0] = QString::number(planList.at(i).initPos.ballX)+"|"+QString::number(planList.at(i).initPos.ballY);

        for(int j = 0; j < 6; j++)
        {
            if(planList.at(i).initPos.AgentX[j] == -100 || planList.at(i).initPos.AgentY[j] == -100)
                tempInitStr[j+1] = "na";
            else
                tempInitStr[j+1] = QString::number(planList.at(i).initPos.AgentX[j])+"|"+QString::number(planList.at(i).initPos.AgentY[j]);
        }
        tempQStr = "INSERT INTO poplanlist VALUES ("+QString::number(i)+", '"+tempTableName+"' , "\
                   " "+QString::number(int(planList.at(i).planMode))+" , "+QString::number(planList.at(i).agentSize)+" , "\
                   " '"+tempInitStr[0]+"' , '"+tempInitStr[1]+"' , '"+tempInitStr[2]+"' , '"+tempInitStr[3]+"' , "\
                   " '"+tempInitStr[4]+"' , '"+tempInitStr[5]+"' , '"+tempInitStr[6]+"' );";

        squery.exec(tempQStr);

        squery.exec("CREATE TABLE "+tempTableName+" "+tempTableStruct);
        addEachPlanToSql(planList.at(i), tempTableName);
    }
}

int kkPOPlanSQL::findMaxSteps(planStruct &_plan)
{
    int max = -1;
    for(int i = 0; i < 6; i++)
    {
        if(max < _plan.AgentPlan[i].count())
            max = _plan.AgentPlan[i].count();
    }
    return max;
}

void kkPOPlanSQL::addEachPlanToSql(planStruct _plan, QString tName)
{
    int maxStep = findMaxSteps(_plan);
    for(int i = 0; i < maxStep; i++)
    {
        addStepToSql(tName, i,
                    _plan.AgentPlan[0],
                    _plan.AgentPlan[1],
                    _plan.AgentPlan[2],
                    _plan.AgentPlan[3],
                    _plan.AgentPlan[4],
                    _plan.AgentPlan[5]);
    }
}

void kkPOPlanSQL::addStepToSql(QString tName, int tid, QList<playOffRobot> &tA1, QList<playOffRobot> &tA2, QList<playOffRobot> &tA3, QList<playOffRobot> &tA4, QList<playOffRobot> &tA5, QList<playOffRobot> &tA6)
{
    QString tempStr = "";
    if(tA1.count() > tid)
        tempStr += convertStructToSQLCmd(1, tA1.at(tid));
    else
        tempStr += convertEmptyStructToSQLCmd(1);

    if(tA2.count() > tid)
        tempStr += " , "+convertStructToSQLCmd(2, tA2.at(tid));
    else
        tempStr += " , "+convertEmptyStructToSQLCmd(2);

    if(tA3.count() > tid)
        tempStr += " , "+convertStructToSQLCmd(3, tA3.at(tid));
    else
        tempStr += " , "+convertEmptyStructToSQLCmd(3);

    if(tA4.count() > tid)
        tempStr += " , "+convertStructToSQLCmd(4, tA4.at(tid));
    else
        tempStr += " , "+convertEmptyStructToSQLCmd(4);

    if(tA5.count() > tid)
        tempStr += " , "+convertStructToSQLCmd(5, tA5.at(tid));
    else
        tempStr += " , "+convertEmptyStructToSQLCmd(5);

    if(tA6.count() > tid)
        tempStr += " , "+convertStructToSQLCmd(6, tA6.at(tid));
    else
        tempStr += " , "+convertEmptyStructToSQLCmd(6);

    tempStr = "INSERT INTO "+tName+" VALUES ( "+QString::number(tid)+" , "+tempStr+" ); ";
    QSqlQuery tempQuery;
    tempQuery.exec(tempStr);
    qDebug()<< "SqLite error:" << tempQuery.lastError().text() << ", SqLite error code:" << tempQuery.lastError().number();
}

QString kkPOPlanSQL::convertStructToSQLCmd(int _agentId, playOffRobot tStep)
{
    QString tempStr;
    for(int i = 0; i < 3; i++)
    {
        if(tStep.skill[i] == ReceivePassSkill && tStep.IAMode[i])
            tStep.skill[i] = ReceivePassIASkill;
    }
    tempStr += " '";
    //robot geo
    tempStr += QString::number(tStep.x)+"|";
    tempStr += QString::number(tStep.y)+"|";
    tempStr += QString::number(int(tStep.angle));
    tempStr += "' , '";
    //tolerance
    tempStr += QString::number(tStep.tolerance);
    tempStr += "' , '";
    //skill size
    tempStr += QString::number(tStep.skillSize);
    tempStr += "' , '";
    //skill1
    tempStr += QString::number(int(tStep.skill[0]))+"|";
    tempStr += QString::number(int(tStep.skillData[0][0]))+"|";
    tempStr += QString::number(int(tStep.skillData[0][1]));
    tempStr += "' , '";
    //skill2
    tempStr += QString::number(int(tStep.skill[1]))+"|";
    tempStr += QString::number(int(tStep.skillData[1][0]))+"|";
    tempStr += QString::number(int(tStep.skillData[1][1]));
    tempStr += "' , '";
    //skill3
    tempStr += QString::number(int(tStep.skill[2]))+"|";
    tempStr += QString::number(int(tStep.skillData[2][0]))+"|";
    tempStr += QString::number(int(tStep.skillData[2][1]));
    tempStr += "' , '";
    //pass target
    tempStr += QString::number(tStep.target.agent)+"|";
    tempStr += QString::number(tStep.target.index);
    tempStr += "' ";
    return tempStr;

}

QString kkPOPlanSQL::convertEmptyStructToSQLCmd(int _agentId)
{
    QString tempStr;
    tempStr += " '";
    tempStr += "na";
    tempStr += "' , '";
    tempStr += "na";
    tempStr += "' , '";
    tempStr += "na";
    tempStr += "' , '";
    tempStr += "na";
    tempStr += "' , '";
    tempStr += "na";
    tempStr += "' , '";
    tempStr += "na";
    tempStr += "' , '";
    tempStr += "na";
    tempStr += "' ";
    return tempStr;
}

int kkPOPlanSQL::addPlan(QList<playOffRobot> tPlan[], POInitPos tInitPos, POMODE tPOMode, int agentSize, int itemId)
{
    qDebug()<<planList.count()<<itemId;;
    planStruct tempPlan;
    tempPlan.agentSize = agentSize;
    tempPlan.planMode = tPOMode;
    tempPlan.initPos = tInitPos;

    playOffRobot tempPOP;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < tPlan[i].count(); j++)
        {
            tempPOP = tPlan[i].at(j);
            tempPlan.AgentPlan[i].append(tempPOP);
        }
    }
    if(planList.count() <= itemId)
        planList.append(tempPlan);
    else
        planList.replace(itemId, tempPlan);

    return planList.count();
}

void kkPOPlanSQL::disableSQL()
{
    planSQL.close();
    //planSQL.removeDatabase(planSQL.connectionName());
}

void kkPOPlanSQL::enableSQL()
{
    planSQL = QSqlDatabase::addDatabase("QSQLITE");
    planSQL.setDatabaseName("temppo.db");
    if (!planSQL.open()) {
        QMessageBox::critical(0, "Cannot open database",
        "Unable to establish a database connection.\n"\
              "This example needs SQLite support. Please read "\
              "the Qt SQL driver documentation for information how "\
              "to build it.\n\n"\
              "Click Cancel to exit.", QMessageBox::Cancel);
    }
}

int kkPOPlanSQL::loadPlan()
{
    QSqlQuery squery;
    squery.exec("SELECT pname, pmode, psize FROM poplanlist ORDER BY id ASC");
    planList.clear();
    while(squery.next())
    {
        planStruct tempPlan;
        tempPlan.planMode = POMODE(squery.value(1).toInt());
        tempPlan.agentSize = squery.value(2).toInt();
        loadEachPlan(tempPlan, squery.value(0).toString());
        planList.append(tempPlan);
    }
    return planList.count();
}

void kkPOPlanSQL::loadEachPlan(planStruct &_plan, QString _name)
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

bool kkPOPlanSQL::loadSQLtoStruct(QSqlQuery _query, int _rIndex, playOffRobot &temp)
{
    if(_query.value(_rIndex*7 + 1).toString() == "na")
        return false;
    //playOffRobot temp;
    temp.x = _query.value(_rIndex*7 + 1).toString().split("|").at(0).toInt();
    temp.y = _query.value(_rIndex*7 + 1).toString().split("|").at(1).toInt();
    temp.angle = _query.value(_rIndex*7 + 1).toString().split("|").at(2).toInt();

    temp.tolerance = _query.value(_rIndex*7 + 2).toInt();

    temp.skillSize = _query.value(_rIndex*7 + 3).toInt();

    temp.agent = _rIndex;

    temp.temp = false;

    for(int i = 0; i < 3; i++)
    {
        if(_query.value(_rIndex*7 + 4).toString() != "na")
        {
            if(POffSkills(_query.value(_rIndex*7 + 4 + i).toString().split("|").at(0).toInt()) == ReceivePassIASkill)
            {
                temp.skill[i] = ReceivePassSkill;
                temp.skillData[i][0] = _query.value(_rIndex*7 + 4 + i).toString().split("|").at(1).toInt();
                temp.skillData[i][1] = _query.value(_rIndex*7 + 4 + i).toString().split("|").at(2).toInt();
                temp.IAMode[i] = true;
            }
            else
            {
                temp.skill[i] = POffSkills(_query.value(_rIndex*7 + 4 + i).toString().split("|").at(0).toInt());
                temp.skillData[i][0] = _query.value(_rIndex*7 + 4 + i).toString().split("|").at(1).toInt();
                temp.skillData[i][1] = _query.value(_rIndex*7 + 4 + i).toString().split("|").at(2).toInt();
                temp.IAMode[i] = false;
            }
        }
        else
        {
            temp.skill[i] = NoSkill;
            temp.skillData[i][0] = 1000;
            temp.skillData[i][1] = 1000;
        }
    }

    temp.target.agent = _query.value(_rIndex*7 + 7).toString().split("|").at(0).toInt();
    temp.target.index = _query.value(_rIndex*7 + 7).toString().split("|").at(1).toInt();

    return true;
}

void kkPOPlanSQL::insertPlanToQList(QList<playOffRobot> _planList[], planMData &mData, int index)
{
    if(index < 0) index = 0;
    if(index > planList.count() - 1) index = planList.count() - 1;
    if(index < 0) return;
    mData.agentSize = planList.at(index).agentSize;
    mData.planMode = planList.at(index).planMode;
    for(int i = 0; i < 6; i++)
    {
        _planList[i].clear();
        for(int j = 0; j < planList.at(index).AgentPlan[i].count(); j++)
        {
            _planList[i].append(planList.at(index).AgentPlan[i].at(j));
        }
    }
}

int kkPOPlanSQL::getPlanSize()
{
    return planList.count();
}

void kkPOPlanSQL::cleanPlans()
{
    planList.clear();
}

void kkPOPlanSQL::removePlan(int index)
{
    if(planList.count() <= index)
        return;
    planList.removeAt(index);
}
