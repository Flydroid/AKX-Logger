#include "sensor.h"



//counting var for generating the channel array
int ch_stat=0;


sensor::sensor(){
#ifdef HCLA
Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);

#ifdef SENSOR0
sensor::setCh(SENSOR0);
#endif 
#ifdef SENSOR1
sensor::setCh(SENSOR1);
#endif  
#ifdef SENSOR2
sensor::setCh(SENSOR2);
#endif // SENSOR2

#ifdef SENSOR3
sensor::setCh(SENSOR3);
#endif 
#ifdef SENSOR4
sensor::setCh(SENSOR4);
#endif 
#ifdef SENSOR5
sensor::setCh(SENSOR5);
#endif
#ifdef SENSOR6
sensor::setCh(SENSOR6);
#endif
#ifdef SENSOR7
sensor::setCh(SENSOR7);
#endif





#endif

#ifdef MPU9150

#endif

}


uint16_t sensor::readHCLA(int channel){
	sensor::SelectChannel(channel);
	
	uint8_t msb, lsb;
	uint16_t rawPressure;
	Wire.requestFrom(PRESSURE_SENSOR_ADRESS, 2, I2C_STOP);
	msb = Wire.readByte();
	lsb = Wire.readByte();
	if (Wire.getError() == 0){
		return rawPressure = (int)(msb << 8) | lsb;
	}
	else{
	//Error Funktion
	}
}


//channel select f�r 8 Kan�le
void sensor::SelectChannel(int channel){

	int controlPin[] = { s0, s1, s2};

	int muxChannel[8][3] = {
		{ 0, 0, 0 }, //channel 0
		{ 1, 0, 0 }, //channel 1
		{ 0, 1, 0}, //channel 2
		{ 1, 1, 0}, //channel 3
		{ 0, 0, 1}, //channel 4
		{ 1, 0, 1}, //channel 5
		{ 0, 1, 1}, //channel 6
		{ 1, 1, 1}, //channel 7
	};

	//loop through the 3 controllpins
	for (int i = 0; i < 3; i++){
		digitalWrite(controlPin[i], muxChannel[channel][i]);
	}
}

/*
float sensor::calc_airspeed(){
	uint16_t temp = readHCLA(CH_AIRSPEED);
	float airspeed;
	//temp*x; // Airspeed calculation	
	 
	return airspeed;
}
*/
 //generates an array which contains the sensor channel numbers
 void sensor::setCh(int chnum){ 
	channels  = new int;
	channels[ch_stat] = chnum;
	ch_stat++;
 }					