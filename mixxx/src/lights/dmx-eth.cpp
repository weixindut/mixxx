#include <QtDebug>

#include "lights/dmx-eth.h"

DMX_Handle* new_dmx_handle(char* address) {
    DMX_Handle * handle = new DMX_Handle();
    if ((handle->sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        qDebug() << "Couldn't initialize DMX socket.";
    }
    handle->destsa.sin_family = AF_INET;
    handle->destsa.sin_addr.s_addr = inet_addr(address);
    handle->destsa.sin_port = htons(KINET_UDP_PORT);
    memset(handle->dmx_state, 0, sizeof(handle->dmx_state));
    handle->dmx_state[512] = 255;
    handle->dmx_state[513] = 191;
    return handle;
}

void free_dmx_handle(DMX_Handle* pHandle) {
    delete pHandle;
}

void send_dmx_data(DMX_Handle* pHandle, unsigned char *data, int dlen) {
    int i;
    int len;
    unsigned char buf[2048];
    KiNET_DMXout *kdmxout=(KiNET_DMXout *)buf;

    kdmxout->hdr.magic=KINET_MAGIC;
    kdmxout->hdr.ver=KINET_VERSION;
    kdmxout->hdr.type=KTYPE_DMXOUT;
    kdmxout->hdr.seq=0;
    kdmxout->port=0;
    kdmxout->timerVal=0;
    kdmxout->uni=-1;
    len=sizeof(KiNET_DMXout)+dlen;
    memcpy(buf+sizeof(KiNET_DMXout),data,dlen);
    if (sendto(pHandle->sock, buf, len, 0,
               (struct sockaddr *)&(pHandle->destsa),
               sizeof(pHandle->destsa)) == -1) {
        qDebug() << "Error sending DMX message.";
    }
}

void set_dmx_color(DMX_Handle* pHandle, int light,
                   unsigned char r, unsigned char g, unsigned char b) {
    pHandle->dmx_state[3*light + 1] = r;
    pHandle->dmx_state[3*light + 2] = g;
    pHandle->dmx_state[3*light + 3] = b;
}

void publish_dmx_update(DMX_Handle* pHandle) {
    send_dmx_data(pHandle, pHandle->dmx_state, 514);
}
