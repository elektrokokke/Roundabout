#ifndef ROUNDABOUT_H
#define ROUNDABOUT_H

#include <QMainWindow>

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
};

#endif // ROUNDABOUT_H
