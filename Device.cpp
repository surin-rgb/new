#include "Device.h"

Device::Device(int id, const std::string& name, DeviceType type)
    : id(id), name(name), type(type), state(DeviceState::OFF) {}

Device::~Device() {}

int Device::getId() const {
    return id;
}

std::string Device::getName() const {
    return name;
}

DeviceType Device::getType() const {
    return type;
}

DeviceState Device::getState() const {
    return state;
}

void Device::turnOn() {
    state = DeviceState::ON;
}

void Device::turnOff() {
    state = DeviceState::OFF;
}

void Device::activate() {
    state = DeviceState::ACTIVE;
}

void Device::deactivate() {
    state = DeviceState::INACTIVE;
}

void Device::setState(DeviceState newState) {
    state = newState;
}

void Device::toggle() {
    if (type == DeviceType::SENSOR) {
        if (state == DeviceState::ACTIVE) {
            deactivate();
        } else {
            activate();
        }
    } else {
        if (state == DeviceState::ON) {
            turnOff();
        } else {
            turnOn();
        }
    }
}

std::string Device::getStateString() const {
    switch (state) {
        case DeviceState::ON: return "ON";
        case DeviceState::OFF: return "OFF";
        case DeviceState::ACTIVE: return "ACTIVE";
        case DeviceState::INACTIVE: return "INACTIVE";
        default: return "UNKNOWN";
    }
}
