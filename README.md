# Emergency vehicle light Control
Light control for emergency (model) vehicles.

## Aim of the project
The aim of this project was to create a simple sketch which controls four independent emergency (model) vehicle light groups. This could be needed to build a model of a car / train or any other accident.

## Introduction
This Software was originally programed for an [ESP32-S2-Saola-1](https://amzn.to/3UJ6Ole) but any other microcontroller with at least 4 timers can be used as well. 
With slide changes groups can be added or removed in order to use an other microcontroller.

## Pin-out
Regarding to the pin-setup configuration in the [main](https://github.com/AndyyF/emergency-vehicle-lightcontrol/blob/main/emergency_vehicle_light/src/main.cpp) code from line 52 to 70 the used pins have to be implemented.

## Configuration
In order to use a certain amount of pin groups you have to modify the line 45 in the [main](https://github.com/AndyyF/emergency-vehicle-lightcontrol/blob/main/emergency_vehicle_light/src/main.cpp) code.

## timing table for one pin group
```
Floodlight + Taillights: +3,3 V

flashlight 1 & 2
      : _____            _____                                                                       : __
LED1  :|     |          |     |                                                                      :|  
      :|  #  |    2#    |  #  |                                   14#                                :|  
  ____:|     |__________|     |______________________________________________________________________:|  
      :                                              _____            _____                          :      
LED2  :                                             |     |          |     |                         :       
      :                  9#                         |  #  |     2#   |  #  |                         :       
  ____:_____________________________________________|     |__________|     |_________________________:___
      :                                                                                              :
      :
blue-light-bar 1 & hazard light 1:                                                                   :
      : ______________________________                                ______________________________ :
LED3  :|                              |                              |                              |:
      :|              6#              |              6#              |              6#              |:
  ____:|                              |______________________________|                              |:___
      :                                                                                              :
      :
blue-light-bar 2 & hazard light 2:                                                                   :
  ____:                                ______________________________                                :___
LED4  :|                              |                              |                              |:    
      :|              6#              |              6#              |              6#              |:    
      :|______________________________|                              |______________________________|:    
      :

Legend:     #   stands for  50ms
           2#   stands for 100ms
           6#   stands for 300ms
           9#   stands for 450ms
          14#   stands for 700ms
```

# Sample vehicle

![samplecar]https://github.com/AndyyF/emergency-vehicle-lightcontrol/blob/main/emergency_vehicle_light/pictures/samplecar1.jpeg "samplecar")

### Disclaimer
All used links in this project are affiliate links. This means that you support me as a programmer with no additional costs for you. – Thanks :)

In order to use all 4 implemented channels your microcontroller must support up to 16 independent PWM-channels.

### Used products
* [ESP32-S2-Saola-1](https://amzn.to/3UJ6Ole)
* [Evemodel SMD LED Golden White 0805](https://amzn.to/3EyFtxf)
* [Evemodel SMD LED Yellow 0402](https://amzn.to/3UtOJYT)
* [Evemodel SMD LED Red 0402](https://amzn.to/3NXDZ2i)
* [Evemodel SMD LED Blue 0402](https://amzn.to/3EpJML2)
