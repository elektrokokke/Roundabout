#include "roundabout.h"
#include "ui_roundabout.h"

Roundabout::Roundabout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Roundabout)
{
    ui->setupUi(this);
}

Roundabout::~Roundabout()
{
    delete ui;
}
