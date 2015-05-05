
#include "config.h"

#ifdef MAVLINK
#include "mavlink2/minimal/mavlink.h"
#include "serial.h"
serial mav;

#endif

#ifdef SD_LOG
#include "SdFat\SdFat.h"
#endif

#ifdef CANBUS
#include "FlexCAN\FlexCAN.h"
#include "canbus.h"
canbus cbus;
CAN_message_t c_msg;
CAN_message_t c_msg2;
#endif

#ifdef HCLA
#include "i2c_t3\i2c_t3.h"
#include "sensor.h"


sensor hcla;
#endif

void setup(){

	Serial.begin(115200);
	Serial.println("Serial online");
	pinMode(13, OUTPUT);
	//IntervalTimer messTimer;
	//messTimer.begin(measureHCLA, 10);












}


void loop(){
#ifdef MAVLINK
	digitalWrite(13, HIGH);
	int time1 = micros();
	mav.heartbeat();
	int time2 = micros();
	//mav.air_speed();
	Serial.println("Heartbeat");
	Serial.println(time2 - time1);
	digitalWrite(13, LOW);
	delay(2000);
	
#endif


#ifdef MASTER
	cbus.write(cbus.broadcast);

#endif




#ifdef MESSMODUL
	cbus.read(c_msg);
	c_msg.id = MESSMODUL_ID;
	c_msg2.id = MESSMODUL_ID;
	if (c_msg.id == cbus.broadcast.id  && c_msg.buf == cbus.broadcast.buf){
		for (int i = 0; i < sizeof(hcla.channels); i++){
			uint16_t tmp = hcla.readHCLA(hcla.channels[i]);
			if (i < 4){
				c_msg.buf[0 + i * 2] = tmp << 8;
				c_msg.buf[1 + i * 2] = tmp;
			}
			else{
				c_msg2.buf[-8 + i * 2] = tmp << 8;
				c_msg2.buf[-7 + i * 2] = tmp;
			}
		}
		c_msg.len = sizeof(c_msg.buf);
		cbus.write(c_msg);
		if (sizeof(c_msg2) != 0){
			cbus.write(c_msg2);
		}

	}

#endif
}

