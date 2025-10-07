//
// Created by regal on 9/3/25.
//

#include "FileMonitor.h"
#include "FileMonitorLinux.h"
#include <iostream>
#include "UserInput.h"

void printMenu()
{
    std::cout << "\n \n";
    std::cout << "1. Add Path \n";
    std::cout << "2. Remove Path \n";
    std::cout << "3. Start File Monitor \n";
    std::cout << "4. Stop File Monitor and exit \n";
    std::cout << "NOTE: File monitor will be stopped when paths are added/removed! Be sure to start it again. \n";
}

void handleChoice(const int choice, std::unique_ptr<IFileMonitor>& fm)
{
    switch (choice)
    {
    case 1:
        {
            if (dynamic_cast<FileMonitorLinux*>(fm.get())->getRunning())
            {
                fm->stop();
            }
            const std::vector<std::string> paths{UserInput::getPaths()};
            for (const auto& path: paths)
            {
                if (!fm->addWatch(path))
                {
                    std::cout << path << " already being monitored! \n";
                }
            }
            break;
        }
    case 2:
        {
            if (dynamic_cast<FileMonitorLinux*>(fm.get())->getRunning()) fm->stop();
            const std::vector<std::string> paths{UserInput::getPaths()};
            for (const auto& path: paths)
            {
                if (!fm->removeWatch(path))
                {
                    std::cout << path << " does not exist to be removed! \n";
                }
            }
            break;
        }
    case 3:
        {
            fm->start();
            break;
        }
    default:
        {
            std::cout << "Invalid choice! \n";
            break;
        }
    }
}

int main()
{
    std::cout << "CPP File Monitor using Inotify \n";
    std::cout << "NOTE: File monitor will be stopped when paths are added/removed! Be sure to start it again. \n";
    std::unique_ptr<IFileMonitor> fm{createFileMonitor()};
    while (true)
    {
        printMenu();
        const int choice{UserInput::getChoice()};
        if (choice == 4)
        {
            fm->stop();
            break;
        }
        handleChoice(choice, fm);
    }
    std::cout << "Thank you for using Team Nagul's CPP FileMonitor \n";
    return 0;
}