#include "tags.h"

tags::tags(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tags)
{
    ui->setupUi(this);
    initTagLabel();
    //Tag Input stylesheet
    ui->textTag->setStyleSheet("QLineEdit {padding: 5px 10px;}");
    tagToEditIndex = -1;
}

tags::~tags()
{
    delete ui;
}

void tags::initTagLabel() {
    for (int i = 0; i < 48; i++) {
        tagLabels[i][0] = new QLabel(this);

        tagLabels[i][0]->setStyleSheet("QLabel { background-color : #89a1b5; color : white; font-weight: light; font-family: Ubuntu Mono;} QLabel:HOVER { background-color : #2f78b3; }");
        tagLabels[i][0]->setText(QString::number(i));
        tagLabels[i][0]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        tagLabels[i][0]->hide();
//        tagLabels[1].append(new QLabel(ui->tagsGroupBox));

        tagLabels[i][1] = new QLabel(this);

        tagLabels[i][1]->setStyleSheet("QLabel { background-color : #EEA500; color : white; font-weight: light;} QLabel:HOVER { background-color : #FFA500; }");
        tagLabels[i][1]->setCursor(Qt::PointingHandCursor);
        tagLabels[i][1]->setText("X");
        tagLabels[i][1]->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
        tagLabels[i][1]->hide();
    }
}

void tags::paintTags()
{
    for (int i = tagsList.length(); i < 48; i++) {
        tagLabels[i][0]->hide();
        tagLabels[i][1]->hide();
    }

    QRect geoRect;
    int tempX = 10, tempY = 100;
    int tagLength = 0;
    for (int i = 0; i < tagsList.length(); i++) {
        tagLength = tagsList.at(i).length()*8;
        tagLength += 10;
        if (tempX > this->geometry().width() - (tagLength + 25)) {
            tempX = 10;
            tempY += 24;
        }
        geoRect.setRect(tempX, tempY, tagLength, 20);
        tagLabels[i][0]->setGeometry(geoRect);
        tagLabels[i][0]->setText(tagsList.at(i));
        tagLabels[i][0]->show();

        geoRect.setRect(tempX + tagLength, tempY, 20, 20);
        tagLabels[i][1]->setGeometry(geoRect);
        tagLabels[i][1]->show();

        tempX += tagLength + 24;
    }


}

void tags::loadTagsByString(QString str)
{
    ui->textTag->setText("");
    tagsList.clear();
    QStringList tempStrList = str.split("|");
    for (int i = 0; i < tempStrList.length(); i++) {
        if (tempStrList.at(i).length() > 0) {
            addTag(tempStrList.at(i), true);
        }
    }
    paintTags();
}

void tags::addTag(QString str, bool noPaint)
{
    if (!hasTag(str)) {
        tagsList.append(str);
        if (!noPaint) {
            paintTags();
        }
    }
    else {
        QMessageBox::warning(this, "Already exists", "This tag is already exist", QMessageBox::Ok);
    }
}

void tags::editTag(QString str)
{
    if (!hasTag(str)) {
        tagsList.replace(tagToEditIndex, str);
        paintTags();
        tagToEditIndex = -1;
        ui->addTagBtn->setText("Add Tag");
    }
    else {
        QMessageBox::warning(this, "Already exists", "This tag is already exist", QMessageBox::Ok);
    }
}

bool tags::hasTag(QString str)
{
    return tagsList.contains(str);
}

void tags::on_addTagBtn_clicked()
{
    QString tagText = ui->textTag->text();
    tagText = tagText.toLower();
    tagText.replace(' ', '_');

    if (tagToEditIndex == -1) {
        addTag(tagText);
    }
    else {
        editTag(tagText);
    }
}

void tags::mousePressEvent(QMouseEvent *event)
{
    bool deleted = false;
    int editIndex = -1;
    for (int i = 0; i < tagsList.length(); i++) {
        if (tagLabels[i][1]->underMouse()) {
            tagsList.removeAt(i);
            deleted = true;
        }
        if (tagLabels[i][0]->underMouse()) {
            editIndex = i;
        }
    }

    if (deleted) {
        paintTags();
    }

    if (editIndex != -1) {
        tagToEditIndex = editIndex;
        ui->addTagBtn->setText("Edit Tag");
        ui->textTag->setText(tagsList.at(editIndex));
    }

    if (!deleted && editIndex == -1) {
        tagToEditIndex = -1;
        ui->addTagBtn->setText("Add Tag");
    }
}

void tags::closeEvent(QCloseEvent *event)
{
    tagsString = "";
    bool first = true;
    for (int i = 0; i < tagsList.length(); i++) {
        if (first) {
            tagsString = tagsList.at(i);
            first = false;
        }
        else {
            tagsString += "|"+tagsList.at(i);
        }
    }

    emit tagExit(tagsString);
}
