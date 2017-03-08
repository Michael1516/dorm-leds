/*
  Dorm LED Project: main.ino
  Main file for the LED project.

  Created by: Michael Fischler
  9/20/2016 @ WPI
*/

// Standard C++ Library
#include <unwind-cxx.h>
#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <utility.h>

// LED Library
#include <Adafruit_NeoPixel.h>

// Liquid Crystal Display Library
#include <LiquidCrystal.h>

// Configuration
#include "conf/config.h"
#include "conf/strips.h"
#include "conf/dev_types.h"

// System
#include "lib/strip_id.h"
#include "lib/strip_ownership.h"

#include "lib/MemObj.cpp"
#include "lib/LocalStack.cpp"

#include "lib/Executable.cpp"
#include "lib/Animation.cpp"
#include "lib/Process.cpp"

#include "lib/Thread.cpp"
#include "lib/ThreadHandler.cpp"

#include "lib/Device.cpp"
#include "lib/DeviceManager.cpp"

// Animations
#include "def/animations.h"

// Devices (like drivers)
#include "dev/devices.h"

// Utility Files
#include "util/led.c"
#include "util/mem.c"
#include "util/free_memory.c"

// Timing
unsigned long int prev_time = millis();
unsigned long int cur_time = millis();

// LCD Display
LiquidCrystal lcd(LCD_E_PIN, LCD_RS_PIN, LCD_D4_PIN, LDC_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// Core System Variables and Class Instances
unsigned long int dT = 0;
ThreadHandler thread_handler = ThreadHandler();
DeviceManager device_manager = DeviceManager();

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.println(F("Initializing..."));

	// random seed
	Serial.println(F("Generating random seed..."));
	randomSeed(analogRead(A0));

	// initialize pins
	Serial.println(F("Setting pin modes..."));
	set_pin_modes();

	// initialize LED strips and set them to off
	Serial.println(F("Initializing neopixel strips..."));
	init_strips();

	// initialize LCD display
	Serial.println(F("Initializing LCD display..."));
	init_lcd();

	// manual queue
	mem_available = freeMemory();
	Serial.println(F("Queueing system threads..."));
	queue_sys_threads();

	// manual queue
	mem_available = freeMemory();
	Serial.println(F("Manually queueing animations..."));
	led_man_queue();

	// initialize timing
	Serial.println(F("Initializing timing system..."));
	init_timing();

	Serial.println(F("System Initialized."));
}

// the loop function runs over and over again forever
// run multithreaded system code
void loop() {
	// set change in time
	cur_time = millis();
	dT = cur_time - prev_time;

	// tell each thread the time change
	thread_handler.updateTimeAccumulated(dT);

	// execute commands that it is time to execute
	thread_handler.executeTick();

	// save this time as previous time
	prev_time = millis();

	// prevent ticks less than a millisecond
	delay(1);

	// print memory
	//Serial.print(F("Free SRAM: "));
	//Serial.print(freeMemory());
	//Serial.println(F(" bytes"));
}
