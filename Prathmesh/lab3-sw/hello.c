/*
 * Userspace program that communicates with the vga_ball device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "vga_ball.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define S_RATE  (44100)
#define REAL_S_RATE (44100)
#define BUF_SIZE (2000) /* 5 second buffer for L/R */
 
int vga_ball_fd;
long unsigned int buffer[BUF_SIZE];
int idx;
/* Read and print the background color */
// void print_background_color() {
//   vga_ball_arg_t vla;
  
//   if (ioctl(vga_ball_fd, VGA_BALL_READ_BACKGROUND, &vla)) {
//       perror("ioctl(VGA_BALL_READ_BACKGROUND) failed");
//       return;
//   }
//   printf("%02x %02x %02x\n",
// 	 vla.background.red, vla.background.green, vla.background.blue);
// }

void read_samples() {
    vga_ball_arg_t vla;
    printf("read_samples\n");
    if (ioctl(vga_ball_fd, AUDIO_READ_SAMPLES_X, &vla)) {
        perror("ioctl(AUDIO_READ_SAMPLES) failed");
        return;
    }
		
		buffer[idx++] = vla.samples.l;
}



// /* Set the background color */
void set_background_color(const audio_data_t *data)
{
  vga_ball_arg_t vla;
  vla.data = *data;
      if (ioctl(vga_ball_fd, WRITE_CONFIG, &vla)) {
      perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
      return;
  }
}

// void set_hv(const vga_ball_hv_t *c)
// {
//   vga_ball_arg_t vla;
//   vla.hv = *c;
//   if (ioctl(vga_ball_fd, VGA_BALL_WRITE_HV, &vla)) {
//       perror("ioctl(VGA_BALL_SET_HV) failed");
//       return;
//   }
// }


int main()
{
  vga_ball_arg_t vla;
  int i;
  static const char filename[] = "/dev/audio";
  idx = 0;
  printf("VGA ball Userspace program started\n");

  if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }
  printf(" Opened /dev/audio\n ");
  audio_data_t data;
  data.write = 1; 
  set_background_color(&data);



  while(idx < BUF_SIZE){
      read_samples();
  }

  printf("sample read done, before write_wa\n");
  for (int j = 0; j < 100; j++){
    printf("samps:");
    for (i = 0; i < 10; i++){
      printf("%lu\t", buffer[j*10+i]);
    }
    printf("\n");
  }

  // printf("initial state: ");
  // //print_background_color();
  // unsigned int radius = 10;
  // int h_count = radius;
  // int v_count = radius;
  // bool h_flag = false;
  // bool v_flag = false;
  // for (i = 0 ; i < 20000; i++) {
  //   set_background_color(&colors[2]);
  //   hv_val.h = h_count;
  //   hv_val.v = v_count;

  //   if(v_count < 480-radius &&v_flag == false){
	// v_count++;
  //   }
 
  //   if(v_count >= 480-radius || v_flag == true){
	// v_count--;
  //       v_flag = true;
  //      if(v_count == radius)
	// v_flag = false;
  //   }

  //   if(h_count < 640-radius && h_flag == false){
  //       h_count++;
  //   }
  //   if(h_count >= 640-radius || h_flag == true){
  //       h_count--;
	// h_flag = true;
	// if(h_count == radius)
	//  h_flag = false;
  //   }


  //   set_hv(&hv_val);
  //   print_background_color();
  //   usleep(9000);
    
  //}


  
  printf("VGA BALL Userspace program terminating\n");
  return 0;
}
