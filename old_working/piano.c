#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define AMPLITUDE 4294967296 // Maximum amplitude for 16-bit audio

int main() {
    FILE *file;
    file = fopen("sine_wave.txt", "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    double frequency = 830.0; // Frequency of the sine wave (440 Hz for A4 note)
    double duration = 2.0; // Duration of the sine wave in seconds
    int num_samples = duration * SAMPLE_RATE;
    double increment = frequency * 2.0 * M_PI / SAMPLE_RATE; // Increment for each sample

    for (int i = 0; i < num_samples; i++) {
        double sample = sin(increment * i) * AMPLITUDE;
        fprintf(file, "%.0f\n", sample); // Write sample to file
    }

    fclose(file);
    printf("Sine wave saved to sine_wave.txt\n");

    return 0;
}
