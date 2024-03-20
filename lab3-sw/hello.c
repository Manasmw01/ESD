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

int vga_ball_fd;

/* Read and print the background color */
void print_background_color() {
  vga_ball_arg_t vla;
  
  if (ioctl(vga_ball_fd, VGA_BALL_READ_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_READ_BACKGROUND) failed");
      return;
  }
  printf("%02x %02x %02x\n",
	 vla.background.red, vla.background.green, vla.background.blue);
}

/* Set the background color */
void set_background_color(const vga_ball_color_t *c)
{
  vga_ball_arg_t vla;
  vla.background = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
      return;
  }
}

void set_ball_coordinates(const vga_ball_coordinates *c)
{
  vga_ball_arg_t vla;
  vla.coordinates = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_COORDINATE, &vla)) {
      perror("ioctl(VGA_BALL_SET_COORDINATES) failed");
      return;
  }
}

int main()
{
  vga_ball_arg_t vla;
  int i;
  static const char filename[] = "/dev/vga_ball";

  static const vga_ball_color_t colors[] = {
    { 0xff, 0x00, 0x00 }, /* Red */
    { 0x00, 0xff, 0x00 }, /* Green */
    { 0x00, 0x00, 0xff }, /* Blue */
    { 0xff, 0xff, 0x00 }, /* Yellow */
    { 0x00, 0xff, 0xff }, /* Cyan */
    { 0xff, 0x00, 0xff }, /* Magenta */
    { 0x80, 0x80, 0x80 }, /* Gray */
    { 0x00, 0x00, 0x00 }, /* Black */
    { 0xff, 0xff, 0xff }  /* White */
  };

# define COLORS 9

  printf("VGA ball Userspace program started\n");

  if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  printf("initial state: ");
  print_background_color();
  vga_ball_coordinates coordinates;

  //x-coordinate most sigficant 20
  //y-coordinate max is 15
  int MAX_Y = 14;
  int MAX_X = 18;
  coordinates.x = 5;
  coordinates.y = 5;
  int incy = 1;
  int incx = 1;
  while(1) {
    set_ball_coordinates(&coordinates);
    if(coordinates.y+1 > MAX_Y){
      incy = -1;
    }
    if(coordinates.y-1 < 0){
      incy = 1;
    }
    if(coordinates.x+1 > MAX_X){
      incx = -1;
    }
    if(coordinates.x-1 < 0){
      incx = 1;
    }
    coordinates.x+= incx;
    coordinates.y += incy;
    usleep(400000);
  }
  
  printf("VGA BALL Userspace program terminating\n");
  return 0;
}
