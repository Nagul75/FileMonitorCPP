//
// Created by regal on 9/11/25.
//

#include "UserInput.h"
#include <iostream>
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