#ifndef DEVICE_H
#define DEVICE_H

#include <string>

enum class DeviceType {
    LIGHT,
    FAN,
    AC,
    SENSOR
};

enum class DeviceState {
    OFF,
    ON,
    ACTIVE,  // For sensors or special states
    INACTIVE
};

class Device {
protected:
    int id;
    std::string name;
    DeviceType type;
    DeviceState state;

public:
    Device(int id, const std::string& name, DeviceType type);
    virtual ~Device();

    int getId() const;
    std::string getName() const;
    DeviceType getType() const;
    DeviceState getState() const;

    virtual void turnOn();
    virtual void turnOff();
    virtual void setState(DeviceState newState);
    virtual void toggle();

    virtual std::string getStateString() const;
};

#endif // DEVICE_H
