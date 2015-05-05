#include "serial.h"






serial::serial()
{
#ifdef MAVLINK
	int system_type = MAV_TYPE_FIXED_WING;
	int autopilot_type = MAV_AUTOPILOT_GENERIC;
	mavlink_message_t msg;
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	Serial1.begin(57600);

#endif
#ifdef GPS
#endif
#ifdef ACT_TELEMETRY
#endif
	
}




#ifdef MAVLINK
void serial::heartbeat(){
	mavlink_msg_heartbeat_pack(100, 200, &msg, system_type, autopilot_type, MAV_MODE_FLAG_SAFETY_ARMED,0, MAV_STATE_ACTIVE);
	uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
	Serial1.write(buf, len);
}
   /*
void serial::air_speed(){
	float airspeed = _HCLA.calc_airspeed();
	mavlink_msg_airspeed_pack(100, 200, &msg, airspeed);
	uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
	if (Serial1.write(buf, len) != len){
		// Fehlerroutine
	}
	
}
*/
#endif
	
