#include "Scheduler.h"
#include <iostream>
#include <ctime>
#include <algorithm>

void Scheduler::addSchedule(std::shared_ptr<Schedule> schedule) {
    schedules.push_back(schedule);
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
            // Execute scheduled action
            schedule->action();
            std::cout << "Schedule executed for device ID: " << schedule->deviceId << std::endl;

            // For one-time schedules, remove after execution
            if (schedule->scheduleType == "once") {
                removeSchedule(schedule->id);
            } else {
                // For recurring schedules, update next scheduled time based on type
                // This part can be enhanced for weekly/daily updates
            }
        }
    }
}