# Firecore - game for *Arduino Esplora*
![Logo ](pics/firecore-logo_title.png "Logo")
***

Colored game which looks like old school 8bit games.
Genre: shoot em up

#### Story describtion:
Invaders come to galaxy from nowhere, one of doges send a message to hero
what live at edge of galaxy and ask him to save the galaxy and their home planet.

#### Game have: 
- simple 8bit color graphics;
- animated sprites;
- 4 different ships (4th unlocks after last level);
- moving background;
- level bosses;
- primitive story;
- qute well performance;
- save maximum score in *EEPROM*;
- *~~explosions~~*;
- *~~sounds~~*;
- *~~a lot of random~~*;

![Gameplay screen ](pics/firecore-gamplay.png "Gameplay screen")


#### Esplora board feature usage:
- RGB LED used for warn player of ship's low health (it's flash red);
- Light sensor, temperature sensor and microphone used to seed random fuction;
- Buzzer used to ganerate annoying sounds (i.e sfx);

#### Controls:
- Fire: *Switch 4 button* (named as *A*);
- Move ship: *Analoge joystick*;
- High Score reset: *Analoge joystick button* + *Switch 2* + *Switch 3* (Wait for *beep*);

Developed only for *Arduino Esplora*!
To upload game to board you need to install *esploraAPI* folder to *Arduino's* libs library!
- OSX: ~/Documents/Arduino/libraries/
- WIN: My Documents\Arduino\libraries\

Game and drivers was writen on C (maybe somewhere can be a tiny of C++, but not for a long time...).

#### At current moment:
- whole game consume 23,104 bytes ROM and 962 RAM;
- all graphics in game consume 5,644 bytes ROM;
- there are 5,568 bytes of free ROM space to add any cool stuff! 

#### Hardware:
- *Arduino Esplora* board;
- MCU: *ATmega32u4*;
- ROM: 28,672 bytes;
- RAM: 2,560 bytes;
- TFT LCD: *ST7735* 160x128.

[To see how does it work check this link](https://www.youtube.com/channel/UCDXVQ9ZfQl8Ddeu_3qiwSiA "My YouTube channel")
***

> ### KNOWNING ISSUE
> "My *Esplora* board doesn't upload any program any more and no Serial port in system" :scream:
> #### How to fix it?
> To reupload ANY program to *Esplora* board you need:
> * Press upload button as usual;
> * When *Arduino IDE* starts to load new firmware press *Reset* button on board!
> * Esplora will going to bootloader and enable virtual USB-COM;
> * Program starts upload as usuial.
>
> #### Why it's happen?
> Because *esploraAPI* doesn't contain any *USB stack* code to simulate COM port,
> i.e. USB is just disabled!
> I made it as it reques a lot of ROM, and it useless in this project.
> But in bootloader it works and still allow you to upload new firmware!
> 
> #### How back to normal?
> Just upload any *NORMAL Arduino sketch* which not contain *esploraAPI*.


> ## :exclamation: ATTENTION! :exclamation:
>  * This game is still unstable and in develop! :beetle:
>  * Any changes are possible at any time!  
