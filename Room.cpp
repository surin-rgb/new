#include "Room.h"

Room::Room(int id, const std::string& name)
    : id(id), name(name) {}

Room::~Room() {}

int Room::getId() const {
    return id;
}

std::string Room::getName() const {
    return name;
}

void Room::setName(const std::string& newName) {
    name = newName;
}

void Room::addDevice(std::shared_ptr<Device> device) {
    devices.push_back(device);
}

bool Room::removeDevice(int deviceId) {
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        if ((*it)->getId() == deviceId) {
            devices.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<Device>> Room::getDevices() const {
    return devices;
}

std::shared_ptr<Device> Room::getDeviceById(int deviceId) const {
    for (const auto& device : devices) {
        if (device->getId() == deviceId) {
            return device;
        }
    }
    return nullptr;
}

bool Room::toggleDevice(int deviceId) {
    for (auto& device : devices) {
        if (device->getId() == deviceId) {
            device->toggle();
            return true;
        }
    }
    return false;
}

