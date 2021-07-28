#ifndef CO2-SENSE
#define CO2-SENSE

#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "MHZ19.h"   

class Co2Sense{
	private:
	
	public: 
	static int getByte(uint32_t x, int n);
	
	struct Colors{
	   uint32_t green = 0x00ff00;
 	   uint32_t yellow = 0xff8000;
   	   uint32_t orange= 0xff4500;
   	   uint32_t red = 0xff0000;
       uint32_t blue = 0x0000ff;
	};
	
	class ledRing{
		private:
   		int8_t pin_led;
   		int8_t led_num;
  		 uint32_t color;
 		 Adafruit_NeoPixel strip;
 		 public:
 			 unsigned int brightness;
			 ledRing(int8_t pin_led, int8_t led_num, unsigned int brightness);
			 void begin();
			 void ledRing::set_color(uint32_t color, unsigned int brightness);
		   	void ledRing::set_brightness(unsigned int brightness);
		void ledRing::breathe(uint32_t color,unsigned int brightness, unsigned int wait);
	};
	class SerialInterface{
		 private:
   			byte RX;
 		    byte TX;
    		SoftwareSerial port = SoftwareSerial(NULL,NULL);
  
  		public:
    		SerialInterface(byte RX, byte TX);
			void begin(unsigned int baud);
			void SerialInterface::listen();
			void send(char* input);
			bool available();
			char * read();
	};
	class Co2Sensor{
		private:
  			SoftwareSerial CO2_SERIAL = SoftwareSerial(NULL,NULL);
  			MHZ19 myMHZ19; // Constructor for CO2 sensor library

 		 public:
			bool autocalibration = false;
   			Co2Sensor(uint8_t rx, uint8_t tx);
			void begin();
			void listen();
			int read_co2();
		    float read_temperature();
			int get_background();
			void calibrate();
	};
	
	static void CommandListener(SerialInterface &  inter, ledRing & myLEDs, Co2Sensor & mySensor);
	static void co2_control2(Co2Sensor & mySensor, ledRing & myLEDs, SerialInterface & mySI);
	
};

#endif // CO2-SENSE