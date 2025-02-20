#include "inkAlarm.h"
#include "rtcMem.h"

#if INK_ALARMS

uint64_t offsetToAddForDays(uint64_t unixTimeNow, inkAlarm *theAlarm, tmElements_t timeAlarm)
{
    int day = weekday(unixTimeNow);
    switch (day)
    {
    case 1:
    {
        // "Sun";
        day = 6;
        break;
    }
    case 2:
    {
        // "Mon";
        day = 0;
        break;
    }
    case 3:
    {
        // "Tues";
        day = 1;
        break;
    }
    case 4:
    {
        // "Wed";
        day = 2;
        break;
    }
    case 5:
    {
        // "Thurs";
        day = 3;
        break;
    }
    case 6:
    {
        // "Fri";
        day = 4;
        break;
    }
    case 7:
    {
        // "Sat";
        day = 5;
        break;
    }
    }
    debugLog("Day is now: " + String(day));
    debugLog("The days we want to alarm: " + alarmGetDays(theAlarm));

    if(getBit(theAlarm->days, day) == true) {
        if (!(theAlarm->hour < timeAlarm.Hour || (theAlarm->hour == timeAlarm.Hour && theAlarm->minute < timeAlarm.Minute)))
        {
            debugLog("It's gonna trigger today, offset for days is zero then");
            return 0;
        }
    } else {
        // Find the nearest day
        for(int i = 0; i < 7; i++) {
            int ii = i + day;
            if(ii > 6) {
                ii = ii - 7;
            }
            debugLog("ii is in iter: " + String(ii));
            if(getBit(theAlarm->days, ii) == true) {
                debugLog("So final ii is: " + String(ii) + " when days is: " + String(theAlarm->days) + " and i is: " + String(i));
                return 86400 * i;
            }
        }
    }

    debugLog("We should not be here, or no days are enabled? " + String(theAlarm->days));
    return 0;
}

uint64_t getUnixTimeOfAlarm(inkAlarm *theAlarm)
{
    if (!theAlarm->enabled)
    {
        return UINT64_MAX;
    }
    uint64_t offsetToAdd = 0;

    tmElements_t timeAlarm = timeRTCLocal;
    uint64_t unixTimeNow = getUnixTime(timeRTCLocal);
    if (theAlarm->onlyOnce == true)
    {

        if (theAlarm->hour < timeAlarm.Hour || (theAlarm->hour == timeAlarm.Hour && theAlarm->minute < timeAlarm.Minute))
        {
            offsetToAdd = offsetToAdd + 86400; // One day
        }
    } else {
        uint64_t daysOffset = offsetToAddForDays(unixTimeNow, theAlarm, timeAlarm);
        debugLog("offsetToAddForDays added: " + String(daysOffset));
        offsetToAdd = offsetToAdd + daysOffset;
    }

    timeAlarm.Hour = theAlarm->hour;
    timeAlarm.Minute = theAlarm->minute;


    uint64_t unixTimeCalc = getUnixTime(timeAlarm);
    unixTimeCalc = (unixTimeCalc / 60) * 60; // Remove seconds
    unixTimeCalc = unixTimeCalc + offsetToAdd;
    debugLog("Unix time now is: " + String(unixTimeNow));
    debugLog("Unix time for alarm is: " + String(unixTimeCalc));
    debugLog("The alarm name is: " + alarmNameGet(theAlarm));
    return unixTimeCalc;
}

void calculateNextAlarm() {
    uint64_t smallestUnix = UINT64_MAX;
    uint8_t c = 0;
    for(uint8_t i = 0; i < MAX_ALARMS; i++) {
        uint64_t alarmUnix = getUnixTimeOfAlarm(&rM.alarms[i]);
        if(alarmUnix < smallestUnix) {
            smallestUnix = alarmUnix;
            c = i;
        }
    }
    debugLog("Next alarm is: " + String(smallestUnix) + " on index: " + String(c));
    rM.nextAlarm = smallestUnix;
    rM.nextAlarmIndex = c;
}

String alarmGetTime(inkAlarm *theAlarm)
{
    String hour = String(theAlarm->hour);
    if (hour.length() < 2)
    {
        hour = "0" + hour;
    }
    String minute = String(theAlarm->minute);
    if (minute.length() < 2)
    {
        minute = "0" + minute;
    }
    return hour + ":" + minute;
}

String alarmGetDays(inkAlarm *theAlarm)
{
    String days = "";
    if (getBit(theAlarm->days, 0) == true)
    {
        days = days + "Mo ";
    }
    if (getBit(theAlarm->days, 1) == true)
    {
        days = days + "Tu ";
    }
    if (getBit(theAlarm->days, 2) == true)
    {
        days = days + "We ";
    }
    if (getBit(theAlarm->days, 3) == true)
    {
        days = days + "Th ";
    }
    if (getBit(theAlarm->days, 4) == true)
    {
        days = days + "Fr ";
    }
    if (getBit(theAlarm->days, 5) == true)
    {
        days = days + "Sa ";
    }
    if (getBit(theAlarm->days, 6) == true)
    {
        days = days + "Su ";
    }
    if (days.length() > 1)
    {
        days.remove(days.length() - 1);
    }
    else
    {
        days = "No days";
    }
    return days;
}

String alarmNameGet(inkAlarm *theAlarm)
{
    String alarmName = "";
    alarmName = alarmGetTime(theAlarm);
    if (theAlarm->onlyOnce == true)
    {
        alarmName = alarmName + ", once";
    }
    else
    {
        alarmName = alarmName + ", " + alarmGetDays(theAlarm);
    }
    return alarmName;
}

#endif