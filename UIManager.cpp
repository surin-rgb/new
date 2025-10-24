#include "UIManager.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <chrono>
#include <ctime>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define COLORRESET "\033[0m"
#define COLORRED "\033[31m"
#define COLORGREEN "\033[32m"
#define COLORYELLOW "\033[33m"
#define COLORBLUE "\033[34m"
#define COLORCYAN "\033[36m"
#define COLORBOLD "\033[1m"

UIManager::UIManager(std::shared_ptr<DatabaseManager> dbManager)
    : dbManager(dbManager) {
    rooms.clear();
    auto roomList = dbManager->loadRooms();
    for (const auto& room : roomList) {
        rooms[room->getName()] = room;
        auto devices = dbManager->loadDevices(room->getId());
        for (auto& dev : devices) {
            room->addDevice(dev);
        }
    }
}

void UIManager::initialize() {
    // No initialization required at this time.
}

void UIManager::run() {
    while (true) {
        clearScreen();
        printMainMenu();
        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            std::cout << "Exiting Application.\n";
            break;
        }
        else if (input == "S" or input == "s") {
            sceneMenu();
        }
        else 
        {
            try 
            {
                int roomNum = std::stoi(input);
                if (roomNum > 0 && roomNum <= (int)rooms.size()) {
                    auto it = std::next(rooms.begin(), roomNum - 1);
                    roomMenu(it->second);
                } 
                else {
                    std::cout << "Invalid room number.\n";
                    pause();
                }
            } 
            catch (...) {
                std::cout << "Invalid input.\n";
                pause();
            }
        }
    }
}

void UIManager::printMainMenu() {
    std::cout << "+----------------------------------------+\n";
    std::cout << COLORBLUE << "         SMART HOME AUTOMATION SYSTEM     " << COLORRESET << "\n";
    std::cout << "+----------------------------------------+\n";

    int idx = 1;
    int roomsPerLine = 3;  // Or 4 based on your visual preference
    int count = 0;
    for (const auto& pair : rooms) {
        std::cout << "[" << idx++ << "] " << pair.first << "  ";
        count++;
        if (count == roomsPerLine) {
            std::cout << std::endl;
            count = 0;
        }
    }
    if (count != 0) std::cout << std::endl;
    std::cout << "\n[0] Exit   [S] Smart Scenes/Modes\n";

}

void UIManager::roomMenu(std::shared_ptr<Room> room) {
    while (true) {
        clearScreen();
        printHeader(room->getName() + " - DEVICES", 40);
        std::cout << std::left << std::setw(5) << "ID"
                  << std::setw(20) << "Device"
                  << std::setw(10) << "State"
                  << std::setw(15) << "Last Action" << "\n";
        std::cout << "-----------------------------------------------\n";
        for (const auto& device : room->getDevices()) {
            std::string stateText = device->getStateString();
            std::string color = COLORRESET;
            if (stateText == "ON") color = COLORGREEN;
            else if (stateText == "OFF") color = COLORRED;
            else if (stateText == "ACTIVE") color = COLORYELLOW;
            else if (stateText == "INACTIVE") color = COLORCYAN;
            std::cout << std::left << std::setw(5) << device->getId()
                      << std::setw(20) << device->getName()
                      << color << std::setw(10) << stateText << COLORRESET
                      << std::setw(15) << "Manual" << "\n";
        }
        std::cout << "-----------------------------------------------\n";
        std::cout << "[A] Toggle Device  [B] Add Schedule  [C] Apply Scene  [D] Return to Main Menu\n>>> Select Option: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "D" || choice == "d") break;
        else if (choice == "A" || choice == "a") {
            int deviceId;
            std::cout << "Enter Device ID to toggle: ";
            std::cin >> deviceId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (!room->toggleDevice(deviceId))
                std::cout << "Invalid device ID.\n";
            else
                std::cout << "Toggled device!\n";
            pause();
        }
        else if (choice == "B" || choice == "b") {
            int deviceId;
            std::cout << "Enter Device ID to schedule: ";
            std::cin >> deviceId; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string time;
            std::cout << "Enter time for schedule (HH:MM): ";
            std::getline(std::cin, time);
            // Schedule creation logic here; for now, just a stub
            std::cout << "Schedule added (not implemented).\n";
            pause();    
        }
        else if (choice == "C" || choice == "c") {
            std::string sceneName;
            std::cout << "Enter scene name to apply: ";
            std::getline(std::cin, sceneName);
            if (!sceneManager->applyScene(sceneName))
                std::cout << "Scene not found or failed to apply.\n";
            else
                std::cout << "Scene applied.\n";
            pause();
        }
        else {
            std::cout << "Invalid option, try again.\n";
            pause();
        }
    }
}

void UIManager::sceneMenu() {
    clearScreen();
    const int width = 50;
    printBorderLine(width);
    std::cout << COLORBLUE << centerText("SMART SCENES / MODES", width) << COLORRESET << std::endl;
    printBorderLine(width);

    auto sceneNames = sceneManager->listSceneNames();
    if (sceneNames.empty())
        std::cout << "No scenes available.\n";
    else {
        int count = 1;
        for (const auto& name : sceneNames)
            std::cout << count++ << ". " << name << std::endl;
    }
    std::cout << "[N] Create New Scene  [B] Back\nChoose Option: ";

    std::string input;
    std::getline(std::cin, input);

    if (input == "B" || input == "b")
        return;
    else if (input == "N" || input == "n") {
        std::string sceneName, roomName;
        std::cout << "Enter new scene name: ";
        std::getline(std::cin, sceneName);
        std::cout << "Apply to room (enter name) or 'HOUSE' for all rooms: ";
        std::getline(std::cin, roomName);
        if (roomName == "HOUSE" || roomName == "house")
            sceneManager->createHouseScene(sceneName);
        else
            sceneManager->createRoomScene(sceneName, roomName);
        std::cout << "Scene created.\n";
        pause();
    }
    else {
        try {
            int idx = std::stoi(input);
            if (idx > 0 && idx <= (int)sceneNames.size()) {
                sceneManager->applyScene(sceneNames[idx - 1]);
                std::cout << "Scene applied.\n";
                pause();
            }
            else
                throw std::out_of_range("invalid index");
        }
        catch (...) {
            std::cout << "Invalid input.\n";
            pause();
        }
    }
    sceneMenu();  // Redisplay the menu
}

void UIManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    if (std::getenv("TERM")) 
    {
        system("clear");
    }
#endif
}


void UIManager::pause() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void UIManager::printBorderLine(int width) {
    for (int i = 0; i < width; i++) std::cout << "+";
    std::cout << std::endl;
}

std::string UIManager::centerText(const std::string& text, int width) {
    if ((int)text.length() >= width) return text;
    int leftPadding = (width - (int)text.length()) / 2;
    return std::string(leftPadding, ' ') + text;
}

void UIManager::printHeader(const std::string& title, int width) {
    printBorderLine(width);
    std::cout << COLORGREEN << centerText(title, width) << COLORRESET << std::endl;
    printBorderLine(width);
}
