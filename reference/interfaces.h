#ifndef _VGA_ZYLO_H_
#define _VGA_ZYLO_H_

#include <linux/ioctl.h>
#include "vga_zylo.h"
#include "aud.h"

void send_limit(const aud_mem_t *c, const int aud_fd);

void send_address(const aud_mem_t *c, const int aud_fd);

void send_mode(const aud_mem_t *c, const int aud_fd);

int get_aud_data(const int aud_fd);

void send_sprite_positions(const vga_zylo_data_t *c, const int vga_zylo_fd);

void send_score(const vga_zylo_data_t *c, const int vga_zylo_fd);

void send_combo(const vga_zylo_data_t *c, const int vga_zylo_fd);

#endif
