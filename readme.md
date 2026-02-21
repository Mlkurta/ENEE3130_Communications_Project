# Goertzel Algorithm on Arduino

I recently discovered a way to measure the relative power of a particular frequency of an an ADC-sampled analog signal.

It's called the Goertzel algorithm.  Unlike the Discrete Fourier Transform, the Goertzel can be useful for things like specific tone detection or FSK demodulation, where you only want to measure the relative power of a few frequencies (ideally N < 10 or so before FFT wins). 

This simple program demonstrates:

1. An algorithm which is efficient without using floating points.

2. 3 Arrays of sine wave values (You can change the frequency of all of them by modifying the Q_14 coefficient).
   
3. Comparison of the relative power of 3 different input sample frequencies.

## Calculating the Q_14 coefficient

This is the fixed point coefficient which changes based on your target frequency, number of samples, and sample frequency.

1. Find k value:  $k = \frac{(f * N_s)}{F_s}$

   where $f = Target Frequency; N_s = Number Of Samples; F_s = Sample Frequency$

   example: $\frac{1200Hz * 20}{8000} = k = 3$

2. Find base coefficient: $\omega = \frac{(2*\pi*k)}{N}$,   where: $coeff = 2\cos{\omega}$

   example: $\omega = frac{2*\pi*3}{20} = .9425$

   $coeff = 2\cos{\omega} = 1.1756$

3. Multiply base coefficient by 16384 and round, or $2^{14}$

   Q_14 = $1.1756 * 2^{14} = 19262$


## Program

The program outputs the following in the serial terminal:

<img width="919" height="248" alt="Screenshot 2026-02-21 123525" src="https://github.com/user-attachments/assets/985ef856-5bc1-45d5-be5e-7d707a577502" />

The function: goertzel_power_q14(const int16_t *x, int N, int16_t coeff_q14)

const int16_t *x = pointer to array of 16 bit signed integers (ADC input).

int N = number of samples.

int16_t coeff_q_14 = int16 cast of Q_14 coefficient.

Returns a 32 bit unsigned integer representing relative power.

You must run the algorithm once for every target frequency. So for FSK demodulation, you would run it for twice, once for each frequency, and compare the two return values; whichever is higher is your bit ( f_lower > f_higher = '0', f_higher > f_lower = '1').

Note that in the above terminal example, a 1600 Hz input is used, and a Goertzel is ran for 1200 Hz and 2000 Hz, and the results are predictably identical. My guess is the 2000 Hz returns a higher number due to quantization.

*Lastly, if running for a real application, be sure to normalize the DC offset (already pre-computed here by subtracting 512 from each sample).





