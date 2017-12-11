#include "HX711.h"

HX711 scales[4];

void setup() {
  Serial.begin(38400);
  Serial.println("Choptop Interface");
  
  scales[0].begin(A1, A0);
  scales[1].begin(A3, A2);
  scales[2].begin(A5, A4);
  scales[3].begin(3, 2);
  scales[0].set_scale(466.1f);
  scales[1].set_scale(476.2f);
  scales[2].set_scale(476.8f);
  scales[3].set_scale(467.09f);

  Serial.println("Initializing the scale");
  Serial.println("Waiting for tare command");

  while (!Serial.available()) {}
  char command = Serial.read();
  if(command == 't'){
    Serial.println("taring");
    for(int i=0; i < 4; i++){
      scales[i].tare();
    }
  }

}

void loop() {
  for(int i=0; i < 4; i++){
      Serial.println(String(i) + " " + scales[i].get_units(1));
  }
}
