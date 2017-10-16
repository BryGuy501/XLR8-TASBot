/*
 Copyright Bryan R. Craker 2017

 Used to execute arbitrary controller input for an NES using an XLR8 board

 Requires SD card on the SPI interface with CS on D09, NES clock on D02, NES latch on D03

 Released without warranty, hoping people will find it useful

*/

#include <SPI.h>
#include <SD.h>

#define BUFFER_SIZE 400

const int chipSelect = 9;

volatile int frame_cnt = 0;
volatile int clk_cnt = 0;
volatile uint8_t buttons_hold = 0;
volatile uint8_t buttons = 0;
uint8_t port_buffer = 0;
volatile uint8_t button_state_a[BUFFER_SIZE];
volatile uint8_t button_state_b[BUFFER_SIZE];
volatile boolean send_data = false;
volatile boolean use_a = true;
volatile boolean change_buffer = false;
boolean current_a = true;
File dataFile;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  pinMode(6, INPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  //Serial.println("Initializing SD");

  if (!SD.begin(chipSelect)) {
    //Serial.println("Failed to initialize SD");
    return;
  }

  //Serial.println("Opening file");

  dataFile = SD.open("smb_dh.bin");
  //dataFile = SD.open("smb_wr.bin");
  if (!dataFile) {
    //Serial.println("Failed to open data file");
    return;
  }

  //Serial.println("Loading buffer A");

  for (int i = 0; i < BUFFER_SIZE; i++) {
    button_state_a[i] = dataFile.read();
  }

  //Serial.println("Loading buffer B");

  for (int i = 0; i < BUFFER_SIZE; i++) {
    button_state_b[i] = dataFile.read();
  }

  //Serial.println("Attaching interrupts");

  /*Serial.println("PRINTING A");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.println(button_state_a[i], HEX);
    delay(50);
  }
  Serial.println("PRINTING B");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.println(button_state_b[i], HEX);
    delay(50);
  }*/

  attachInterrupt(digitalPinToInterrupt(2), clock_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(3), latch_handler, RISING);

  while (!digitalRead(6)) {}
  frame_cnt = 0;
}

void loop() {
  if (change_buffer) {
    if (dataFile.available()) {
      if (current_a) {
        //Serial.println("LOADING A");
        current_a = false;
        for (int i = 0; i < BUFFER_SIZE; i++) {
          button_state_a[i] = dataFile.read();
        }
      } else if (!current_a) {
        //Serial.println("LOADING B");
        current_a = true;
        for (int i = 0; i < BUFFER_SIZE; i++) {
          button_state_b[i] = dataFile.read();
        }
      }
    } else {
      if (current_a) {
        current_a = false;
        for (int i = 0; i < BUFFER_SIZE; i++) {
          button_state_a[i] = 0x00;
        }
      } else if (!current_a) {
        current_a = true;
        for (int i = 0; i < BUFFER_SIZE; i++) {
          button_state_b[i] = 0x00;
        }
      }
      //dataFile.close();
    }
    change_buffer = false;
  }
}

void initialize() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    button_state_a[i] = 0x00;
    button_state_b[i] = 0x00;
  }
}

void clock_handler() {
  if (send_data) {
    PORTD = port_buffer;
    clk_cnt++;
    port_buffer = B00010000 & (~(0x01 & (buttons_hold >> clk_cnt)) << 4);
    if (clk_cnt > 8) {
      frame_cnt++;
      send_data = false;
      PORTD = B00010000;
      if (frame_cnt == BUFFER_SIZE) {
        //Serial.println("CHANGING BUFFER");
        //Serial.println(use_a);
        use_a = (use_a) ? 0 : 1;
        change_buffer = true;
        //Serial.println(use_a);
        frame_cnt = 0;
      }
    }
  }
}

void latch_handler() {
  if (!send_data) {
    //clk_cnt = 0;
    buttons_hold = (use_a) ? button_state_a[frame_cnt] : button_state_b[frame_cnt];
    //buttons_hold = button_state_a[frame_cnt];
    /*if (use_a) {
      Serial.println("USING A");
    } else {
      Serial.println("USING B");
    }*/
    //frame_cnt++;
    PORTD = B00010000 & (~(0x01 & (buttons_hold)) << 4);
    send_data = true;
    clk_cnt = 1;
    port_buffer = B00010000 & (~(0x01 & (buttons_hold >> clk_cnt)) << 4);
  }
}

