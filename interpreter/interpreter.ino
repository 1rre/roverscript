#ifdef ARDUINO
extern "C" {
  #include "test_interpreter.h"
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println(test_interpret());
}

void loop() {
}
#endif
