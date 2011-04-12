#include "roundabout.h"
#include "ui_roundabout.h"

Roundabout::Roundabout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Roundabout)
{
    ui->setupUi(this);
    // display our RoundaboutScene in the graphics view:
    ui->graphicsView->setScene(&roundaboutScene);
}

Roundabout::~Roundabout()
{
    delete ui;
}
