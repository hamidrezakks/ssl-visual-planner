#ifndef PLAYONPLANSQL_H
#define PLAYONPLANSQL_H

#include "base.h"

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QList>
#include <QDebug>
#include <QMessageBox>
#include <QtSql/QSqlError>

class playOnPlanSQL
{
public:
  playOnPlanSQL(QString directory);
  bool changeSQLDir(QString directory);


  int getIntByEnum(PEndPolicy tEnum);
  int getIntByEnum(PSkills tEnum);
  PEndPolicy getPolicyByIndex(int index);
  PSkills getPSkillByIndex(int index);

  QString getCommentForSQL(QString string);
  QString getCommentForPlanner(QString string);
  QString savePlanCommand(QString tableName, kkAgentPlan planStruct, int agentSize, int planId);
  void savePlanListInSQL(QSqlQuery &_query);
  void savePlan();

  void insertToList(QList<kkAgentPlan> &list, QSqlQuery query, int agentSize = 1);
  void loadPlan();

  bool addPlan(QList<kkAgentPlan> list, int agentSize = 1);
  bool addPlan(int planId, kkAgentPlan plan, QString &str);

  void removePlan(int _index);

  void disableSQL();
  void enableSQL();

  int getPlanSize();

  kkAgentPlan getPlan(int _index);
private:
  QList<kkPlayOnPlan> playOnPlans;
  QList<kkAgentPlan> agentPlan;

  QSqlDatabase planSQL;
  ~playOnPlanSQL();
};

#endif // PLAYONPLANSQL_H
