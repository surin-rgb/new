#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <vector>
#include <memory>
#include "Device.h"


class Room {
private:
    int id;
    std::string name;
    std::vector<std::shared_ptr<Device>> devices;


public:
    Room(int id, const std::string& name);
    ~Room();


    int getId() const;
    std::string getName() const;
    void setName(const std::string& newName);


    void addDevice(std::shared_ptr<Device> device);
    bool removeDevice(int deviceId);
    std::vector<std::shared_ptr<Device>> getDevices() const;


    bool toggleDevice(int deviceId);
    
    std::shared_ptr<Device> getDeviceById(int deviceId) const;

    // ====== New method added below, does not change old code =======
    std::shared_ptr<Device> getSharedDeviceById(int deviceId) const {
        for (const auto& device : devices) {
            if (device && device->getId() == deviceId) {
                return device;
            }
        }
        return nullptr;
    }
};


#endif // ROOM_H
