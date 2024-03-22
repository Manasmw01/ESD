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
    { 0x00, 0xCf, 0x00 }, /* Green */
//    { 0x00, 0x00, 0xff }, /* Blue */
    { 0xff, 0xff, 0x00 }, /* Yellow */
    { 0x00, 0xff, 0xff }, /* Cyan */
    { 0xff, 0x00, 0xff }, /* Magenta */
    { 0x80, 0x80, 0x80 }, /* Gray */
    { 0x00, 0x00, 0x00 }, /* Black */
    { 0xff, 0xff, 0xff }  /* White */
  };

# define COLORS 8

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
  int MAX_Y = 480-20;
  int MAX_X = 640-20;
//  coordinates.x = 320;
//  coordinates.y = 240;
    coordinates.x = 20;
    coordinates.y = 20;
  int inc_xy = 1;
  int incy = inc_xy;
  int incx = inc_xy;
  while(1) {
    set_background_color(&colors[1]);
    set_ball_coordinates(&coordinates);
    if(coordinates.y+1 > MAX_Y){
      incy = -inc_xy;
      printf("x: %d, y: %d, i = %d\n",coordinates.x, coordinates.y , i);
    }
    if(coordinates.y-1 < 20){
      printf("x: %d, y: %d, i = %d\n",coordinates.x, coordinates.y , i);
      incy = inc_xy;
    }
    if(coordinates.x+1 > MAX_X){
      incx = -inc_xy;
      printf("x: %d, y: %d, i = %d\n",coordinates.x, coordinates.y , i);
    }
    if(coordinates.x-1 < 20){
      printf("x: %d, y: %d, i = %d\n",coordinates.x, coordinates.y , i);
      incx = inc_xy;
    }
    coordinates.x+= incx;
    coordinates.y += incy;
    usleep(15000);
  }
  
  printf("VGA BALL Userspace program terminating\n");
  return 0;
}
