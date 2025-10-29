#include "UIManager.h"
#include "Scheduler.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <thread>

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
    // Ensure sceneManager is initialized AFTER rooms is populated
    sceneManager = std::make_shared<SceneManager>(rooms);
}

void UIManager::initialize() {
    // No initialization required at this time.
}

void UIManager::run() {
    if (!scheduler)
        scheduler = std::make_shared<Scheduler>();
    static bool schedulerStarted = false;
    if (!schedulerStarted) {
        std::thread([this]() 
        {
            std::cout << "[Scheduler] Background thread started.\n";
            scheduler->runLoop(); // runs forever, checks schedules every 30s
        }).detach();
        schedulerStarted = true;
    }
    if (!ruleEngine)
    ruleEngine = std::make_shared<RuleEngine>();
    while (true) {
        clearScreen();
        printMainMenu();
        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            std::cout << "Exiting Application.\n";
            break;
        } 
        else if (input == "S" || input == "s") {
            sceneMenu();
        }
        else if (input == "R" || input == "r") {
            modifySimulatedConditionsMenu();
        }  
        else {
            try {
                int roomNum = std::stoi(input);
                if (roomNum > 0 && roomNum <= (int)rooms.size()) {
                    auto it = std::next(rooms.begin(), roomNum - 1);
                    roomMenu(it->second);
                } else {
                    std::cout << "Invalid room number.\n";
                    pause();
                }
            } catch (...) {
                std::cout << "Invalid input.\n";
                pause();
            }
        }
    }
}

void UIManager::printMainMenu() {
    std::cout << "+----------------------------------------+\n";
    std::cout << COLORBLUE << "      SMART HOME AUTOMATION SYSTEM      " << COLORRESET << "\n";
    std::cout << "+----------------------------------------+\n";
    int idx = 1;
    int roomsPerLine = 3;
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
    std::cout << "\n[0] Exit   [R] Modify Simulated Conditions\n";
    std::cout << "[S] Smart Scenes/Modes\n";
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

        // cache device list locally so we don't iterate over a temporary repeatedly
        auto deviceList = room->getDevices();
        for (const auto& device : deviceList) {
            if (!device) continue;
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
        std::cout << "[A] Toggle Device  [B] Add Schedule  [C] Apply Scene  [D] Rule-Based Automation  [E] Return to Main Menu\n>>> Select Option: ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "E" || choice == "e") break;
        else if (choice == "A" || choice == "a") {
            int deviceId;
            std::cout << "Enter Device ID to toggle: ";
            if (!(std::cin >> deviceId)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid ID entered.\n";
                pause();
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (!room->toggleDevice(deviceId))
                std::cout << "Invalid device ID.\n";
            else
                std::cout << "Toggled device!\n";
            pause();
        }
        else if (choice == "B" || choice == "b") {
            int deviceId;
            std::cout << "Enter Device ID to schedule: ";
            std::cin >> deviceId;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            auto targetDevice = room->getDeviceById(deviceId);
            if (!targetDevice) {
                std::cout << "Invalid device ID.\n";
                pause();
                return;
            }
            std::string timeStr;
            std::cout << "Enter time for schedule (HH:MM): ";
            std::getline(std::cin, timeStr);
            std::string actionType;
            auto deviceType = targetDevice->getType(); // keep it as enum, not string
            if (deviceType == DeviceType::SENSOR)
                std::cout << "Enter action (ACTIVE/INACTIVE): ";
                
            else
                std::cout << "Enter action (ON/OFF): ";
            std::getline(std::cin, actionType);
            int hour, minute;
            if (sscanf(timeStr.c_str(), "%d:%d", &hour, &minute) != 2) {
                std::cout << "Invalid time format!\n";
                pause();
                return;
            }
            auto now = std::chrono::system_clock::now();
            std::time_t tt = std::chrono::system_clock::to_time_t(now);
            std::tm local_tm = *std::localtime(&tt);
            local_tm.tm_hour = hour;
            local_tm.tm_min = minute;
            local_tm.tm_sec = 0;
            auto scheduledTime = std::chrono::system_clock::from_time_t(std::mktime(&local_tm));
            if (scheduledTime < now)
                scheduledTime += std::chrono::hours(24); // schedule for next day if time has passed
            auto schedule = std::make_shared<Schedule>();
            schedule->id = std::rand();
            schedule->deviceId = deviceId;
            schedule->scheduleType = "once";
            schedule->scheduledTime = scheduledTime;
            if (deviceType == DeviceType::SENSOR) {
                if (actionType == "ACTIVE" || actionType == "active")
                    schedule->action = [targetDevice]() { targetDevice->activate(); };
                else
                    schedule->action = [targetDevice]() { targetDevice->deactivate(); };
            } 
            else { 
                if (actionType == "ON" || actionType == "on")
                    schedule->action = [targetDevice]() { targetDevice->turnOn(); };
                else
                    schedule->action = [targetDevice]() { targetDevice->turnOff(); };
            }
            scheduler->addSchedule(schedule);
            std::cout << "Schedule added successfully!\n";
            pause();
        }
        else if (choice == "C" || choice == "c") {
            std::string sceneName;
            std::cout << "Enter scene name to apply: ";
            std::getline(std::cin, sceneName);
            if (!sceneManager) {
                std::cout << "Scene manager not available.\n";
            } else if (!sceneManager->applyScene(sceneName))
                std::cout << "Scene not found or failed to apply.\n";
            else
                std::cout << "Scene applied.\n";
            pause();
        }
        else if (choice == "D" || choice == "d") {
            int deviceId;
            std::cout << "Enter Device ID to automate: ";
            std::cin >> deviceId;
            auto device = room->getDeviceById(deviceId);
            if (!device) {
                std::cout << "Invalid device ID.\n";
                pause();
                return;
            }
            if (device->getType() == DeviceType::LIGHT || device->getType() == DeviceType::SENSOR) {
                std::cout << "Apply motion-based automation? (y/n): ";
                char c; 
                std::cin >> c;
                if (c == 'y' || c == 'Y')
                    ruleEngine->addRule(Rule(deviceId, "motion"));
            } 
            else {
                std::cout << "1. Turn ON above temperature\n2. Turn OFF below temperature\nSelect option: ";
                int opt; std::cin >> opt;
                float threshold;
                std::cout << "Enter temperature threshold (in Celcius): ";
                std::cin >> threshold;
                bool turnOnAbove = (opt == 1);
                ruleEngine->addRule(Rule(deviceId, "temperature", threshold, turnOnAbove));
            }
            ruleEngine->applyRules(room);
            pause();
        }
        else {
            std::cout << "Invalid option, try again.\n";
            pause();
        }
    }
}

void UIManager::sceneMenu() {
    if (!sceneManager) {
        std::cerr << "ERROR: SceneManager not initialized!\n";
        pause();
        return;
    }

    const int width = 50;

    // turn sceneMenu into a loop instead of recursion
    while (true) {
        clearScreen();
        printBorderLine(width);
        std::cout << COLORBLUE << centerText("SMART SCENES / MODES", width) << COLORRESET << std::endl;
        printBorderLine(width);

        sceneManager->listSceneNames();

        std::cout << "\n[N] Create New Scene  [A] Apply Scene  [B] Back to Main Menu\nChoose Option: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "B" || input == "b") {
            return;
        }
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
        else if (input == "A" || input == "a") {
            std::string sceneName;
            std::cout << "Enter scene name to apply: ";
            std::getline(std::cin, sceneName);

            if (sceneManager->applyScene(sceneName)) {
                std::cout << "Scene applied successfully!\n";
            } else {
                std::cout << "Scene not found or failed to apply.\n";
            }
            pause();
        }
        else {
            std::cout << "Invalid input.\n";
            pause();
        }
    }
}

void UIManager::modifySimulatedConditionsMenu() {
    clearScreen();
    const int width = 50;
    printBorderLine(width);
    std::cout << COLORBLUE << centerText("MODIFY SIMULATED CONDITIONS", width) << COLORRESET << std::endl;
    printBorderLine(width);
    std::cout << "\n[1] Set Global Temperature\n[2] Set Global Motion (Detected/Not Detected)\n[3] Set Temperature for a Specific Room\n[4] Set Motion for a Specific Room\n[B] Back to Main Menu\n\nChoose Option: ";
    std::string choice;
    std::cin >> choice;
    if (choice == "b" || choice == "B") {
        return;
    }
    if (choice == "1") {
        float temp;
        std::cout << "Enter temperature (in Celcius): ";
        std::cin >> temp;
        for (auto &entry : rooms)
            ruleEngine->setRoomTemperature(entry.second->getName(), temp);
        pause();
    } 
    else if (choice == "2") {
        char motion;
        std::cout << "Is motion detected? (y/n): ";
        std::cin >> motion;
        bool detect = (motion == 'y' || motion == 'Y');
        for (auto &entry : rooms)
            ruleEngine->setRoomMotion(entry.second->getName(), detect);
        pause();
    } 
    else if (choice == "3") {
        std::string roomName;
        std::cout << "Enter room name: ";
        std::cin.ignore();
        std::getline(std::cin, roomName);
        float temp;
        std::cout << "Enter temperature (in Celcius): ";
        std::cin >> temp;
        ruleEngine->setRoomTemperature(roomName, temp);
        pause();
    } 
    else if (choice == "4") {
        std::string roomName;
        std::cout << "Enter room name: ";
        std::cin.ignore();
        std::getline(std::cin, roomName);
        char motion;
        std::cout << "Is motion detected? (y/n): ";
        std::cin >> motion;
        bool detect = (motion == 'y' || motion == 'Y');
        ruleEngine->setRoomMotion(roomName, detect);
        pause();
    }
    else
    {
        std::cout << "Invalid input.\n";
        pause();
    }

    for (auto &entry : rooms)
        ruleEngine->applyRules(entry.second);
    pause();
}

void UIManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    if (std::getenv("TERM")) {
        system("clear");
    }
#endif
}

void UIManager::pause() {
    std::cout << "Press Enter to continue...";
    std::string tmp;
    std::getline(std::cin, tmp);
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
