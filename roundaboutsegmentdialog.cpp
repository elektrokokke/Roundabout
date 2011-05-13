#include "roundaboutsegmentdialog.h"
#include "ui_roundaboutsegmentdialog.h"

RoundaboutSegmentDialog::RoundaboutSegmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoundaboutSegmentDialog)
{
    ui->setupUi(this);
}

RoundaboutSegmentDialog::~RoundaboutSegmentDialog()
{
    delete ui;
}

bool RoundaboutSegmentDialog::editSegment(int &branchFrequency, int &continueFrequency)
{
    ui->spinBoxBranchFrequency->setValue(branchFrequency);
    ui->spinBoxContinueFrequency->setValue(continueFrequency);
    if (exec() == QDialog::Accepted) {
        branchFrequency = ui->spinBoxBranchFrequency->value();
        continueFrequency = ui->spinBoxContinueFrequency->value();
        return true;
    } else {
        return false;
    }
}
