# Escape Room: Infinite Rooms — Architecture, Save/Load & Testing Documentation

This document provides a comprehensive technical breakdown of the **Save/Load System**, **File-Handling Architecture**, **Exception-Handling Strategy**, and **Unit Testing Suites** implemented for *Escape Room: Infinite Rooms*.

---

## 1. System Architecture Overview

The application follows clean Object-Oriented Design (OOD) principles with separation of concerns:

- **World & Exploration Layer**: `Room` abstract base class with polymorphic derived rooms (`EntranceRoom`, `LibraryRoom`, `LabRoom`, `SecretRoom`, `EscapeRoom`).
- **Graph & Navigation Layer**: `RoomGraph` maintaining adjacency lists and shortest-path BFS traversal.
- **Puzzle & Door Layer**: `Puzzle` hierarchy (`MathPuzzle`, `LogicPuzzle`, `WordPuzzle`, `SequencePuzzle`) unlocking `Door` instances.
- **Game Engine & Orchestrator**: `GameManager` coordinating player position, navigation, moves counter, score tracking, and runtime menu.
- **Persistence & Serialization Layer**: `GameState` snapshot and `SaveLoadManager` providing serialization, deserialization, checksum verification, and file I/O.
- **Robust Exception Architecture**: `GameException` hierarchy extending standard C++ exceptions (`std::runtime_error`).

```
+-------------------------------------------------------------+
|                         GameManager                         |
|   - Coordinates Game Loop, Input, Navigation & Score        |
+--------------+------------------+------------------+--------+
               |                  |                  |
               v                  v                  v
       +---------------+  +---------------+  +---------------+
       |   RoomGraph   |  |     Room*     |  | SaveLoadMgr   |
       | (Connections) |  | (Polymorphic) |  | (Persistence) |
       +---------------+  +---------------+  +-------+-------+
                                                     |
                                                     v
                                             +---------------+
                                             |   GameState   |
                                             |  (Snapshot)   |
                                             +---------------+
```

---

## 2. Save / Load Persistence System

### 2.1 Design Objectives
- **Zero External Dependencies**: Pure standard C++17 (`<fstream>`, `<sstream>`, `<map>`, `<vector>`).
- **Deterministic Round-Trip**: Every persistent attribute is serialized and restored with 100% fidelity.
- **Tamper & Corruption Resilience**: Embedded checksums and strict structure verification detect any manual file manipulation or incomplete writes.
- **Version Compatibility**: Version headers guard against schema mismatches across engine revisions.

### 2.2 Game State Representation (`GameState`)
`GameState` captures the complete runtime snapshot:
- `int version`: Format specification version (currently `1`).
- `std::string timestamp`: Date/time or label when the state was saved.
- `int currentRoomId`: Identifier of the player's active room.
- `int score`: Player's accumulated puzzle and room points.
- `int movesCount`: Number of transitions completed.
- `std::vector<int> visitedRooms`: Path history of rooms explored.
- `std::map<int, bool> roomCompleted`: Completion flags for each room.
- `std::map<int, bool> puzzleSolved`: Solved flags for each puzzle.
- `std::map<int, bool> doorUnlocked`: Unlock status for each door.

### 2.3 Save File Format Specification

Save files (typically `.sav`) follow a structured, human-readable section layout:

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

#### Parsing & Validation Rules:
1. **Magic Header**: Must begin with `[ESCAPE_ROOM_SAVE_V1]`. Any deviation immediately throws `CorruptSaveException`.
2. **Version Tag**: Parsed and checked against `CURRENT_SAVE_VERSION`. Incompatible versions throw `SaveVersionMismatchException`.
3. **Data Integrity Checksum**: Computed over the exact payload using the DJB2 algorithm (ignoring carriage returns `\r` to support cross-platform line-endings). If the calculated checksum does not match `[CHECKSUM]`, a `CorruptSaveException` is thrown.
4. **EOF Verification**: Must terminate with the `[EOF]` marker. Truncated files missing this marker throw `CorruptSaveException`.
5. **Semantic Constraints**: Values are validated (`currentRoomId >= 0`, `score >= 0`, `moves >= 0`). Violations throw `InvalidGameStateDataException`.

---

## 3. Exception-Handling Architecture

### 3.1 Class Hierarchy
All game-specific exceptions derive from `GameException`, which inherits from `std::runtime_error`:

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

### 3.2 Exception Classes Summary

| Exception Class | Inherits From | Thrown When | Key Methods / Attributes |
|---|---|---|---|
| `GameException` | `std::runtime_error` | Base class for general game errors | `what()` |
| `SaveLoadException` | `GameException` | Base class for file and persistence errors | `getFilename()`, `what()` |
| `SaveFileNotFoundException` | `SaveLoadException` | Attempting to load from a non-existent file path | `getFilename()` |
| `FileWriteException` | `SaveLoadException` | Disk write fails, path is read-only, or permissions denied | `getFilename()` |
| `FileReadException` | `SaveLoadException` | Stream read fails unexpectedly | `getFilename()` |
| `CorruptSaveException` | `SaveLoadException` | File is empty, header invalid, truncated, or checksum fails | Reason string |
| `SaveVersionMismatchException` | `SaveLoadException` | File version does not match engine version | `getFileVersion()`, `getExpectedVersion()` |
| `InvalidGameStateDataException` | `SaveLoadException` | Stored data violates logic (negative ID, unmapped room) | Detail string |
| `RoomNotFoundException` | `GameException` | Requested room ID does not exist in the graph/manager | `getRoomId()` |

### 3.3 Strong Exception Safety Guarantee
When restoring state via `GameManager::restoreState(const GameState& state)`:
1. The incoming state is first semantically validated.
2. The current room and all referenced room IDs are confirmed to exist in the target `GameManager` instance.
3. If **any** validation fails, an exception is raised **before** mutating any internal fields of `GameManager`.
4. As a result, the active game session remains in its last known good state without half-applied corruptions.

---

## 4. File-Handling Architecture & RAII

- **Resource Acquisition Is Initialization (RAII)**: All file operations utilize `std::ifstream` and `std::ofstream`. File handles automatically close upon scope exit, even if an exception is thrown mid-parsing.
- **Atomic-Style Overwriting**: Save files are opened with `std::ios::trunc` to ensure old data is cleanly overwritten rather than appended.
- **Multiple Save Slots**: The system natively supports arbitrary file paths (e.g. `slot1.sav`, `quicksave.sav`, `backup.sav`).
- **File Utilities**: `SaveLoadManager::fileExists` and `SaveLoadManager::deleteSaveFile` provide non-throwing filesystem utility operations.

---

## 5. Testing Suites & Coverage

Three dedicated test binaries validate the engine:

### 5.1 File-Handling Tests (`tests/file_handling_tests.cpp`)
Validates physical file creation, reading, writing, and cleanup:
1. `testSaveFileCreation`: Asserts that saving creates a physical file on disk with size > 0.
2. `testSaveAndLoadExactRoundTrip`: Asserts that all fields of a complex state survive serialization and deserialization intact (`original == loaded`).
3. `testFileOverwrite`: Asserts that writing twice to the same filename completely replaces previous contents without residual data.
4. `testMultipleSaveSlots`: Asserts that multiple distinct slot files (`slot1.sav`, `slot2.sav`, `slot3.sav`) can coexist independently.
5. `testDeleteSaveFile`: Asserts clean deletion via `SaveLoadManager::deleteSaveFile`.
6. `testFileStreamClosedProperly`: Asserts that file locks are released immediately upon completion of save/load operations.
7. `testGameManagerEndToEndPersistence`: Full game integration test (moves player, updates score, sets room completed, saves, loads into fresh game, checks equivalence).

### 5.2 Exception-Handling Tests (`tests/exception_handling_tests.cpp`)
Validates defensive error detection and exception safety:
1. `testSaveFileNotFoundException`: Confirms loading a missing file throws `SaveFileNotFoundException` with correct path in message.
2. `testEmptyFileException`: Confirms loading a 0-byte file throws `CorruptSaveException`.
3. `testCorruptHeaderException`: Confirms invalid headers throw `CorruptSaveException`.
4. `testVersionMismatchException`: Confirms future versions (e.g., version 99) throw `SaveVersionMismatchException`.
5. `testTruncatedFileException`: Confirms files missing `[EOF]` throw `CorruptSaveException`.
6. `testMalformedNumericDataException`: Confirms non-integer tokens for integer fields throw `CorruptSaveException`.
7. `testNegativeValuesException`: Confirms negative room IDs, negative scores, or negative move counts throw `InvalidGameStateDataException`.
8. `testTamperedChecksumException`: Confirms that modifying data without recalculating checksum throws `CorruptSaveException`.
9. `testInvalidRoomInGameManagerRestore`: Confirms loading a save referencing an unregistered room throws `InvalidGameStateDataException`.
10. `testStrongExceptionSafetyOnLoadFailure`: Confirms that failed loads do NOT alter the existing `GameManager` state.
11. `testGameManagerPreconditionExceptions`: Confirms `addRoom(nullptr)`, duplicate rooms, and invalid room connections throw `std::invalid_argument`.
12. `testPolymorphicCatching`: Confirms exceptions can be caught polymorphically via `const SaveLoadException&`, `const GameException&`, and `const std::exception&`.
13. `testEmptyPathExceptions`: Confirms empty file paths throw `FileWriteException` / `SaveFileNotFoundException`.

### 5.3 Puzzle System Tests (`tests/puzzle_tests.cpp`)
Validates puzzle answer normalization, puzzle solving states, and door unlocking interactions.

---

## 6. How to Build and Run

### 6.1 Using CMake & CTest
```bash
# Generate build configuration
cmake -B build

# Build all targets
cmake --build build

# Run all test suites
ctest --test-dir build --output-on-failure
```

### 6.2 Direct Compilation (MinGW / GCC)
```bash
# Compile and run File-Handling Tests
g++ -std=c++17 -I include tests/file_handling_tests.cpp src/*.cpp -o file_tests.exe
./file_tests.exe

# Compile and run Exception-Handling Tests
g++ -std=c++17 -I include tests/exception_handling_tests.cpp src/*.cpp -o exception_tests.exe
./exception_tests.exe

# Compile and run Puzzle Tests
g++ -std=c++17 -I include tests/puzzle_tests.cpp src/Puzzle.cpp src/Door.cpp -o puzzle_tests.exe
./puzzle_tests.exe

# Compile and run the Main Game
g++ -std=c++17 -I include src/main.cpp src/*.cpp -o EscapeRoom.exe
./EscapeRoom.exe
```
