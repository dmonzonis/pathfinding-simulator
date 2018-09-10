#include "newmapdialog.h"
#include "ui_newmapdialog.h"
#include "utils.h"

NewMapDialog::NewMapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewMapDialog),
    accepted(false),
    width(0),
    height(0)
{
    ui->setupUi(this);
}

NewMapDialog::~NewMapDialog()
{
    delete ui;
}

bool NewMapDialog::hasAccepted() const
{
    return accepted;
}

int NewMapDialog::getNumber(QLineEdit *text)
{
    // Try to convert it to a strictly positive integer
    bool ok;
    int number = text->text().toInt(&ok);
    if (!ok || number < 1)
    {
        showErrorMessage("Invalid number entered.");
        return -1;
    }
    return number;
}

int NewMapDialog::getWidth() const
{
    return width;
}

int NewMapDialog::getHeight() const
{
    return height;
}

void NewMapDialog::on_buttonBox_accepted()
{
    width = getNumber(ui->textWidth);
    if (width == -1)
    {
        // Invalid data, early close
        close();
        return;
    }
    height = getNumber(ui->textHeight);
    if (height != -1)
    {
        // We have all valid data
        accepted = true;
    }
    close();
}

void NewMapDialog::on_buttonBox_rejected()
{
    accepted = false;
    close();
}
