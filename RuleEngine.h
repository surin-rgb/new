#ifndef RULEENGINE_H
#define RULEENGINE_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Room.h"
#include "Device.h"

// Base class for a rule
class Rule {
public:
    int deviceId;
    std::string ruleType; // "motion" or "temperature"
    float threshold;
    bool turnOnAbove;
    Rule(int id, const std::string &type, float th = 0.0f, bool onAbove = true)
        : deviceId(id), ruleType(type), threshold(th), turnOnAbove(onAbove) {}
    virtual ~Rule() = default;
};

class RuleEngine {
private:
    std::vector<Rule> rules;
    std::unordered_map<std::string, float> roomTemperatureMap;
    std::unordered_map<std::string, bool> roomMotionMap;

public:
    RuleEngine() = default;

    void addRule(const Rule &rule);
    void applyRules(std::shared_ptr<Room> room);

    // Simulation setters
    void setRoomTemperature(const std::string &roomName, float temp);
    void setRoomMotion(const std::string &roomName, bool motionDetected);

    float getRoomTemperature(const std::string &roomName);
    bool getRoomMotion(const std::string &roomName);

    void printRules() const;
    void startMonitoring(const std::shared_ptr<Room>& room);
};

#endif
