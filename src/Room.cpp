#include "Room.h"

Room::Room(int id, const string& name, const string& description) {
    this->id = id;
    this->name = name;
    this->description = description;
    this->completed = false;
}

Room::~Room() {
}

void Room::displayInfo() const {
    cout << "\n====================================\n";
    cout << "Room ID: " << id << endl;
    cout << "Room Name: " << name << endl;
    cout << "Description: " << description << endl;
    cout << "Status: " << (completed ? "Completed" : "Not Completed") << endl;
    cout << "====================================\n";
}

int Room::getId() const {
    return id;
}

string Room::getName() const {
    return name;
}

string Room::getDescription() const {
    return description;
}

bool Room::isCompleted() const {
    return completed;
}

void Room::markCompleted() {
    completed = true;
}

void Room::setCompleted(bool status) {
    completed = status;
}
