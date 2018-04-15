#ifndef PLAYONPLANSQL_H
#define PLAYONPLANSQL_H

#include <base.h>

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
  ~playOnPlanSQL();
  bool changeSQLDir(QString directory);


  int getIntByEnum(EndPolicy tEnum);
  int getIntByEnum(Skills tEnum);
  EndPolicy getPolicyByIndex(int index);
  Skills getPSkillByIndex(int index);

  QString getCommentForSQL(QString string);
  QString getCommentForPlanner(QString string);
  QString savePlanCommand(QString tableName, AgentPlan planStruct, int agentSize, int planId);
  void savePlanListInSQL(QSqlQuery &_query);
  void savePlan();

  void insertToList(QList<AgentPlan> &list, QSqlQuery query, int agentSize = 1);
  void loadPlan();

  bool addPlan(int planId, AgentPlan plan, QString &str);

  void removePlan(int _index);

  void disableSQL();
  void enableSQL();

  int getPlanSize();

  AgentPlan getPlan(int _index);
private:
  QList<PlayOnPlan> playOnPlans;
  QList<AgentPlan> agentPlan;

  QSqlDatabase planSQL;
};

#endif // PLAYONPLANSQL_H
