cmake_minimum_required(VERSION 3.16)

project(EscapeRoomInfiniteRooms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Main game executable
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
    src/Puzzle.cpp
    src/Door.cpp
     src/Item.cpp
    src/Inventory.cpp
    src/Player.cpp
    src/Trap.cpp
)

target_include_directories(EscapeRoom PRIVATE include)

# Separate puzzle-system tests
add_executable(PuzzleTests
    tests/puzzle_tests.cpp
    src/Puzzle.cpp
    src/Door.cpp
    src/Item.cpp
    src/Inventory.cpp
    src/Player.cpp
    src/Trap.cpp
)

target_include_directories(PuzzleTests PRIVATE include)

enable_testing()

add_test(NAME PuzzleSystemTests COMMAND PuzzleTests)