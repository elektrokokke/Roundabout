#include "roundabout.h"
#include "ui_roundabout.h"
#include "manhattanstyle.h"
#include "roundabouttestitem.h"

Roundabout::Roundabout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Roundabout)
{
    ui->setupUi(this);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    // display our RoundaboutScene in the graphics view:
    ui->graphicsView->setScene(&roundaboutScene);
    // add a test item to the scene:
    roundaboutScene.addItem(new RoundaboutTestItem());

    QString baseName = qApp->style()->objectName();
    qApp->setStyle(new ManhattanStyle(baseName));
}

Roundabout::~Roundabout()
{
    delete ui;
}
