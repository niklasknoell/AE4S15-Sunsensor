#include <BasicLinearAlgebra.h>

// Sensor pins
// TODO: determine right pins from hardware 
// should be first 2 +x, then 2 +y, then 2 -x, then 2-y
int sens_array[] = {A0, A1, A2, A3, A4, A5, A6, A7};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // just to spit out some status
}

void loop() {
  // put your main code here, to run repeatedly:

  int readings[8];

  for (int i = 0; i < 8; i++) {
    readings[i] = analogRead(sens_array[i]);
  }

  int error_threshold = 100;


  int intensities[4];

  for (int i = 0; i<4; i++) {
    // basically, if the two ldrs on each side disagree by too much we need to do something maybe
    if (abs(readings[i*2] - readings[i*2 + 1]) > error_threshold) { 
      // set error or choose which one to use in some intelligent way
    }
    else {
      intensities[i] = (readings[i*2] + readings[i*2 + 1])/2; // average of ldrs
    }
  }

  // now we have figured out the intensities from input data
  // lets calculate the orientation
  // equation goes like this (I: intensity vec, s: sun vector, N: matrix where each row has the transposed normal vector of each intensity)
  // I = N * s 
  // N is 4x3, so need to take left inverse of N of the form (NT * N)^-1 * NT
  // such that s = (NT * N)^-1 * NT * I

  // maybe reduce rows by one if we find bad data on one side of pyramid?

  BLA::Matrix<4,3> N = {1,0,1,
                        1,0,1, // bogus values, gotta put in right angles and stuff
                        1,0,1,
                        1,0,1};

  BLA::Matrix<4,1> I = {intensities[0], intensities[1], intensities[2], intensities[3]};

  BLA::Matrix<3,1> s = Inverse(~N * N) * ~N * I;

  // should be sun vector in local x-y-z frame printed on sensor 






}
