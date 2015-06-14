

#include "config.h"


#ifdef MAVLINK
#include "mavlink.h"
#include "serial.h"

serial mav;

#endif

#ifdef SDCARD
#include <SdFat.h>
#endif // SDCARD



#ifdef GPS
#include "TinyGPSPlus/TinyGPS++.h"
TinyGPSPlus gps;
#endif

#include "FlexCAN.h"
#include "canbus.h"
canbus cbus;
CAN_message_t c_msg;
CAN_message_t c_msg2;

#ifdef HCLA
#include "i2c_t3.h"
#include "sensor.h"

sensor hcla;
#endif

#ifdef MASTER

//struct with all information on every connected module
struct MODULES{
	uint8_t ID;			      //Module ID
	uint8_t	ID_EXT = 0x00;	  // different than 0x00 if more than 4 Sensors
	uint8_t SYSTEM_STATE;
	String NAME;			  //Name of the Module
	bool READ_STATE = 0;	  //If the Module has been read for this cycle
	uint8_t data1[8];
	uint8_t data2[8];
	uint8_t SENSOR_NUMBER;	   //Numbers of sensors
	uint8_t SENSOR_OR;	     //Which of the 8 possible Sensors are active
}tmp;
MODULES *module_data;

//Define timer and volatiles timing control
IntervalTimer broadcast;
IntervalTimer telemetry;
volatile bool c_broadcast;
volatile bool c_telemetry;

#endif

#ifdef MESSMODUL
struct SYSTEM{
	uint8_t POWER_UP = 0;
	uint8_t READY = 1;
	uint8_t ERROR = 2;
} SYSTEM_STATE;

#endif

//Interrupt function
void measureHCLA(){
}

void send_broadcast(){
	c_broadcast = true;
}

void send_telemetry(){
	c_telemetry = true;
}

void setup(){
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Serial online");
#endif

#ifdef MASTER

	/*
	Master boot up
	*/

	//get Date and Time
	//gps.encode(SERIAL_GPS.read());
	//struct datetime {
	//	char date;
	//	char time;
	//} datetime;
	//datetime.date = gps.date.value();
	//datetime.time = gps.time.value();

	

	broadcast.begin(send_broadcast, 10000);
	telemetry.begin(send_telemetry, 1000000);

	//Bootup end

	int m_num = 0;
	char m_tmp[8];
	elapsedMillis m_checkin;

	/*	reads the incoming check-in massages from the different modules
	Every module gets registered with its ids, state and name
	an array of structs is created which holds the information.
	if during 1000 milliseconds no module checks in, all modules are accounted for
	--->>> Check during testing
	*/
	while (m_checkin < 1000){
		CAN_message_t msg;
		if (!cbus.read(msg)){
			continue;
		}

		module_data = new MODULES;
		module_data[m_num].ID = msg.buf[0];
		module_data[m_num].ID_EXT = msg.buf[1];
		module_data[m_num].SYSTEM_STATE = msg.buf[2];
		module_data[m_num].SENSOR_NUMBER = msg.buf[3];
		cbus.read(c_msg2);
		for (int i = 0; i < 8; i++)
		{
			m_tmp[i] = c_msg2.buf[i];
		}
		module_data[m_num].NAME = String(m_tmp);
		m_checkin = 0;
		m_num++;
	}

	//sort the check-in modules according their id' value
	int max = 5;
	bool change;
	for (int t = 0; t < max; t++)  {
		for (int i = 0; i < sizeof(module_data); i++){
			if (module_data[i].ID < module_data[i + 1].ID){
				tmp = module_data[i];
				module_data[i] = module_data[i + 1];
				module_data[i + 1] = tmp;
				change = 1;
			}
		}
		if (!change){
			break;
		}
		change = 0;
	}

	//shift all elements in module_data by one, because master is module_data[0x00]
	for (int i = 0; i < sizeof(module_data); i++){
		tmp = module_data[i + 1];
		module_data[i + 1] = module_data[i];
		module_data[i + 2] = tmp;
	}

	//Write Master sensor data to module_date[0x00]
	module_data[MASTER_ID].ID = MASTER_ID;
	module_data[MASTER_ID].NAME = MASTER_NAME;
	module_data[MASTER_ID].SENSOR_NUMBER = sizeof(hcla.channels);

	//Start display, show module name and data

	/*Write FileHeader

	*/
#ifdef SDCARD

	
	SD_File.print("AK-X Logger v.1\nLogdatei vom: ");
	//SD_File.print(datetime.date);
	SD_File.print(" ");
	//SD_File.print(datetime.time);
	SD_File.print("\nEingeckete Module\t");
	SD_File.print(sizeof(hcla.channels));
	SD_File.print("\n");
	for (int i = 1; i < sizeof(module_data); i++){
		SD_File.print(module_data[i].NAME);								  
		SD_File.print("\t");
		SD_File.print(module_data[i].ID);
		SD_File.print("\tSensoren\t");
		SD_File.print(module_data[i].SENSOR_NUMBER);
		SD_File.print("\n");
	}
	SD_File.print("Binary Date:\n");
	//Close File
	SD_File.close();

#endif // SDCARD

#endif
	
#ifdef MESSMODUL

	//wait for master to boot
	delay(5000);
	//announce module to master
	c_msg.id = MESSMODUL_ID;
	c_msg.buf[0] = MESSMODUL_ID;
	c_msg.buf[1] = MESSMODUL_ID_EXT;
	c_msg.buf[2] = SYSTEM_STATE.POWER_UP;
	c_msg.buf[3] = sizeof(hcla.channels);
	//c_msg.buf[4] =
	c_msg.len = sizeof(c_msg.buf);
	cbus.write(c_msg);
	c_msg2.id = MESSMODUL_ID_EXT;
	for (int i = 0; i < 8; i++){
		c_msg2.buf[i] = int(MOD_NAME[i]);
	}
	c_msg2.len = 8;
	cbus.write(c_msg2);

	IntervalTimer messTimer;
	messTimer.begin(measureHCLA, 10);

#endif
}

void loop(){
#ifdef MASTER

	//Read sensor, send broadcast, get data from Modules and Store it.
	if (c_broadcast){
		//send broadcast
		cbus.write(cbus.broadcast);
		//get Time for data synchonisation
		uint32_t time = millis();		

		//read the master sensors
		uint16_t s_buf[sizeof(hcla.channels)];
		for (int i = 0; i < sizeof(hcla.channels); i++){
			uint16_t tmp = hcla.readHCLA(hcla.channels[i]);
			s_buf[0 + i * 2] = tmp << 8;
			s_buf[1 + i * 2] = tmp;
		}

		//read measurement modules
		for (int recv_data = 1; recv_data < sizeof(module_data); recv_data++){
			CAN_message_t msg, msg2;
			cbus.read(msg);
			//if READ_STATE is true
			if (module_data[msg.id].READ_STATE){
				continue;
			}
			if (msg.id == module_data[msg.id].ID){
				for (int i = 0; i < 8; i++){
					module_data[msg.id].data1[i] = msg.buf[i];
				}
			}
			else{
				// error: wrong first module massage
			}

			if (module_data[msg.id].ID_EXT != 0x00){
				cbus.read(msg2);
				if (msg2.id == module_data[msg.id].ID_EXT){
					for (int i = 0; i < 8; i++){
						module_data[msg.id].data2[i] = msg.buf[i];
					}
				}
				else{
					//error wrong second module massage
				}
			}
			module_data[msg.id].READ_STATE = 1;
		}

		//Store Timestamp
		log_buffer[sel_buf].data[log_buffer[sel_buf].count] = 0x09; // \t
		log_buffer[sel_buf].count++;
		for (int i = 0; i < 4; i++){
			log_buffer[sel_buf].data[log_buffer[sel_buf].count] = time >> 24-i*8;
			log_buffer[sel_buf].count++;
		}
		log_buffer[sel_buf].data[log_buffer[sel_buf].count] = 0x09;	// \t
		log_buffer[sel_buf].count++;
		
		//Store data
		for (int i = 1; i < sizeof(module_data); i++){
			if (module_data[i].SENSOR_NUMBER >4){
				for (int x = 0; x < 8; x++){
					log_buffer[sel_buf].data[log_buffer[sel_buf].count] = module_data[i].data1[x + log_buffer[sel_buf].count];
					log_buffer[sel_buf].count++;
					if (log_buffer[sel_buf].count == 512){
						SD_File.write(log_buffer[sel_buf].data, 512);
						log_buffer[sel_buf].count = 0;
						if (sel_buf == 0){
							sel_buf = 1;
						}
						else{
							sel_buf = 0;
						}
					}
				}
			}
			else{
				for (int x = 0; x < module_data[i].SENSOR_NUMBER; x++){
					log_buffer[sel_buf].data[log_buffer[sel_buf].count] = module_data[i].data1[x + log_buffer[sel_buf].count];
					log_buffer[sel_buf].count++;
					if (log_buffer[sel_buf].count == 512){
						log_buffer[sel_buf].count = 0;
						SD_File.write(log_buffer[sel_buf].data, 512);
						if (sel_buf == 0){
							sel_buf = 1;
						}
						else{
							sel_buf = 0;
						}
					}
				}
			}
			if (module_data[i].ID_EXT != 0){
				for (int x = 0; x < module_data[i].SENSOR_NUMBER - 4; x++){
					log_buffer[sel_buf].data[log_buffer[sel_buf].count] = module_data[i].data2[x + log_buffer[sel_buf].count];
					log_buffer[sel_buf].count++;
					if (log_buffer[sel_buf].count == 512){
						SD_File.write(log_buffer[sel_buf].data, 512);
						log_buffer[sel_buf].count = 0;
						if (sel_buf == 0){
							sel_buf = 1;
						}
						else{
							sel_buf = 0;
						}
					}
				}
			}
		}

		noInterrupts();
		c_broadcast = false;
		interrupts();
	}

	if (c_telemetry){
		/*
		calculate Telemetry data
		*Airspeed
		*angel of attack
		*slipangle
		*GPS
		*lift
		send Telemetry data
		*/
		//Send heartbeat
#ifdef MAVLINK
		mav.heartbeat();
#endif

		noInterrupts();
		c_telemetry = false;
		interrupts();
	}

#endif

#ifdef MESSMODUL

	if (cbus.available()){
		cbus.read(c_msg);
		if (c_msg.id == MASTER_ID){
			switch (c_msg.buf[0]){
			case 1:
				c_msg.id = MESSMODUL_ID;
				c_msg2.id = MESSMODUL_ID_EXT;
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
					c_msg2.len = sizeof(c_msg2.buf);
					cbus.write(c_msg2);
				}
			case 2:
				// Send errorlog
			default:
				break;
			}
		}
	}
#endif

	//Loop end
}