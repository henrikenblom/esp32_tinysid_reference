/*
 * sid.h
 *
 *  Created on: 14 lip 2016
 *      Author: Korzo
 */

#ifndef SID_SID_H_
#define SID_SID_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sid_info.h"

//----------------------------------------------

void synth_init(unsigned long mixfrq);

void sidPoke(int reg, unsigned char val);

void cpuReset(void);

void cpuResetTo(unsigned short npc, unsigned char na);

void cpuJSR(unsigned short npc, unsigned char na);

void c64Init(int nSampleRate);

bool is_PSID(void *data);

bool sid_load_from_memory(void *data, size_t size, struct sid_info *info);

void set_volume(int8_t new_volume);

void sid_synth_render(int16_t *buffer, size_t len);

extern unsigned char memory[];

//----------------------------------------------

#endif /* SID_SID_H_ */
