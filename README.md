INTRODUCTION:
This project is a Smart Home Automation System developed in C++ as part of our university-level Object-Oriented Programming (OOP) course. 
It simulates core smart home features using a console-based UI and an SQLite database for persistent storage. The system allows users to control devices across multiple rooms, automate scenes, and schedule device actions.

FEATURES IMPLEMENTED:
1. Manual Device Control:
    Users can toggle devices such as:
    Lights (ON/OFF)
    Fans (ON/OFF)
    Air Conditioners (ON/OFF)
    Sensors (ACTIVE/INACTIVE)
2. Scheduling (One-time Schedule):
    Users can:
    Add a schedule to any device
    Set a specific time (HH:MM)
    System checks schedules in the background thread
    Executes the action at the correct time
3. Smart Scenes/Modes:
    Users can:
    Create modes (ex: Night Mode, Away Mode)
    Define default states for all devices
    Save scenes in the database
    Apply scenes at any time
4. Multi-Room Environment:
    Supports multiple rooms, where each room contains its own devices, independently controlled, like:
    Bedroom
    Living Room
    Bathroom
    Kitchen
    Garage
5. SQLite Database Integration:
    The system uses SQLite, to ensure that device states and scenes remain saved even after restarting the program, to persist:
    Rooms
    Devices
    Device states
    Scenes
    Scene-device mappings

HOW TO COMPILE THE PROJECT:
    Open MSYS2 MinGW64 or any g++ compiler and run (Ensure all .cpp files and the SQLite3 files (sqlite3.c, sqlite3.h) are in the same directory):
        1. g++ -std=c++17 -Wall -Wextra -I. -pthread \-c main.cpp Device.cpp Room.cpp Scheduler.cpp \SceneManager.cpp DatabaseManager.cpp UIManager.cpp
        2. gcc -c sqlite3.c
        3. g++ -std=c++17 -pthread \main.o Device.o Room.o Scheduler.o \SceneManager.o DatabaseManager.o UIManager.o sqlite3.o \-o SmartHomeBackend
    After successfully executing these functions without any errors and compiling application, run this function to start Console UI:
        1. ./SmartHomeBackend

REQUIREMENTS:
1. g++ with C++17 support
2. SQLite3 library (sqlite3 API already in uploaded file)
3. Windows (MSYS2) / Linux / Mac

FILE STRUCTURE:
    SmartHomeProject/
    ├── main.cpp
    ├── Device.cpp / Device.h
    ├── Room.cpp / Room.h
    ├── SceneManager.cpp / SceneManager.h
    ├── Scheduler.cpp / Scheduler.h
    ├── DatabaseManager.cpp / DatabaseManager.h
    ├── UIManager.cpp / UIManager.h
    ├── sqlite3.c / sqlite3.h
    ├── init_schema.sql
    ├── sample_data.sql
    └── README.md  ← (This file)

CREDITS:
    Mentor: Dr. Minni Jain
    SQLite Open-Source Project
    C++ STL and compiler toolchain developers
    Family & friends for support throughout development


