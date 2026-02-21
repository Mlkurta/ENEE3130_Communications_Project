
// k = (f * N samples) / Fs   = (1200 * 20) / 8000 = 3. w = (2*pi*k) / N =  0.9425 rads;  coeff = 2cos(w) = 1.1756;  Q14 = coeff * 2^14 = 19261
const int16_t COEFF_Q14_1200HZ = 19262;   

// k = (f * N samples) / Fs   = (2000 * 20) / 8000 = 5. w = (2*pi*k) / N =  1.5707 rads;  coeff = 2cos(1.5707) = 0;  Q14 = coeff * 2^14 = 0
const int16_t COEFF_Q14_2000HZ = 0;

const int N = 20;

uint32_t gPwr;
uint32_t gPwr2;

// 1200 Hz  -> 20 Samples sampled every 125 us ----> [1 / (125 us * 20 samples)] * 3 cycles = 1200 Hz
int16_t adcArray[20] = {0,411,496,202,
                        -302,-512,-302,202,
                        496,411,0,-411,
                        -496,-202,302,511,
                        302,-202,-496,-411};

// 2000 Hz  -> 20 Samples sampled every 125 us ----> [1 / (125 us * 20 samples)] * 5 cycles = 2000 Hz
int16_t adcArray2[20] = {0, 512, 0, -512, 0,
                        512, 0, -512, 0, 512,
                        0, -512, 0, 512, 0,
                        -512, 0, 512, 0, -512};                        
                        
// 1600 Hz  -> 20 Samples sampled every 125 us ----> [1 / (125 us * 20 samples)] * 4 cycles = 1600 Hz
int16_t adcArray3[20] = {0, 486, 300, -301, -487,
                        0, 486, 300, -301, -487,
                        0, 486, 300, -301, -487,
                        0, 486, 300, -301, -487};

uint32_t goertzel_power_q14(const int16_t *x, int N, int16_t coeff_q14)
{
  int32_t s_prev = 0;
  int32_t s_prev2 = 0;

  for (int i = 0; i < N; i++) {
    // s = x + coeff*s_prev - s_prev2
    int32_t prod = ( (int32_t)coeff_q14 * s_prev ) >> 14;
    int32_t s = (int32_t)x[i] + prod - s_prev2;
    s_prev2 = s_prev;
    s_prev  = s;
  }

  // power ≈ s_prev^2 + s_prev2^2 - coeff*s_prev*s_prev2
  // keep in 64-bit for safety
  int64_t sp  = s_prev;
  int64_t sp2 = s_prev2;

  int64_t term = ( (int64_t)coeff_q14 * sp * sp2 ) >> 14;
  int64_t power = sp*sp + sp2*sp2 - term;

  if (power < 0) power = 0;
  if (power > 0xFFFFFFFF) power = 0xFFFFFFFF;
  return (uint32_t)power;
}

void setup() {

  Serial.begin(9600);
  Serial.println();

  // Test relative power of 1200 Hz and 2000 Hz given a 1200 Hz ADC array
  gPwr = goertzel_power_q14(adcArray, N, COEFF_Q14_1200HZ);
  gPwr2 = goertzel_power_q14(adcArray, N, COEFF_Q14_2000HZ);

  Serial.print("Magnitude of 1200Hz Signal with 1200Hz input: ");
  Serial.println(gPwr);
  Serial.print("Magnitude of 2000Hz Signal with 1200Hz input: ");
  Serial.println(gPwr2);

  // Test relative power of 1200 Hz and 2000 Hz given a 2000 Hz ADC array
  gPwr = goertzel_power_q14(adcArray2, N, COEFF_Q14_1200HZ);
  gPwr2 = goertzel_power_q14(adcArray2, N, COEFF_Q14_2000HZ);

  Serial.println();
  Serial.print("Magnitude of 1200Hz Signal with 2000Hz input: ");
  Serial.println(gPwr);
  Serial.print("Magnitude of 2000Hz Signal with 2000Hz input: ");
  Serial.println(gPwr2);

  gPwr = goertzel_power_q14(adcArray3, N, COEFF_Q14_1200HZ);
  gPwr2 = goertzel_power_q14(adcArray3, N, COEFF_Q14_2000HZ);
  
  // Test relative power of 1200 Hz and 2000 Hz given a 1600 Hz ADC array
  Serial.println();
  Serial.print("Magnitude of 1200Hz Signal with 1600Hz input: ");
  Serial.println(gPwr);
  Serial.print("Magnitude of 2000Hz Signal with 1600Hz input: ");
  Serial.println(gPwr2);

}

void loop() {
  // put your main code here, to run repeatedly:

}
