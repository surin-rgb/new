#include "RuleEngine.h"
#include <iostream>

TemperatureRule::TemperatureRule(float threshold) : threshold(threshold) {}

bool TemperatureRule::apply(std::shared_ptr<Room> room) {
    // Example: If temperature sensor > threshold, turn OFF AC devices in room
    bool changed = false;
    for (auto& device : room->getDevices()) {
        if (device->getType() == DeviceType::SENSOR) {
            // Simplified: Assume sensors have state ACTIVE when temperature > threshold
            if (device->getState() == DeviceState::ACTIVE) {
                for (auto& d : room->getDevices()) {
                    if (d->getType() == DeviceType::AC && d->getState() == DeviceState::ON) {
                        d->turnOff();
                        std::cout << "Rule triggered: Turning OFF AC in room " << room->getName() << std::endl;
                        changed = true;
                    }
                }
            }
        }
    }
    return changed;
}

MotionSensorRule::MotionSensorRule() {}

bool MotionSensorRule::apply(std::shared_ptr<Room> room) {
    // Placeholder logic for motion sensor rule
    bool changed = false;
    // Implement motion based actions here as required
    return changed;
}

void RuleEngine::addRule(std::shared_ptr<Rule> rule) {
    rules.push_back(rule);
}

void RuleEngine::applyRules(std::shared_ptr<Room> room) {
    for (auto& rule : rules) {
        rule->apply(room);
    }
}
