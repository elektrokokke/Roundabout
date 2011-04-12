#ifndef ROUNDABOUT_H
#define ROUNDABOUT_H

#include <QMainWindow>
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

private:
    Ui::Roundabout *ui;
    RoundaboutScene roundaboutScene;
};

#endif // ROUNDABOUT_H
