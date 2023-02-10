#include <xc.h>
#include <stdio.h>
#include "datetime.h"
#include "interrupts.h"
#include "LCD.h"

// Function to change clocks based on daylight savings which occur on the last sunday of the month of March and October
date_time set_daylight_savings(date_time curr) {
    if(curr.month==3 && curr.day==7 && (curr.date+7>31) && curr.daysave==0 && curr.hour==1) { // For March
        curr.hour = curr.hour + 1; // Advance clocks by 1 hour at 1am
        curr.daysave = 1;
    }
    if (curr.month==10 && curr.day==7 && (curr.date+7>31) && curr.daysave==1 && curr.hour==2) { // For October
        curr.hour = curr.hour - 1; // Backtrack clock by 1 hour at 2am
        curr.daysave = 0; // This prevents the time from re-setting again
    }
    return curr;
}

// Function to increment months based of the number of days in them and also takes care of leap years
date_time set_date2(date_time curr) {
    char monthdays[2][12] = {{31,29,31,30,31,30,31,31,30,31,30,31},  // for leap year
                             {31,28,31,30,31,30,31,31,30,31,30,31}}; // for non-leap year
    unsigned char i;
    for (i=0;i<12;i++){
        if(curr.year%4==0){ // For leap year
            if(curr.month == (i+1) && curr.date>monthdays[0][i]){ // Check if the date of the month has gone past maximum number of days for said month
                curr.date = 1; // Reset date to 1st
                curr.month = curr.month + 1; // Increment month by 1
            }
        }
        if(curr.year%4!=0){ // Similar as above but for non-leap year
            if(curr.month == (i+1) && curr.date>monthdays[1][i]){
                curr.date = 1;
                curr.month = curr.month + 1;
            }
        }
        if (curr.month>12) {             // Special case if December 31 just passed
            curr.year = curr.year+1;     // Begin a new year
            curr.month = 1;              // Reset the month to January
        }
    }
    return curr;
}

// This function updates the time by incrementing seconds followed by minutes, hours, days, months and years
date_time incre_time2(date_time curr) {
    unsigned char arr1[3] = {9,59,23}; // Array to hold maximum seconds, minutes and hours values
    unsigned char arr2[4] = {curr.second,curr.minute,curr.hour,curr.day}; // array holding copies of current seconds, minutes, hours and days values
    
    while(tmr_ovf){ // When the timer0 overflows
        arr2[0]++; // increment the second
        unsigned char i;
        for(i=0;i<3;i++){ // a for loop to increment the minute, hours and days and reset where necessary
            if(arr2[i]>arr1[i]){ // check if maximum value is passed
                arr2[i] = 0; // reset value when max is passed
                if((i+1)==3){ // this part increments the day and date
                    arr2[i+1] = arr2[i+1] + 1;
                    curr.date = curr.date + 1;
                }
                else{ // this increments the minutes, and hours
                    arr2[i+1] = arr2[i+1] + 1;
                }
            }
            // Check if days has passed 7 (Sunday)
            if (arr2[3]>7){
                arr2[3] = 1; // Reset to 1 (Monday)
            }
        }
        curr.second = arr2[0]; // Set the seconds to its updated value
        curr.minute = arr2[1]; // Set the minutes to its updated value 
        curr.hour = arr2[2]; // Set the hours to its updated value
        curr.day = arr2[3]; // Set the days to its updated value
        
        curr = set_daylight_savings(curr); // Call function to check if time needs to be advanced or backtracked based on daylight savings
        curr = set_date2(curr); // Call function to check if date needs to be reset signifying new month
        
        // Display the date on the 1st line of the LCD
        char buf1[40];
        LCD_setline(1);
        sprintf(buf1,"%02d/%02d/%04d-SR:%02d",curr.date,curr.month,curr.year,curr.sunrise_hour);
        LCD_sendstring(buf1);
        
        // Display time on the 2nd line of the LCD
        char buf2[40];
        LCD_setline(2);
        sprintf(buf2,"%02d:%02d:%02d-SS:%02d",curr.hour,curr.minute,curr.second,curr.sunset_hour);
        LCD_sendstring(buf2);
        
        tmr_ovf = 0; // Reset the overflow watcher
    }
    return curr;
}

// Function to set sunset time, called only when sunset has occurred
date_time sunset(date_time curr){
    curr.sunset_hour = curr.hour;
    curr.sunset_minute = curr.minute;
    curr.sunset_second = curr.second;
    
    return curr;
}

// Function to update time based on sun-synchronocity, this will only be called when sunrise has occurred
date_time sun_sync(date_time curr){
    // Calculate the length of night time (how long its been dark for) i.e. length of time between sunset and sunrise
    signed char hourdiff,minutediff,seconddiff; // These variables hold the difference for time between sunset and sunrise
    
    hourdiff = (curr.hour + 24) - curr.sunset_hour; // Current hour (which is sunrise hour) minus sunset hour, below is similar for minute and second
    minutediff = curr.minute - curr.sunset_minute;
    seconddiff = curr.second - curr.sunset_second;
    // If minute or second is negative update hour or minute, the hour will always be positive
    if(minutediff<0){
        hourdiff = hourdiff - 1; // minus 1 from the hour difference
        minutediff = minutediff + 60; // add 60 to the negative minute difference
    }
    // Similar for seconds difference
    if(seconddiff<0){
        minutediff = minutediff - 1;
        seconddiff = seconddiff + 60;
    }
    
    // Calculate the offset between time suggested by solar midnight and current time
    signed char houroffset,minuteoffset,solartime; // hour and minute offsets hold the offset values of time while solar time holds solar midnight time approximate
    solartime = 0 + (hourdiff/2); // 0 is midnight added to half of the time difference
    // 3 cases solartime>current time OR solartime<current time OR solartime==current time (exclusively for the hour), we will deal with these cases seperately
    // CASE 1:
    if(solartime>curr.hour){
        houroffset = solartime - curr.hour;
        minuteoffset = (minutediff/2) - curr.minute; // We are only interested in the hour and minute, seconds can be neglected
        if(minuteoffset<0){ // Similar method as above to deal with negative values
            houroffset = houroffset - 1;
            minuteoffset = minuteoffset + 60;
        }
    }
    // CASE 2:
    if(solartime<curr.hour){
        houroffset = curr.hour - solartime;
        minuteoffset = curr.minute - (minutediff/2);
        if(minuteoffset<0){ // Similar method as above to deal with negative values
            houroffset = houroffset - 1;
            minuteoffset = minuteoffset + 60;
        }
    }
    // CASE 3:
    if(solartime==curr.hour){ // This case we only need minute offset
        minuteoffset = curr.minute - (minutediff/2);
        if(minuteoffset<0){ // Turn negative value for minute offset to positive
            minuteoffset = -1*(minuteoffset);
        }
    }
    // Now check if the offset in time is greater than some defined value (say 45 minutes)
    // Update time if its greater to maintain synchronocity with the sun
    minuteoffset = minuteoffset + (houroffset*60); // Turn to overall minutes offset
    if(minuteoffset>45){ // Check if value is greater than pre-defined value (45 mins)
        // Update time
        curr.hour = solartime;
        curr.minute = (minutediff/2);
        curr.second = (seconddiff/2);
    }
    return curr;
}

// Function to call both sunset and sun_sync based on sunset and sunrise flag triggers, this function will be called in main.c
date_time set_sunsynchronocity(date_time curr){
    if(sunset_flag == 1 && curr.hour > 12){ // If sunset has occurred
        curr = sunset(curr); // Set sunset time
        sunset_flag = 0; // Clear sunset flag
    }
    if(sunrise_flag == 1 && curr.hour < 12){ // If sunrise has occurred
        curr.sunrise_hour = curr.hour; // Set sunrise time 
        curr.sunrise_minute = curr.minute;
        curr.sunrise_second = curr.second;
        curr = sun_sync(curr); // Update time if necessary
        sunrise_flag = 0; // Clear sunrise flag
    }
    return curr;
}