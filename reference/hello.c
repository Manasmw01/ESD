/*
 * Userspace program that communicates with the aud and vga_vylo device driver
 * through ioctls
 * current amplitude will be represented as the y position of the sprite from vga_vylo
 * reads audio and then sends amplituded
 * ayu2126
 * Columbia University
 */

#include <stdio.h>
#include "vga_zylo.h"
#include "aud.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define X_MAX 640 
#define Y_MAX 480

int vga_zylo_fd;
int aud_fd;

void send_limit(const aud_mem_t *c) {
	aud_arg_t amt;
	amt.memory = *c;
	if (ioctl(aud_fd, AUD_WRITE_LIMIT, &amt)) {
		perror("ioctl(AUD_WRITE_LIMIT) failed");
		return;
	}
}
void send_address(const aud_mem_t *c) {
	aud_arg_t aat;
	aat.memory = *c;
	if (ioctl(aud_fd, AUD_WRITE_ADDRESS, &aat)) {
		perror("ioctl(AUD_WRITE_ADDRESS) failed");
		return;
	}
}
void send_mode(const aud_mem_t *c) {
	aud_arg_t aat;
	aat.memory = *c;
	if (ioctl(aud_fd, AUD_WRITE_MODE, &aat)) {
		perror("ioctl(AUD_WRITE_ADDRESS) failed");
		return;
	}
}
int get_aud_data() {
	aud_arg_t aat;
	if (ioctl(aud_fd, AUD_READ_DATA, &aat)) {
		perror("ioctl(AUD_READ_DATA) failed");
		return 0;
	}
	return aat.memory.data;
}

void send_sprite_positions(const vga_zylo_data_t *c) {
	vga_zylo_arg_t vzat;
	vzat.packet = *c;
	if (ioctl(vga_zylo_fd, VGA_ZYLO_WRITE_PACKET, &vzat)) {
		perror("ioctl(VGA_ZYLO_WRITE_PACKET) failed");
		return;
	}
}
void send_score(const vga_zylo_data_t *c) {
	vga_zylo_arg_t vzat;
	vzat.packet = *c;
	if (ioctl(vga_zylo_fd, VGA_ZYLO_WRITE_SCORE, &vzat)) {
		perror("ioctl(VGA_ZYLO_WRITE_SCORE) failed");
		return;
	}
}
void send_combo(const vga_zylo_data_t *c) {
	vga_zylo_arg_t vzat;
	vzat.packet = *c;
	if (ioctl(vga_zylo_fd, VGA_ZYLO_WRITE_COMBO, &vzat)) {
		perror("ioctl(VGA_ZYLO_WRITE_COMBO) failed");
		return;
	}
}

void updatesprite(sprite *obj) {
	obj->x += obj->dx;
	obj->y += obj->dy;
	
	if (obj->x < 1 || obj->x >= X_MAX)
		obj->dx = -obj->dx;

	if (obj->y < 1 || obj->y >= Y_MAX)
		obj->dy = -obj->dy;
}

/*
//get received note from audio, see which of four notes it is
//get a string in return which we compare with what we get from song.txt
//not sure what parameter to give and how to go about converting audio sample to note_id.
char* audio_received() {
    	//example
    	char *note_string = malloc(6);
    	strcopy(note_string,"0001");

    	return note_string;
}*/

//compare received note with the actual note required
int compare_note(char* actual_note, char* received_note) {
    	int note_same;
    	int i;
    	for (i = 0; i < sizeof(actual_note); i++) {
        	if (actual_note[i] != received_note[i]) {
            		note_same = 0;
            		break;
        	} else {
            		note_same = 1;
        	}
    	}
    	return note_same;
}


int main()
{
	vga_zylo_arg_t vzat;
	
	aud_arg_t aat;
	aud_mem_t amt;

	int i;
	sprite sprite_obj0 = {.x = 123, .y =  42, .dx = -1, .dy = -1};
	sprite sprite_obj1 = {.x = 423, .y = 211, .dx = -1, .dy = 1};
	sprite sprite_obj2 = {.x =  10, .y = 123, .dx = 1,  .dy = -1};
	sprite sprite_obj3 = {.x = 532, .y = 271, .dx = 1,  .dy = 1};
	mem mem_obj = {.data = 0, .address = 0, .limit = 48000, .mode = 1};

	static const char filename1[] = "/dev/vga_zylo";
	static const char filename2[] = "/dev/aud";

	printf("VGA sprite Userspace program started\n");
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
 	FILE *fp = fopen("test.txt", "w");
	if (fp == NULL)	return -1;

	vga_zylo_data_t vzdt;
 	
	while (1) {	
		vzdt.data[0] = sprite_obj0.x + (sprite_obj0.y<<10) + (1<<20) + (1<<26);
		vzdt.data[1] = sprite_obj1.x + (sprite_obj1.y<<10) + (1<<20) + (2<<26);
		vzdt.data[2] = sprite_obj2.x + (sprite_obj2.y<<10) + (1<<20) + (3<<26);
		vzdt.data[3] = sprite_obj3.x + (sprite_obj3.y<<10) + (1<<20) + (4<<26);
		//printf("%d, %d\n", sprite_obj3.x, sprite_obj3.y);
		printf("%08x\n", vzdt.data[3]);
		send_sprite_positions(&vzdt);
		updatesprite(&sprite_obj0);
		updatesprite(&sprite_obj1);
		updatesprite(&sprite_obj2);
		updatesprite(&sprite_obj3);

		usleep(20000);
	}



	// amt.size = (int) mem_obj.limit;
	// amt.mode = (int) mem_obj.mode;
	// usleep(1200000);
	// send_mode(&amt);
	// send_limit(&amt);
	// usleep(12000000);
	// int amp = 0;
	// amt.size = 0;
	// // send_limit(&amt);
	// for(int counter = 0; counter < amt.size; counter++) {
	// 		mem_obj.address = counter;
	// 		amt.address = mem_obj.address;
	// 	//printf("%d: ", amt.address);
	// 		send_address(&amt);	
	// 		amt.data = get_data();
	// 		amp = amt.data;
	// 	//printf("%d\n", amt.data);
	// 		fprintf(fp, "%08x\n", amp);
	// }
	// fclose(fp);
	
	
	/*very simple game logic to compare notes. Will need changes for audio_received function and rate of testing especially*//*
	score = 0;
    	combo = 0;
	
	//read song.txt line by line to get the notes that should be played.
    	FILE *textfile;
    	char line[6];
    	char *incoming_note;

   	textfile = fopen("song.txt","r");
	if (textfile == NULL) return -1;

    	//need to change rate at which we test
    	while (fgets(line, sizeof(line), textfile)) {
        	line[strcspn(line,"\n")] = '\0';

        	//see what the incoming note is, convert it to a string
        	incoming_note = audio_received(); //see what parameter to give
		
		//compare incoming note with expected note
        	same_note = compare_note(line,incoming_note);
        
       		if (same_note == 1) {
            		combo += 1;
            		score += 10 + (5*combo);
            		//change sprite color to show correct note played?
        	} else {
            		combo = 0;
            		//change sprite color to show wrong note played?
        	}
        	free(incoming_note);

        	//how to change rate at which we compare notes?
		//can we use usleep to delay the loop?
    	}
	
	
	printf("VGA sprite Userspace program terminating\n");
	*/
	return 0;
}
