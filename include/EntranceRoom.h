#ifndef ENTRANCE_ROOM_H
#define ENTRANCE_ROOM_H

#include "Room.h"

class EntranceRoom : public Room {
public:
EntranceRoom();
void enterRoom() override;
};

#endif