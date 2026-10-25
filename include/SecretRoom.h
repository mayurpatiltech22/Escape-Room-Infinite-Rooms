#ifndef SECRET_ROOM_H
#define SECRET_ROOM_H

#include "Room.h"

class SecretRoom : public Room {
public:
SecretRoom();
void enterRoom() override;
};

#endif