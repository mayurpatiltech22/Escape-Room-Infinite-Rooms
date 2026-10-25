#include "LabRoom.h"

LabRoom::LabRoom()
: Room(2, "Laboratory",
"A strange laboratory containing mysterious equipment.") {
}

void LabRoom::enterRoom() {
cout << "\nYou entered the Laboratory.\n";
cout << "Machines hum quietly. Something is hidden here.\n";
}
