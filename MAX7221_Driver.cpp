//
//  MAX7221_Driver.cpp
//  MAX7221 Library
//
//  Created by James Fulton on 6/19/21 (2:56PM)
//  Copyright © 2021 James Fulton. All rights reserved.
// 
// Version 1.0
// This "library" allows one to use either a 2D array or a 64-bit hexideximal identifier 
// to ouput data to a 2D LED array that utilizes the MAX7221 display driver.
// 
// Fuctional but inneficient.
//

#include "MAX7221_Driver.h"
#define SAFE_DELAY 10

//------------Constructors & Deconstructors------------

Display::Display(int csPin, int numChips, int chipRows): csPin(csPin), numChips(numChips){
    rowDepth = chipRows*8;
    colDepth = numChips*8 / chipRows;
    hexPlot = new uint64_t[numChips]();
    bitPlot = new int*[rowDepth];
    for(int i = 0; i < rowDepth; i++){ bitPlot[i] = new int[colDepth](); }
}

Display::Display(int csPin, int numChips){ Display(csPin, numChips, 1); }

Display::~Display(){
    if(hexPlot){ delete[] hexPlot; }
    if(bitPlot){
        for(int i = 0; i < rowDepth; i++){ delete[] bitPlot[i]; }
        delete[] bitPlot;
    }
}

//------------Config Methods------------

void Display::begin(){
    
    pinMode(csPin, OUTPUT);
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    SPI.begin();
    
    //SAFE_DELAY used to temporarily remedy setup errors where chips become unresponsive
    for(int i=1; i<=numChips; i++){
        printRow(DISPLAY_TEST, 0x00, i);
        delay(SAFE_DELAY);
        printRow(SCAN_LIMIT, 0x0f, i);
        delay(SAFE_DELAY);
        printRow(INTENSITY, 255/10, i);
        delay(SAFE_DELAY);
        printRow(DECODE_MODE, 0x00, i);
        delay(SAFE_DELAY);
        clearArray(true);
        delay(SAFE_DELAY);
        printRow(SHUTDOWN, 0x00, i);
        delay(SAFE_DELAY);
    }
}

//------------Setter Methods------------

//True designates shutdown
//False clears shutdown
void Display::setShutdown(bool state){
    uint8_t value = 0x01;
    if(state){ value = 0x00; }
    for(int i = 0; i < numChips; i++){
      printRow(SHUTDOWN, value, i+1);
      delay(SAFE_DELAY);
    }
}

//-1 sends intensity to all chips
void Display::setIntensity(int intensity, int chip){
    if(chip < 0){
        for(int i = 1; i <= numChips ; i++){
          printRow(INTENSITY, intensity, i); 
          delay(SAFE_DELAY);
         }
    }
    else{ printRow(INTENSITY, intensity, chip); }
}

void Display::setIntensity(int intensity){
    setIntensity(intensity, -1); 
}


//------------Output Methods------------

void Display::printBitPlot(){
    uint8_t output[numChips];

    //Data reconfiguration
    for(int row = 0; row < 8; row++){
      
      //Clearing output array
      for(int i = 0; i < numChips; i++){ output[i] = 0; }
      
      //Used to correctly organize data for registers
      for(int rowMult = 0; rowMult < rowDepth/8; rowMult++){
        for(int col = 0; col < colDepth; col++){
          if(bitPlot[row+8*rowMult][col] != 0){
            
            //Conditions used to determine regsiter count & configuration
            if(rowDepth == 8 || colDepth == 8 && rowDepth != colDepth){
              output[rowMult+col/8] += ceil(pow(2, col%8)); //ceiling used for double rounding errors
            }
            else{
                output[(rowMult*numChips/2)+col/8] += ceil(pow(2, col%8)); 
            }
            
          }
        }
      }
      
      //DIN output to specific row
      digitalWrite(csPin, LOW);
      for(int i = numChips-1; i >= 0; i--){
        SPI.transfer(8-row);
        SPI.transfer(output[i]);
      }
      digitalWrite(csPin, HIGH);
    }     
}

//Outputs data for one specific row for one specific chip
//8-bit input directly represents each LED per 8 LED row
void Display::printRow(uint8_t row, uint8_t value, int chip){
    digitalWrite(csPin, LOW);
    for(int i = 1; i < chip; i++){ SPI.transfer16(0x0000); } //Skipping undesired chips
    SPI.transfer(row);
    SPI.transfer(value);
    for(int i = chip; i < numChips; i++){ SPI.transfer16(0x0000); } //Skipping undesired chips
    digitalWrite(csPin, HIGH);  
}


//Outputs a 64-bit hexadecimal 8x8 LED cube representation to a specific chip
void Display::printCube(uint64_t values, int chip){
    for(int i = 0; i < 8; i++){
      byte row  = (values >> i*8) & 0xFF;
      printRow(8-i, row, chip);
    }
}

void Display::printHexPlot(){
    for(int i=0; i<numChips; i++){
      printCube(hexPlot[i], i+1); 
    }
}


//------------Modification Methods------------

//clearArray can be configured only clear the local variables
void Display::clearArray(bool output){
    for(int i=0; i<numChips; i++){
      hexPlot[i] = 0x0000000000000000; 
    }
    for(int i=0; i<rowDepth; i++){
        for(int j=0; j<colDepth; j++){ bitPlot[i][j] = 0; }
    }
    if(output){ printHexPlot(); }
}

void Display::clearArray(){ clearArray(true); }
