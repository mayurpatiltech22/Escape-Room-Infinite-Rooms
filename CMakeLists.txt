cmake_minimum_required(VERSION 3.16)

project(EscapeRoomInfiniteRooms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core game logic source files
set(GAME_CORE_SOURCES
    src/Room.cpp
    src/EntranceRoom.cpp
    src/LibraryRoom.cpp
    src/LabRoom.cpp
    src/SecretRoom.cpp
    src/EscapeRoom.cpp
    src/RoomGraph.cpp
    src/GameManager.cpp
    src/SaveLoadManager.cpp
    src/Puzzle.cpp
    src/Door.cpp
    src/Item.cpp
    src/Inventory.cpp
    src/Player.cpp
    src/Trap.cpp
)

# Graphics and audio sources
set(GRAPHICS_SOURCES
    src/GameRenderer.cpp
    src/SoundSystem.cpp
    src/RoomVisuals.cpp
    src/InteractiveEntity.cpp
    src/PlayerEntity.cpp
    src/HUD.cpp
    src/PuzzleModal.cpp
    src/GameDialog.cpp
)

# 1. Main game executable
add_executable(EscapeRoom
    src/main.cpp
    ${GAME_CORE_SOURCES}
    ${GRAPHICS_SOURCES}
)

target_include_directories(EscapeRoom PRIVATE include)

find_package(SFML CONFIG REQUIRED
    COMPONENTS Graphics Window System Audio
)

target_link_libraries(EscapeRoom PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)
# 2. Puzzle system unit tests
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

# 3. File-handling unit tests
add_executable(FileHandlingTests
    tests/file_handling_tests.cpp
    ${GAME_CORE_SOURCES}
)
target_include_directories(FileHandlingTests PRIVATE include)

# 4. Exception-handling unit tests
add_executable(ExceptionHandlingTests
    tests/exception_handling_tests.cpp
    ${GAME_CORE_SOURCES}
)
target_include_directories(ExceptionHandlingTests PRIVATE include)

# Enable CTest suite
enable_testing()

add_test(NAME PuzzleSystemTests COMMAND PuzzleTests)
add_test(NAME FileHandlingTests COMMAND FileHandlingTests)
add_test(NAME ExceptionHandlingTests COMMAND ExceptionHandlingTests)
