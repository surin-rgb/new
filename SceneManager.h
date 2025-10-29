#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "Room.h"
#include "Device.h"

struct SceneDeviceState {
    int deviceId;
    DeviceState state;
};

enum class SceneType { ROOM, HOUSE };

struct Scene {
    std::string name;
    SceneType type;
    std::string targetRoom;
    std::vector<SceneDeviceState> deviceStates;
};

class SceneManager {
private:
    std::vector<Scene> scenes;
    std::map<std::string, std::shared_ptr<Room>> rooms;
    mutable std::mutex scenesMutex; // protect scenes vector

    // helper to find scene by name; returns index or -1 if not found
    int findSceneIndex(const std::string& sceneName) const;

public:
    SceneManager(const std::map<std::string, std::shared_ptr<Room>>& rooms);

    void createRoomScene(const std::string& sceneName, const std::string& roomName);
    void createHouseScene(const std::string& sceneName);

    void listSceneNames() const;

    bool applyScene(const std::string& sceneName);
    bool deleteScene(const std::string& sceneName);
};

#endif // SCENEMANAGER_H
