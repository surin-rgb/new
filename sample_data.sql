-- Insert sample rooms
INSERT OR IGNORE INTO rooms (name) VALUES ('Living Room');
INSERT OR IGNORE INTO rooms (name) VALUES ('Bedroom');
INSERT OR IGNORE INTO rooms (name) VALUES ('Kitchen');
INSERT OR IGNORE INTO rooms (name) VALUES ('Bathroom');
INSERT OR IGNORE INTO rooms (name) VALUES ('Garage');
INSERT OR IGNORE INTO rooms (name) VALUES ('Dining Room');

-- Insert Format: (name, type, state, room_id)
-- 'type' corresponds to DeviceType enum as follows:
-- 0 = LIGHT
-- 1 = FAN
-- 2 = AC
-- 3 = SENSOR
-- 'state' corresponds to DeviceState enum as follows:
-- 0 = OFF
-- 1 = ON
-- 2 = ACTIVE (typically for sensors)

-- Insert sample devices for Living Room
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Main Light', 0, 1, (SELECT id FROM rooms WHERE name = 'Living Room'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Ceiling Fan', 1, 0, (SELECT id FROM rooms WHERE name = 'Living Room'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('AC Unit', 2, 1, (SELECT id FROM rooms WHERE name = 'Living Room'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Motion Sensor', 3, 2, (SELECT id FROM rooms WHERE name = 'Living Room'));

-- Devices for Bedroom
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Bedside Lamp', 0, 1, (SELECT id FROM rooms WHERE name = 'Bedroom'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Ceiling Fan', 1, 1, (SELECT id FROM rooms WHERE name = 'Bedroom'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('AC Unit', 2, 0, (SELECT id FROM rooms WHERE name = 'Bedroom'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Window Sensor', 3, 2, (SELECT id FROM rooms WHERE name = 'Bedroom'));

-- Devices for Kitchen
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Ceiling Light', 0, 1, (SELECT id FROM rooms WHERE name = 'Kitchen'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Exhaust Fan', 1, 1, (SELECT id FROM rooms WHERE name = 'Kitchen'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Sensor', 3, 2, (SELECT id FROM rooms WHERE name = 'Kitchen'));

-- Devices for Bathroom
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Bathroom Light', 0, 0, (SELECT id FROM rooms WHERE name = 'Bathroom'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Exhaust Fan', 1, 1, (SELECT id FROM rooms WHERE name = 'Bathroom'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Humidity Sensor', 3, 2, (SELECT id FROM rooms WHERE name = 'Bathroom'));

-- Devices for Garage
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Garage Light', 0, 0, (SELECT id FROM rooms WHERE name = 'Garage'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Garage Door Sensor', 3, 2, (SELECT id FROM rooms WHERE name = 'Garage'));

-- Devices for Dining Room
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Dining Light', 0, 1, (SELECT id FROM rooms WHERE name = 'Dining Room'));
INSERT OR IGNORE INTO devices (name, type, state, room_id)
VALUES ('Ceiling Fan', 1, 0, (SELECT id FROM rooms WHERE name = 'Dining Room'));
