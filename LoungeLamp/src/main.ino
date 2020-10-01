////////////////////////////////////////////////////////////////////////
//
//  ###
//   #  #    #  ####  #      #    # #####  ######  ####
//   #  ##   # #    # #      #    # #    # #      #
//   #  # #  # #      #      #    # #    # #####   ####
//   #  #  # # #      #      #    # #    # #           #
//   #  #   ## #    # #      #    # #    # #      #    #
//  ### #    #  ####  ######  ####  #####  ######  ####
//
////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <Streaming.h>

////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     # ###### ###### # #    # # ##### #  ####  #    #  ####
//  #     # #      #      # ##   # #   #   # #    # ##   # #
//  #     # #####  #####  # # #  # #   #   # #    # # #  #  ####
//  #     # #      #      # #  # # #   #   # #    # #  # #      #
//  #     # #      #      # #   ## #   #   # #    # #   ## #    #
//  ######  ###### #      # #    # #   #   #  ####  #    #  ####
//
////////////////////////////////////////////////////////////////////////
#define ledPin D2

#define redPin D5
#define greenPin D6
#define bluePin D7

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #####  #    #   ##   #####  ######
//  #     #  #  #  #    # #    # #    #  #  #  #    # #
//  ####### #    # #    # #    # #    # #    # #    # #####
//  #     # ###### #####  #    # # ## # ###### #####  #
//  #     # #    # #   #  #    # ##  ## #    # #   #  #
//  #     # #    # #    # #####  #    # #    # #    # ######
//
////////////////////////////////////////////////////////////////////////
IRrecv irrecv(ledPin);

decode_results results;

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #   ##   #####  #      ######  ####
//  #     #  #  #  #    # #  #  #  #    # #      #      #
//  #     # #    # #    # # #    # #####  #      #####   ####
//   #   #  ###### #####  # ###### #    # #      #           #
//    # #   #    # #   #  # #    # #    # #      #      #    #
//     #    #    # #    # # #    # #####  ###### ######  ####
//
////////////////////////////////////////////////////////////////////////
unsigned long keys[6][4] = {
    {0xFF906F, 0xFFB847, 0xFFF807, 0xFFB04F},
    {0xFF9867, 0xFFD827, 0xFF8877, 0xFFA857},
    {0xFFE817, 0xFF48B7, 0xFF6897, 0xFFB24D},
    {0xFF02FD, 0xFF32CD, 0xFF20DF, 0xFF00FF},
    {0xFF50AF, 0xFF7887, 0xFF708F, 0xFF58A7},
    {0xFF38C7, 0xFF28D7, 0xFFF00F, 0xFF30CF}};

unsigned long actions[6][4] = {
    {0x000000, 0x000000, 0x000000, 0x000000},
    {0xff0000, 0x00ff00, 0x0000ff, 0xffffff},  //rgbw
    {0xFFA500, 0x90EE90, 0x00008B, 0x000000},
    {0xBDB76B, 0x00FFFF, 0x4169E1, 0x000000},
    {0xFFFF00, 0xADD8E6, 0x8B4513, 0x000000},
    {0xFFFFE0, 0x87CEEB, 0xEE82EE, 0x000000}};

////////////////////////////////////////////////////////////////////////
//
//  ######                                                #####
//  #     # #####   ####   ####  #####    ##   #    #    #     # #####   ##   #####  ##### #    # #####
//  #     # #    # #    # #    # #    #  #  #  ##  ##    #         #    #  #  #    #   #   #    # #    #
//  ######  #    # #    # #      #    # #    # # ## #     #####    #   #    # #    #   #   #    # #    #
//  #       #####  #    # #  ### #####  ###### #    #          #   #   ###### #####    #   #    # #####
//  #       #   #  #    # #    # #   #  #    # #    #    #     #   #   #    # #   #    #   #    # #
//  #       #    #  ####   ####  #    # #    # #    #     #####    #   #    # #    #   #    ####  #
//
////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  irrecv.enableIRIn();  // Start the receiver
}

///////////////////////////////////////////////////////////////////////
//
//  #     #                    ######
//  ##   ##   ##   # #    #    #     # #####   ####   ####  #####    ##   #    #
//  # # # #  #  #  # ##   #    #     # #    # #    # #    # #    #  #  #  ##  ##
//  #  #  # #    # # # #  #    ######  #    # #    # #      #    # #    # # ## #
//  #     # ###### # #  # #    #       #####  #    # #  ### #####  ###### #    #
//  #     # #    # # #   ##    #       #   #  #    # #    # #   #  #    # #    #
//  #     # #    # # #    #    #       #    #  ####   ####  #    # #    # #    #
//
///////////////////////////////////////////////////////////////////////
void loop() {
  // Serial << "RGB code ";
  // Serial << actions[0][1];
  // serialPrintUint64(actions[1][0], HEX);
  // Serial << endl;

  testPrint();

  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    // if (results.value == test[0]) {
    //   Serial << "Here" << endl;
    // }

    // Serial << "RGB code ";
    // Serial << actions[0][1];
    // Serial << endl;

    for (int y = 0; y < 6; y++) {
      for (int x = 0; x < 4; x++) {
        if (results.value == keys[y][x]) {
          // updateLEDs(keys[x][y]);
          // Serial << keys[x][y] << endl;
          Serial << "Keys " << x << " " << y << endl;

          Serial << "Key code ";
          // serialPrintUint64(keys[y][x], HEX);
          Serial << actions[y][x];
          Serial << endl;

          // Serial << "RGB code ";
          // Serial << actions[0][1];
          // Serial << endl;
        }
      }
    }

    for (int y = 0; y < 6; y++) {
      for (int x = 0; x < 4; x++) {
        if (results.value == keys[y][x]) {
          updateLEDs(actions[y][x]);
        }
      }
    }

    irrecv.resume();  // Receive the next value
  }
}

void updateLEDs(unsigned long hexValue) {
  int red, green, blue;

  red = (hexValue & 0xff0000) >> 16;
  green = (hexValue & 0x00ff00) >> 8;
  blue = (hexValue & 0x0000ff);

  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);

  Serial << "Red: " << red << " Green: " << green << " Blue: " << blue << endl;
}

// #include <iostream>

// using namespace std;

// unsigned long test = 0xffffff;
// unsigned shift = (test & 0x00ff00) >> 8;

// void convert(unsigned int value )
// {
//     int red, green, blue;

//     red = (value & 0xff0000) >> 16;
//     green = (value & 0x00ff00) >> 8;
//     blue = (value & 0x0000ff);

//     cout << "Red: " << red << " Green: " << green << " Blue: " << blue << endl;
// }

// int main()
// {
//    convert(0x125436);

//    return 0;
// }