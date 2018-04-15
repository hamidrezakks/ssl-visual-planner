#ifndef TAGS_H
#define TAGS_H

#include <QDialog>
#include <QString>
#include <QLabel>
#include <QMouseEvent>
#include <QMessageBox>
#include <QStringList>

#include <QDebug>

namespace Ui {
class tags;
}

class tags : public QDialog
{
    Q_OBJECT

public:
    explicit tags(QWidget *parent = 0);
    void loadTagsByString(QString str);
    ~tags();

    void mousePressEvent(QMouseEvent *);
    void closeEvent(QCloseEvent *);

private slots:
    void on_addTagBtn_clicked();

private:
    Ui::tags *ui;

    QString tagsString;
    QList<QString> tagsList;

    void initTagLabel();
    void removeTag(QString str);
    void removeTag(int index);
    void addTag(QString str, bool noPaint = false);
    void editTag(QString str);
    void paintTags();
    bool hasTag(QString str);

    QLabel *tagLabels[48][2];

    int tagToEditIndex;

signals:
    void tagExit(QString str);

};

#endif // TAGS_H
