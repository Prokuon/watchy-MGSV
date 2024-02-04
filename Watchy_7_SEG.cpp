#include "Watchy_7_SEG.h"

// change it to your location 
//          latitude, longitude, timezone
#define LOC 31.00, 121.00, 8

#define Time1Zone 8
#define Time2Zone -5

RTC_DATA_ATTR bool DARKMODE = false;
RTC_DATA_ATTR bool showState = false;
RTC_DATA_ATTR bool HOUR_SET = true;

void Watchy7SEG::handleButtonPress()
{
    if (guiState == WATCHFACE_STATE)
    {
        uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
        if (wakeupBit & UP_BTN_MASK) 
        {
            showState = !showState;
            RTC.read(currentTime);
            showWatchFace(true);
            return;
        }
        if (wakeupBit & DOWN_BTN_MASK) 
        {
            HOUR_SET = !HOUR_SET;
            RTC.read(currentTime);
            showWatchFace(true);
            return;
        }
        if (wakeupBit & BACK_BTN_MASK) 
        {
            DARKMODE = !DARKMODE;
            RTC.read(currentTime);
            showWatchFace(true);
            return;
        } 
        if (wakeupBit & MENU_BTN_MASK) 
        {
            Watchy::handleButtonPress();
            return;
        }
    } 
    else {Watchy::handleButtonPress();}
    return;
}

void Watchy7SEG::drawWatchFace()
{
    display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
    display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    drawHands();
    drawBg ();
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();
    display.drawBitmap(152, 98, WIFI_CONFIGURED ? wifion : wifioff, 25, 18, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    display.drawBitmap(179, 98, BLE_CONFIGURED ? bleon : bleoff, 11, 18, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    drawSun();
}

void Watchy7SEG::drawBg()
{
    display.drawBitmap(0, 0, background, 200, 200, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawTime()
{
    long ss = currentTime.Hour * 60 + currentTime.Minute;
    long l5 = 33 * ss / (60 * 24);

    display.fillRect(158, 52, l5, 7, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);


    int sh = ss / 60;

    // PM
    if (HOUR_SET == false && sh >= 12)
        display.drawBitmap(26, 112, PM, 9, 5, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    // AM
    else if (HOUR_SET == false && sh < 12)
        display.drawBitmap(26, 95, AM, 9, 5, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    int sk = sh;
    // adjust hour to 21-hour
    if (HOUR_SET == false && sh > 12)
        sk = sh - 12;

    int sm = ss % 60;
    int a = sk >= 10 ? sh / 10 : 0;
    int b = sk % 10;
    int c = sm >= 10 ? sm / 10 : 0;
    int d = sm % 10;

    drawNuma(38, 95, a);
    drawNuma(58, 95, b);
    drawNuma(87, 95, c);
    drawNuma(107, 95, d);

    if (showState == true)
    {
        sh = sh + Time2Zone - Time1Zone;
        if (sh < 0)
            sh = sh + 24;
        else if (sh > 23)
            sh = sh % 24;
        

        // PM
        if (HOUR_SET == false && sh >= 12)
            display.drawBitmap(26, 43, PM, 9, 5, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
        // AM
        else if (HOUR_SET == false && sh < 12)
            display.drawBitmap(26, 26, AM, 9, 5, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

        // adjust hour to 21-hour
        if (HOUR_SET == false && sh > 12)
            sh -= 12;

        sm = ss % 60;
        a = sh >= 10 ? sh / 10 : 0;
        b = sh % 10;
        c = sm >= 10 ? sm / 10 : 0;
        d = sm % 10;

        drawNuma(38, 26, a);
        drawNuma(58, 26, b);
        drawNuma(87, 26, c);
        drawNuma(107, 26, d);

        display.fillRect(79, 43, 4, 4, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

        display.fillRect(79, 35, 4, 4, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

        display.drawBitmap(181, 10, dual, 10, 7, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
        
    }
}


void Watchy7SEG::drawDate()
{

    String dayOfWeek = dayStr(currentTime.Wday);
    drawWeek(155, 131, dayOfWeek);

    String month = monthShortStr(currentTime.Month);

    drawMonth(79, 157, month);

    int da = currentTime.Day;

    int ye = currentTime.Year + 1970;
    int a = da / 10;
    int b = da % 10;
    ye = ye % 1000;
    ye = ye % 100;
    int e = ye / 10;
    ye = ye % 10;
    int f = ye;
    if (a > 0)
        drawNumb(147, 162, a);
    drawNumb(164, 162, b);
    drawNuma(19, 157, e);
    drawNuma(39, 157, f);
}


void Watchy7SEG::drawSteps()
{
    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0)
    {
        sensor.resetStepCounter();
    }
    uint32_t stepCount = sensor.getCounter();

    if (showState == false)
    {
        uint32_t l5 = 33 * stepCount / 10000;

        if (l5 > 33)
        {
            l5 = 33;
        }

        display.fillRect(158, 37, l5, 7, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
        // display.

        int a = stepCount / 10000;
        stepCount = stepCount % 10000;
        int b = stepCount / 1000;
        stepCount = stepCount % 1000;
        int c = stepCount / 100;
        stepCount = stepCount % 100;
        int d = stepCount / 10;
        int e = stepCount % 10;

        if (a != 0)
            drawNuma(27, 26, a);
        if (!(a == 0 && b == 0))
            drawNuma(47, 26, b);
        if (!(a == 0 && b == 0 && c == 0))
            drawNuma(67, 26, c);
        if (!(a == 0 && b == 0 && c == 0 && d == 0))
            drawNuma(87, 26, d);
        drawNuma(107, 26, e);
    }

}

void Watchy7SEG::drawBattery()
{
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.1)
    {
        batteryLevel = 33;
    }
    else if(VBAT > 4.05 && VBAT <= 4.1)
    {
        batteryLevel = 29;
    }
    else if(VBAT > 4 && VBAT <= 4.05)
    {
        batteryLevel = 25;
    }

    else if(VBAT > 3.95 && VBAT <= 4)
    {
        batteryLevel = 21;
    }
    else if(VBAT > 3.9 && VBAT <= 3.95)
    {
        batteryLevel = 17;
    }
    else if(VBAT > 3.85 && VBAT <= 3.9)
    {
        batteryLevel = 13;
    }
    else if(VBAT > 3.8 && VBAT <= 3.85)
    {
        batteryLevel = 8;
    }
    else if(VBAT > 3.75 && VBAT <= 3.8)
    {
        batteryLevel = 4;
    }
    else if(VBAT <= 3.75)
    {
        batteryLevel = 0;
    }


    display.fillRect(158, 22, batteryLevel, 7, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawSun() 
{
    Dusk2Dawn location(LOC);
    int year = currentTime.Year + 1970;
    int32_t month = currentTime.Month;
    int32_t day = currentTime.Day;
    int sr = location.sunrise(year, month, day, false);
    int ss = location.sunset(year, month, day, false);

    int lft = 33 * sr / 24 / 60;
    int rgt = 33 * ss / 24 / 60;

    display.fillRect(158 + lft, 60, rgt - lft, 3, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

    int rh = sr / 60;
    int rm = sr % 60;
    int sh = ss / 60;
    int sm = ss % 60;

    if (HOUR_SET == false && rh > 12)
        rh -= 12;

    if (HOUR_SET == false && sh >12)
        sh -= 12;

    int a = sh >= 10 ? sh / 10 : 0;
    int b = sh % 10;
    int c = sm >= 10 ? sm / 10 : 0;
    int d = sm % 10;


    int e = rh >= 10 ? rh / 10 : 0;
    int f = rh % 10;
    int g = rm >= 10 ? rm / 10 : 0;
    int h = rm % 10;

    drawNumc(165, 68, a);
    drawNumc(171, 68, b);
    drawNumc(179, 68, c);
    drawNumc(185, 68, d);

    drawNumc(165, 83, e);
    drawNumc(171, 83, f);
    drawNumc(179, 83, g);
    drawNumc(185, 83, h);
}

void Watchy7SEG::drawNuma(int a, int b, int c) 
{
    if (c == 0)
        display.drawBitmap(a, b, numa0, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 1)
        display.drawBitmap(a, b, numa1, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 2)
        display.drawBitmap(a, b, numa2, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 3)
        display.drawBitmap(a, b, numa3, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 4)
        display.drawBitmap(a, b, numa4, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 5)
        display.drawBitmap(a, b, numa5, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 6)
        display.drawBitmap(a, b, numa6, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 7)
        display.drawBitmap(a, b, numa7, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 8)
        display.drawBitmap(a, b, numa8, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 9)
        display.drawBitmap(a, b, numa9, 17, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawNumb(int a, int b, int c)
{
    if (c == 0)
        display.drawBitmap(a, b, numb0, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 1)
        display.drawBitmap(a, b, numb1, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 2)
        display.drawBitmap(a, b, numb2, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 3)
        display.drawBitmap(a, b, numb3, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 4)
        display.drawBitmap(a, b, numb4, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 5)
        display.drawBitmap(a, b, numb5, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 6)
        display.drawBitmap(a, b, numb6, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 7)
        display.drawBitmap(a, b, numb7, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 8)
        display.drawBitmap(a, b, numb8, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 9)
        display.drawBitmap(a, b, numb9, 15, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawWeek(int a, int b, String c)
{
    if (c == "Monday")
        display.drawBitmap(a, b, week1, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Tuesday")
        display.drawBitmap(a, b, week2, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Wednesday")
        display.drawBitmap(a, b, week3, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Thursday")
        display.drawBitmap(a, b, week4, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Friday")
        display.drawBitmap(a, b, week5, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Saturday")
        display.drawBitmap(a, b, week6, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Sunday")
        display.drawBitmap(a, b, week7, 32, 25, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawMonth(int a, int b, String c)
{
    if (c == "Jan")
        display.drawBitmap(a, b, month1, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Feb")
        display.drawBitmap(a, b, month2, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Mar")
        display.drawBitmap(a, b, month3, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Apr")
        display.drawBitmap(a, b, month4, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "May")
        display.drawBitmap(a, b, month5, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Jun")
        display.drawBitmap(a, b, month6, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Jul")
        display.drawBitmap(a, b, month7, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Aug")
        display.drawBitmap(a, b, month8, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Sep")
        display.drawBitmap(a, b, month9, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Oct")
        display.drawBitmap(a, b, month10, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Nov")
        display.drawBitmap(a, b, month11, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == "Dec")
        display.drawBitmap(a, b, month12, 60, 30, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawNumc(int a, int b, int c)
{
    if (c == 0)
        display.drawBitmap(a, b, numc0, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 1)
        display.drawBitmap(a, b, numc1, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 2)
        display.drawBitmap(a, b, numc2, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 3)
        display.drawBitmap(a, b, numc3, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 4)
        display.drawBitmap(a, b, numc4, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 5)
        display.drawBitmap(a, b, numc5, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 6)
        display.drawBitmap(a, b, numc6, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 7)
        display.drawBitmap(a, b, numc7, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 8)
        display.drawBitmap(a, b, numc8, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    else if (c == 9)
        display.drawBitmap(a, b, numc9, 5, 9, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}

void Watchy7SEG::drawHands()
{
    if (currentTime.Minute == 1)
        display.drawBitmap(20, 13, uphands01, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 2)
        display.drawBitmap(20, 13, uphands02, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 3)
        display.drawBitmap(20, 13, uphands03, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 4)
        display.drawBitmap(20, 13, uphands04, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 5)
        display.drawBitmap(20, 13, uphands05, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 6)
        display.drawBitmap(20, 13, uphands06, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute >= 7 || currentTime.Minute == 0)
        display.drawBitmap(20, 13, uphands07, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 8)
        display.drawBitmap(127, 20, rgthands08, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 9)
        display.drawBitmap(127, 20, rgthands09, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 10)
        display.drawBitmap(127, 20, rgthands10, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 11)
        display.drawBitmap(127, 20, rgthands11, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 12)
        display.drawBitmap(127, 20, rgthands12, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 13)
        display.drawBitmap(127, 20, rgthands13, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 14)
        display.drawBitmap(127, 20, rgthands14, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 15)
        display.drawBitmap(127, 20, rgthands15, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 16)
        display.drawBitmap(127, 20, rgthands16, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 17)
        display.drawBitmap(127, 20, rgthands17, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 18)
        display.drawBitmap(127, 20, rgthands18, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 19)
        display.drawBitmap(127, 20, rgthands19, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 20)
        display.drawBitmap(127, 20, rgthands20, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 21)
        display.drawBitmap(127, 20, rgthands21, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute >= 22 || currentTime.Minute == 0)
        display.drawBitmap(127, 20, rgthands22, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 23)
        display.drawBitmap(20, 127, downhands23, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 24)
        display.drawBitmap(20, 127, downhands24, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 25)
        display.drawBitmap(20, 127, downhands25, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 26)
        display.drawBitmap(20, 127, downhands26, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 27)
        display.drawBitmap(20, 127, downhands27, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 28)
        display.drawBitmap(20, 127, downhands28, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 29)
        display.drawBitmap(20, 127, downhands29, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 30)
        display.drawBitmap(20, 127, downhands30, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 31)
        display.drawBitmap(20, 127, downhands31, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 32)
        display.drawBitmap(20, 127, downhands32, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 33)
        display.drawBitmap(20, 127, downhands33, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 34)
        display.drawBitmap(20, 127, downhands34, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 35)
        display.drawBitmap(20, 127, downhands35, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 36)
        display.drawBitmap(20, 127, downhands36, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute >= 37 || currentTime.Minute == 0)
        display.drawBitmap(20, 127, downhands37, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 38)
        display.drawBitmap(13, 20, lfthands38, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 39)
        display.drawBitmap(13, 20, lfthands39, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 40)
        display.drawBitmap(13, 20, lfthands40, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 41)
        display.drawBitmap(13, 20, lfthands41, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 42)
        display.drawBitmap(13, 20, lfthands42, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 43)
        display.drawBitmap(13, 20, lfthands43, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 44)
        display.drawBitmap(13, 20, lfthands44, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 45)
        display.drawBitmap(13, 20, lfthands45, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 46)
        display.drawBitmap(13, 20, lfthands46, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 47)
        display.drawBitmap(13, 20, lfthands47, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 48)
        display.drawBitmap(13, 20, lfthands48, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 49)
        display.drawBitmap(13, 20, lfthands49, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 50)
        display.drawBitmap(13, 20, lfthands50, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 51)
        display.drawBitmap(13, 20, lfthands51, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute >= 52 || currentTime.Minute == 0)
        display.drawBitmap(13, 20, lfthands52, 11, 111, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 53)
        display.drawBitmap(20, 13, uphands53, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 54)
        display.drawBitmap(20, 13, uphands54, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 55)
        display.drawBitmap(20, 13, uphands55, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 56)
        display.drawBitmap(20, 13, uphands56, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 57)
        display.drawBitmap(20, 13, uphands57, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 58)
        display.drawBitmap(20, 13, uphands58, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 59)
        display.drawBitmap(20, 13, uphands59, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if (currentTime.Minute == 0)
        display.drawBitmap(20, 13, uphands60, 111, 11, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

}
