#include <SerialCommand.hpp>

#define arduinoLED 13  // Arduino LED on board

// #define __SOFTWARE_Console__
#ifdef __SOFTWARE_Console__
#include <SoftwareSerial.h>
#define BAUD_RATE 115200
#define Console Serial
#define BT_RX A4
#define BT_TX A5
SoftwareSerial BlueTooth(BT_RX, BT_TX);
SerialCommand<SoftwareSerial, HardwareSerial> sCmd(BlueTooth, Console);
#else
#define BAUD_RATE 115200
#define Console Serial
#define BlueTooth Serial
SerialCommand<HardwareSerial, HardwareSerial> sCmd(BlueTooth, Console);
#endif

void LED_on() {
  Console.println("LED on");
  digitalWrite(arduinoLED, HIGH);
}

void LED_off() {
  Console.println("LED off");
  digitalWrite(arduinoLED, LOW);
}

void sayHello() {
  char *arg;
  arg = sCmd.next();  // Get the next argument from the ConsoleCommand object buffer
  if (arg != NULL) {  // As long as it existed, take it
    Console.print("Hello ");
    Console.println(arg);
  } else {
    Console.println("Hello, whoever you are");
  }
}

void processCommand() {
  int aNumber;
  char *arg;

  Console.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);  // Converts a char string to an integer
    Console.print("First argument was: ");
    Console.println(aNumber);
  } else {
    Console.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = atol(arg);
    Console.print("Second argument was: ");
    Console.println(aNumber);
  } else {
    Console.println("No second argument");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Console.println("What?");
}

void loop() {
  sCmd.readSerial();  // We don't do much, just process Console commands
}

void setup() {
  pinMode(arduinoLED, OUTPUT);    // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);  // default to LED off

  BlueTooth.begin(115200);
  if (Console != BlueTooth) {
    Console.begin(115200);
  }

  // Setup callbacks for ConsoleCommand commands
  sCmd.addCommand("ON", LED_on);         // Turns LED on
  sCmd.addCommand("OFF", LED_off);       // Turns LED off
  sCmd.addCommand("HELLO", sayHello);    // Echos the string argument back
  sCmd.addCommand("P", processCommand);  // Converts two arguments to integers and echos them back
  sCmd.setDefaultHandler(unrecognized);  // Handler for command that isn't matched  (says "What?")
  Console.println("Ready");
}
