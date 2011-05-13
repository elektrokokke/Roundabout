#ifndef ROUNDABOUTSEGMENTDIALOG_H
#define ROUNDABOUTSEGMENTDIALOG_H

#include <QDialog>

namespace Ui {
    class RoundaboutSegmentDialog;
}

class RoundaboutSegmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoundaboutSegmentDialog(QWidget *parent = 0);
    ~RoundaboutSegmentDialog();

    bool editSegment(int &branchFrequency, int &continueFrequency);

private:
    Ui::RoundaboutSegmentDialog *ui;
};

#endif // ROUNDABOUTSEGMENTDIALOG_H
