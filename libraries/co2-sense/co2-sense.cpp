#include <Arduino.h>
#include <co2-sense.h>

void (* resetMCU) (void) = 0;

static int Co2Sense::getByte(uint32_t x, int n)
{
    return (x >> (n << 3)) & 0xFF;
}

   
  Co2Sense::ledRing::ledRing(int8_t pin_led, int8_t led_num, unsigned int brightness) {
    this->pin_led = pin_led;
    this->led_num = led_num;
    this->brightness = brightness;
    this->strip = Adafruit_NeoPixel(this->led_num,this->pin_led,NEO_GRB+NEO_KHZ800);
  }
  
  void Co2Sense::ledRing::begin(){
    Co2Sense::ledRing::strip.begin();
  }

  void Co2Sense::ledRing::set_color(uint32_t color, unsigned int brightness){
    Serial.println(color,HEX);
    this->color = color;
    this->brightness = ((brightness == 0) ? this->brightness : brightness);
     uint8_t red = Co2Sense::getByte(color,2);
      uint8_t green = Co2Sense::getByte(color,1);
      uint8_t blue = Co2Sense::getByte(color,0);
      Serial.println("red: "+String(red) +" green: "+String(green)+" blue: "+String(blue));
      red = (uint8_t) (float) red*(float)this->brightness/((float)255);
      green = (uint8_t) (float) green*(float)this->brightness/((float)255);
      blue = (uint8_t) (float) blue*(float)this->brightness/((float)255);
      color = strip.Color(red,green,blue);
      Serial.println(String("Converted: ")+"red: "+String(red) +" green: "+String(green)+" blue: "+String(blue));
    for(uint8_t i = 0; i<led_num;i++){
      strip.setPixelColor(i,color);
    }
      strip.show();
  }

  void Co2Sense::ledRing::set_brightness(unsigned int brightness) {
    brightness = brightness % 256;
    this->brightness = brightness;
    
    for(uint8_t i = 0; i<led_num;i++){
      uint32_t color = this->color;
      uint8_t red = Co2Sense::getByte(color,2);
      uint8_t green = Co2Sense::getByte(color,1);
      uint8_t blue = Co2Sense::getByte(color,0);
      red = (uint8_t) (float) red*(float)brightness/((float)255);
      green = (uint8_t) (float) green*(float)brightness/((float)255);
      blue = (uint8_t) (float) blue*(float)brightness/((float)255);
      color = strip.Color(red,green,blue);
      strip.setPixelColor(i,color);
    }
      strip.show();
  }

  void Co2Sense::ledRing::breathe(uint32_t color,unsigned int brightness, unsigned int wait) {
  /*
   * Breathe implementation
   */
   this->color = color;
   this->brightness = ((brightness == 0) ? this->brightness : brightness);
  uint8_t i = 0;
  unsigned long time_now = 0;
  for(i; i < this->brightness; i++) {
    for(uint8_t j=0; j<led_num; j++){
      uint8_t red = Co2Sense::getByte(color,2);
      uint8_t green = Co2Sense::getByte(color,1);
      uint8_t blue = Co2Sense::getByte(color,0);
      red = (uint8_t) (float) red*(float)i/((float)255);
      green = (uint8_t) (float) green*(float)i/((float)255);
      blue = (uint8_t) (float) blue*(float)i/((float)255);
      uint32_t color_changed = strip.Color(red,green,blue);   

      strip.setPixelColor(j,color_changed);
    }
    strip.show();
    time_now = millis();
    while(millis()< time_now + wait/this->brightness) {
      
    }
  }
   for(i; i>0; i--) {
    for(uint8_t j=0; j<led_num; j++){
      uint8_t red = Co2Sense::getByte(color,2);
      uint8_t green = Co2Sense::getByte(color,1);
      uint8_t blue = Co2Sense::getByte(color,0);
      red = (uint8_t) (float) red*(float)i/((float)255);
      green = (uint8_t) (float) green*(float)i/((float)255);
      blue = (uint8_t) (float) blue*(float)i/((float)255);
      uint32_t color_changed = strip.Color(red,green,blue);
      strip.setPixelColor(j,color_changed);
    }
    strip.show();
    time_now = millis();
    while(millis()< time_now + wait/this->brightness) {
      
    }
  }
  strip.clear();
}
  

  
    Co2Sense::SerialInterface::SerialInterface(byte RX, byte TX) {
      this->RX = RX;
      this-> TX = TX;
     this->port = SoftwareSerial(this->RX,this->TX);
    }
  
    void Co2Sense::SerialInterface::begin(unsigned int baud) {
      port.begin(baud);
    }

    void Co2Sense::SerialInterface::listen(){
      port.listen();
    }

    void Co2Sense::SerialInterface::send(char* input) {
      port.println((char*)input);
    }

    bool Co2Sense::SerialInterface::available(){
      if(port.available()>0){
        return true;
      }
      else{
        return false;
      }
    }

      char * Co2Sense::SerialInterface::read() {
      const unsigned int MAX_INPUT = 50;
      static char input_line[MAX_INPUT];
      static unsigned int input_pos = 0;
      while(port.available()>0){
      //const byte inByte = port.read();
      const byte inByte = port.read(); //DEBUG with serial monitor

      switch (inByte) {

        case '\n': //end of incoming line
          input_line[input_pos] = '\0'; //terminating the null byte
          input_pos = 0;
         // Serial.println(input_line);
          return (char*) input_line;

         case '\r': //we want to discard carriage return
         break;

         default:
         if(input_pos < (MAX_INPUT-1)) {
            input_line[input_pos++] = inByte;
            break;
         }
      }
    }
      return (char*) "NULL";
    }
  

  
    Co2Sense::Co2Sensor::Co2Sensor(uint8_t rx, uint8_t tx){
      CO2_SERIAL = SoftwareSerial(rx,tx);
    }

    void Co2Sense::Co2Sensor::begin(){
      CO2_SERIAL.begin(9600);                           
      myMHZ19.begin(CO2_SERIAL);  
      myMHZ19.autoCalibration(this->autocalibration); 
    }

    void Co2Sense::Co2Sensor::listen(){
      CO2_SERIAL.listen();
    }
    
    int Co2Sense::Co2Sensor::read_co2(){
      return this->myMHZ19.getCO2();
    }

    float Co2Sense::Co2Sensor::read_temperature(){
      return this->myMHZ19.getTemperature();
    }

    int Co2Sense::Co2Sensor::get_background(){
      return this->myMHZ19.getBackgroundCO2();
    }

    void Co2Sense::Co2Sensor::calibrate(){
      myMHZ19.calibrate();
      Serial.println("Sensor calibrated. Resetting MCU");
      resetMCU();
    }


static void Co2Sense::CommandListener(Co2Sense::SerialInterface &  inter, Co2Sense::ledRing & myLEDs, Co2Sense::Co2Sensor & mySensor){
  Co2Sense::Colors myColors;
  char * msg = (char*) malloc(100);
  char * command;
  char * parameter;
  char * value;
  strcpy(msg,inter.read());
  if (strcmp(msg,"NULL") != 0 && strcmp(msg,"")!=0){
  Serial.println(msg);
  char * temp = (char*) malloc(100);
  strcpy(temp,msg);
  Serial.println(temp);
  char * token;
  token = strtok(temp," ");
  if(token != NULL){
  command = calloc(strlen(token)+1,sizeof(char));
  strcpy(command,token);
  Serial.println(command);
  }

  token = strtok(NULL," ");
  if(token != NULL){
  parameter = calloc(strlen(token)+1,sizeof(char));
  strcpy(parameter,token);
  Serial.println(parameter);
  }

  token = strtok(NULL," ");
  if(token != NULL){
  value = calloc(strlen(token)+1,sizeof(char));
  strcpy(value,token);
  Serial.println(value);
  }

  if(strcmp(command,"set") == 0 && strcmp(parameter, "color") == 0){
    if(strcmp(value,"red")==0){
      myLEDs.set_color(myColors.red,myLEDs.brightness);
    }
    else if(strcmp(value,"green")==0){
      myLEDs.set_color(myColors.green,myLEDs.brightness);
    }
    else if(strcmp(value,"orange")==0){
      myLEDs.set_color(myColors.orange,myLEDs.brightness);
    }
    else if(strcmp(value,"yellow")==0){
      myLEDs.set_color(myColors.yellow,myLEDs.brightness);
    }
    else if(strcmp(value,"blue")==0){
      myLEDs.set_color(myColors.blue,myLEDs.brightness);
    }
  }

  else if(strcmp(command,"set") == 0 && strcmp(parameter, "brightness") == 0){
   char my_buffer[50];
   int num_value = atoi(value);
    if(num_value != 0){
      num_value = (((0<num_value)&&(num_value <=255)) ? num_value : NULL);
      if(num_value != NULL) {
        Serial.println(num_value);
        myLEDs.set_brightness(num_value);
      }
      else{
      sprintf(my_buffer,"Error: Invalid Input '%s'", value);
      Serial.println(my_buffer);
      }
    }
    else {
      sprintf(my_buffer,"Error: Invalid Input '%s'", value);
      Serial.println(my_buffer);
    }
  }

  else if(strcmp(command,"set") == 0 && strcmp(parameter, "breathe") == 0){
    if(strcmp(value,"red") == 0){
      myLEDs.breathe(myColors.red,128,1000);
    }
    else if(strcmp(value,"orange") == 0){
      myLEDs.breathe(myColors.orange,128,1000);
    }
    else if(strcmp(value,"blue") == 0){
      myLEDs.breathe(myColors.blue,128,1000);
    }

    else {
      Serial.println("Unknown command: "); Serial.print(msg); Serial.print('\n');
    }
  }

  else if(strcmp(command,"calibrate") == 0 && strcmp(parameter, "sensor") == 0){
     Serial.println("Starting calibration sequence.");
      Serial.println("Place the sensor outside now. Calibration will commence automatically after 20 min.");
    unsigned long my_timer = millis();
    while(millis()<my_timer+12e5){
      myLEDs.breathe(myColors.blue,255,1000);
    }
    mySensor.listen();
    mySensor.calibrate();
    inter.listen();
  }

  else if(strcmp(command,"show") == 0 && strcmp(parameter, "background_co2") == 0){
    char myBuffer[50];
    mySensor.listen();
    sprintf(myBuffer,"Background CO2: %i",mySensor.get_background());
    inter.listen();
    inter.send(myBuffer);
  }

    else if(strcmp(command,"show") == 0 && strcmp(parameter, "brightness") == 0){
      char myBuffer[50];
      sprintf(myBuffer,"Brightness: %i", myLEDs.brightness);
      inter.send(myBuffer);
    }

  if(temp != NULL){
  free(temp);
  }
   if(command != NULL){
  free(command);
  }
   if(parameter != NULL){
  free(parameter);
  }
  if(value != NULL){
  free(value);
  }
  }
  if(msg != NULL){
  free(msg);
  }
  msg = "NULL";
}

void co2_control(Co2Sense::Co2Sensor & mySensor, Co2Sense::ledRing & myLEDs, Co2Sense::SerialInterface & mySI){
  Co2Sense::Colors myColors;
  static int co2 = 0;
  static int last_co2 = 0;
  static int temp = 0;
  static unsigned long my_timer;
  static bool warmed_up = ((co2 > 300 && abs(last_co2-co2) < 100) ? true : false);
  if(!warmed_up){
    mySI.send("Warming up...");
    myLEDs.breathe(myColors.orange,0,1000);
    last_co2 = co2;
    mySensor.listen();
    co2 = mySensor.read_co2();
    mySI.listen();
    char myBuffer[50];
  sprintf(myBuffer,"Warmed-Up: %i -- CO2: %i -- last CO2: %i",warmed_up,co2,last_co2);
   Serial.println(myBuffer);
  }
  else{
    mySensor.listen();
    temp = mySensor.read_temperature();
    last_co2 = co2;
    co2 = mySensor.read_co2();
    mySI.listen();
    char myBuffer[50];
    sprintf(myBuffer,"CO2: %i ppm Temp: %i °C",co2,temp);
    mySI.send(myBuffer);
    if(co2 < 1000) {
      myLEDs.set_color(myColors.green,0);
    }
    else if ((co2>=1000) && (co2 < 2000)){
      myLEDs.set_color(myColors.yellow,0);
    }
    else if(co2 >= 2000){
      myLEDs.set_color(myColors.red,0);
    }
    my_timer = millis();
    while(millis()<my_timer+10000){
      
    }
  }
}

static void Co2Sense::co2_control2(Co2Sense::Co2Sensor & mySensor, Co2Sense::ledRing & myLEDs, Co2Sense::SerialInterface & mySI){
  Co2Sense::Colors myColors;
  static int co2 = 0;
  static int last_co2 = 0;
  static int temp = 0;
  static unsigned long my_timer = millis();
  static bool warmed_up;
  warmed_up = ((co2 > 300 && abs(last_co2-co2) < 100) ? true : false);
  if(!warmed_up){
  mySI.send("Warming up...");
  for(int i = 0; i<6; i++) {
    myLEDs.breathe(myColors.orange,0,1000);
  }
    last_co2 = co2;
    mySensor.listen();
    co2 = mySensor.read_co2();
    mySI.listen();
    char myBuffer[50];
  sprintf(myBuffer,"Warmed-Up: %i -- CO2: %i -- last CO2: %i",warmed_up,co2,last_co2);
   Serial.println(myBuffer);
  }
  else{
    if(millis()>my_timer+10000){
    mySensor.listen();
    temp = mySensor.read_temperature();
    last_co2 = co2;
    co2 = mySensor.read_co2();
    mySI.listen();
    char myBuffer[50];
    sprintf(myBuffer,"CO2: %i ppm Temp: %i °C",co2,temp);
    mySI.send(myBuffer);
    if(co2 < 1000) {
      myLEDs.set_color(myColors.green,0);
    }
    else if ((co2>=1000) && (co2 < 2000)){
      myLEDs.set_color(myColors.yellow,0);
    }
    else if(co2 >= 2000){
      myLEDs.set_color(myColors.red,0);
    }
      my_timer = millis();
  }
  }
}