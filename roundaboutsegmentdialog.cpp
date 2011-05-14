#include "roundaboutsegmentdialog.h"
#include "ui_roundaboutsegmentdialog.h"

RoundaboutSegmentDialog::RoundaboutSegmentDialog(int branchFrequency, int continueFrequency, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoundaboutSegmentDialog)
{
    ui->setupUi(this);
    ui->spinBoxBranchFrequency->setValue(branchFrequency);
    ui->spinBoxContinueFrequency->setValue(continueFrequency);
}

RoundaboutSegmentDialog::~RoundaboutSegmentDialog()
{
    delete ui;
}

int RoundaboutSegmentDialog::getBranchFrequency() const
{
    return ui->spinBoxBranchFrequency->value();
}

int RoundaboutSegmentDialog::getContinueFrequency() const
{
    return ui->spinBoxContinueFrequency->value();
}
