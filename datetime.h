#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "datetime.h"

typedef struct {
    unsigned int year;
    unsigned char date,month,day,hour,minute,second,daysave,sunset_hour,sunset_minute,sunset_second,sunrise_hour,sunrise_minute,sunrise_second;
} date_time;

date_time set_daylight_savings(date_time curr);
date_time set_date(date_time curr);
date_time incre_time2(date_time curr);
date_time sunset(date_time curr);
date_time sun_sync(date_time curr);
date_time set_sunsynchronocity(date_time curr);

#endif

