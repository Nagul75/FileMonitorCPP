//
// Created by regal on 9/3/25.
//

#include <sys/inotify.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>

#define EVENT_BUFF_LEN (1024 * (sizeof(struct inotify_event)+ 16))

int main(const int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path to watch>" << '\n';
        return 1;
    }

    const int fd {inotify_init1(0)};

    if (fd < 0) std::exit(1);

    std::vector<std::pair<int, std::string>> watches;

    for (int i{1}; i < argc; i++)
    {
        int current_wd = inotify_add_watch(fd, argv[i], IN_CREATE | IN_DELETE | IN_CLOSE_WRITE | IN_DELETE_SELF);
        if (current_wd < 0) {
            perror("inotify_add_watch");
            close(fd);
            std::exit(1);
        }
        watches.emplace_back(current_wd, argv[i]);
        std::cout << "Watching path: " << argv[i] << '\n';
    }
    std::cout << "Press ctrl+c to stop. \n";

    char buffer[EVENT_BUFF_LEN];

    while (!watches.empty())
    {
        const std::ptrdiff_t length {read(fd, buffer, EVENT_BUFF_LEN)};
        if (length < 0)
        {
            std::perror("read");
            break;
        }
        for (size_t i = 0; i < static_cast<size_t>(length); ) {
            const auto* event = reinterpret_cast<const struct inotify_event*>(&buffer[i]);

            if (event->mask & IN_DELETE_SELF) {
                // Find the path associated with this watch descriptor
                for (auto it = watches.begin(); it != watches.end(); ++it) {
                    if (it->first == event->wd) {
                        std::cout << "EVENT: Watched directory '" << it->second << "' was deleted." << '\n';
                        watches.erase(it);
                        break;
                    }
                }
            }
            else if (event->len) {
                const std::string filename = event->name;

                if (!filename.empty() && filename.back() == '~') {
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

    std::cout << "Cleaning up and exiting. \n";
    for (int i{0}; i < argc - 1; i++)
    {
        inotify_rm_watch(fd, watches[i].first);
    }
    close(fd);

    return 0;
}

