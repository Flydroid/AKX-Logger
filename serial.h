#ifndef SERIAL_H
#define SERIAL_H

/*
Klasse f�r die Serialkommunikation mit PC, Funkmodul und Telemetrie
sowie die Implemntierug der Weitergabe von Fehlermeldungen
*/

#include "config.h"
#include "sensor.h"



class serial
{
public:

	serial();

	void heartbeat();
	void air_speed();

private:
	int system_type;
	int autopilot_type;
#ifdef MAVLINK
	mavlink_message_t msg;
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
#endif
};

#endif // !SERIAL_H
