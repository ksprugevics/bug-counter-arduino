#include <EEPROM.h>
#include <TM1637Display.h>
#include <EZButton.h>

const int CLOCK_PIN = 20;
const int DIO_PIN = 21;

const int COUNTER_ADDRESS = 0;

const int BUTTON_INDEX = 0;
const int BUTTON_PIN = 2;

const unsigned long BUTTON_HOLD_THRESHOLD = 500;
const unsigned long BUTTON_HOLD_INTERVAL  = 300;
const unsigned long BUTTON_DEBOUNCE_TIME  = 15;

const int COUNTER_MIN = 0;
const int COUNTER_MAX = 9999;
int counter = 0;

void ReadButtons(bool *states, int num) {
  states[BUTTON_INDEX] = !digitalRead(BUTTON_PIN);
}

TM1637Display display(CLOCK_PIN, DIO_PIN);
EZButton ezButton(1, ReadButtons, BUTTON_HOLD_THRESHOLD, BUTTON_HOLD_INTERVAL, BUTTON_DEBOUNCE_TIME);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  counter = loadSavedCounter();
  setupDisplay();

  ezButton.Subscribe(BUTTON_INDEX, buttonPressHandler, RELEASED);
  ezButton.Subscribe(BUTTON_INDEX, buttonHoldHandler, HOLD_RELEASED);
}

int loadSavedCounter() {
  int storedValue;
  EEPROM.get(COUNTER_ADDRESS, storedValue);

  if (!validCounterValue(storedValue)) {
    return 0;
  }
  
  return storedValue;
}

boolean validCounterValue(int value) {
  if (value < COUNTER_MIN || value > COUNTER_MAX) {
    return false;
  }

  return true;
}

void setupDisplay() {
  display.setBrightness(0x01);
  display.clear();
  display.showNumberDec(counter, true);
}

void loop() {
  ezButton.Loop();
}

void buttonPressHandler(int buttonIndex) {
  updateCounter(counter + 1);
}

void buttonHoldHandler(int buttonIndex) {
  updateCounter(counter - 1);
}

void updateCounter(int newValue) {
  if (!validCounterValue(newValue)) {
    return;
  }

  counter = newValue;
  EEPROM.put(COUNTER_ADDRESS, counter);
  display.showNumberDec(counter, true);
}
