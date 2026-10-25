#ifndef ESCAPE_ROOM_H
#define ESCAPE_ROOM_H

#include "Room.h"

class EscapeRoom : public Room {
public:
EscapeRoom();
void enterRoom() override;
};

#endif