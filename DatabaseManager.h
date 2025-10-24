#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite3.h"

#ifdef __cplusplus
}
#endif
#include <string>
#include <vector>
#include <memory>
#include "Room.h"
#include "Device.h"
#include "SceneManager.h"

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbFilePath;

    bool executeSQLFile(const std::string& filePath);

public:
    DatabaseManager(const std::string& dbFile = "smarthome.db");
    ~DatabaseManager();

    bool openConnection();
    void closeConnection();

    bool initializeDatabase(const std::string& schemaFile, const std::string& sampleDataFile);

    // CRUD methods
    std::vector<std::shared_ptr<Room>> loadRooms();
    bool saveRoom(const Room& room);

    std::vector<std::shared_ptr<Device>> loadDevices(int roomId);
    bool saveDevice(const Device& device, int roomId);

    // Add methods for schedules and scenes similarly
};

#endif // DATABASEMANAGER_H
