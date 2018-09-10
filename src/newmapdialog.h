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

    /**
     * @brief Return true if the dialog is in accepted state, that is, it finished doing what
     * it needed to do and all data is valid.
     */
    bool hasAccepted() const;

    /**
     * @brief Get a strictly positive number from a line edit.
     *
     * It will get the text from the line edit and try to convert it to a number. If the
     * conversion isn't possible or the number is 0 or less, it will show an error message
     * and return -1. If all goes well, it returns the number.
     *
     * @return The integer in the line edit if it's valid, -1 otherwise.
     */
    int getNumber(QLineEdit* text);

    /**
     * @brief Return the width parameter, which can be set by accepting the dialog with a valid
     * number written in the width line edit.
     */
    int getWidth() const;

    /**
     * @brief Return the height parameter, which can be set by accepting the dialog with a valid
     * number written in the height line edit.
     */
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
