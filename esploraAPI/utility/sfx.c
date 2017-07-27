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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "sfx.h"
#include "help.h" // for random funcion


#define NUM_CHANNELS 3 //number of sound channels, between 0 and 4
#define EXTENDED_NOTE_RANGE 0 //allows the use of notes above A 5... please avoid that they sound really bad

//sound
#define VOLUME_GLOBAL_MAX 7
#define VOLUME_CHANNEL_MAX 255/NUM_CHANNELS/7/9 //7=instrument volume 9=note volume

#define PRESCALLER_VAL 1

#define TRACK_END_MARK  0xFFFF   // end of the track


#ifndef max
 #define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
 #define min(a,b) ((a)<(b)?(a):(b))
#endif

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

//declare these variables globally for faster access
uint8_t _rand = 1;
uint8_t _chanCount[NUM_CHANNELS]; //counts until the next change of the waveform
uint8_t _chanHalfPeriod[NUM_CHANNELS]; //duration of half the period of the waveform
uint8_t _chanOutputVolume[NUM_CHANNELS]; //amplitude of the outputted waveform
uint8_t _chanOutput[NUM_CHANNELS]; //current value of the outputted waveform

bool _chanState[NUM_CHANNELS]; //if the waveform is currently high or low
bool _chanNoise[NUM_CHANNELS]; //if a random value should be added to the waveform to generate noise


// ------ from .cpp ------- //
bool trackIsPlaying[NUM_CHANNELS];
bool patternIsPlaying[NUM_CHANNELS];

uint8_t outputPitch[NUM_CHANNELS];
int8_t outputVolume[NUM_CHANNELS];

uint8_t globalVolume = VOLUME_GLOBAL_MAX;
uint8_t volumeMax = VOLUME_GLOBAL_MAX;
//uint8_t prescaler = PRESCALLER_VAL;

//tracks data
uint16_t *trackData[NUM_CHANNELS];
uint8_t trackCursor[NUM_CHANNELS];
uint16_t **patternSet[NUM_CHANNELS];
int8_t patternPitch[NUM_CHANNELS];

// pattern data
uint16_t *patternData[NUM_CHANNELS];
uint16_t **instrumentSet[NUM_CHANNELS];
uint16_t patternCursor[NUM_CHANNELS];
bool patternLooping[NUM_CHANNELS];

// note data
uint8_t notePitch[NUM_CHANNELS];
uint8_t noteDuration[NUM_CHANNELS];
int8_t noteVolume[NUM_CHANNELS];
bool notePlaying[NUM_CHANNELS];

// commands data
int8_t commandsCounter[NUM_CHANNELS];
int8_t volumeSlideStepDuration[NUM_CHANNELS];
int8_t volumeSlideStepSize[NUM_CHANNELS];
uint8_t arpeggioStepDuration[NUM_CHANNELS];
int8_t arpeggioStepSize[NUM_CHANNELS];
uint8_t tremoloStepDuration[NUM_CHANNELS];
int8_t tremoloStepSize[NUM_CHANNELS];


// instrument data
uint16_t *instrumentData[NUM_CHANNELS];
uint8_t instrumentLength[NUM_CHANNELS]; //number of steps in the instrument
uint8_t instrumentLooping[NUM_CHANNELS]; //how many steps to loop on when the last step of the instrument is reached
uint16_t instrumentCursor[NUM_CHANNELS]; //which step is being played
uint8_t instrumentNextChange[NUM_CHANNELS]; //how many frames before the next step

//current step data
int8_t stepVolume[NUM_CHANNELS];
uint8_t stepPitch[NUM_CHANNELS];

uint8_t chanVolumes[NUM_CHANNELS];
// ------------------------ //


// ---------------------------------------------- //

const uint16_t squareWaveInstrument[] PROGMEM = {0x0101, 0x03F7};
const uint16_t noiseInstrument[] PROGMEM = {0x0101, 0x03FF};
const uint16_t * const defaultInstruments[] PROGMEM = {squareWaveInstrument,noiseInstrument};

const uint16_t playOKPattern[] PROGMEM = {0x0005,0x138,0x168,0x0000};
const uint16_t playCancelPattern[] PROGMEM = {0x0005,0x168,0x138,0x0000};
const uint16_t playTickP[] PROGMEM = {0x0045,0x168,0x0000};

// ---------------------------------------------- //

#if(EXTENDED_NOTE_RANGE > 0)
//extended note range
#define NUM_PITCH 59
const uint8_t _halfPeriods[NUM_PITCH] PROGMEM= {246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33,31,29,28,26,25,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6};
#else
//regular note range
#define NUM_PITCH 36
const uint8_t _halfPeriods[NUM_PITCH] PROGMEM= {246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33};
#endif /*EXTENDED_NOTE_RANGE*/

// ---------------------------------------------- //

void initSFX(void)
{
	//prescaler = 1; // for 20 fps and every 50 ms
	for(uint8_t i=0; i<NUM_CHANNELS; i++){
		chanVolumes[i] = VOLUME_CHANNEL_MAX;
		sfxChangeInstrumentSet(defaultInstruments, i); //load default instruments. #0:square wave, #1: noise
		sfxCommand(CMD_INSTRUMENT, 0, 0, i); //set the default instrument to square wave
	}
  
  // setup PWM output pin
  DDRD |= (1<<PD7);
  PORTD |= (1<<PD7);
  
  cli(); // disable all interrupts
  
  // base timer4 init
  TCCR4B |= (1<<CS40);    // set timer4 prescaler to 1 -> 30kHz PWM on pin
  // enable PWM mode for comparator OCR4D and connect pwm to pin on channel D
  TCCR4C |= ((1<<PWM4D) | (1<<COM4D1));
  TCCR4D |= (1<<WGM40);		// put timer4 in phase- and frequency-correct PWM mode
  OCR4D = 1;	            // set pwm duty
  
  // initialize timer1
  TCCR1A = 0;
  TCCR1B = ((1 << WGM12) | (1 << CS10));  // CTC mode and prescaler 1
  TCNT1 = 0;
  OCR1A = 280;             // compare match register
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei(); // enable all interrupts
}

void sfxUpdateAll(void)
{
  for(uint8_t i=0; i<NUM_CHANNELS; i++) {
    sfxUpdateTrack(i);
    sfxUpdatePattern(i);
    sfxUpdateNote(i);
  }
}

void sfxPlayTrack(const uint16_t *track, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	sfxStopTrack(channel);
	trackCursor[channel] = 0;
	trackData[channel] = (uint16_t*)track;
	trackIsPlaying[channel] = true;
}

void sfxStopTrack(uint8_t channel)
{
  if(channel >= NUM_CHANNELS)
    return;
  
  trackIsPlaying[channel] = false;
  sfxStopPattern(channel);
}

void sfxStopAllTracks(void)
{
  for(uint8_t i=0; i<NUM_CHANNELS; i++) {
    sfxStopTrack(i);
  }
}

void sfxUpdateTrack(uint8_t channel)
{
  if(channel >= NUM_CHANNELS)
    return;
  
  if(trackIsPlaying[channel] && !patternIsPlaying[channel]) {
    uint16_t data = pgm_read_word(trackData[channel] + trackCursor[channel]);
    if(data == TRACK_END_MARK) {
      trackIsPlaying[channel] = false;
      return;
    }
    uint8_t patternID = data & 0xFF;
    patternPitch[channel] = data>>8;
    sfxPlayPattern((const uint16_t*)pgm_read_word(&(patternSet[channel][patternID])), channel);
    ++trackCursor[channel];
  }
}

void sfxUpdateAllTracks(void)
{
  for(uint8_t i=0; i<NUM_CHANNELS; i++) {
    sfxUpdateTrack(i);
  }
}

void sfxChangePatternSet(const uint16_t * const *patterns, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	patternSet[channel] = (uint16_t**)patterns;
}

void sfxPlayPattern(const uint16_t *pattern, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
	
  sfxStopPattern(channel);
	patternData[channel] = (uint16_t*)pattern;
	patternCursor[channel] = 0;
	patternIsPlaying[channel] = true;
	noteVolume[channel] = 9;
	//reinit commands
	volumeSlideStepDuration[channel] = 0;
	arpeggioStepDuration[channel] = 0;
	tremoloStepDuration[channel] = 0;
}

void sfxChangeInstrumentSet(const uint16_t * const *instruments, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	instrumentSet[channel] = (uint16_t**)instruments;
}

void sfxUpdatePattern(uint8_t i)
{
  if(i >= NUM_CHANNELS)
    return;
  
  if(patternIsPlaying[i]) {
    if(noteDuration[i] == 0) { // if the end of the previous note is reached
      
      uint16_t data = pgm_read_word(patternCursor[i] + patternData[i]);
      
      if(data == 0) { // end of the pattern reached
        if(patternLooping[i]) {
          patternCursor[i] = 0;
          data = pgm_read_word(patternCursor[i] + patternData[i]);
        } else {
          patternIsPlaying[i] = false;
          if(trackIsPlaying[i]) { // if this pattern is part of a track, get the next pattern
            sfxUpdateTrack(i);
            data = pgm_read_word(patternCursor[i] + patternData[i]);
          } else {
            sfxStopNote(i);
            return;
          }
        }
      }
      
      while(data & 0x0001) { // read all commands and instrument changes
        data >>= 2;
        uint8_t cmd = data & 0x0F;
        data >>= 4;
        uint8_t X = data & 0x1F;
        data >>= 5;
        int8_t Y = data - 16;
        sfxCommand(cmd, X, Y, i);
        
        ++patternCursor[i];
        data = pgm_read_word(patternCursor[i] + patternData[i]);
      }
      
      data >>= 2;
      uint8_t pitch = data & 0x003F;
      data >>= 6; // left only duration
      
      sfxPlayNote(pitch, data, i);
      ++patternCursor[i];
    }
  }
}

void sfxUpdateAllPatterns(void)
{
  for(uint8_t i=0; i<NUM_CHANNELS; i++) {
    sfxUpdatePattern(i);
  }
}

void sfxStopAllPatterns(void)
{
  for(uint8_t i=0; i<NUM_CHANNELS; i++) {
    sfxStopPattern(i);
  }
}

void sfxStopPattern(uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	sfxStopNote(channel);
	patternIsPlaying[channel] = false;
}

void sfxCommand(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i)
{
	if(i >= NUM_CHANNELS)
		return;
  
	switch(cmd)
  {
    case CMD_VOLUME: { // volume
	    X = constrain(X, 0, 10);
		  noteVolume[i] = X;
    } break;
      
    case CMD_INSTRUMENT: { // instrument
		  instrumentData[i] = (uint16_t*)pgm_read_word(&(instrumentSet[i][X]));
		  instrumentLength[i] = pgm_read_word(&(instrumentData[i][0])) & 0x00FF; //8 LSB
		  instrumentLength[i] *= PRESCALLER_VAL;
		  instrumentLooping[i] = min((pgm_read_word(&(instrumentData[i][0])) >> 8), instrumentLength[i]); //8 MSB - check that the loop is shorter than the instrument length
		  instrumentLooping[i] *= PRESCALLER_VAL;
    } break;
      
    case CMD_SLIDE: { // volume slide
		  volumeSlideStepDuration[i] = X * PRESCALLER_VAL;
		  volumeSlideStepSize[i] = Y;
    } break;
      
    case CMD_ARPEGGIO: {
		  arpeggioStepDuration[i] = X * PRESCALLER_VAL;
		  arpeggioStepSize[i] = Y;
    } break;
      
    case CMD_TREMOLO: {
		  tremoloStepDuration[i] = X * PRESCALLER_VAL;
		  tremoloStepSize[i] = Y;
    } break;
	  
    default: break;
	}
}

void sfxPlayNote(uint8_t pitch, uint8_t duration, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	// set note
	notePitch[channel] = pitch;
	noteDuration[channel] = duration * PRESCALLER_VAL;
	
  // reinit vars
	instrumentNextChange[channel] = 0;
	instrumentCursor[channel] = 0;
	notePlaying[channel] = true;
	_chanState[channel] = true;
	commandsCounter[channel] = 0;
}

void sfxStopNote(uint8_t channel)
{
  if(channel >= NUM_CHANNELS)
    return;
  
  notePlaying[channel] = false;
  // counters
  noteDuration[channel] = 0;
  instrumentCursor[channel] = 0;
  commandsCounter[channel] = 0;
  
  // output
  _chanOutput[channel] = 0;
  _chanOutputVolume[channel] = 0;
  _chanState[channel] = false;
  sfxUpdateOutput();
}

void sfxStopAllNotes(void)
{
  for(uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
    sfxStopNote(channel);
  }
}

void sfxUpdateNote(uint8_t i)
{
	if(i >= NUM_CHANNELS)
		return;
  
	if(notePlaying[i]) {
		
		if(noteDuration[i] == 0) {
			sfxStopNote(i);
			return;
		} else {
			--noteDuration[i];
		}
		
		if(instrumentNextChange[i] == 0) {
			// read the step data from the progmem and decode it
			uint16_t thisStep = pgm_read_word(&(instrumentData[i][1 + instrumentCursor[i]]));
			
			stepVolume[i] = thisStep & 0x0007;
			thisStep >>= 3;
			
			uint8_t stepNoise = thisStep & 0x0001;
			thisStep >>= 1;
			
			uint8_t stepDuration = thisStep & 0x003F;
			thisStep >>= 6;
			
			stepPitch[i] = thisStep;
			
			// apply the step settings
			instrumentNextChange[i] = stepDuration * PRESCALLER_VAL;
			
			_chanNoise[i] = stepNoise;
			
			++instrumentCursor[i];
			
			if(instrumentCursor[i] >= instrumentLength[i]) {
				if(instrumentLooping[i]) {
					instrumentCursor[i] = instrumentLength[i] - instrumentLooping[i];
				} else {
					sfxStopNote(i);
				}
			}
		}
		--instrumentNextChange[i];
		++commandsCounter[i];
		
		//UPDATE VALUES	
		//pitch
		outputPitch[i] = notePitch[i] + stepPitch[i] + patternPitch[i];
		if(arpeggioStepDuration[i]){
		  outputPitch[i] += commandsCounter[i] / arpeggioStepDuration[i] * arpeggioStepSize[i];
		}
		outputPitch[i] = (outputPitch[i] + NUM_PITCH) % NUM_PITCH; //wrap
		//volume
		outputVolume[i] = noteVolume[i];
		if(volumeSlideStepDuration[i]){
		  outputVolume[i] += commandsCounter[i] / volumeSlideStepDuration[i] * volumeSlideStepSize[i];
		}
		if(tremoloStepDuration[i]){
			outputVolume[i] += ((commandsCounter[i]/tremoloStepDuration[i]) % 2) * tremoloStepSize[i];
		}
		outputVolume[i] = constrain(outputVolume[i], 0, 9);
		if(notePitch[i] == 63){
			outputVolume[i] = 0;
		}
    
		cli();
		_chanHalfPeriod[i] = pgm_read_byte(_halfPeriods + outputPitch[i]);
    uint8_t dataOutput = ((outputVolume[i] * chanVolumes[i] * stepVolume[i]) << (globalVolume)) >> 7;
    _chanOutputVolume[i] = dataOutput;
    _chanOutput[i] = dataOutput;
		sei();
	}
}

void sfxUpdateAllNotes(void)
{
  for(uint8_t i = 0; i < NUM_CHANNELS; i++) {
    sfxUpdateNote(i);
  }
}

void sfxSetChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	_chanHalfPeriod[channel] = halfPeriod;
	_chanState[channel] = false;
	_chanCount[channel] = 0;
	sfxUpdateOutput();
}

// timer compare interrupt service routine
ISR(TIMER1_COMPA_vect)
{
  bool outputChanged = false;
  //no for loop here, for the performance sake (this function runs 15 000 times per second...)
  //CHANNEL 0
  if(_chanOutputVolume[0]) {
    ++_chanCount[0];
    if(_chanCount[0] >= _chanHalfPeriod[0]) {
      _chanCount[0] = 0;
      outputChanged = true;
      _chanState[0] = !_chanState[0];
      if(_chanNoise[0]) {
        _rand = 67 * _rand + 71;
        _chanOutput[0] = _rand % _chanOutputVolume[0];
      }
    }
  }
  
  
  //CHANNEL 1
#if (NUM_CHANNELS > 1)
  if(_chanOutputVolume[1]) {
    ++_chanCount[1];
    if(_chanCount[1] >= _chanHalfPeriod[1]) {
      _chanCount[1] = 0;
      outputChanged = true;
      _chanState[1] = !_chanState[1];
      if(_chanNoise[1]) {
        _rand = 67 * _rand + 71;
        _chanOutput[1] = _rand % _chanOutputVolume[1];
      }
    }
  }
#endif
  
  //CHANNEL 2
#if (NUM_CHANNELS > 2)
  if(_chanOutputVolume[2]) {
    ++_chanCount[2];
    if(_chanCount[2] >= _chanHalfPeriod[2]) {
      _chanCount[2] = 0;
      outputChanged = true;
      _chanState[2] = !_chanState[2];
      if(_chanNoise[2]) {
        _rand = 67 * _rand + 71;
        _chanOutput[2] = _rand % _chanOutputVolume[2];
      }
    }
  }
#endif
  
  //CHANNEL 3
#if (NUM_CHANNELS > 3)
  if(_chanOutputVolume[3]) {
    ++_chanCount[3];
    if(_chanCount[3] >= _chanHalfPeriod[3]) {
      _chanCount[3] = 0;
      outputChanged = true;
      _chanState[3] = !_chanState[3];
      if(_chanNoise[3]) {
        _rand = 67 * _rand + 71;
        _chanOutput[3] = _rand % _chanOutputVolume[3];
      }
    }
  }
#endif
  
  if(outputChanged) {
    sfxUpdateOutput();
  }
}

void sfxUpdateOutput(void)
{
  uint8_t output = 0;
  
  //CHANNEL 0
  if(_chanState[0]) {
    output += _chanOutput[0];
  }
  
  //CHANNEL 1
#if (NUM_CHANNELS > 1)
  if(_chanState[1]) {
    output += _chanOutput[1];
  }
#endif
  
  //CHANNEL 2
#if (NUM_CHANNELS > 2)
  if(_chanState[2]) {
    output += _chanOutput[2];
  }
#endif
  
  //CHANNEL 3
#if (NUM_CHANNELS > 3)
  if(_chanState[3]) {
    output += _chanOutput[3];
  }
#endif
  
  OCR4D = output;
}

void sfxSetPatternLooping(bool loop, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
	patternLooping[channel] = loop;
}

void sfxPlayOK(void)
{
	sfxPlayPattern(playOKPattern, SFX_CH_0);
}

void sfxPlayCancel(void)
{
	sfxPlayPattern(playCancelPattern, SFX_CH_0);
}

void sfxPlayTick(void)
{
	sfxPlayPattern(playTickP, SFX_CH_0);
}

void sfxSetVolume(int8_t volume)
{
	globalVolume = (volume < 0) ? volumeMax : volume % (volumeMax+1); //wrap volume value
}

void sfxSetVolumeCh(int8_t volume, uint8_t channel)
{
	if(channel >= NUM_CHANNELS)
		return;
  
	volume = (volume > VOLUME_CHANNEL_MAX) ? VOLUME_CHANNEL_MAX : volume;
	volume = (volume < 0) ? 0 : volume;
	chanVolumes[channel] = volume;
}

uint8_t sfxGetGlobalVolume(void)
{
  return globalVolume;
}

uint8_t sfxGetVolumeCh(uint8_t channel)
{
	if(channel>=NUM_CHANNELS)
		return 255;

	return chanVolumes[channel];
}
