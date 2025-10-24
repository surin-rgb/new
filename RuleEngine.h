#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#include <vector>
#include <memory>
#include "Room.h"

class Rule {
public:
    virtual ~Rule() {}
    // Applies the rule on the supplied room, returns true if any changes were made
    virtual bool apply(std::shared_ptr<Room> room) = 0;
};

class TemperatureRule : public Rule {
private:
    float threshold;
public:
    TemperatureRule(float threshold);
    bool apply(std::shared_ptr<Room> room) override;
};

class MotionSensorRule : public Rule {
public:
    MotionSensorRule();
    bool apply(std::shared_ptr<Room> room) override;
};

class RuleEngine {
private:
    std::vector<std::shared_ptr<Rule>> rules;
public:
    RuleEngine() = default;
    void addRule(std::shared_ptr<Rule> rule);
    void applyRules(std::shared_ptr<Room> room);
};

#endif // RULE_ENGINE_H
