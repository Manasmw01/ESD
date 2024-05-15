/*
 * Userspace program that communicates with the aud and vga_zylo device driver
 * through ioctls
 * radomly generates notes at top of screen at fixed intervals
 * reads from hardware the detected note and checks if it matches the note currently in the green zone
 * Alex Yu, Rajat Tyagi, Sienna Brent, Riona Westphal
 * Columbia University
 */
#define SHOW_SPRITES
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
int done = 0;
typedef struct {
    double real;
    double imag;
} Complex;


const float piano_notes[] = {
        27.50, 30.87, 32.70, 36.71, 38.89, 43.65, 46.25, 51.91, 
        55.00, 61.74, 65.41, 73.42, 77.78, 87.31, 92.50, 103.83, 
        110.00, 123.47, 130.81, 146.83, 155.56, 174.61, 185.00, 207.65, 
        220.00, 246.94, 261.63, 293.66, 311.13, 349.23, 369.99, 415.30, 
        440.00, 493.88, 523.25, 587.33, 622.25, 698.46, 783.99, 830.61, 
        880.00, 987.77, 1046.50, 1174.66, 1244.51, 1396.91, 1567.98, 1661.22, 
        1760.00, 1975.53, 2093.00, 2349.32, 2489.02, 2793.83, 3135.96, 3322.44, 
        3520.00, 3951.07, 4186.01
    };

const char* note_names[] = {
        "A0", "B0", "C1", "D1", "E1", "F1", "G1", 
        "A1", "B1", "C2", "D2", "E2", "F2", "G2", 
        "A2", "B2", "C3", "D3", "E3", "F3", "G3", 
        "A3", "B3", "C4", "D4", "E4", "F4", "G4", 
        "A4", "B4", "C5", "D5", "E5", "F5", "G5", 
        "A5", "B5", "C6", "D6", "E6", "F6", "G6", 
        "A6", "B6", "C7", "D7", "E7", "F7", "G7", 
        "A7", "B7", "C8"
    };

const float midpoints[] = {
    29.185, 31.785, 34.705, 37.80, 41.27, 44.95, 49.08, 53.455, 58.37, 63.575, 
    69.415, 75.60, 82.545, 89.905, 98.165, 106.915, 116.735, 127.14, 138.82, 
    151.195, 165.085, 179.805, 196.325, 213.825, 233.47, 254.285, 277.645, 
    302.395, 330.18, 359.61, 392.645, 427.65, 466.94, 508.565, 555.29, 604.79, 
    660.355, 741.225, 807.30, 855.305, 933.885, 1017.135, 1110.58, 1209.585, 
    1320.71, 1482.445, 1614.60, 1710.61, 1867.765, 2034.265, 2221.16, 2419.17, 
    2641.425, 2964.895, 3229.20, 3421.22, 3735.535, 4068.54
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
	
	while (1) {

#ifdef SHOW_SPRITES
		if ((counter%10)==0) {
		    // gamecounter++;
		    // printf("%d\n", gamecounter);
		}

		// if ((counter%132)==0) {
		//     if(noteCount < MAX_NOTE_COUNT) 
		//     	spawnnote(sprites, (rand() % 5));
		//     noteCount++; 
		// }
		
		validleft = check_valid_region(sprites, 23);
		validright = check_valid_region(sprites, validleft+1);
		// if (sprites[validleft].y == 399) {
		    // combo_flag = 0;
		    // combo = 0;
	    // }
#endif


		amt.data = get_aud_data(aud_fd);

		//printf("AUD DATA: %d\n", amt.data);
		done = 0;
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
			done = 1;


		if (done == 1) {
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
			if (frequency < midpoints[0]) {
        		score = 0;
    		} else if (frequency >= midpoints[0] && frequency < midpoints[1]) {
        		score = 1;
    		} else if (frequency >= midpoints[1] && frequency < midpoints[2]) {
        		score = 2;
   		 	} else if (frequency >= midpoints[2] && frequency < midpoints[3]) {
        		score = 3;
    		} else if (frequency >= midpoints[3] && frequency < midpoints[4]) {
        		score = 4;
    		} else if (frequency >= midpoints[4] && frequency < midpoints[5]) {
        		score = 5;
    		} else if (frequency >= midpoints[5] && frequency < midpoints[6]) {
        		score = 6;
    		} else if (frequency >= midpoints[6] && frequency < midpoints[7]) {
        		score = 7;
    		} else if (frequency >= midpoints[7] && frequency < midpoints[8]) {
        		score = 8;
    		} else if (frequency >= midpoints[8] && frequency < midpoints[9]) {
        		score = 9;
    		} else if (frequency >= midpoints[9] && frequency < midpoints[10]) {
        		score = 10;
    		} else if (frequency >= midpoints[10] && frequency < midpoints[11]) {
        		score = 11;
    		} else if (frequency >= midpoints[11] && frequency < midpoints[12]) {
        		score = 12;
    		} else if (frequency >= midpoints[12] && frequency < midpoints[13]) {
        		score = 13;
		    } else if (frequency >= midpoints[13] && frequency < midpoints[14]) {
        		score = 14;
		    } else if (frequency >= midpoints[14] && frequency < midpoints[15]) {
        		score = 15;
		    } else if (frequency >= midpoints[15] && frequency < midpoints[16]) {
        		score = 16;
		    } else if (frequency >= midpoints[16] && frequency < midpoints[17]) {
        		score = 17;
		    } else if (frequency >= midpoints[17] && frequency < midpoints[18]) {
        		score = 18;
		    } else if (frequency >= midpoints[18] && frequency < midpoints[19]) {
        		score = 19;
		    } else if (frequency >= midpoints[19] && frequency < midpoints[20]) {
        		score = 20;
		    } else if (frequency >= midpoints[20] && frequency < midpoints[21]) {
        		score = 21;
		    } else if (frequency >= midpoints[21] && frequency < midpoints[22]) {
        		score = 22;
		    } else if (frequency >= midpoints[22] && frequency < midpoints[23]) {
        		score = 23;
		    } else if (frequency >= midpoints[23] && frequency < midpoints[24]) {
        		score = 24;
		    } else if (frequency >= midpoints[24] && frequency < midpoints[25]) {
        		score = 25;
		    } else if (frequency >= midpoints[25] && frequency < midpoints[26]) {
        		score = 26;
		    } else if (frequency >= midpoints[26] && frequency < midpoints[27]) {
        		score = 27;
		    } else if (frequency >= midpoints[27] && frequency < midpoints[28]) {
        		score = 28;
		    } else if (frequency >= midpoints[28] && frequency < midpoints[29]) {
        		score = 29;
		    } else if (frequency >= midpoints[29] && frequency < midpoints[30]) {
        		score = 30;
		    } else if (frequency >= midpoints[30] && frequency < midpoints[31]) {
        		score = 31;
		    } else if (frequency >= midpoints[31] && frequency < midpoints[32]) {
        		score = 32;
		    } else if (frequency >= midpoints[32] && frequency < midpoints[33]) {
        		score = 33;
		    } else if (frequency >= midpoints[33] && frequency < midpoints[34]) {
        		score = 34;
		    } else if (frequency >= midpoints[34] && frequency < midpoints[35]) {
        		score = 35;
		    } else if (frequency >= midpoints[35] && frequency < midpoints[36]) {
        		score = 36;
		    } else if (frequency >= midpoints[36] && frequency < midpoints[37]) {
        		score = 37;
		    } else if (frequency >= midpoints[37] && frequency < midpoints[38]) {
        		score = 38;
		    } else if (frequency >= midpoints[38] && frequency < midpoints[39]) {
        		score = 39;
		    } else if (frequency >= midpoints[39] && frequency < midpoints[40]) {
        		score = 40;
		    } else if (frequency >= midpoints[40] && frequency < midpoints[41]) {
        		score = 41;
		    } else if (frequency >= midpoints[41] && frequency < midpoints[42]) {
        		score = 42;
		    } else if (frequency >= midpoints[42] && frequency < midpoints[43]) {
        		score = 43;
		    } else if (frequency >= midpoints[43] && frequency < midpoints[44]) {
        		score = 44;
    		} else if (frequency >= midpoints[44] && frequency < midpoints[45]) {
        		score = 45;
		    } else if (frequency >= midpoints[45] && frequency < midpoints[46]) {
        		score = 46;
		    } else if (frequency >= midpoints[46] && frequency < midpoints[47]) {
        		score = 47;
		    } else if (frequency >= midpoints[47] && frequency < midpoints[48]) {
        		score = 48;
		    } else if (frequency >= midpoints[48] && frequency < midpoints[49]) {
        		score = 49;
		    } else if (frequency >= midpoints[49] && frequency < midpoints[50]) {
        		score = 50;
		    } else if (frequency >= midpoints[50] && frequency < midpoints[51]) {
        		score = 51;
		    } else if (frequency >= midpoints[51] && frequency < midpoints[52]) {
        		score = 52;
		    } else if (frequency >= midpoints[52] && frequency < midpoints[53]) {
        		score = 53;
		    } else if (frequency >= midpoints[53] && frequency < midpoints[54]) {
        		score = 54;
		    } else if (frequency >= midpoints[54] && frequency < midpoints[55]) {
        		score = 55;
		    } else if (frequency >= midpoints[55] && frequency < midpoints[56]) {
        		score = 56;
    		} else {	
        		hitcount = 0;
    		}
		    }
 		}
		}

		fprintf(fptr, "%d\n", amt.data);
#ifdef SHOW_SPRITES
 		
 		//update_score(sprites, amt.data);
		//update_combo(sprites, 1+(sprites[validleft].id-17)>>1);
		update_score(sprites, score);
		// update_combo(sprites, combo);
		// update_max(sprites, max);
	
		//package the sprites together
		// for (int i = 0; i < SIZE; i++) {
		// 	vzdt.data[i] = (sprites[i].index<<26) + (sprites[i].id<<20) + (sprites[i].y<<10) + (sprites[i].x<<0);
		// }
		//send package to hardware
		// send_sprite_positions(&vzdt, vga_zylo_fd);
		//update spirtes x and y based on dx and dy on software side
		// for (int i = 0; i < SIZE; i++) {
		// 	updateBall(&sprites[i]);
		// }
		combo_flag = 1;
		//pause to let hardware catch up
		counter++;
#endif

	}
		
	return 0;
}

