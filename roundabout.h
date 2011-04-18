#ifndef ROUNDABOUT_H
#define ROUNDABOUT_H

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

#include <QMainWindow>
#include <QSplashScreen>
#include <QTimer>
#include "roundaboutscene.h"

namespace Ui {
    class Roundabout;
}

class Roundabout : public QMainWindow
{
    Q_OBJECT

public:
    explicit Roundabout(QWidget *parent = 0);
    ~Roundabout();

private slots:
    void on_actionCreate_roundabout_triggered();
    void on_actionCreate_conductor_triggered();
    void on_actionAbout_triggered();

private:
    Ui::Roundabout *ui;
    QSplashScreen splashScreen;
    QTimer splashTimer;
    RoundaboutScene roundaboutScene;
};

#endif // ROUNDABOUT_H
