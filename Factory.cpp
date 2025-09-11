//
// Created by regal on 9/11/25.
//

#include "FileMonitor.h"

#ifdef __linux__
#include "FileMonitorLinux.h"
#endif

std::unique_ptr<IFileMonitor> createFileMonitor()
{
#ifdef __linux__
    return std::make_unique<FileMonitorLinux>();
#endif
}