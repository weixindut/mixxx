#ifndef DMXETH_H
#define DMXETH_H

#include <arpa/inet.h>

#define KINET_UDP_PORT 6038
#define KINET_VERSION 0x0001
#define KINET_MAGIC 0x4adc0104
#define KTYPE_DMXOUT 0x0101

#ifndef Byte
#define Byte unsigned char
#endif
#ifndef Word
#define Word unsigned short
#endif
#ifndef DWord
#define DWord unsigned int
#endif


// typedef struct {
//     unsigned int magic;
//     unsigned short ver;
//     unsigned short type;
//     unsigned int seq;
// } KiNET_Hdr;

// typedef struct {
//     KiNET_Hdr hdr;
//     unsigned char port;
//     unsigned char flags;
//     unsigned short timerVal;
//     unsigned int uni;
//     //DWord numChannels;
//     // need to add dmx data here
// } KiNET_DMXout;

typedef struct {
  DWord magic;
  Word ver;
  Word type;
  DWord seq;
} KiNET_Hdr;

typedef struct {
  KiNET_Hdr hdr;
  Byte port;
  Byte flags;
  Word timerVal;
  DWord uni;
  //DWord numChannels;
  // need to add dmx data here
} KiNET_DMXout;

typedef struct {
    int sock;
    struct sockaddr_in destsa;
    unsigned char dmx_state[1024];
} DMX_Handle;

DMX_Handle* new_dmx_handle(char* address);
void free_dmx_handle(DMX_Handle* pHandle);
void send_dmx_data(DMX_Handle* pHandle, unsigned char *data, int dlen);
void set_dmx_color(DMX_Handle* pHandle, int light,
                   unsigned char r, unsigned char g, unsigned char b);
void publish_dmx_update(DMX_Handle* pHandle);

#endif /* DMXETH_H */

