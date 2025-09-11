#include "FileMonitorLinux.h"
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <poll.h>

FileMonitorLinux::FileMonitorLinux()
{
     m_fileDescriptor = inotify_init1(0);
    if (m_fileDescriptor < 0)
    {
        throw std::runtime_error("Failed to initialize inotify instance");
    }
}

FileMonitorLinux::~FileMonitorLinux() {
    stop();
}

bool FileMonitorLinux::addWatch(const std::string& path)
{
    const auto it{std::ranges::find(m_paths, path)};
    if (it != m_paths.end())
    {
        std::cout << "Path already present in monitor. \n";
        return false;
    }
    m_paths.emplace_back(path);
    return true;

}

bool FileMonitorLinux::removeWatch(const std::string& path)
{
    const auto it{std::ranges::find(m_paths, path)};
    if (it == m_paths.end())
    {
        std::cout << "Path not present in monitor. \n";
        return false;
    }
    m_paths.erase(it);
    return true;
}

void FileMonitorLinux::monitorLoop() const
{
    std::unordered_map<int, std::string> watches{};

    for (const auto& path: m_paths)
    {
        int current_wd {inotify_add_watch(m_fileDescriptor, path.c_str(), IN_CREATE | IN_DELETE | IN_CLOSE_WRITE | IN_DELETE_SELF)};

        if (current_wd < 0) // handle failed watch creation
        {
            std::string errorMsg{"Failed to add watch for path: "};
            errorMsg += path;

            // remove existing watches
            std::cout << "Removing existing watches and exiting ... \n";
            for (const auto& watch: watches)
            {
                inotify_rm_watch(m_fileDescriptor, watch.first);
            }

            throw std::runtime_error(errorMsg);
        }

        watches.insert({current_wd, path});
        std::cout << "Watching path: " << path << '\n';
    }

    struct pollfd pfd{};
    pfd.fd = m_fileDescriptor;
    pfd.events = POLLIN;

    while (!m_stopRequested && !watches.empty())
    {
        int ret{poll(&pfd, 1, 250)};
        if (ret < 0)
        {
            break;
        }
        if (ret == 0)
        {
            continue;
        }
        if (pfd.revents & POLLIN)
        {
            char buffer[EVENT_BUFF_LENGTH];

            while (!watches.empty())
            {
                const std::ptrdiff_t length{read(m_fileDescriptor, buffer, EVENT_BUFF_LENGTH)};
                if (length < 0)
                {
                    throw std::runtime_error("Read error");
                }
                for (std::size_t i{}; i < static_cast<std::size_t>(length); )
                {
                    const auto* event{reinterpret_cast<const struct inotify_event*>(&buffer[i])};

                    if (event->mask & IN_DELETE_SELF)
                    {
                        const auto it {watches.find(event->wd)};
                        if (it != watches.end())
                        {
                            std::cout << "EVENT: Watched directory '" << it->second << "' was deleted." << '\n';
                            watches.erase(it);
                        }
                    }
                    else if (event->len)
                    {
                        const std::string fileName{event->name};
                        if (!fileName.empty() && fileName.back() == '~')
                        {
                            i += sizeof(struct inotify_event) + event->len;
                            continue;
                        }
                        if (event->mask & IN_CREATE) {
                            std::cout << "EVENT: File '" << event->name << "' was created." << '\n';
                        } else if (event->mask & IN_CLOSE_WRITE) {
                            std::cout << "EVENT: File '" << event->name << "' was modified." << '\n';
                        } else if (event->mask & IN_DELETE) {
                            std::cout << "EVENT: File '" << event->name << "' was deleted." << '\n';
                        }
                    }
                    i += sizeof(struct inotify_event) + event->len;
                }
            }
        }
    }
    for (const auto& watch : watches) {
        inotify_rm_watch(m_fileDescriptor, watch.first);
    }
}

void FileMonitorLinux::start()
{
    if (m_monitorThread.joinable()) {
        // Already running, do nothing or throw an error
        return;
    }
    m_stopRequested = false;
    // Launch monitorLoop() on a new thread
    m_monitorThread = std::thread(&FileMonitorLinux::monitorLoop, this);
}

void FileMonitorLinux::stop()
{
    if (m_monitorThread.joinable()) {
        m_stopRequested = true; // Signal the thread to stop
        m_monitorThread.join(); // Wait for the thread to finish
    }
}

