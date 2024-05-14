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

void send_limit(const aud_mem_t *c, const int aud_fd) {
	aud_arg_t amt;
	amt.memory = *c;
	if (ioctl(aud_fd, AUD_WRITE_LIMIT, &amt)) {
		perror("ioctl(AUD_WRITE_LIMIT) failed");
		return;
	}
}
void send_address(const aud_mem_t *c, const int aud_fd) {
	aud_arg_t aat;
	aat.memory = *c;
	if (ioctl(aud_fd, AUD_WRITE_ADDRESS, &aat)) {
		perror("ioctl(AUD_WRITE_ADDRESS) failed");
		return;
	}
}
void send_mode(const aud_mem_t *c, const int aud_fd) {
	aud_arg_t aat;
	aat.memory = *c;
	if (ioctl(aud_fd, AUD_WRITE_MODE, &aat)) {
		perror("ioctl(AUD_WRITE_ADDRESS) failed");
		return;
	}
}
int get_aud_data(const int aud_fd) {
	aud_arg_t aat;
	if (ioctl(aud_fd, AUD_READ_DATA, &aat)) {
		perror("ioctl(AUD_READ_DATA) failed");
		return 0;
	}
	return aat.memory.data;
}

void send_sprite_positions(const vga_zylo_data_t *c, const int vga_zylo_fd) {
	vga_zylo_arg_t vzat;
	vzat.packet = *c;
	if (ioctl(vga_zylo_fd, VGA_ZYLO_WRITE_PACKET, &vzat)) {
		perror("ioctl(VGA_ZYLO_WRITE_PACKET) failed");
		return;
	}
}
void send_score(const vga_zylo_data_t *c, const int vga_zylo_fd) {
	vga_zylo_arg_t vzat;
	vzat.packet = *c;
	if (ioctl(vga_zylo_fd, VGA_ZYLO_WRITE_SCORE, &vzat)) {
		perror("ioctl(VGA_ZYLO_WRITE_SCORE) failed");
		return;
	}
}
void send_combo(const vga_zylo_data_t *c, const int vga_zylo_fd) {
	vga_zylo_arg_t vzat;
	vzat.packet = *c;
	if (ioctl(vga_zylo_fd, VGA_ZYLO_WRITE_COMBO, &vzat)) {
		perror("ioctl(VGA_ZYLO_WRITE_COMBO) failed");
		return;
	}
}
