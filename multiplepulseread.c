//////////////////////////////////////////////////////
//
// RaspberryPi5 read and measure pulsed input of GPIO13(pin#33)
// and set similar output state to GPIO27(pin#13)
// This code measures the time  in microseconds which it will take 
// to measure a number(defined by PulseNumbers) of pulses (high state).
// The timing starts when the pulse changes from 0 to 1 and stops
// when is changes back from 1 to 0
//
// The read state is also outputed to GPIO27 to which you can connect
// e.g. a LED or oscilliscope.
//
// The libgpiod-dev library is used.
//
// Robert J. Heerekop 2025
// 
// Prerequisites:
// HW: Raspberry Pi 5 Model B Rev 1.1
// OS: RaspberryPi OS64:
// (Linux raspberrypi 6.6.74+rpt-rpi-2712 #1 SMP PREEMPT Debian 1:6.6.74-1+rpt1 (2025-01-27))
//  sudo apt update && sudo apt install -y libgpiod-dev
// Compile with:
//  g++ -Wall -O3 -std=c++17 -o multiplepulseread multiplepulseread.c -lgpiod
// Run by:
//  sudo nice -n -20 ./multiplepulseread
//

#include <iostream>
#include <gpiod.h>
#include <chrono>
#include <thread>
#include <chrono>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif
#define CaptureSize 1000
using namespace std;
using namespace chrono; 					// this namespace is used to perform time calculation


int PulseNumbers = 100;					// number of pulses (active high time) to measure
int PulseCounter = 0;


long Pulse_shortest = 0;
long Pulse_longest = 0;

int CaptureBank[CaptureSize];				// Storage to store raw gpio input capture
int i=0;									//generic counter
using namespace std;
using namespace chrono; // this namespace is used to perform calculation
auto start1 = high_resolution_clock::now();
auto stop1 = high_resolution_clock::now(); 
auto T_pulse = stop1 - start1;



const char* chipname = "gpiochip0";
const int gpio_read_pin = 13;  // GPIO13 INPUT
const int gpio_write_pin = 27;  // GPIO27 LED




int main() {

    //std::cout << "Initialisation..."  << std::endl;
    // Open GPIO chip
    gpiod_chip* chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        std::cerr << "Error opening chip" << std::endl;
        return 1;
    }
    // Initialize input line
    gpiod_line* read_line = gpiod_chip_get_line(chip, gpio_read_pin);
    if (!read_line) {
        std::cerr << "Error getting input line" << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }
    // Initialize output line
    gpiod_line* write_line = gpiod_chip_get_line(chip, gpio_write_pin);
    if (!write_line) {
        std::cerr << "Error setting output line" << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }
    // Request input mode
    if (gpiod_line_request_input(read_line, "gpio-reader") < 0) {
        std::cerr << "Error requesting input" << std::endl;
        gpiod_line_release(read_line);
        gpiod_chip_close(chip);
        return 1;
    }
    // Request output mode
    if (gpiod_line_request_output(write_line, CONSUMER, 0) < 0) {
        std::cerr << "Request line as output failed\n" << std::endl;
        gpiod_line_release(write_line);
        gpiod_chip_close(chip);
        return 1;
    }


	printf("Measurering %d pulses (high state time)\n", PulseNumbers);

    while (PulseCounter<PulseNumbers) {

    //std::cout << "Capturing..."  << std::endl;
    i=0;
	int T_start = 0;
	int T_stop = 0;
	int Found_start = 0;
	int Found_stop = 0;
	int previous_state = -1;
    while (i<CaptureSize) {
	   int readstate = gpiod_line_get_value(read_line);
       int writestate = gpiod_line_set_value(write_line, readstate);
       CaptureBank[i] = readstate;
	   
	   if (( previous_state == 0 ) & ( readstate == 1 ) & (Found_start == 0 )) {
		   start1 = high_resolution_clock::now();
		   T_start = i;	
		   Found_start = 1;
           //printf("*\n");	   
	   }   

	   if (( previous_state == 1 ) & ( readstate == 0 ) & (Found_stop == 0 ) & (Found_start == 1 )) {
		   stop1 = high_resolution_clock::now(); 
		   T_stop = i;
		   Found_stop = 1;
		   T_pulse = stop1 - start1;
           //printf("*\n");		   
	   } 	   
	   //printf("%d %d %d \n", i, previous_state, readstate);
 	   previous_state = readstate;
	   i++;
   
	   
	}	// end of pulse detection loop


/*     //std::cout << "Printing..."  << std::endl;
    i=0;
    while (i<CaptureSize) {
       //printf("%d:  %d\n", i, CaptureBank[i] );
       int writestate = gpiod_line_set_value(write_line, CaptureBank[i]);
	   i++;
	}
	//printf("T_start: %d\n", T_start);
	//printf("T_stop: %d\n", T_stop); */


  long smallerNumber = static_cast<long>(std::chrono::duration_cast<std::chrono::microseconds>(T_pulse).count());
  //std::cout << "Time measured: " << smallerNumber << " microseconds" << std::endl;

	if ((smallerNumber < Pulse_shortest) || (Pulse_shortest == 0)) {
	Pulse_shortest = smallerNumber;
	}
	
	if (smallerNumber > Pulse_longest) {
	Pulse_longest = smallerNumber;
	}

	 PulseCounter++;
	} 	// end of loop measuring multiple pulses
	
  std::cout << "Pulse_shortest: " << Pulse_shortest << " microseconds" << std::endl;
  std::cout << "Pulse_longest: " << Pulse_longest << " microseconds" << std::endl;
  
  //std::cout << "Done..."  << std::endl;

    // Cleanup
    gpiod_line_release(read_line);
	gpiod_line_release(write_line);
    gpiod_chip_close(chip);
    return 0;
}
