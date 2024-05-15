#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define N 8192
#define SAMPLING_RATE 44100.0

// Complex number structure
typedef struct {
    double real;
    double imag;
} Complex;


void removeAdjacentDuplicates(int arr[], int n, int result[]) {
    int j = 0; // Index for the result array

    // Iterate through the original array
    for (int i = 0; i < 15000; i++) {
        // If the current element is not equal to the next element (or it's the last element),
        // copy it to the result array
        if (j>N){
            return;
        }
        if (arr[i] != arr[i + 1]) {
            result[j] = arr[i];
            j++;
        }
    }
}

// Function to perform FFT
void FFT(Complex* input, int n) {
    if (n <= 1)
        return;

    // Divide
    Complex *even = (Complex*)malloc(n / 2 * sizeof(Complex));
    Complex *odd = (Complex*)malloc(n / 2 * sizeof(Complex));

    for (int i = 0; i < n / 2; ++i) {
        even[i] = input[2 * i];
        odd[i] = input[2 * i + 1];
    }

    // Conquer
    FFT(even, n / 2);
    FFT(odd, n / 2);

    // Combine
    for (int k = 0; k < n / 2; ++k) {
        double theta = -2 * M_PI * k / n;
        Complex twiddle = {cos(theta), sin(theta)};
        Complex temp = {0}; // Temporary variable for calculation

        // Apply twiddle factor to odd part
        temp.real = odd[k].real * twiddle.real - odd[k].imag * twiddle.imag;
        temp.imag = odd[k].real * twiddle.imag + odd[k].imag * twiddle.real;

        // Combine even and odd parts
        input[k].real = even[k].real + temp.real;
        input[k].imag = even[k].imag + temp.imag;
        input[k + n / 2].real = even[k].real - temp.real;
        input[k + n / 2].imag = even[k].imag - temp.imag;
    }

    free(even);
    free(odd);
}

// int main() {
//     // Open file for reading
//     FILE *file = fopen("sine_wave.txt", "r");
//     if (file == NULL) {
//         printf("Error opening file!\n");
//         return 1;
//     }

//     // Read data from file
//     int inputSignal[120000];
//     int count = 0;
//     while (count < 120000) {
//         fscanf(file, "%d", &inputSignal[count]);
//         count++;
//     }

//     int resultArray[N];

//     removeAdjacentDuplicates(inputSignal, N, resultArray);
//      for (int i = 0; i < N; i++) {
//         printf("%d\n",resultArray[i]);
//      }
//     fclose(file);

//     // Convert the input signal to Complex
//     Complex signal[N];
//     for (int i = 0; i < N; ++i) {
//         signal[i].real = resultArray[i];
//         signal[i].imag = 0;
//     }



//     // Perform FFT
//     FFT(signal, N);

//     // Calculate frequency resolution
//     double delta_f = SAMPLING_RATE / N;

//     // Save FFT result to a file
//     FILE *f = fopen("fft_output.txt", "w");
//     if (f == NULL) {
//         printf("Error opening file!\n");
//         return 1;
//     }

//     // Write FFT result to the file
//     for (int i = 0; i < N; ++i) {
//         // Calculate frequency corresponding to this bin
//         double frequency = i * delta_f;
//         //fprintf(f, "%.2lfHz: %lf %lf\n", frequency, signal[i].real, signal[i].imag);
//         fprintf(f, "%lf %lf\n", signal[i].real, signal[i].imag);
//     }

//     fclose(f);
//     printf("FFT result saved to fft_output.txt\n");

//     return 0;
// }
