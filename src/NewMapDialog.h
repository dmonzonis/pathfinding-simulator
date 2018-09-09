#ifndef NEWMAPDIALOG_H
#define NEWMAPDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class NewMapDialog;
}

class NewMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewMapDialog(QWidget *parent = nullptr);
    ~NewMapDialog();
    bool hasAccepted() const;
    int getNumber(QLineEdit* text);
    int getWidth() const;
    int getHeight() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::NewMapDialog *ui;
    bool accepted;
    int width, height;
};

#endif // NEWMAPDIALOG_H
