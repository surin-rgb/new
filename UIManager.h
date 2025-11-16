#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <memory>
#include <map>
#include <vector>
#include <string>
#include "Room.h"
#include "Scheduler.h"
#include "SceneManager.h"
#include "DatabaseManager.h"

class UIManager {
private:
    std::shared_ptr<DatabaseManager> dbManager;
    std::map<std::string, std::shared_ptr<Room>> rooms;
    std::shared_ptr<SceneManager> sceneManager;
    std::shared_ptr<Scheduler> scheduler;

    void clearScreen();
    void pause();

    void printBorderLine(int width);
    std::string centerText(const std::string& text, int width);
    void printHeader(const std::string& title, int width);
    void printColored(const std::string& text, int colorCode);

    void printMainMenu();
    void roomMenu(std::shared_ptr<Room> room);
    void sceneMenu();
    void deviceControlMenu(const std::shared_ptr<Room>& room);
    void scheduleMenu(const std::shared_ptr<Room>& room);

public:
    explicit UIManager(std::shared_ptr<DatabaseManager> dbManager);

    void initialize();
    void run();
};

#endif // UIMANAGER_H
