#ifndef LIBRARY_ROOM_H
#define LIBRARY_ROOM_H

#include "Room.h"

class LibraryRoom : public Room {
public:
LibraryRoom();
void enterRoom() override;
};

#endif