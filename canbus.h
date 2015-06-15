#ifndef CANBUS_H
#define CANBUS_H

#include "FlexCAN/FlexCAN.h"

/*
Klasse f�r die CAN-BUS Kommunikation
Hier wird die CAN-BUS Funktionalit�t und die Logik f�r Master und Messmodule implemnetiert
*/

#include "FlexCAN/FlexCAN.h"

class canbus
{
public:

	canbus();

	void write(CAN_message_t msg);
	bool read(CAN_message_t msg);
	FlexCAN can;
	CAN_message_t broadcast; //Startbefehl f�r die Messmodule
	CAN_message_t errorlog;
};

#endif // !CANBUS_H