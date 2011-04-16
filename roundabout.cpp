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
    RoundaboutTestConnectionItem *connectionItem = new RoundaboutTestConnectionItem(27);
    RoundaboutTestItem *testItem1 = new RoundaboutTestItem();
    RoundaboutTestItem *testItem2 = new RoundaboutTestItem();
    testItem2->setPos(testItem2->rect().width() + 56, 0);
    roundaboutScene.addItem(connectionItem);
    roundaboutScene.addItem(testItem1);
    roundaboutScene.addItem(testItem2);
    testItem1->setConnectionItem(connectionItem, RoundaboutTestConnectionItem::P1);
    testItem2->setConnectionItem(connectionItem, RoundaboutTestConnectionItem::P2);
}

Roundabout::~Roundabout()
{
    delete ui;
}
