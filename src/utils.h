#ifndef UTILS_H
#define UTILS_H

#include <QMessageBox>

/**
 * @brief Shows a dialog with an error message.
 * @param msg Text to show in the dialog.
 */
void showErrorMessage(std::string msg);

/**
 * @brief Returns whether two numbers are within a certain tolerance from each other.
 *
 * For example, 1.0001 and 1.00015 would be considered approximately equal with a tolerance
 * of 0.0001
 */
bool approxEqual(double a, double b, double tolerance = 1e-5);

#endif // UTILS_H
