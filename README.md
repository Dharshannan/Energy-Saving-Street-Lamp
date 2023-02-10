# Mini project - Energy saving automatic outside light - Dharshannan and Pablo
## Table of contents
- [Instructions to users](#instructions-to-users)
- [Hardware setup](#hardware-setup)
- [Demonstration video](#demonstration-video)
- [Program structure](#program-structure)
- [Achieving sun synchronocity](#achieving-sun-synchronocity)
- [Learning outcomes](#learning-outcomes)
- [Project brief](#project-brief)
- [Project specification](#project-specification)
- [Supplementary information and help](#supplementary-information-and-help)



## Instructions to users
Please follow the following procedures to set up your energy saving automatic outside light:
1. Set up your hardware as detailed in [Hardware setup](#hardware-setup);
1. Manually input the date (required), time (required), Daylight Savings Period (required), sunrise timing (optional) and sunset timing (optional) in ```main.c```;
1. To operate in "normal mode" change the prescaler time on ```timers.c``` to 0b1000 (1:256)
1. To operate in "test mode" change the prescaler time on ```timers.c``` to 0b0000 (1:1)


## Hardware setup
Our hardware has been set up as in Lab 4:
- The **LED on pin RD7** is used as the street light (it switches on at dusk and off at dawn (it is off from 1am-5am too to save energy);
- The **LCD screen** is used as the date and time indicator in all modes (it displays the date and time in decimal).

![Street Lamp](gifs/Mini_Project.jpg) 



## Demonstration video
Our video demonstrates operation of the street light during daylight savings change of clock (note that the clocks changed on 27 October 2023 (Sunday) at 2am - it turned back 1 hour to 1am):

https://www.youtube.com/watch?v=r1nD_SjOgA4


## Program structure
Our program has been structured as follows:
1. ```"main.c"```

We configure our oscillators and windowed watchdog timer here. All other header files used in this project are included here too. 

The source file contains our main function. We start by manually inputting the date and time when the device is first programmed. The sunrise time on the day that the device is first programmed can also be manually inputted too if known, or left to the default timing of 7am if unknown. We then proceed to initialise all our hardware modules. The program then runs indefinitely in an infinite while loop (or more realistically, until the hardware fails). During each iteration:
    - we check whether the sunrise/sunset flag has been toggled (by our high priority interrupt) and turn the street light off/on as required;
    - we check whether the time flag has been toggled (by our low priority interrupt) and increment our time for every second that passes by;
    - we switch off the street light from 1am-5am and disable the high priority interrupt (to prevent accidentally switching on the street light);
    - we display the current time on the LCD screen

1. ```"datetime.c/h"```

    The header file defines a structure ```datetime``` which is used throughout the project:
    ```
    typedef struct {
        unsigned int year;
        unsigned char date,month,day,hour,minute,second,daysave,sunset_hour,sunset_minute,sunset_second;
    } date_time;
    ```
    
    The source file contains 2 key functions:
    - ```date_time incre_time(date_time curr)``` which updates our current time every second, functions to check for clock changes due to daylight savings and changes to the date, month and year are also called here;
    - ```date_time set_sunsynchronocity(date_time curr)``` which synchronises our time with the sun indefinitely.

1. ```"ADC.c/h"```

    The header file simply contains the function prototypes from the corresponding source file.
    
    The source file contains functions to initialise our ADC module, and obtain the current ADC readings for the LDR voltage.

1. ```"comparator.c/h"```

    The header file simply contains the function prototypes from the corresponding source file.
    
    The source file contains functions initialise the comparator module to use in our interrupts.

1. ```"timers.c/h"```

    The header file defines the prescaler values and Timer0 initial bits for both the "testing mode" and "normal mode".
    
    The source file initialises Timer0 to be used in the low priority interrupt (to increment out time each second).

1. ```"interrupts.c/h"```

    The source file turns on interrupts, and defines:
    - a high priority interrupt every time sunrise or sunset occurs;
    - a low priority interrupt every time the timer overflows (1 second has passed).

1. ```"LEDconfig.c/h"```

    The source file contains functions to initialise LED RD7.

1. ```"LEDarray.c/h"```

    The source file contains functions to initialise the LED array.

1. ```"LCD.c/h"```

    The source contains functions to initialise the LCD screen.
    
## Achieving sun synchronocity
To achieve sun synchronocity we implemented the "Solar Midnight" method. This method involves calculating the length of time between sunset and sunrise. This value is then divided by 2 and added to 0 (representing midnight-12am). This gives us the theoretical time suggested by sun synchronocity. We check this time with the current time at the moment sunrise occurs. If the difference between the current time and the "suggested" time is greater than +-45 minutes, we reset the current time to the "suggested" time. 

We also take into account the time period we are in. If we are in the "daylight savings" period (daysave=1) the time will reset accordingly at 2am on the last sunday of October to 1am. Once we come out of the "daylight savings" period (daysave=0), which is the last Sunday of March, the timer will reset from 1am to 2am.



## Learning outcomes
The principal learning objectives for this project are:
- Implement a working microprocessor based system to achieve a more complex real world task
- Develop your ability to independently plan, organise and structure your code 
- Improve your grasp of the C language and writing your own functions

## Learning outcomes

The principal learning objectives for this project are:

- Implement a working microprocessor based system to achieve a more complex real world task
- Develop your ability to independently plan, organise and structure your code 
- Improve your grasp of the C language and writing your own functions

## Brief

Outside lights that respond to ambient light levels are commonplace (i.e. street lights, porch lights, garden lights etc). These types of lights switch on at dusk and then turn off at dawn. However, energy and money can be saved if these lights are switched off during the small hours of the morning (for example, between 1am and 5am), when there are very few people around. Many councils in the UK have implemented/trialled this idea for street lights (https://www.bbc.co.uk/news/uk-england-derbyshire-16811386). Your task is to use the knowledge of microcontrollers and hardware that you have gained in labs 1-3 from this module to develop a fully automated solution.

## Specification
Design and program a device that meets the following requirements:

1. Monitors light level with the LDR and turns on an LED in low light conditions (i.e. night-time) and off in bright conditions (i.e. daytime)
1. Displays the current hour of day on the LED array in binary
1. Turns the light off between approx. 1am and 5am
1. Adjusts for daylight savings time
1. Maintain synchronicity with the sun indefinitely
1. Be fully automatic (requires zero maintenance after installation)

Please use this GitHub repo to manage your software development and submit your mini project code.

## Supplementary information and help
At first the task may seem quite straightforward but there are several points that often prove more tricky. The first is how to test code during development? You could test in real world conditions but you would be limited to one test cycle per day and this would severely slow down your development and debugging progress. To get around this you could implement a "testing mode" and pretend that a day lasts 24 seconds. This could be done using a #define directive to switch between "normal" and "testing" modes for your code.

Adjusting for daylight savings time is not too tricky. The clocks always change (in the UK) on the last Sunday in March (they go forward an hour) and the last Sunday in October (they go back an hour). One method of achieving this is to initialise what day it is when device is first switched on (using manual input) and then keep track of the days that pass and what the day of the week it is. Another method might be to automatically figure out what time of year it is (see below). Also don't forget about leap years! 

No clock is perfect, they can all run slightly fast/slow and can by influenced be external factors such as temperature. Ultimately this will result in drift over time and eventually the time will drift so far out of sync with real time that it is meaningless. For the purposes of our device the main requirement is that it remains in sync with the sun. You could use light from the sun to keep your clock in sync. Although the length of daylight varies considerably during the year, the midpoint between dusk and dawn only varies by a few minutes. This is termed solar midnight approx. 12am or solar noon approx. 12pm. One method of staying in sync with the sun is to use the LDR and record/calculate when these times occur and adjust your clock accordingly. The length of daylight also tells us information about what time of year it is and can be used to help us know when to adjust for daylight savings time.

![Day length](gifs/day-length-london.jpg)
http://wordpress.mrreid.org/2010/10/31/why-change-the-clocks/





