#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

DatabaseManager::DatabaseManager(const std::string& dbFile)
    : db(nullptr), dbFilePath(dbFile) 
{
    if (!openConnection()) {
        std::cerr << "[DatabaseManager] Failed to open database: " << dbFilePath << std::endl;
    } else {
        // Optional: Create tables if not exist
        const char* createRooms =
            "CREATE TABLE IF NOT EXISTS rooms ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT UNIQUE NOT NULL);";

        const char* createDevices =
            "CREATE TABLE IF NOT EXISTS devices ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "type INTEGER,"
            "state INTEGER,"
            "room_id INTEGER,"
            "FOREIGN KEY(room_id) REFERENCES rooms(id));";

        char* errMsg = nullptr;
        sqlite3_exec(db, createRooms, nullptr, nullptr, &errMsg);
        if (errMsg) { std::cerr << "SQL error: " << errMsg << std::endl; sqlite3_free(errMsg); }
        sqlite3_exec(db, createDevices, nullptr, nullptr, &errMsg);
        if (errMsg) { std::cerr << "SQL error: " << errMsg << std::endl; sqlite3_free(errMsg); }
    }
}

DatabaseManager::~DatabaseManager() {
    closeConnection();
}

bool DatabaseManager::openConnection() {
    if (db) return true; // already open

    int rc = sqlite3_open(dbFilePath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
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
    if (!db && !openConnection()) return rooms;

    const char* sql = "SELECT id, name FROM rooms;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare room query: " << sqlite3_errmsg(db) << std::endl;
        return rooms;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        std::string name = nameText ? reinterpret_cast<const char*>(nameText) : "Unnamed Room";
        auto room = std::make_shared<Room>(id, name);
        rooms.push_back(room);
    }

    sqlite3_finalize(stmt);
    return rooms;
}

bool DatabaseManager::saveRoom(const Room& room) {
    if (!db && !openConnection()) return false;

    const char* sql = "INSERT OR REPLACE INTO rooms (id, name) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare room save query.\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, room.getId());
    sqlite3_bind_text(stmt, 2, room.getName().c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to save room: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<std::shared_ptr<Device>> DatabaseManager::loadDevices(int roomId) {
    std::vector<std::shared_ptr<Device>> devices;
    if (!db && !openConnection()) return devices;

    const char* sql = "SELECT id, name, type, state FROM devices WHERE room_id = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare devices query: " << sqlite3_errmsg(db) << std::endl;
        return devices;
    }

    sqlite3_bind_int(stmt, 1, roomId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        int typeInt = sqlite3_column_int(stmt, 2);
        int stateInt = sqlite3_column_int(stmt, 3);

        std::string name = nameText ? reinterpret_cast<const char*>(nameText) : "Unnamed Device";
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
    if (!db && !openConnection()) return false;

    const char* sql =
        "INSERT OR REPLACE INTO devices (id, name, type, state, room_id) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare device save query.\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, device.getId());
    sqlite3_bind_text(stmt, 2, device.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, static_cast<int>(device.getType()));
    sqlite3_bind_int(stmt, 4, static_cast<int>(device.getState()));
    sqlite3_bind_int(stmt, 5, roomId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to save device: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}
