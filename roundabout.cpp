/*
    Copyright 2011 Arne Jacobs <jarne@jarne.de>

    This file is part of Roundabout.

    Roundabout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Roundabout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Roundabout.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "roundabout.h"
#include "ui_roundabout.h"

Roundabout::Roundabout(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Roundabout),
    splashScreen(QPixmap(":/png/images/splash.png"), Qt::WindowStaysOnTopHint)
{
    ui->setupUi(this);
    QActionGroup *toolGroup = new QActionGroup(this);
    toolGroup->addAction(ui->actionTool1);
    toolGroup->addAction(ui->actionTool2);
    toolGroup->addAction(ui->actionTool3);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    // display our RoundaboutScene in the graphics view:
    ui->graphicsView->setScene(&roundaboutScene);
    QObject::connect(&splashTimer, SIGNAL(timeout()), &splashScreen, SLOT(close()));
    splashScreen.show();
    splashTimer.start(2000);
}

Roundabout::~Roundabout()
{
    delete ui;
}

void Roundabout::on_actionCreate_roundabout_triggered()
{
    roundaboutScene.createCircle();
}

void Roundabout::on_actionCreate_conductor_triggered()
{
    roundaboutScene.createConductor();
}

void Roundabout::on_actionAbout_triggered()
{
    splashScreen.show();
    splashTimer.start(5000);
}
