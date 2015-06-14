#ifndef SDCARD_H
#define SDCARD_H
#include "config.h"
#include "SdFat.h"

class sdcard
{
public:
	sdcard(datetime *datetime);
	~sdcard();

	SdFat SD;
	SdFile SD_File;



	//Buffer for L
	struct buffer_t{
		uint16_t count;
		uint8_t data[512];
	};
	//variable for buffer selection
	



	void write(uint8_t &module_datadata, int n_byte);


private:
	buffer_t _log_buffer[2];
	uint8_t _sel_buf = 0;


};

#endif 