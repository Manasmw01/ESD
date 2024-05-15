#ifndef _VGA_ZYLO_H
#define _VGA_ZYLO_H

#include <linux/ioctl.h>

//number of supported sprites
#define SIZE 48

typedef struct {
  int data[SIZE];
  int score;
  int combo;
} vga_zylo_data_t;
  
typedef struct {
  	vga_zylo_data_t packet;
} vga_zylo_arg_t;

typedef struct {
	int x, y, dx, dy, id, index, hit;
} sprite;
#define VGA_ZYLO_MAGIC 'q'

/* ioctls and their arguments */
#define VGA_ZYLO_WRITE_PACKET _IOW(VGA_ZYLO_MAGIC, 5, vga_zylo_arg_t *)
#define VGA_ZYLO_WRITE_SCORE _IOW(VGA_ZYLO_MAGIC, 6, vga_zylo_arg_t *)
#define VGA_ZYLO_WRITE_COMBO _IOW(VGA_ZYLO_MAGIC, 7, vga_zylo_arg_t *)
#define VGA_ZYLO_READ_PACKET _IOR(VGA_ZYLO_MAGIC, 8, vga_zylo_arg_t *)

#endif
