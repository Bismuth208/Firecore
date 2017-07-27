/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _SFX_H
#define	_SFX_H

#include <stdint.h>
#include <stdbool.h>

//commands
#define CMD_VOLUME 0
#define CMD_INSTRUMENT 1
#define CMD_SLIDE 2
#define CMD_ARPEGGIO 3
#define CMD_TREMOLO 4


#define SFX_CH_0 0
#define SFX_CH_1 1
#define SFX_CH_2 2
#define SFX_CH_3 3

#ifdef __cplusplus
extern "C"{
#endif
  
	void initSFX(void);
  
  void sfxUpdateAll(void);
	
	void sfxPlayTrack(const uint16_t* track, uint8_t channel);
	void sfxUpdateAllTracks(void);
  void sfxUpdateTrack(uint8_t channel);
	void sfxStopAllTracks(void);
  void sfxStopTrack(uint8_t channel);
	void sfxChangePatternSet(const uint16_t* const* patterns, uint8_t channel);

	void sfxPlayPattern(const uint16_t* pattern, uint8_t channel);
	void sfxChangeInstrumentSet(const uint16_t* const* instruments, uint8_t channel);
	void sfxUpdateAllPatterns(void);
  void sfxUpdatePattern(uint8_t i);
	void sfxSetPatternLooping(bool loop, uint8_t channel);
	void sfxStopAllPatterns(void);
  void sfxStopPattern(uint8_t channel);
	
	void sfxCommand(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i);
	void sfxPlayNote(uint8_t pitch, uint8_t duration, uint8_t channel);
	void sfxUpdateAllNotes(void);
	void sfxUpdateNote(uint8_t i);
	void sfxStopAllNotes(void);
  void sfxStopNote(uint8_t channel);

	void sfxSetVolume(int8_t volume);
	void sfxSetVolumeCh(int8_t volume, uint8_t channel);
  uint8_t sfxGetGlobalVolume(void);
	uint8_t sfxGetVolumeCh(uint8_t channel);
	
	void sfxPlayOK();
	void sfxPlayCancel();
	void sfxPlayTick();

	void sfxSetChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod);

	void sfxUpdateOutput();

#ifdef __cplusplus
} // extern "C"
#endif

#endif	/* _SFX_H */

