#include "DatabaseManager.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "sqlite3.h"

#ifdef __cplusplus
}
#endif
#include <iostream>
#include <fstream>
#include <sstream>


DatabaseManager::DatabaseManager(const std::string& dbFile)
    : db(nullptr), dbFilePath(dbFile) {}

DatabaseManager::~DatabaseManager() {
    closeConnection();
}

bool DatabaseManager::openConnection() {
    int rc = sqlite3_open(dbFilePath.c_str(), &db);
    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

void DatabaseManager::closeConnection() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool DatabaseManager::executeSQLFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "SQL file not found: " << filePath << std::endl;
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::initializeDatabase(const std::string& schemaFile, const std::string& sampleDataFile) {
    if (!openConnection()) return false;

    if (!executeSQLFile(schemaFile)) return false;
    if (!executeSQLFile(sampleDataFile)) return false;

    return true;
}

std::vector<std::shared_ptr<Room>> DatabaseManager::loadRooms() {
    std::vector<std::shared_ptr<Room>> rooms;
    const char* sql = "SELECT id, name FROM rooms;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare room query." << std::endl;
        return rooms;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        std::string name = std::string(reinterpret_cast<const char*>(nameText));
        auto room = std::make_shared<Room>(id, name);
        rooms.push_back(room);
    }
    sqlite3_finalize(stmt);
    return rooms;
}

bool DatabaseManager::saveRoom(const Room& room) {
    // Implementation to insert or update room data into DB
    return true;
}

std::vector<std::shared_ptr<Device>> DatabaseManager::loadDevices(int roomId) {
    std::vector<std::shared_ptr<Device>> devices;
    const char* sql = "SELECT id, name, type, state FROM devices WHERE room_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare devices query." << std::endl;
        return devices;
    }

    if (sqlite3_bind_int(stmt, 1, roomId) != SQLITE_OK) {
        std::cerr << "Failed to bind room_id parameter." << std::endl;
        sqlite3_finalize(stmt);
        return devices;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        int typeInt = sqlite3_column_int(stmt, 2);
        int stateInt = sqlite3_column_int(stmt, 3);

        std::string name = std::string(reinterpret_cast<const char*>(nameText));
        DeviceType type = static_cast<DeviceType>(typeInt);
        DeviceState state = static_cast<DeviceState>(stateInt);

        auto device = std::make_shared<Device>(id, name, type);
        device->setState(state);
        devices.push_back(device);
    }
    sqlite3_finalize(stmt);
    return devices;
}

bool DatabaseManager::saveDevice(const Device& device, int roomId) {
    // Implementation to insert or update device data into DB
    return true;
}
