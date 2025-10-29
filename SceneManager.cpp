#include "SceneManager.h"
#include <iostream>
#include <algorithm>

SceneManager::SceneManager(const std::map<std::string, std::shared_ptr<Room>>& rooms)
    : rooms(rooms) {}

int SceneManager::findSceneIndex(const std::string& sceneName) const {
    std::lock_guard<std::mutex> lock(scenesMutex);
    for (size_t i = 0; i < scenes.size(); ++i) {
        if (scenes[i].name == sceneName) return static_cast<int>(i);
    }
    return -1;
}

void SceneManager::createRoomScene(const std::string& sceneName, const std::string& roomName) {
    auto it = rooms.find(roomName);
    if (it == rooms.end()) {
        std::cerr << "Room '" << roomName << "' not found." << std::endl;
        return;
    }

    auto room = it->second;
    auto roomDevices = room->getDevices();
    if (roomDevices.empty()) {
        std::cerr << "No devices found in this room!" << std::endl;
        return;
    }

    Scene scene;
    scene.name = sceneName;
    scene.type = SceneType::ROOM;
    scene.targetRoom = roomName;

    std::cout << "\nConfiguring scene '" << sceneName << "' for room '" << roomName << "':\n";

    for (auto& devicePtr : roomDevices) {
        if (!devicePtr) continue;
        std::string userInput;
        std::cout << "Device: " << devicePtr->getName() << " (ID: " << devicePtr->getId()
                  << ") - Current state: " << devicePtr->getStateString() << "\n";
        auto deviceType = devicePtr->getType();
        if (deviceType == DeviceType::SENSOR)
            std::cout << "Enter desired state for this device in scene (ACTIVE/INACTIVE/skip): ";
        else
            std::cout << "Enter desired state for this device in scene (ON/OFF/skip): ";
        std::getline(std::cin, userInput);

        if (userInput == "skip" || userInput == "SKIP" || userInput.empty()) {
            continue;
        }

        SceneDeviceState sds;
        sds.deviceId = devicePtr->getId();

        if (deviceType == DeviceType::SENSOR) {
            if (userInput == "ACTIVE" || userInput == "active")
                sds.state = DeviceState::ACTIVE;
            else if (userInput == "INACTIVE" || userInput == "inactive")
                sds.state = DeviceState::INACTIVE;
            else
                continue; // invalid input → skip
        } 
        else {
            if (userInput == "ON" || userInput == "on")
                sds.state = DeviceState::ON;
            else if (userInput == "OFF" || userInput == "off")
                sds.state = DeviceState::OFF;
            else
                continue; // invalid input → skip
        }

        scene.deviceStates.push_back(sds);
    }

    {
        std::lock_guard<std::mutex> lock(scenesMutex);
        scenes.push_back(scene);
    }

    std::cout << "\nRoom scene '" << sceneName << "' created successfully with "
              << scene.deviceStates.size() << " devices configured.\n";
}


void SceneManager::createHouseScene(const std::string& sceneName) {
    if (rooms.empty()) {
        std::cerr << "No rooms found in the system!" << std::endl;
        return;
    }

    Scene scene;
    scene.name = sceneName;
    scene.type = SceneType::HOUSE;
    scene.targetRoom = "";

    std::cout << "\nConfiguring HOUSE scene '" << sceneName << "':\n";

    for (auto& pair : rooms) {
        auto& roomName = pair.first;
        auto roomPtr = pair.second;
        auto roomDevices = roomPtr->getDevices();

        if (roomDevices.empty()) {
            std::cout << "\n[Room: " << roomName << "] No devices found, skipping...\n";
            continue;
        }

        std::cout << "\n[Room: " << roomName << "]\n";

        for (auto& devicePtr : roomDevices) {
            if (!devicePtr) continue;

            std::string userInput;
            std::cout << "Device: " << devicePtr->getName() << " (ID: " << devicePtr->getId()
                      << ") - Current state: " << devicePtr->getStateString() << "\n";
            auto deviceType = devicePtr->getType();
            if (deviceType == DeviceType::SENSOR)
                std::cout << "Enter desired state for this device in scene (ACTIVE/INACTIVE/skip): ";
            else
                std::cout << "Enter desired state for this device in scene (ON/OFF/skip): ";
            std::getline(std::cin, userInput);

            if (userInput == "skip" || userInput == "SKIP" || userInput.empty()) {
                continue;
            }

            SceneDeviceState sds;
            sds.deviceId = devicePtr->getId();

            if (deviceType == DeviceType::SENSOR) {
                if (userInput == "ACTIVE" || userInput == "active")
                    sds.state = DeviceState::ACTIVE;
                else if (userInput == "INACTIVE" || userInput == "inactive")
                    sds.state = DeviceState::INACTIVE;
                else
                    continue; // invalid input → skip
            } 
            else {
                if (userInput == "ON" || userInput == "on")
                    sds.state = DeviceState::ON;
                else if (userInput == "OFF" || userInput == "off")
                    sds.state = DeviceState::OFF;
                else
                    continue; // invalid input → skip
            }

            scene.deviceStates.push_back(sds);
        }
    }

    {
        std::lock_guard<std::mutex> lock(scenesMutex);
        scenes.push_back(scene);
    }

    std::cout << "\nHouse scene '" << sceneName << "' created successfully with "
              << scene.deviceStates.size() << " devices configured.\n";
}


void SceneManager::listSceneNames() const {
    std::lock_guard<std::mutex> lock(scenesMutex);
    if (scenes.empty()) {
        std::cout << "No scenes available." << std::endl;
        return;
    }

    std::cout << "\n===== Available Scenes =====" << std::endl;
    int count = 1;
    for (const auto& scene : scenes) {
        std::cout << count++ << ". " << scene.name;
        if (scene.type == SceneType::ROOM) {
            std::cout << " (Room: " << scene.targetRoom << ")";
        } else {
            std::cout << " (House-wide)";
        }
        std::cout << std::endl;
    }
    std::cout << "===========================\n" << std::endl;
}

bool SceneManager::applyScene(const std::string& sceneName) {
    int idx = findSceneIndex(sceneName);
    if (idx < 0) {
        std::cerr << "Error: Scene '" << sceneName << "' not found." << std::endl;
        return false;
    }

    // Copy the scene under lock, operate on local copy to minimize locked duration
    Scene sceneCopy;
    {
        std::lock_guard<std::mutex> lock(scenesMutex);
        sceneCopy = scenes[static_cast<size_t>(idx)];
    }

    std::cout << "Applying scene: '" << sceneName << "'..." << std::endl;

    if (sceneCopy.type == SceneType::ROOM) {
        auto roomIt = rooms.find(sceneCopy.targetRoom);
        if (roomIt == rooms.end()) {
            std::cerr << "Room " << sceneCopy.targetRoom << " not found." << std::endl;
            return false;
        }

        auto room = roomIt->second;
        auto deviceList = room->getDevices(); // cache the vector locally
        for (const auto& sds : sceneCopy.deviceStates) {
            for (auto& devicePtr : deviceList) {
                if (!devicePtr) continue;
                if (devicePtr->getId() == sds.deviceId) {
                    devicePtr->setState(sds.state);
                    break; // stop inner loop once matched
                }
            }
        }
    }
    else { // HOUSE
        for (auto& pair : rooms) {
            auto room = pair.second;
            auto deviceList = room->getDevices(); // cache locally
            for (const auto& sds : sceneCopy.deviceStates) {
                for (auto& devicePtr : deviceList) {
                    if (!devicePtr) continue;
                    if (devicePtr->getId() == sds.deviceId) {
                        devicePtr->setState(sds.state);
                        break;
                    }
                }
            }
        }
    }

    std::cout << "Scene '" << sceneName << "' applied successfully!" << std::endl;
    return true;
}

bool SceneManager::deleteScene(const std::string& sceneName) {
    std::lock_guard<std::mutex> lock(scenesMutex);
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        if (it->name == sceneName) {
            scenes.erase(it);
            std::cout << "Scene '" << sceneName << "' deleted." << std::endl;
            return true;
        }
    }
    std::cerr << "Scene '" << sceneName << "' not found." << std::endl;
    return false;
}
