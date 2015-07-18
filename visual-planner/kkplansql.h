#ifndef KKPLANSQL_H
#define KKPLANSQL_H

#include "base.h"

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QList>
#include <QDebug>
#include <QMessageBox>
#include <QtSql/QSqlError>

class kkPlanSQL
{
public:
  kkPlanSQL(QString directory);

  bool changeSQLDir(QString directory);


  int getIntByEnum(PEndPolicy tEnum);
  int getIntByEnum(PSkills tEnum);
  PEndPolicy getPolicyByIndex(int index);
  PSkills getPSkillByIndex(int index);

  QString getCommentForSQL(QString string);
  QString getCommentForPlanner(QString string);
  QString savePlanCommand(QString tableName, kkAgentPlan planStruct, int agentSize, int planId);
  void savePlan();

  void insertToList(QList<kkAgentPlan> &list, QSqlQuery query, int agentSize = 1);
  void loadPlan(QList<kkAgentPlan> &list, int agentSize = 1);

  bool addPlan(QList<kkAgentPlan> list, int agentSize = 1);

  void disableSQL();
  void enableSQL();
private:
  QList<kkAgentPlan> *_1AgentPlan;
  QList<kkAgentPlan> *_2AgentPlan;
  QList<kkAgentPlan> *_3AgentPlan;
  QList<kkAgentPlan> *_4AgentPlan;
  QList<kkAgentPlan> *_5AgentPlan;

  QSqlDatabase planSQL;
  ~kkPlanSQL();
};

#endif // KKPLANSQL_H
