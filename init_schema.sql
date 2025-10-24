-- Table to store rooms in the smart home
CREATE TABLE IF NOT EXISTS rooms (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- Table to store devices within rooms
CREATE TABLE IF NOT EXISTS devices (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    type INTEGER NOT NULL,        -- Maps to DeviceType enum (0=LIGHT, 1=FAN, ...)
    state INTEGER NOT NULL,       -- Maps to DeviceState enum (0=OFF, 1=ON, ...)
    room_id INTEGER NOT NULL,
    FOREIGN KEY (room_id) REFERENCES rooms(id) ON DELETE CASCADE
);

-- Table to store device schedules
CREATE TABLE IF NOT EXISTS schedules (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id INTEGER NOT NULL,
    schedule_type TEXT NOT NULL, -- e.g., "daily", "weekly", "once"
    scheduled_time TEXT NOT NULL,-- ISO 8601 format timestamp
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
);

-- Table to store smart scenes/modes
CREATE TABLE IF NOT EXISTS scenes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    type TEXT NOT NULL,          -- "room" or "house"
    target_room TEXT             -- NULL or room name for room scenes
);

-- Table mapping scenes to device states
CREATE TABLE IF NOT EXISTS scene_devices (
    scene_id INTEGER NOT NULL,
    device_id INTEGER NOT NULL,
    device_state INTEGER NOT NULL, -- DeviceState enum
    PRIMARY KEY (scene_id, device_id),
    FOREIGN KEY (scene_id) REFERENCES scenes(id) ON DELETE CASCADE,
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
);
