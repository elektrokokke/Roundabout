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

#include <QtGui/QApplication>
#include <QMessageBox>
#include "roundabout.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RoundaboutThread *thread = new RoundaboutThread();
    if (!thread->isValid()) {
        QMessageBox::critical(0, "Jack not running?", "Could not connect to the Jack server. Please make sure that the Jack server is running.");
        return -1;
    }
    Roundabout w(thread);
    w.show();
    return a.exec();
}
