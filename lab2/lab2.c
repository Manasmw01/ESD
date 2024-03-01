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
  int keycode; 

 if(keycode0!= 0 && keycode1!= 0 ){
   keycode = keycode1;
 }
  if(keycode1!= 0){
    keycode = keycode1;
  }
  else{
    keycode = keycode0;
  }
  if( keycode >= 04 && keycode<= 0x1d && modifiers == 0 ){
    return keycode+93;
  }
  if( keycode >= 04 && keycode<= 0x1d && (modifiers == 2|| modifiers == 0x20)){
    return keycode+61;
  }
  if( keycode >= 0x1e && keycode<= 0x26 && modifiers == 0){
    return keycode+19;
  }
  if( keycode == 0x27 && modifiers == 0){
    return keycode+9;
  }
  if( keycode == 0x36 && modifiers == 0){
    return 44;
  }
  if( keycode== 0x37 && modifiers == 0){
    return 46;
  }
  if( keycode == 0x34 && modifiers == 0){
    return 39;
  }

  if( keycode == 0x50 && modifiers == 0){
    return -3; // LEFT
  }
  if( keycode == 0x4F && modifiers == 0){
    return -4; // RIGHT
  }

  if(keycode== 0x2a){
    return -1;
  }
  if(keycode== 0x2c){
    return 32;
  }
  if(keycode== 0x28){
    return -2;
  }


if( keycode == 0x1e  && modifiers == 2||modifiers == 0x20){
    return 33;
  }
  if( keycode == 0x1f  && modifiers == 2||modifiers == 0x20){
    return 64;
  }
  if( keycode == 0x20  && modifiers == 2||modifiers == 0x20){
    return 35;
  }
  if( keycode == 0x21  && modifiers == 2||modifiers == 0x20){
    return 36;
  }
  if( keycode == 0x22  && modifiers == 2||modifiers == 0x20){
    return 37;
  }
  if( keycode == 0x23  && modifiers == 2||modifiers == 0x20){
    return 94;
  }
  if( keycode == 0x24  && modifiers == 2||modifiers == 0x20){
    return 38;
  }
  if( keycode == 0x25  && modifiers == 2||modifiers == 0x20){
    return 42;
  }
  if( keycode == 0x26  && modifiers == 2||modifiers == 0x20){
    return 40;
  }
  if( keycode == 0x27  && modifiers == 2||modifiers == 0x20){
    return 41;
  }

  //-
  if( keycode == 0x2d && modifiers == 0){
    return keycode;
  }
  //=
    if( keycode == 0x2e && modifiers == 0){
    return keycode+15;}
  //_
  if( keycode == 0x2d && modifiers == 2||modifiers == 0x20){
    return keycode+50;
  }
    //+
    if( keycode == 0x2e && modifiers == 2||modifiers == 0x20){
        return keycode-3; }
     //[
     if( keycode == 0x2f && modifiers == 0){
    return keycode+44;}
    //]
    if( keycode == 0x30 && modifiers == 0){
    return keycode+45;}
    //{
     if( keycode == 0x2f && modifiers == 2||modifiers == 0x20){
        return keycode+76;}
     //}
     if( keycode == 0x30 && modifiers == 2||modifiers == 0x20){
        return keycode+77; }

    //;
     if( keycode == 0x33 && modifiers == 0){
    return 59;}
    //'
    if( keycode == 0x34 && modifiers == 0){
    return 39;}
    //"\"
    if( keycode == 0x31 && modifiers == 0){
    return 92;}
    //:
     if( keycode == 0x33 && modifiers == 2||modifiers == 0x20){
        return 58;}
     //""
     if( keycode == 0x34 && modifiers == 2||modifiers == 0x20){
        return 34; }
    //|
     if( keycode == 0x31 && modifiers == 2||modifiers == 0x20){
        return 124; }

     //,
     if( keycode == 0x36 && modifiers == 0){
    return 44;}
    //.
    if( keycode == 0x37 && modifiers == 0){
    return 46;}
    ///
    if( keycode == 0x38 && modifiers == 0){
    return 47;}
    //<
     if( keycode == 0x36 && modifiers == 2||modifiers == 0x20){
        return 60;}
     //>
     if( keycode == 0x37 && modifiers == 2||modifiers == 0x20){
        return 62; }
    //?
     if( keycode == 0x38 && modifiers == 2||modifiers == 0x20){
        return 63; }
//`
    if( keycode == 0x35 && modifiers == 0){
    return 96;}
  //~
  if( keycode == 0x35 && modifiers == 2||modifiers == 0x20){
   return 126;
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
  char SENDbuff[130] =  "";

  char keystate[130] = "";
  char keystate0[65] = "";
  int size = 0;
  for (;;) {
    //keystate[cursor] = '_';
    libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, sizeof(packet),
			      &transferred, 0);
      
    if (transferred == sizeof(packet) && (packet.keycode[0] != 0 || packet.keycode[1] != 0 || packet.modifiers!= 0) && !(packet.keycode[0] == 0x00 && packet.keycode[1] == 0 && packet.modifiers == 0x20) && !(packet.keycode[0] == 0x00 && packet.keycode[1] == 0 && packet.modifiers == 0x02)  ) {
      int c = keycode_to_ascii(packet.modifiers, packet.keycode[0],
	    packet.keycode[1]);
      if(c >= 0){
        if( size == 128){
          continue;
        }
        printf("enter\n");
        if(cursor!=cols)
        {
          for(int i = cols+1; i>= cursor; i--)
          {
            //keystate[i] = keystate[i-1];          
            SENDbuff[i] = SENDbuff[i-1]; 
            keystate[i] = keystate[i-1];          
                     
          }
          SENDbuff[cursor] = c;
          keystate[cursor] = c;
          size++;
        }
        else
        {
          SENDbuff[size] = c;
          keystate[size] = c;
          keystate[size+1] = '_';
          size++;
        }
        if(size <64){
          fbputs(keystate, rows, 0);
        }
        else{
          strncpy(keystate0, keystate, 64*sizeof(char));
          keystate0[64] = '\0';
          fbputs(keystate0, rows, 0);
          fbputs(&keystate[64], rows+1, 0);
        }
        cols++;
        cursor++;
        printf("%02x %02x %02x\n", packet.modifiers, packet.keycode[0],
        packet.keycode[1]);
      }

      else if (c == -1){
        if(cols == 0 && rows == 13){
          continue; 
        }
        if(cols == -1)
        {
          cols = 63;
          rows--;
          cursor = 63;
        }
        if(cursor != cols)
        {
          for(int i = cursor; i<= size; i++)
          {
            SENDbuff[i-1] = SENDbuff[i];          
            keystate[i-1] = keystate[i];          
          }
          printf("%s", keystate);
          keystate[size] = ' ';
          SENDbuff[size-1] = ' ';
          size--;
        if(size <64){
        fbputs(keystate, rows, 0);
        }
        else{
          strncpy(keystate0, keystate, 64*sizeof(char));
          keystate0[64] = '\0';
          fbputs(keystate0, rows, 0);
          fbputs(&keystate[64], rows+1, 0);
        }
        // SENDbuff[cursor] = "";
        }
        else
        {
        size--;
        SENDbuff[size] = ' ';
        keystate[size] = '_';
        keystate[size+1] = ' ';
        if(size <64){
          fbputs(keystate, rows, 0);
        }
        else{
          strncpy(keystate0, keystate, 64*sizeof(char));
          keystate0[64] = '\0';
          fbputs(keystate0, rows, 0);
          fbputs(&keystate[64], rows+1, 0);
        }
        }
        cols--;
        cursor--;
      }
      else if (c == -2){
        for (int r = 13; r < 24; r++){
          for (int co = 0; co < 64; co++){
            fbputs(" ", r, co);
          }
        }
        rows = 13;
        cols = 0;
        cursor = 0;
        write(sockfd, SENDbuff, size);
        for (int i = 0; i <= size; i++){
            printf("%c", keystate[i]);
        }
        for (int i = 0; i <= 130; i++){
          SENDbuff[i] = '\0';
          keystate[i] = '\0';
        }
        printf("%s\n", keystate);

        size = 0;
      }
      else if (c == -3){ // LEFT
        if(cursor == 0){
          continue;
        }
          char temp = keystate[cursor-1];
          keystate[cursor-1] =  keystate[cursor];
          keystate[cursor] = temp;
          if(cursor != 0)
          {
            cursor--;
          }
          
        if(size <64){
          fbputs(keystate, rows, 0);
        }
        else{
          strncpy(keystate0, keystate, 64*sizeof(char));
          keystate0[64] = '\0';
          fbputs(keystate0, rows, 0);
          fbputs(&keystate[64], rows+1, 0);
        }

      }
      else if (c == -4){ // right 
        if(cursor == cols){
          continue;
        }
          char temp = keystate[cursor];
          keystate[cursor] =  keystate[cursor+1];
          keystate[cursor+1] = temp;
          if(cursor != cols)
          {
            cursor++;
          }
        if(size <64){
          fbputs(keystate, rows, 0);
        }
        else{
          strncpy(keystate0, keystate, 64*sizeof(char));
          keystate0[64] = '\0';
          fbputs(keystate0, rows, 0);
          fbputs(&keystate[64], rows+1, 0);
        }

      }


      // if (cols == 64){
      //   rows++;
      //   cols = 0;
      // }
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
  int j = 1;
  while ( (n = read(sockfd, &recvBuf, BUFFER_SIZE - 1)) > 0 ) {
    if(j == 1)
    {
      for (int r = 1 ; r<12; r++){
        for(int c = 0; c<64; c++){
          fbputs(" ", r, c);
        }
      }
    }
    recvBuf[n] = '\0';
    printf("%s", recvBuf);
    if (n <64){
      for (int col = 0 ; col < 64 ; col++) {
        fbputchar(' ', j, col);
      }
      fbputs(recvBuf, j, 0);
      
    }
    else if (n>64 && n<128){
      char recBuf0[65] = "";
      strncpy(recBuf0, recvBuf, 64);
      recBuf0[64] = '\0';
      if(j == 11){
        j = 1;
        for (int r = 1 ; r<12; r++){
          for(int c = 0; c<64; c++){
              fbputs(" ", r, c);
          }
        }
      }
      fbputs(recBuf0, j, 0);
      fbputs(&recvBuf[64], ++j, 0);

    }
    else{
      // char recBuf0[65] = "";
      // char recBuf0[6] = "";

      // strncpy(recBuf0, recvBuf, 64);
      // recBuf0[64] = '\0';1
      // fbputs(recBuf0, j, 0);
      // if(j == 11){
      //   j = 0;
      //   fbputs(&recvBuf[64], j, 0);
      // }
      // else{
      //   fbputs(&recvBuf[64], ++j, 0);

      // }

    }
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
      j = 1;
    }

  }

  return NULL;
}