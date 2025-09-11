//
// Created by regal on 9/3/25.
//

#ifndef FILEMONITOR_FILEMONITOR_H
#define FILEMONITOR_FILEMONITOR_H

#include <memory>

class IFileMonitor
{
protected:
    virtual ~IFileMonitor() = default;
    virtual bool addWatch(const std::string& path) = 0;
    virtual bool removeWatch(const std::string& path) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

std::unique_ptr<IFileMonitor> createFileMonitor();

#endif //FILEMONITOR_FILEMONITOR_H