//
// Created by regal on 9/11/25.
//

#ifndef FILEMONITOR_FILEMONITORLINUX_H
#define FILEMONITOR_FILEMONITORLINUX_H

#include "FileMonitor.h"
#include <vector>
#include <sys/inotify.h>
#include <thread>
#include <atomic>

class FileMonitorLinux final : public IFileMonitor
{
public:
    FileMonitorLinux();
    bool addWatch(const std::string& path) override;
    bool removeWatch(const std::string& path) override;
    void start() override;
    void stop() override;
    ~FileMonitorLinux() override;

    [[nodiscard]] std::vector<std::string>& getPaths() {return m_paths;}
private:
    void monitorLoop() const;
    static constexpr int EVENT_BUFF_LENGTH{1024 * (sizeof(struct inotify_event)+ 16)};
    std::vector<std::string> m_paths{};
    int m_fileDescriptor{};

    std::thread m_monitorThread;
    std::atomic<bool> m_stopRequested{false};
};

#endif //FILEMONITOR_FILEMONITORLINUX_H