#include <iostream>
#include <memory>
#include "Device.h"
#include "Room.h"
#include "RuleEngine.h"
#include "Scheduler.h"
#include "SceneManager.h"
#include "DatabaseManager.h"
#include "UIManager.h"

int main() {
    // Initialize Database Manager
    std::string dbFile = "smarthome.db";
    std::string schemaFile = "init_schema.sql";
    std::string sampleDataFile = "sample_data.sql";

    auto dbManager = std::make_shared<DatabaseManager>(dbFile);

    std::cout << "Initializing database..." << std::endl;
    bool initSuccess = dbManager->initializeDatabase(schemaFile, sampleDataFile);
    if (!initSuccess) {
        std::cerr << "Failed to initialize database." << std::endl;
        return 1;
    }
    std::cout << "Database initialized successfully." << std::endl;

    // Initialize UI Manager which loads rooms and devices via DatabaseManager
    UIManager uiManager(dbManager);
    uiManager.initialize();

    // Main run loop for UI and interaction
    uiManager.run();

    std::cout << "Smart Home Simulator exiting. Goodbye!" << std::endl;
    return 0;
}
