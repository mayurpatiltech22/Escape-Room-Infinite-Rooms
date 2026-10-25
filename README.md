# Escape Room: Infinite Rooms

An Object-Oriented C++ text adventure and puzzle game featuring procedural room graphs, interactive puzzles, unlockable doors, and an enterprise-grade **Save/Load System** with comprehensive **File-Handling** and **Exception-Handling** test suites.

---

## Table of Contents
- [Project Overview](#project-overview)
- [Key Features](#key-features)
- [Architecture & Class Hierarchy](#architecture--class-hierarchy)
- [Save/Load System & File Handling](#saveload-system--file-handling)
- [Exception-Handling Strategy](#exception-handling-strategy)
- [Unit Testing Suites](#unit-testing-suites)
- [Building & Running](#building--running)
- [Member Contribution & Role](#member-contribution--role)
- [Project Documentation Links](#project-documentation-links)

---

## Project Overview

*Escape Room: Infinite Rooms* challenges players to navigate through connected chambers, solve diverse puzzles (math, logic, word, sequence), unlock doors, and escape the facility. The engine leverages modern C++ principles (C++17) emphasizing dynamic polymorphism, encapsulation, exception safety, and persistent state management.

---

## Key Features

- **Polymorphic Rooms**: Abstract `Room` base class with concrete chambers (`EntranceRoom`, `LibraryRoom`, `LabRoom`, `SecretRoom`, `EscapeRoom`).
- **Graph-Based Navigation**: `RoomGraph` utilizing Breadth-First Search (BFS) to compute shortest escape routes.
- **Puzzle & Door Mechanics**: Multi-type puzzle engine (`MathPuzzle`, `LogicPuzzle`, `WordPuzzle`, `SequencePuzzle`) tied to unlockable security doors.
- **Robust Save & Load System**:
  - Full game snapshot capture and restoration.
  - Checksum validation for tamper and corruption detection.
  - Support for custom filenames and multiple save slots (`quicksave.sav`, `slot1.sav`, etc.).
- **Strong Exception Safety**:
  - Comprehensive custom exception hierarchy (`SaveFileNotFoundException`, `CorruptSaveException`, `SaveVersionMismatchException`, `FileWriteException`, `InvalidGameStateDataException`).
  - Active game state rollback / preservation if a load fails halfway.
- **Extensive Test Coverage**: Automated test suites for file handling, exception handling, and puzzles.

---

## Architecture & Class Hierarchy

```
+--------------------------------------------------------------+
|                         GameManager                          |
|  - currentRoomId, score, movesCount, visitedRooms            |
|  - captureState(), restoreState(), saveGame(), loadGame()    |
+--------------+-------------------+-------------------+-------+
               |                   |                   |
               v                   v                   v
        +--------------+   +---------------+   +---------------+
        |  RoomGraph   |   |     Room      |   |SaveLoadManager|
        |  (BFS Path)  |   | (Polymorphic) |   | (Persistence) |
        +--------------+   +-------+-------+   +-------+-------+
                                   |                   |
              +--------------------+                   v
              |            |                   +---------------+
              v            v                   |   GameState   |
         EntranceRoom  LibraryRoom             |  (Snapshot)   |
          LabRoom      SecretRoom              +---------------+
          EscapeRoom
```

### Exception Hierarchy
```
std::exception
  └── std::runtime_error
        └── GameException
              ├── RoomNotFoundException
              └── SaveLoadException
                    ├── SaveFileNotFoundException
                    ├── FileWriteException
                    ├── FileReadException
                    ├── CorruptSaveException
                    ├── SaveVersionMismatchException
                    └── InvalidGameStateDataException
```

---

## Save/Load System & File Handling

The persistence layer is implemented in [`SaveLoadManager`](include/SaveLoadManager.h) and [`GameState`](include/SaveLoadManager.h).

### Save File Format (`.sav`)
The format is human-readable, deterministic, and includes an embedded checksum:

```ini
[ESCAPE_ROOM_SAVE_V1]
VERSION:1
TIMESTAMP:2026-10-25 21:00:00
CURRENT_ROOM:2
SCORE:75
MOVES_COUNT:4
[VISITED_ROOMS]
0,1,2
[ROOM_COMPLETED]
0=1
1=1
2=0
3=0
4=0
[PUZZLES_SOLVED]
1=1
2=0
[DOORS_UNLOCKED]
7=1
[CHECKSUM]
1293847291
[EOF]
```

### File-Handling Highlights:
1. **RAII (Resource Acquisition Is Initialization)**: All file streams (`std::ifstream`, `std::ofstream`) automatically close upon scope exit or error, preventing file locks.
2. **Atomic Overwrites**: Files are opened with `std::ios::trunc` to ensure old content is cleanly replaced without trailing garbage.
3. **Multiple Slots**: Supports arbitrary paths and slots (e.g. `slot1.sav`, `slot2.sav`, `autosave.sav`).
4. **Data Verification**: Embedded DJB2 checksum detects manual tampering or truncated file writes.

---

## Exception-Handling Strategy

The project implements a defensive exception handling architecture:
1. **Semantic Input Validation**: Validates all incoming data (e.g., negative room IDs, unknown rooms, negative scores).
2. **Defensive Parsing**: Incomplete files, bad headers, unsupported versions, and malformed numeric values trigger distinct, descriptive exceptions.
3. **Strong Exception Safety**: In `GameManager::restoreState`, incoming data is validated against active room registries *before* mutating game state. If an error occurs, the active session is left completely untouched.
4. **Polymorphic Catching**: Callers can catch specific errors (`SaveFileNotFoundException`) or generalized groups (`SaveLoadException`, `GameException`, `std::exception`).

---

## Unit Testing Suites

The project provides three automated test runners in `tests/`:

### 1. File-Handling Tests (`tests/file_handling_tests.cpp`)
Verifies file operations and disk I/O:
- `testSaveFileCreation`: Asserts file is created on disk with size > 0.
- `testSaveAndLoadExactRoundTrip`: Asserts that all fields of `GameState` survive serialization and loading intact (`original == loaded`).
- `testFileOverwrite`: Asserts save files are overwritten cleanly without residual data.
- `testMultipleSaveSlots`: Asserts independent coexistence of multiple save files.
- `testDeleteSaveFile`: Asserts clean deletion of save files.
- `testFileStreamClosedProperly`: Asserts file locks are freed immediately (RAII).
- `testGameManagerEndToEndPersistence`: Full game persistence integration test.

### 2. Exception-Handling Tests (`tests/exception_handling_tests.cpp`)
Verifies error detection and defensive resilience:
- `testSaveFileNotFoundException`: Confirms missing files throw `SaveFileNotFoundException`.
- `testEmptyFileException`: Confirms 0-byte files throw `CorruptSaveException`.
- `testCorruptHeaderException`: Confirms invalid headers throw `CorruptSaveException`.
- `testVersionMismatchException`: Confirms incompatible save versions throw `SaveVersionMismatchException`.
- `testTruncatedFileException`: Confirms missing `[EOF]` throws `CorruptSaveException`.
- `testMalformedNumericDataException`: Confirms invalid integer tokens throw `CorruptSaveException`.
- `testNegativeValuesException`: Confirms negative IDs or scores throw `InvalidGameStateDataException`.
- `testTamperedChecksumException`: Confirms tampered data triggers checksum failure.
- `testInvalidRoomInGameManagerRestore`: Confirms unmapped room references are rejected.
- `testStrongExceptionSafetyOnLoadFailure`: Confirms failed loads leave active game state unmodified.
- `testGameManagerPreconditionExceptions`: Confirms preconditions throw `std::invalid_argument` / `RoomNotFoundException`.
- `testPolymorphicCatching`: Confirms exception inheritance hierarchy.
- `testEmptyPathExceptions`: Confirms empty file paths throw appropriate write/read errors.

### 3. Puzzle System Tests (`tests/puzzle_tests.cpp`)
Tests puzzle answers, trimming/case normalization, and door unlocking interactions.

---

## Building & Running

### Option A: Using CMake & CTest (Recommended)
```bash
# 1. Generate build configuration
cmake -B build

# 2. Build game and test targets
cmake --build build

# 3. Run all unit test suites
ctest --test-dir build --output-on-failure

# 4. Launch main game
./build/EscapeRoom
```

### Option B: Direct Compilation with G++ / MinGW
```bash
# Run File-Handling Tests
g++ -std=c++17 -I include tests/file_handling_tests.cpp src/*.cpp -o file_tests.exe
./file_tests.exe

# Run Exception-Handling Tests
g++ -std=c++17 -I include tests/exception_handling_tests.cpp src/*.cpp -o exception_tests.exe
./exception_tests.exe

# Run Puzzle Tests
g++ -std=c++17 -I include tests/puzzle_tests.cpp src/Puzzle.cpp src/Door.cpp -o puzzle_tests.exe
./puzzle_tests.exe

# Run Main Game
g++ -std=c++17 -I include src/main.cpp src/*.cpp -o EscapeRoom.exe
./EscapeRoom.exe
```

---

## Member Contribution & Role

This module encompasses the following core deliverables:
1. **Save/Load System**:
   - Designed and implemented [`include/SaveLoadManager.h`](include/SaveLoadManager.h) and [`src/SaveLoadManager.cpp`](src/SaveLoadManager.cpp).
   - Designed serializable `GameState` snapshot, DJB2 checksum calculation, and deterministic text format.
   - Integrated state capture (`captureState`), restoration (`restoreState`), and menu options into [`include/GameManager.h`](include/GameManager.h) and [`src/GameManager.cpp`](src/GameManager.cpp).
2. **File-Handling Tests**:
   - Implemented [`tests/file_handling_tests.cpp`](tests/file_handling_tests.cpp) covering file creation, round-trip serialization, overwrites, multiple slots, RAII streams, and end-to-end integration.
3. **Exception-Handling Tests**:
   - Designed custom exception hierarchy in [`include/GameExceptions.h`](include/GameExceptions.h).
   - Implemented [`tests/exception_handling_tests.cpp`](tests/exception_handling_tests.cpp) covering missing files, corrupt headers, version mismatch, truncation, checksum tampering, strong exception safety, and polymorphic catches.
4. **Project Documentation**:
   - Authored complete project [`README.md`](README.md) and technical architecture document in [`docs/ARCHITECTURE_AND_TESTING.md`](docs/ARCHITECTURE_AND_TESTING.md).
   - Authored comprehensive submission-grade report in [`docs/PROJECT_DOCUMENTATION.md`](docs/PROJECT_DOCUMENTATION.md).
   - Configured [`CMakeLists.txt`](CMakeLists.txt) with CTest integration.

---

## Project Documentation Links

The project provides complete, publication-grade documentation:
- **Comprehensive Project Report & Verification Manual:** [`docs/PROJECT_DOCUMENTATION.md`](docs/PROJECT_DOCUMENTATION.md) — Includes requirements specification (SRS), UML class diagrams, sequence diagrams, checksum specification, complete V&V test execution matrix, and gameplay manual.
- **Technical Architecture & Format Specification:** [`docs/ARCHITECTURE_AND_TESTING.md`](docs/ARCHITECTURE_AND_TESTING.md) — In-depth architectural decomposition, RAII design, exception hierarchy details, and test coverage summaries.
