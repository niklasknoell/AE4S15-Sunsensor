#include <BasicLinearAlgebra.h>

// Sensor pins
// TODO: determine right pins from hardware 
// should be first 2 +x, then 2 +y, then 2 -x, then 2-y
int sens_array[] = {A0, A1, A2, A3, A4, A5, A6, A7};

int too_dark_threshold = 1015;
int too_bright_threshold = 3;

void get_sun_vector(int *ADC_readings, BLA::Matrix<3> *s);

const byte n_bytes = 32;
char rx_buffer[n_bytes];
char rx_num[5]; // allow for a 4 digit number + termination

int loop_delay = 100;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // just to spit out some status
}

void get_sun_vector(int *ADC_readings, BLA::Matrix<3> *s){
  
  int intensities[4] = {0};

  for (int i = 0; i<4; i++) {
    // basically, if one LDR is outside the thresholds, use the other one
    // if they are both outside the thresholds, return zero sun vector
    if ((ADC_readings[i*2] > too_dark_threshold) || (ADC_readings[i*2] < too_bright_threshold)) 
    { 
      if ((ADC_readings[i*2+1] > too_dark_threshold) || (ADC_readings[i*2+1] < too_bright_threshold))
      {
        *s = {0, 0, 0};
        return 1; // Both sensors are outside expected range, return 1
      }
      else
      {
        intensities[i] = ADC_readings[i*2+1]; // Only use the second LDR
      }
    }
    else if((ADC_readings[i*2+1] > too_dark_threshold) || (ADC_readings[i*2+1] < too_bright_threshold))
    {
      intensities[i] = ADC_readings[i*2];
    }
    else 
    {
      intensities[i] = (ADC_readings[i*2] + ADC_readings[i*2 + 1])/2; // average of ldrs
    }
  }
  
  // Make the values negative s.t. the signal scales with brightness
  for (int i = 0; i < 4; i++)
  {
    intensities[i] = -intensities[i];
  }

  // now we have figured out the intensities from input data
  // lets calculate the orientation
  // equation goes like this (I: intensity vec, s: sun vector, N: matrix where each row has the transposed normal vector of each intensity)
  // I = N * s 
  // N is 4x3, so need to take left inverse of N of the form (NT * N)^-1 * NT
  // such that s = (NT * N)^-1 * NT * I

  // maybe reduce rows by one if we find bad data on one side of pyramid?

  BLA::Matrix<4,3> N = {0,-2,1,
                        0,2,1,
                        2,0,1,
                        -2,0,1};

  BLA::Matrix<4,1> I = {intensities[0], intensities[1], intensities[2], intensities[3]};

  *s = Inverse(~N * N) * ~N * I;
  return 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  int readings[8];

  for (int i = 0; i < 8; i++) {
    readings[i] = analogRead(sens_array[i]);
  }

  BLA::Matrix<3,1> s;
  get_sun_vector(readings, &s);

  // Serial.print("Raw values: \n");
  // for (int i = 0; i < 8; i++){
  //   Serial.print("Sensor1: ");
  //   Serial.print(readings[i], DEC);
  //   Serial.print(" ");
  // }

  delay(loop_delay);

  Serial.print("X:");
  Serial.print(s(0), DEC);
  Serial.print(" Y:");
  Serial.print(s(1), DEC);
  Serial.print(" Z:");
  Serial.print(s(2), DEC);
  Serial.print("\n");

  // Check if the user is sending data over serial:
  if (Serial.available() > 0){
    Serial.readBytesUntil('\0', rx_buffer, n_bytes);
    Serial.println(rx_buffer);
    
    for (int i = 0; i < 4; i++){
      rx_num[i] = rx_buffer[i+1];
    }
    rx_num[4] = '\0';

    if (rx_buffer[0] == 'l'){
      if (atoi(rx_num) != 0){
        too_bright_threshold = atoi(rx_num);
        Serial.println(rx_num); // for debugging
      }
    }
    else if (rx_buffer[0] == 'u'){
      if (atoi(rx_num) != 0){
        too_dark_threshold = atoi(rx_num);
        Serial.println(rx_num);
      }
    }
    else{
      Serial.println("FAIL");
    }
  }

}
