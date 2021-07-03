//
//  MAX7221_Driver.h
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
// Future Modifications:
// 1. Remove 2D array data storage in favor of 64-bit hexadecimal representation. 
//    Defined as cubePlot (or something else), it will perform exactly the same as a 2D array.
//    While this may require more processing power/time, this attempt will greatly reduce memory usage when using a large 2D array.
// 2. As an extention of 1, I could on the other hand favor the hexidecimal rep. while keeping the 2D array if desired by the user.
//    With this configuration, the user would hypothetically desire runtime speed over memory usage.
//    I can imagine a use case for this dilema with a desire to display rapidly changing conditions. (ie. monitoring Engine RPM or crankshaft angle with high precision).
//    I do however believe the difference in speed will be neglegible as I develop more optimized methods of displaying and encoding data.
// 3. Remove Arduino supplied register modifers in favor of direct register configuration to greatly improve program speed.

#ifndef MAX7221_Driver_h
#define MAX7221_Driver_h

#include <SPI.h>
#define DECODE_MODE 9
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F

class Display{
public:
    //------------Constructors & Deconstructors------------
    Display(int csPin, int numChips);
    Display(int csPin, int numChips, int chipRows);
    ~Display();
    
    //------------Config Methods------------
    void begin();
    
    //------------Set Methods------------
    void setShutdown(bool state);
    void setCsPin(int csPin){ this->csPin = csPin; }
    void setIntensity(int intensity, int chip);
    void setIntensity(int intensity);

    //------------Get Methods------------
    int getNumChips() const{ return numChips; }
    int getRowDepth() const{ return rowDepth; }
    int getColDepth() const{ return colDepth; }
    int getCsPin() const{ return csPin; }
    int getIntensity() const{ return intensity; }
    int getHardwareType() const{ return hardwareType; }
    uint64_t getHexCube(int index) const{ return hexPlot[index]; }
    uint64_t* getHexPlot() const{ return hexPlot; }
    int** getBitPlot() const{ return bitPlot; }
    
    //------------Output Methods------------
    void printBitPlot();
    void printRow(uint8_t row, uint8_t value, int chip);
    void printCube(uint64_t values, int chip);
    void printHexPlot();
    
    //------------Modification Methods------------
    void clearArray(bool output);
    void clearArray();
    
private:
    int hardwareType, csPin, numChips, intensity, rowDepth, colDepth;
    uint64_t* hexPlot;
    int** bitPlot;
};

#endif /* MAX7221_Driver_h */
