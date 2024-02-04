#ifndef WATCHY_7_SEG_H
#define WATCHY_7_SEG_H
#include <math.h>
#include <Watchy.h>
#include "Dusk2Dawn.h"
#include "icons.h"

class Watchy7SEG : public Watchy{
    using Watchy::Watchy;
    public:
        void drawWatchFace();
        void drawHands();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();
        void drawBg();
        void drawSun();

        void drawNuma(int a, int b, int c);
        void drawNumb(int a, int b, int c);
        void drawNumc(int a, int b, int c);
        void drawWeek(int a, int b, String c);
        void drawMonth(int a, int b, String c);
        virtual void handleButtonPress();


};



#endif
