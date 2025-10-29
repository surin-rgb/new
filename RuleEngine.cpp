#include "RuleEngine.h"
#include <iostream>
#include <thread>
#include <chrono>

void RuleEngine::addRule(const Rule &rule) {
    rules.push_back(rule);
    std::cout << "Rule added successfully for device ID " << rule.deviceId << "\n";
}

void RuleEngine::setRoomTemperature(const std::string &roomName, float temp) {
    roomTemperatureMap[roomName] = temp;
    std::cout << "Temperature in " << roomName << " set to " << temp << " Celcius\n";
}

void RuleEngine::setRoomMotion(const std::string &roomName, bool motionDetected) {
    roomMotionMap[roomName] = motionDetected;
    std::cout << "Motion in " << roomName << ": " << (motionDetected ? "Detected" : "Not Detected") << "\n";
}

float RuleEngine::getRoomTemperature(const std::string &roomName) {
    if (roomTemperatureMap.find(roomName) != roomTemperatureMap.end())
        return roomTemperatureMap[roomName];
    return 25.0f; // default
}

bool RuleEngine::getRoomMotion(const std::string &roomName) {
    if (roomMotionMap.find(roomName) != roomMotionMap.end())
        return roomMotionMap[roomName];
    return false;
}

void RuleEngine::applyRules(const std::shared_ptr<Room> room) {
    if (!room) return;

    std::string roomName = room->getName();
    float currentTemp = getRoomTemperature(roomName);
    bool motionDetected = getRoomMotion(roomName);

    for (const auto &rule : rules) {
        auto device = room->getDeviceById(rule.deviceId);
        if (!device) continue;

        if (rule.ruleType == "temperature") {
            if (rule.turnOnAbove) {
                if (currentTemp > rule.threshold) device->turnOn();
                else device->turnOff();
            } else {
                if (currentTemp < rule.threshold) device->turnOn();
                else device->turnOff();
            }
        } else if (rule.ruleType == "motion") {
            if (motionDetected) device->turnOn();
            else device->turnOff();
        }
    }
}

void RuleEngine::printRules() const {
    std::cout << "\nActive Rules:\n";
    for (const auto &r : rules) {
        std::cout << "  • Device ID: " << r.deviceId << " | Type: " << r.ruleType;
        if (r.ruleType == "temperature")
            std::cout << " | Threshold: " << r.threshold << " Celcius";
        std::cout << "\n";
    }
}

void RuleEngine::startMonitoring(const std::shared_ptr<Room>& room) 
{
    while (true) {
        applyRules(room);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
