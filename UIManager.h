#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <memory>
#include <string>
#include <map>
#include "Room.h"
#include "RuleEngine.h"
#include "Scheduler.h"
#include "SceneManager.h"
#include "DatabaseManager.h"

class UIManager {
private:
    std::map<std::string, std::shared_ptr<Room>> rooms;
    std::shared_ptr<RuleEngine> ruleEngine;
    std::shared_ptr<Scheduler> scheduler;
    std::shared_ptr<SceneManager> sceneManager;
    std::shared_ptr<DatabaseManager> dbManager;

    void printMainMenu();
    void roomMenu(std::shared_ptr<Room> room);
    void deviceControlMenu(const std::shared_ptr<Room>& room);
    void scheduleMenu(const std::shared_ptr<Room>& room);
    void sceneMenu();
    void ruleMenu(const std::shared_ptr<Room>& room);

    void clearScreen();
    void pause();

    void printHeader(const std::string& title, int width = 50);
    std::string centerText(const std::string& text, int width);
    void printBorderLine(int width);

    // Helper for colored output
    void printColored(const std::string& text, int colorCode);

public:
    UIManager(std::shared_ptr<DatabaseManager> dbManager);

    void initialize();
    void run();
};

#endif // UI_MANAGER_H