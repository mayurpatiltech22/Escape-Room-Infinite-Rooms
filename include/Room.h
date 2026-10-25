#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <string>

using namespace std;

class Room {
protected:
    int id;
    string name;
    string description;
    bool completed;

public:
    // Constructor
    Room(int id, const string& name, const string& description);

    // Virtual destructor for inheritance
    virtual ~Room();

    // Every room must implement its own entry behavior
    virtual void enterRoom() = 0;

    // Display room information
    virtual void displayInfo() const;

    // Getter functions
    int getId() const;
    string getName() const;
    string getDescription() const;
    bool isCompleted() const;

    // Mark room as completed / set status
    void markCompleted();
    void setCompleted(bool status);
};

#endif
