#ifndef _VGA_BALL_H
#define _VGA_BALL_H

#include <linux/ioctl.h>

typedef struct {
  unsigned int l;
} audio_samples_t;

typedef struct {
  int audio_ready;
} audio_ready_t;


typedef struct {
  int write;
} audio_data_t;


typedef struct {
  audio_samples_t samples;
  audio_ready_t ready;
  audio_data_t data;
} vga_ball_arg_t;


#define VGA_BALL_MAGIC 'q'

/* ioctls and their arguments */
#define VGA_BALL_WRITE_BACKGROUND _IOW(VGA_BALL_MAGIC, 1, vga_ball_arg_t *)
#define VGA_BALL_READ_BACKGROUND  _IOR(VGA_BALL_MAGIC, 2, vga_ball_arg_t *)
#define VGA_BALL_WRITE_HV _IOW(VGA_BALL_MAGIC, 3, vga_ball_arg_t *)
#define VGA_BALL_READ_HV _IOR(VGA_BALL_MAGIC, 4, vga_ball_arg_t *)
#define AUDIO_READ_SAMPLES _IOR(VGA_BALL_MAGIC, 5, vga_ball_arg_t *)
#define AUDIO_READ_SAMPLES_X _IOR(VGA_BALL_MAGIC, 6, vga_ball_arg_t *)


#endif
