//
// Created by regal on 9/11/25.
//

#include "UserInput.h"
#include <iostream>
#include <limits>
#include <sstream>

std::vector<std::string> splitPathsBySpace(const std::string& paths)
{
    std::vector<std::string> words{};
    std::stringstream ss(paths);
    std::string word{};

    while (ss >> word)
    {
        words.emplace_back(word);
    }
    return words;
}

std::vector<std::string> UserInput::getPaths()
{
    std::string paths{};
    while (true)
    {
        std::cout << "Enter paths of files/directories separated by spaces: ";
        std::getline(std::cin >> std::ws, paths);
        if (paths.empty()) continue;
        return splitPathsBySpace(paths);
    }
}

int UserInput::getChoice()
{
    int choice{};
    while (true)
    {
        std::cout << "Enter command (1, 2, 3, 4): ";
        std::cin >> choice;

        if (!std::cin)
        {
            if (std::cin.eof()) std::exit(0);
            std::cout << "Invalid input! \n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice > 0 && choice < 5)
            return choice;
    }
}
