/*
 *
 * CSEE 4840 Lab 2 for 2019
 *
 * Name/UNI: Please Changeto Yourname (pcy2301)
 */
#include "fbputchar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "usbkeyboard.h"
#include <pthread.h>

/* Update SERVER_HOST to be the IP address of
 * the chat server you are connecting to
 */
/* arthur.cs.columbia.edu */
#define SERVER_HOST "128.59.19.114"
#define SERVER_PORT 42000

#define BUFFER_SIZE 128

/*
 * References:
 *
 * https://web.archive.org/web/20130307100215/http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 *
 * http://www.thegeekstuff.com/2011/12/c-socket-programming/
 * 
 */

int sockfd; /* Socket file descriptor */

struct libusb_device_handle *keyboard;
uint8_t endpoint_address;

pthread_t network_thread;
void *network_thread_f(void *);
int keycode_to_ascii(int modifiers,int keycode0, int keycode1){
  if( keycode0 >= 04 && keycode0<= 0x1d && modifiers == 0 ){
    return keycode0+93;
  }
  if( keycode0 >= 04 && keycode0<= 0x1d && (modifiers == 2||modifiers == 0x20)){
    return keycode0+61;
  }
  if( keycode0 >= 0x1e && keycode0<= 0x26 && modifiers == 0){
    return keycode0+19;
  }
  if( keycode0 == 0x27 && modifiers == 0){
    return keycode0+9;
  }
  if( keycode0 == 0x36 && modifiers == 0){
    return 44;
  }
  if( keycode0 == 0x37 && modifiers == 0){
    return 46;
  }
  if( keycode0 == 0x34 && modifiers == 0){
    return 39;
  }

  if( keycode0 == 0x50 && modifiers == 0){
    return -3; // LEFT
  }
  if( keycode0 == 0x4F && modifiers == 0){
    return -4; // RIGHT
  }

  if(keycode0== 0x2a){
    return -1;
  }
  if(keycode0== 0x2c){
    return 32;
  }
  if(keycode0== 0x28){
    return -2;
  }
}
int main()
{
  int err, col;

  struct sockaddr_in serv_addr;

  struct usb_keyboard_packet packet;
  int transferred;
  // char keystate[128];

  if ((err = fbopen()) != 0) {
    fprintf(stderr, "Error: Could not open framebuffer: %d\n", err);
    exit(1);
  }
  for (int i = 0 ; i<24; i++){
    for(int j = 0; j<64; j++){
      fbputs(" ", i, j);
    }
  }

  /* Draw rows of asterisks across the top and bottom of the screen */
  for (col = 0 ; col < 64 ; col++) {
    fbputchar('_', 12, col);
  }

  /* Open the keyboard */
  if ( (keyboard = openkeyboard(&endpoint_address)) == NULL ) {
    fprintf(stderr, "Did not find a keyboard\n");
    exit(1);
  }
    
  /* Create a TCP communications socket */
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    fprintf(stderr, "Error: Could not create socket\n");
    exit(1);
  }

  /* Get the server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  if ( inet_pton(AF_INET, SERVER_HOST, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "Error: Could not convert host IP \"%s\"\n", SERVER_HOST);
    exit(1);
  }

  /* Connect the socket to the server */
  if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: connect() failed.  Is the server running?\n");
    exit(1);
  }

  /* Start the network thread */
  pthread_create(&network_thread, NULL, network_thread_f, NULL);
  int cols= 0;
  int rows = 13;
  int cursor= 0;


  /* Look for and handle keypresses */
  char SENDbuff[128] = "";

  char keystate[128] = "";
  int size = 0;
  for (;;) {
    libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, sizeof(packet),
			      &transferred, 0);
      
    if (transferred == sizeof(packet) && (packet.keycode[0] != 0 || packet.keycode[1] != 0 || packet.modifiers!= 0) && !(packet.keycode[0] == 0x00 && packet.keycode[1] == 0 && packet.modifiers == 0x20) && !(packet.keycode[0] == 0x00 && packet.keycode[1] == 0 && packet.modifiers == 0x02)  ) {
      int c = keycode_to_ascii(packet.modifiers, packet.keycode[0],
	    packet.keycode[1]);
      if(c >= 0){
        printf("\n");
        if(cursor!=cols)
        {
          char SENDbuff_tmp[128];
          // SENDbuff_tmp = SENDbuff;
          strncpy(SENDbuff_tmp, SENDbuff, sizeof(SENDbuff_tmp));
          size++;
          char keystate_tmp[128];
          strncpy(keystate_tmp, keystate, sizeof(keystate_tmp));
          for(int i = cols; i>= cursor; i--)
          {
          // SENDbuff_tmp = SENDbuff;
            keystate[i] = keystate_tmp[i-1];          
            SENDbuff[i] = SENDbuff_tmp[i-1];          
            SENDbuff[cursor] = c;
            keystate[cursor] = c;
          }
            printf("%c", keystate[cursor]);
          // printf("\n");
        // sprintf(keystate, "%c", c);
        // printf("%s\n", keystate);
        }
        else
        {
        SENDbuff[size] = c;
        // keystate[size] = c;
        size++;
        sprintf(keystate, "%c", c);
        // for (int i = 0; i <= size; i++){
        //     printf("%c", keystate[i]);
        // }
        printf("\n");
        // printf("%s\n", keystate);
        }

        if(cursor!=cols)
        {
          fbputs(keystate, rows, cursor);
            cols++;
         cursor ++;;
        }
        else
        {
          fbputs(keystate, rows, cols);
          cols++;
         cursor = cols;
        }
        printf("%02x %02x %02x\n", packet.modifiers, packet.keycode[0],
        packet.keycode[1]);
      }



      else if (c == -1){
        if(cols == 0 && rows == 13){
          continue; 
        }
        // if(cursor != cols)
        // {
        // cursor--;
        // }
        cols--;
        if(cols == -1)
        {
          cols = 63;
          rows--;
          cursor = 63;
        }
        if(cursor != cols)
        {
          char SENDbuff_tmp[128];
          strncpy(SENDbuff_tmp, SENDbuff, sizeof(SENDbuff_tmp));

          char keystate_tmp[128];
          strncpy(keystate_tmp, keystate, sizeof(keystate_tmp));

        fbputs(" ", rows, cursor);
          for(int i = cursor; i<= cols; i++)
          {
            SENDbuff[i] = SENDbuff_tmp[i+1];          
            keystate[i] = keystate_tmp[i+1];          
            // SENDbuff[cursor] = c;
          }
          size--;
        // SENDbuff[cursor] = "";
        }
        else
        {
        fbputs(" ", rows, cols);
        size--;
        SENDbuff[size] = "";
        keystate[size] = "";
        }
      }
      else if (c == -2){
        for (int r = 13; r < 24; r++){
          for (int co = 0; co < 64; co++){
            fbputs(" ", r, co);
          }
          rows = 13;
          cols = 0;
          cursor = 0;
        }
        write(sockfd, SENDbuff, size);
        for (int i = 0; i <= size; i++){
            printf("%c", keystate[i]);
        }
        for (int i = 0; i <= size; i++){
          SENDbuff[i] = "";
          keystate[i] = "";
        }
        size = 0;
      }
      else if (c == -3){ // LEFT
        if(cursor != 0)
        {
          cursor--;
        }
       fbputs("|", rows, cursor);        
      }

      if (cols == 64){
        rows++;
        cols = 0;
      }
      if(rows == 24)
      {
        rows = 13;
      }
      
      if (packet.keycode[0] == 0x29) { /* ESC pressed? */
	break;
      }
    }
  }

  /* Terminate the network thread */
  pthread_cancel(network_thread);

  /* Wait for the network thread to finish */
  pthread_join(network_thread, NULL);

  return 0;
}

void *network_thread_f(void *ignored)
{
  char recvBuf[BUFFER_SIZE];
  char recvBuf2[BUFFER_SIZE/2];
  int n;
  /* Receive data */
  int j = 0;
  while ( (n = read(sockfd, &recvBuf, BUFFER_SIZE - 1)) > 0 ) {
    if(j == 0)
    {
      for (int i = 0 ; i<12; i++){
        for(int j = 0; j<64; j++){
          fbputs(" ", i, j);
        }
      }
    }
    recvBuf[n] = '\0';
    printf("%s", recvBuf);
    // if (n <64){
      for (int col = 0 ; col < 64 ; col++) {
        fbputchar(' ', j, col);
      }
      fbputs(recvBuf, j, 0);
    // }
    // else{
    //   for (int col = 0 ; col < 64 ; col++) {
    //     fbputchar(' ', j, col);
    //   }
    //   for (int col = 0 ; col < 64 ; col++) {
    //     fbputchar(' ', j+1, col);
    //   }
    //   for (int i = 0; i<64; i++){
    //     recvBuf2[i] = recvBuf[i];
    //     recvBuf[i] = recvBuf[i+64];
    //   }

    //   fbputs(recvBuf2, j, 0);
    //   fbputs(recvBuf, ++j, 0);
    // }
    j++;
    if (j ==12)
    {
      j = 0;
    }

  }

  return NULL;
}

