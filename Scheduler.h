#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <functional>
#include <vector>
#include <chrono>
#include <memory>
#include "Device.h"

struct Schedule {
    int id;
    int deviceId;
    std::string scheduleType; // "daily", "weekly", "once"
    std::chrono::system_clock::time_point scheduledTime;
    std::function<void()> action; // Callback that performs the scheduled action
};

class Scheduler {
private:
    std::vector<std::shared_ptr<Schedule>> schedules;

public:
    Scheduler() = default;

    void addSchedule(std::shared_ptr<Schedule> schedule);
    void removeSchedule(int scheduleId);
    void checkAndRunSchedules();
};

#endif // SCHEDULER_H