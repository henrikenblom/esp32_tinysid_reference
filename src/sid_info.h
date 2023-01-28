//
// Created by Henrik Enblom on 2023-01-19.
//

#ifndef UNTITLED_SID_INFO_H
#define UNTITLED_SID_INFO_H

struct sid_info
{
    uint16_t version;
    uint16_t load_addr;
    uint16_t init_addr;
    uint16_t play_addr;
    uint8_t subsongs;
    uint8_t start_song;
    uint8_t speed;
    char title[32];
    char author[32];
    char released[32];
};


#endif //UNTITLED_SID_INFO_H
