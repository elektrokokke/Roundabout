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
    explicit RoundaboutSegmentDialog(int branchFrequency, int continueFrequency, QWidget *parent = 0);
    ~RoundaboutSegmentDialog();

    int getBranchFrequency() const;
    int getContinueFrequency() const;

private:
    Ui::RoundaboutSegmentDialog *ui;
};

#endif // ROUNDABOUTSEGMENTDIALOG_H
