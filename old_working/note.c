#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define piano note frequencies (in Hz)
const float piano_notes[] = {
    27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, // Octave 1
    55.0, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826,     // Octave 2
    110.0, 116.541, 123.471, 130.813, 138.591, 146.832, 155.564, 164.814, 174.614, 184.997, 195.998, 207.652, // Octave 3
    220.0, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, // Octave 4
    440.0, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, // Octave 5
    880.0, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, // Octave 6
    1760.0, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.964, 3322.438, // Octave 7
    3520.0, 3729.31, 3951.066, 4186.009 // Octave 8
};

// Function to find index of the nearest piano note to a given frequency
int find_nearest_note_index(float frequency) {
    int index = 0;
    float min_diff = fabs(frequency - piano_notes[0]);
    for (int i = 1; i < sizeof(piano_notes) / sizeof(piano_notes[0]); i++) {
        float diff = fabs(frequency - piano_notes[i]);
        if (diff < min_diff) {
            min_diff = diff;
            index = i;
        }
    }
    return index;
}
