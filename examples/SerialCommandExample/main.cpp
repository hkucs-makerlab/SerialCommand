#include <SerialCommand.hpp>

#ifdef AVR
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
#endif // AVR

#ifdef ESP32
#define arduinoLED 4 
#define Console Serial
#define BlueTooth Serial2
SerialCommand<HardwareSerial, HardwareSerial> sCmd(BlueTooth, Console);
#endif // ESP32

void LED_on(SerialCommand<HardwareSerial, HardwareSerial> *sCmd) {
  Console.println("LED on");
  digitalWrite(arduinoLED, HIGH);
}

void LED_off(SerialCommand<HardwareSerial, HardwareSerial> *sCmd) {
  Console.println("LED off");
  digitalWrite(arduinoLED, LOW);
}

void sayHello(SerialCommand<HardwareSerial, HardwareSerial> *sCmd) {
  char *arg;
  arg = sCmd->next();  // Get the next argument from the ConsoleCommand object buffer
  if (arg != NULL) {  // As long as it existed, take it
    Console.print("Hello ");
    Console.println(arg);
  } else {
    Console.println("Hello, whoever you are");
  }
}

void processCommand(SerialCommand<HardwareSerial, HardwareSerial> *sCmd) {
  int aNumber;
  char *arg;

  Console.println("We're in processCommand");
  arg = sCmd->next();
  if (arg != NULL) {
    aNumber = atoi(arg);  // Converts a char string to an integer
    sCmd->console.print("First argument was: ");
    sCmd->console.println(aNumber);
  } else {
    sCmd->console.println("No arguments");
  }

  arg = sCmd->next();
  if (arg != NULL) {
    aNumber = atol(arg);
    sCmd->console.print("Second argument was: ");
    sCmd->console.println(aNumber);
  } else {
    sCmd->console.println("No second argument");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(SerialCommand<HardwareSerial, HardwareSerial> *sCmd, const char *command) {
  sCmd->console.println("What?");
}
// ---------------------------------------------------------------------------
#ifdef ESP32
class LedCommand : public SerialCommand<HardwareSerial, HardwareSerial> {
  // friend void handle_w_cmd(SerialCommand<HardwareSerial, HardwareSerial>
  // *cmd);

 public:
  LedCommand() : SerialCommand(Serial1, Serial2) {
    setDefaultHandler(unrecognized);
    addCommand("on", LED_on);
  }
};

LedCommand ledCmd;

class Robot2 {
  SerialCommand<HardwareSerial, HardwareSerial> _sCmd;

 public:
  Robot2()
      : _sCmd(SerialCommand<HardwareSerial, HardwareSerial>(Serial2, Serial)) {
    _sCmd.setDefaultHandler(unrecognized);
    _sCmd.addCommand("on", LED_on);
  }
  void loop() { _sCmd.readSerial(); }
};
Robot2 robot2;

class Robot {
  SerialCommand<HardwareSerial, HardwareSerial> &_sCmd;

 public:
  Robot(SerialCommand<HardwareSerial, HardwareSerial> &sCmd) : _sCmd(sCmd) {
    _sCmd.setDefaultHandler(unrecognized);
    _sCmd.addCommand("on", LED_on);
  }
  void loop() { _sCmd.readSerial(); }
};

Robot robot(sCmd);
#endif // ESP32

//
void loop() {
  sCmd.readSerial();  // We don't do much, just process Console commands

#ifdef ESP32
  ledCmd.readSerial();
  robot.loop();
  robot2.loop();
#endif
  
}

void setup() {
  pinMode(arduinoLED, OUTPUT);    // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);  // default to LED off

  BlueTooth.begin(115200);
  if (Console != BlueTooth) {
    Console.begin(115200);
  }

  // Setup callbacks for ConsoleCommand commands
  sCmd.addCommand("on", LED_on);         // Turns LED on
  sCmd.addCommand("off", LED_off);       // Turns LED off
  sCmd.addCommand("hello", sayHello);    // Echos the string argument back
  sCmd.addCommand("p", processCommand);  // Converts two arguments to integers and echos them back
  sCmd.setDefaultHandler(unrecognized);  // Handler for command that isn't matched  (says "What?")
  Console.println("[Ready!]");
}
