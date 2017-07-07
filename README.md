# Firecore
*Game for Arduino Esplora*
***

Colored game which looks like old school 8bit games.
Genre: shoot em up

Game have: 
- simple 8bit colored graphics;
- animated sprites;
- moving background;
- level bosses;
- primitive story;
- qute vell performance;
- save maximum score;
- *~~explosions~~*;
- *~~sounds~~*;
- *~~a lot of random~~*;

Story describe:
Invaders come to galaxy from nowhere, one of doges send a message to hero
living at edge of galaxy and ask him to save the galaxy and their home planet. 


Esplora board feature usage:
- RGB LED used for warn player of ship's low health (it's flash red);
- Light sensor, temperature sensor and microphone used to seed random fuction;

Also game saves maximum score in *EEPROM*, so you can always beat yourself!

Controls:
- Fire: *Switch 4 button* (named as *A*);
- Move ship: *Analoge joystick*;

Developed only for *Arduino Esplora*!

To upload game to your board need to install *esploraAPI* folder to *Arduino's* libs library.


> Hardware:
> - *Arduino Esplora* board;
> - MCU: *ATmega32u4*;
> - ROM: 28kb;
> - RAM: 2,5kb;
> - TFT LCD: *ST7735* 160x128.

Whole game writed on C (maybe somewhere can be a tiny of C++, but not for a long time...).
At current moment whole game consume 24,396 bytes ROM and 354 RAM;
All graphics in game consume 10,324 bytes; 

[To see how does it work check this link](https://www.youtube.com/channel/UCDXVQ9ZfQl8Ddeu_3qiwSiA "My YouTube channel")
***

  
> ### ATTENTION!
>  * This game is still unstable and in develop!
>  * Any changes are possible at any time!  
