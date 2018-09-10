#include "utils.h"

void showErrorMessage(std::string msg)
{
    QMessageBox errorMsg;
    errorMsg.critical(nullptr, "Error", QString::fromStdString(msg));
    errorMsg.setFixedSize(500, 200);
}
