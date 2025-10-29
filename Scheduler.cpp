#include "Scheduler.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <thread>

void Scheduler::addSchedule(std::shared_ptr<Schedule> schedule) {
    schedules.push_back(schedule);
    std::cout << "Schedule added for device ID: " << schedule->deviceId << std::endl;
}

void Scheduler::removeSchedule(int scheduleId) {
    schedules.erase(std::remove_if(schedules.begin(), schedules.end(),
        [scheduleId](const std::shared_ptr<Schedule>& sch) {
            return sch->id == scheduleId;
        }), schedules.end());
}

void Scheduler::checkAndRunSchedules() {
    auto now = std::chrono::system_clock::now();

    for (auto& schedule : schedules) {
        if (now >= schedule->scheduledTime) {
            schedule->action();
            std::cout << "Schedule executed for device ID: " << schedule->deviceId << std::endl;

            if (schedule->scheduleType == "once") {
                removeSchedule(schedule->id);
            } else if (schedule->scheduleType == "daily") {
                schedule->scheduledTime += std::chrono::hours(24);
            } else if (schedule->scheduleType == "weekly") {
                schedule->scheduledTime += std::chrono::hours(24 * 7);
            }
        }
    }
}

void Scheduler::runLoop() {
    while (true) {
        checkAndRunSchedules();
        std::this_thread::sleep_for(std::chrono::seconds(30)); // check every 30 seconds
    }
}
