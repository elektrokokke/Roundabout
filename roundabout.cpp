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
#include <QSpinBox>
#include <QLabel>

Roundabout::Roundabout(RoundaboutThread *thread, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Roundabout),
    splashScreen(QPixmap(":/png/images/splash.png"), Qt::WindowStaysOnTopHint),
    roundaboutThread(thread)
{
    ui->setupUi(this);
    roundaboutThread->setParent(this);
    // put all step tempo buttons into one group (to allow only one of them to be active):
    QActionGroup *toolGroup = new QActionGroup(this);
    toolGroup->addAction(ui->actionFourBeatsPerStep);
    toolGroup->addAction(ui->actionTwoBeatsPerStep);
    toolGroup->addAction(ui->actionOneStepPerBeat);
    toolGroup->addAction(ui->actionTwoStepsPerBeat);
    toolGroup->addAction(ui->actionFourStepsPerBeat);
    // add additional controls to the toolbar:
    ui->mainToolBar->insertWidget(ui->actionFourBeatsPerStep, new QLabel("Steps per beat: ", ui->mainToolBar));
    QSpinBox *inputChannelSpinBox = new QSpinBox(ui->mainToolBar);
    ui->mainToolBar->addSeparator();
    inputChannelSpinBox->setRange(0, 15);
    inputChannelSpinBox->setValue(0);
    ui->mainToolBar->addWidget(new QLabel("Input MIDI channel: ", ui->mainToolBar));
    ui->mainToolBar->addWidget(inputChannelSpinBox);
    ui->mainToolBar->addSeparator();
    QObject::connect(inputChannelSpinBox, SIGNAL(valueChanged(int)), roundaboutThread, SLOT(setInputChannel(int)));
    QSpinBox *outputChannelSpinBox = new QSpinBox(ui->mainToolBar);
    ui->mainToolBar->addSeparator();
    outputChannelSpinBox->setRange(0, 15);
    outputChannelSpinBox->setValue(0);
    ui->mainToolBar->addWidget(new QLabel("Output MIDI channel: ", ui->mainToolBar));
    ui->mainToolBar->addWidget(outputChannelSpinBox);
    ui->mainToolBar->addSeparator();
    QObject::connect(outputChannelSpinBox, SIGNAL(valueChanged(int)), roundaboutThread, SLOT(setOutputChannel(int)));

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    // display our RoundaboutScene in the graphics view:
    ui->graphicsView->setScene(&roundaboutScene);
    QObject::connect(&splashTimer, SIGNAL(timeout()), &splashScreen, SLOT(close()));
    splashScreen.show();
    splashTimer.start(2000);
    QObject::connect(roundaboutThread, SIGNAL(createdSequencer(RoundaboutSequencer*)), &roundaboutScene, SLOT(onCreatedSequencer(RoundaboutSequencer*)));
}

Roundabout::~Roundabout()
{
    delete ui;
}

void Roundabout::on_actionCreate_roundabout_triggered()
{
    roundaboutThread->createSequencer();
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

void Roundabout::on_actionOneStepPerBeat_triggered()
{
    roundaboutThread->setStepsPerBeat(1);
}

void Roundabout::on_actionTwoStepsPerBeat_triggered()
{
    roundaboutThread->setStepsPerBeat(2);
}

void Roundabout::on_actionFourStepsPerBeat_triggered()
{
    roundaboutThread->setStepsPerBeat(4);
}

void Roundabout::on_actionTwoBeatsPerStep_triggered()
{
    roundaboutThread->setStepsPerBeat(0.5);
}

void Roundabout::on_actionFourBeatsPerStep_triggered()
{
    roundaboutThread->setStepsPerBeat(0.25);
}
