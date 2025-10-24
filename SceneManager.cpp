#include "SceneManager.h"
#include <iostream>

SceneManager::SceneManager(const std::map<std::string, std::shared_ptr<Room>>& rooms)
    : rooms(rooms) {}

void SceneManager::createRoomScene(const std::string& sceneName, const std::string& roomName) {
    auto it = rooms.find(roomName);
    if (it == rooms.end()) {
        std::cerr << "Room " << roomName << " not found." << std::endl;
        return;
    }

    Scene scene;
    scene.name = sceneName;
    scene.type = SceneType::ROOM;
    scene.targetRoom = roomName;

    auto roomDevices = it->second->getDevices();
    for (auto& device : roomDevices) {
        SceneDeviceState sds;
        sds.deviceId = device->getId();
        sds.state = device->getState();
        scene.deviceStates.push_back(sds);
    }

    scenes.push_back(scene);
    std::cout << "Room scene '" << sceneName << "' created." << std::endl;
}

void SceneManager::createHouseScene(const std::string& sceneName) {
    Scene scene;
    scene.name = sceneName;
    scene.type = SceneType::HOUSE;
    scene.targetRoom = "";  // No specific room target

    // Collect all devices
    for (auto& pair : rooms) {
        auto roomDevices = pair.second->getDevices();
        for (auto& device : roomDevices) {
            SceneDeviceState sds;
            sds.deviceId = device->getId();
            sds.state = device->getState();
            scene.deviceStates.push_back(sds);
        }
    }

    scenes.push_back(scene);
    std::cout << "House scene '" << sceneName << "' created." << std::endl;
}

std::vector<std::string> SceneManager::listSceneNames() const {
    std::vector<std::string> names;
    for (const auto& scene : scenes) {
        names.push_back(scene.name);
    }
    return names;
}

bool SceneManager::applyScene(const std::string& sceneName) {
    for (const auto& scene : scenes) {
        if (scene.name == sceneName) {
            std::cout << "Applying scene: " << sceneName << std::endl;
            
            // Apply device states to rooms
            if (scene.type == SceneType::ROOM) {
                auto roomIt = rooms.find(scene.targetRoom);
                if (roomIt == rooms.end()) {
                    std::cerr << "Room " << scene.targetRoom << " not found." << std::endl;
                    return false;
                }
                auto room = roomIt->second;
                for (auto& sds : scene.deviceStates) {
                    for (auto& device : room->getDevices()) {
                        if (device->getId() == sds.deviceId) {
                            device->setState(sds.state);
                        }
                    }
                }
            }
            else if (scene.type == SceneType::HOUSE) {
                // Apply across all rooms
                for (auto& pair : rooms) {
                    auto room = pair.second;
                    for (auto& sds : scene.deviceStates) {
                        for (auto& device : room->getDevices()) {
                            if (device->getId() == sds.deviceId) {
                                device->setState(sds.state);
                            }
                        }
                    }
                }
            }

            return true;
        }
    }
    return false;
}
