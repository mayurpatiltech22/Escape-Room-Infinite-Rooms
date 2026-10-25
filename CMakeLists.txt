cmake_minimum_required(VERSION 3.16)

project(EscapeRoomInfiniteRooms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(EscapeRoom
    src/main.cpp
    src/Room.cpp
    src/EntranceRoom.cpp
    src/LibraryRoom.cpp
    src/LabRoom.cpp
    src/SecretRoom.cpp
    src/EscapeRoom.cpp
    src/RoomGraph.cpp
    src/GameManager.cpp
)

target_include_directories(EscapeRoom PRIVATE include)