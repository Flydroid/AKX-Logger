#include "sdcard.h"


sdcard::sdcard(String datetime){
	SD.begin(SS_PIN, SPI_FULL_SPEED);
	char fileName[11] = BASE_FILENAME;
	//fileName[0] = datetime.date;
	//+datetime.time + BASE_FILENAME;

	if (!SD_File.open(fileName, O_WRITE | O_APPEND | O_CREAT)){
		//error handling
	}
}


sdcard::~sdcard()
{							    
}


void writeBytes(uint8_t &bytearray, int n_byte){}



void sdcard::write(uint8_t &module_data, int n_byte){

	for (int x = 0; x < 8; x++){
		_log_buffer[_sel_buf].data[_log_buffer[_sel_buf].count] = module_data[i].data1[x + _log_buffer[_sel_buf].count];
		_log_buffer[_sel_buf].count++;
		if (_log_buffer[_sel_buf].count == 512){
			SD_File.write(_log_buffer[_sel_buf].data, 512);
			_log_buffer[_sel_buf].count = 0;
			if (_sel_buf == 0){
				_sel_buf = 1;
			}
			else{
				_sel_buf = 0;
			}
		}
	}
}