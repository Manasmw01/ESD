/*
 * Userspace program that communicates with the aud and vga_zylo device driver
 * through ioctls
 * radomly generates notes at top of screen at fixed intervals
 * reads from hardware the detected note and checks if it matches the note currently in the green zone
 * Alex Yu, Rajat Tyagi, Sienna Brent, Riona Westphal
 * Columbia University
 */

#include <stdio.h>
#include "interfaces.h"
#include "vga_zylo.h"
#include "aud.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <math.h>
#define SAMPLING_RATE 48000
#define X_MAX 639 
#define Y_MAX 479
#define N 8192
int vga_zylo_fd;
int aud_fd;
float frequency;
typedef struct {
    double real;
    double imag;
} Complex;


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

// Function to perform FFT
void FFT(Complex* input, int n) {
    if (n <= 1)
        return;

    // Divide
    Complex even[n/ 2];
    Complex odd[n/ 2];

    for (int i = 0; i < n / 2; ++i) {
        even[i] = input[2 * i];
        odd[i] = input[2 * i + 1];
    }

    // Conquer
    FFT(even, n/ 2);
    FFT(odd, n/ 2);

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
}


void updateBall(sprite *obj) {
	obj->x += obj->dx;
	obj->y += obj->dy;
	if (obj->x < 0 || obj->x >= X_MAX) {
		obj->y = 380;
		obj->x = 700;
		obj->id = 0;
		obj->dy = 0;
		obj->dx = 0;
    }
	if (obj->y < 0 || obj->y > Y_MAX) {
		// obj->dy = -obj->dy;
		obj->id = 0;
		obj->dy = 0;
	}
	// if () {
	// 	obj->dy = -obj->dy;
	// 	obj->id = 0;
	// }
}

void scorecombosetup(sprite *sprites) {
	//index 0 acts strangly
	//'SCORE'
	sprites[1].id = 17; //S
	sprites[2].id = 12; //C
	sprites[3].id = 15; //O
	sprites[4].id = 16; //R
	sprites[5].id = 13; //E
	for (int i = 1; i < 6; i++) {
		sprites[i].x = 480+32*(i-1); 
		sprites[i].y = 40;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].hit = 1;
		sprites[i].index = i;
	}
	sprites[6].id = 10; //0
	sprites[7].id = 10; //0
	sprites[8].id = 10; //0
	for (int i = 6; i < 9; i++) {
		sprites[i].x = 480+32+32*(i-6); 
		sprites[i].y = 90;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].hit = 1;
		sprites[i].index = i;
	}

	//'COMBO'
	sprites[9].id =  12; //C
	sprites[10].id = 15; //O
	sprites[11].id = 14; //M
	sprites[12].id = 11; //B
	sprites[13].id = 15; //O
	for (int i = 9; i < 14; i++) {
		sprites[i].x = 480+32*(i-9); 
		sprites[i].y = 140;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].hit = 1;
		sprites[i].index = i;
	}
	sprites[14].id = 10; //0
	sprites[15].id = 10; //0
	sprites[16].id = 10; //0
	for (int i = 14; i < 17; i++) {
		sprites[i].x = 480+32+32*(i-14); 
		sprites[i].y = 190;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].hit = 1;
		sprites[i].index = i;
	}
	
	//'MAX'
	sprites[17].id = 14; //M
	sprites[18].id = 26; //A
	sprites[19].id = 27; //X
	for (int i = 17; i < 20; i++) {
		sprites[i].x = 480+32+32*(i-17); 
		sprites[i].y = 240;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].hit = 1;
		sprites[i].index = i;
	}
	sprites[20].id = 10; //0
	sprites[21].id = 10; //0
	sprites[22].id = 10; //0
	for (int i = 20; i < 23; i++) {
		sprites[i].x = 480+32+32*(i-20); 
		sprites[i].y = 290;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].hit = 1;
		sprites[i].index = i;
	}
}

void update_combo(sprite *sprites, const int combo) {
	int huds = (int)combo/100;
	int tens = (int)(combo - huds*100)/10;
	int ones = combo - huds*100 - tens*10;
	if (huds == 0) huds = 10;
	if (tens == 0) tens = 10;
	if (ones == 0) ones = 10;
	sprites[14].id = huds; //100s	
	sprites[15].id = tens; //10s
	sprites[16].id = ones; //1s
	return;
}

void update_score(sprite *sprites, const int score) {
	int huds = (int)score/100;
	int tens = (int)(score - huds*100)/10;
	int ones = score - huds*100 - tens*10;
	if (huds == 0) huds = 10;
	if (tens == 0) tens = 10;
	if (ones == 0) ones = 10;
	sprites[6].id = huds; //100s	
	sprites[7].id = tens; //10s
	sprites[8].id = ones; //1s
	return;
}

void update_max(sprite *sprites, const int max) {
	int huds = (int)max/100;
	int tens = (int)(max - huds*100)/10;
	int ones = max - huds*100 - tens*10;
	if (huds == 0) huds = 10;
	if (tens == 0) tens = 10;
	if (ones == 0) ones = 10;
	sprites[20].id = huds; //100s	
	sprites[21].id = tens; //10s
	sprites[22].id = ones; //1s
	return;
}
// dedicate all sprites below 

// spawns a block with sprite.id depending on note
void spawnnote(sprite* sprites, int note) {
	//scans sprite array for empty sprite
	if (note == 0) return;
	int i, j;
	for (i = 23; i < SIZE; i++) {
	    if (sprites[i].id == 0) break;
	}
	for (j = i+1; j < SIZE; j++) {
	    if (sprites[j].id == 0) break;
	}
	// change sprite information to match note
	sprites[i].x = 28 + 120*(note-1); 
	sprites[i].y = 0;
	sprites[i].dx = 0;  
	sprites[i].dy = 1; 
	sprites[i].id = note*2 + 16;
	sprites[i].hit = 0;
	sprites[i].index = i;
	sprites[j].x = 60 + 120*(note-1); 
	sprites[j].y = 0;
	sprites[j].dx = 0;  
	sprites[j].dy = 1; 
	sprites[j].id = note*2 + 17;
	sprites[j].hit = 0;
	sprites[j].index = j;
	return;
}


// return sprite id in region, -1 if none
int check_valid_region(sprite* sprites, int start) {
    int i;
    //int cf = *combo_flag;
    for (i = start; i < SIZE; i++) {
	    if ((sprites[i].y > 330) && (sprites[i].y < 410) && (sprites[i].id != 0) && (sprites[i].hit == 0)) {
	        //if (sprites[i].y == 480) cf = 0;
	        return i;
	    }
	}
	return -1;
}

void screen_refresh(sprite* sprites) {
    for (int i = 1; i < SIZE; i++) {
		sprites[i].x = 630; 
		sprites[i].y = 470;
		sprites[i].dx = 0;  
		sprites[i].dy = 0; 
		sprites[i].id = 0;
		sprites[i].hit = 1;
		sprites[i].index = i;
	}
	return;
}
void addToBuffer(std::vector<int>& buffer, const int data) {
     
	// Check if buffer is not empty and the last element is equal to the new data
    if ((!buffer.empty()) && (buffer.back() == data)) {
        // Data is same as the last data entered, don't add it
		// std::cout << "Buffer empty: " <<buffer.empty() << std::endl;
		// std::cout << "Buffer: " <<buffer.back() << std::endl;
		// std::cout << "Data: " <<data << std::endl;
        //std::cout << "Data is same as the last data entered, not added.\n" << std::endl;
        return;
    }
	buffer.push_back(data);
   // Check if buffer size exceeds the maximum size
    if (buffer.size() > N) {
        // Remove the least recent data from the beginning
        buffer.clear();
		buffer.push_back(data);

    }

}
// simple game of hitting random falling notes when they reach the green zone
int main()
{
    
	vga_zylo_arg_t vzat;
	std::vector<int> buffer;
	aud_arg_t aat;
	aud_mem_t amt;

	srand(time(NULL));
	float fft_output[N];
	static const char filename1[] = "/dev/vga_zylo";
	static const char filename2[] = "/dev/aud";

	printf("VGA zylo test Userspace program started\n");
	printf("%d\n", sizeof(int));	
	printf("%d\n", sizeof(short));

	if ((vga_zylo_fd = open(filename1, O_RDWR)) == -1) {
		fprintf(stderr, "could not open %s\n", filename1);
		return -1;
	}
	if ((aud_fd = open(filename2, O_RDWR)) == -1) {
		fprintf(stderr, "could not open %s\n", filename2);
		return -1;
	}
 	//FILE *fp = fopen("test.txt", "w");
	//if (fp == NULL)	return -1;
	
	sprite *sprites = NULL;	
	// sprites = calloc(SIZE, sizeof(*sprites));

	sprites = (sprite*)calloc(SIZE,sizeof(*sprites));

	screen_refresh(sprites);
	scorecombosetup(sprites);
	Complex signal[N];

	int score = 0;
	int combo = 0;
	//packet of sprite data to send
	vga_zylo_data_t vzdt;
	
	int combo_flag = 1; 
	int counter = 0; 	
	int gamecounter = 0;
    int validleft, validright;
    int max = 0;
    int hitcount = 0;
	int noteCount = 0;  
	int MAX_NOTE_COUNT = 100;   
	FILE *fptr;
	fptr = fopen("sound.txt", "w");
	if (fptr == NULL) {
			printf("Unable to open file.\n");
			return 1;
	}
	int iter = 0;
	
	while (1) {

		if ((counter%10)==0) {
		    gamecounter++;
		    // printf("%d\n", gamecounter);
		}

		if ((counter%132)==0) {
		    if(noteCount < MAX_NOTE_COUNT) 
		    	spawnnote(sprites, (rand() % 5));
		    noteCount++; 
		}
		
		validleft = check_valid_region(sprites, 23);
		validright = check_valid_region(sprites, validleft+1);


		amt.data = get_aud_data(aud_fd);
		if (sprites[validleft].y == 399) {
		    combo_flag = 0;
		    combo = 0;
	    }

		//printf("AUD DATA: %d\n", amt.data);
		addToBuffer(buffer, amt.data);
	
		if(buffer.size() == N){
			//printf("buffer: %d\n", buffer[0]);
			for (int i = 0; i < N; i++){
				signal[i].real = buffer[i];
        		signal[i].imag = 0;
			}
			FFT(signal, N);
			for (int i = 0; i < N; i++) {
				fft_output[i] = sqrt(signal[i].real * signal[i].real + signal[i].imag * signal[i].imag);
			}

			int max_value = 0;
			int peak_index = 0;
			for (int i = 0; i < 700; i++) {
				if (fft_output[i] > max_value) {
					max_value = fft_output[i];
					peak_index = i;
				}
			}
			//printf("Max Value: %d\n", max_value);
			float sample_rate = 48000; // Sample rate, change accordingly if your sample rate is different
			frequency = (float)peak_index * sample_rate / N;

			// Find the index of the nearest piano note
			//int note_index = find_nearest_note_index(frequency);
			// printf("The note played is: %.2f Hz, which is approximately %dth note on a piano.\n", frequency);
			printf("%.2f Hz: \tNote: %d\n", frequency, score);
		}

		fprintf(fptr, "%d\n", amt.data);
		if (1) {
		// if ((amt.data == (1+(sprites[validleft].id-17)>>1)) && (sprites[validleft].id!=0)) {
		    hitcount++;
		    // sprites[validleft].y = 481;
		    // sprites[validright].y = 481;
		    if (hitcount == 8) {
		        hitcount = 0;
		        sprites[validleft].hit = 1;
		        sprites[validright].hit = 1;
		        sprites[validleft].dy = 0;
		        sprites[validright].dy = 0;
		        sprites[validleft].dx = 10;
		        sprites[validright].dx = 10;
		        
		        if (combo_flag) {
		            combo++;
		            if (combo > max)
		                max = combo;
		        }
				// socre = frequency;
				if(frequency < 200)
					score = 1;
				else if (frequency >= 200 && frequency <= 400)
					score = 2;
				else if (frequency >= 400 && frequency <= 600)
					score = 3;
				else if (frequency >= 600 && frequency <= 800)
					score = 4;
				else if (frequency >= 800 && frequency <= 1000)
					score = 5;
				else if (frequency >= 1000 && frequency <= 1200)
					score = 6;
				else if (frequency >= 1200 && frequency <= 1400)
					score = 7;
				else if (frequency >= 1400 && frequency <= 1600)
					score = 8;
				else if (frequency >= 1600 && frequency <= 1800)
					score = 9;
				else if (frequency >= 1800 && frequency <= 2000)
					score = 10;
		        // score = score + 1 + combo/5;
		    }
 		}
 	    else 
 	        hitcount = 0;
 		
 		//update_score(sprites, amt.data);
		//update_combo(sprites, 1+(sprites[validleft].id-17)>>1);
		update_score(sprites, score);
		update_combo(sprites, combo);
		update_max(sprites, max);
	
		//package the sprites together
		for (int i = 0; i < SIZE; i++) {
			vzdt.data[i] = (sprites[i].index<<26) + (sprites[i].id<<20) + (sprites[i].y<<10) + (sprites[i].x<<0);
		}
		//send package to hardware
		send_sprite_positions(&vzdt, vga_zylo_fd);
		//update spirtes x and y based on dx and dy on software side
		for (int i = 0; i < SIZE; i++) {
			updateBall(&sprites[i]);
		}
		combo_flag = 1;
		//pause to let hardware catch up
		counter++;

		iter++;
	}
		
	return 0;
}
