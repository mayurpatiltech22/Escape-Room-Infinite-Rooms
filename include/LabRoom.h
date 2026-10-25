#ifndef LAB_ROOM_H
#define LAB_ROOM_H

#include "Room.h"

class LabRoom : public Room {
public:
LabRoom();
void enterRoom() override;
};

#endif
