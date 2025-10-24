#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
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

public:
    SceneManager(const std::map<std::string, std::shared_ptr<Room>>& rooms);

    void createRoomScene(const std::string& sceneName, const std::string& roomName);
    void createHouseScene(const std::string& sceneName);
    std::vector<std::string> listSceneNames() const;
    bool applyScene(const std::string& sceneName);
    bool deleteScene(const std::string& sceneName);
};

#endif // SCENEMANAGER_H
