#include <QApplication>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>
#include "mainwindow.h"
#include "benchmark.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        // GUI mode
        QApplication app(argc, argv);
        MainWindow window;
        window.show();
        return app.exec();
    }
    // CLI mode
    /*
     * The arg parsing done here is very rudimentary, it was only done this way to get something
     * working fast, and there's really no need to do anything fancier, so if someone ever
     * reads this code (including future me), please keep that in mind and don't judge too harshly.
     */
    else
    {
        std::string option = argv[1];
        // Check for benchmark option
        if (option == "-b")
        {
            try
            {
                // Make sure we have the right amount of arguments
                if (argc != 4)
                {
                    throw std::runtime_error("Incorrect number of arguments.");
                }
                std::string filename = argv[2];
                int count = std::stoi(argv[3]);
                Benchmark benchmark(filename);
                benchmark.run(count);
            }
            catch (std::exception &ex)
            {
                std::cerr << ex.what() << std::endl;
                printUsage();
                return -2;
            }
        }
        else
        {
            printUsage();
            return -1;
        }
    }

}
