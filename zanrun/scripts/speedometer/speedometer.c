#include "zcommon.acs"
#library "speedometer"

#include "speedfuncs.h"

#define BARFONTCOUNT 12

int BarGraphics[BARFONTCOUNT] = // These are the different graphics that make up the MPH bar.
{
    "MPHBAR1", "MPHBAR2", "MPHBAR3", "MPHBAR4", "MPHBAR5", "MPHBAR6",
    "MPHBAR7", "MPHBAR8", "MPHBAR9", "MPHBAR10", "MPHBAR11", "MPHBAR12",
};

/* Parameter meanings:
 *  speed   - the speed to display
 *  id      - the base ID for the speedometer - will use message IDs in the range [id, id+width-1]
 *  x, y    - origin of the speedometer
 *  scale   - how high speed needs to be to fill the bar once
 *  width   - how many bars can be drawn
 */

function void drawSpeedometer(int speed, int id, int x, int y, int scale, int width)
{
    int i, j, k;

    SetFont("SMALLFONT");
    x = itof(x); y = itof(y);
    
    for (i = 0; i < width; i++)
    {
        j = ((itof(i) + 0.5) * scale) / width;

        if (j < speed)
        {
            k = ceil((speed - j) / scale)-1;
            k = max(0, k % BARFONTCOUNT);
            SetFont(BarGraphics[k]);
            HudMessage(s:"A"; HUDMSG_PLAIN, id+i, CR_UNTRANSLATED, (x + (i * 2.0)) + 0.1, y, 0.5, 0.5);
        }
        else
        {
            HudMessage(s:""; HUDMSG_PLAIN, id+i, 0, 0,0, 0);
        }
    }
}

script 425 OPEN clientside
{
    // Use Z height when calculating speed?
    if (!GetCVar("zanrun_mph_usez"))
    {
        ConsoleCommand("set zanrun_mph_usez 0");
        ConsoleCommand("archivecvar zanrun_mph_usez");
    }

    // Choose between MPH, km/h, units/tic (integer), or units/tic (float) display
    if (!GetCVar("zanrun_mph_km"))
    {
        ConsoleCommand("set zanrun_mph_km 0");
        ConsoleCommand("archivecvar zanrun_mph_km");
    }

    // How tall do you think a Doomguy is, in inches?
    // Doomguy's height determines how long a meter is, so this CVar scales MPH and km/h display.
    if (!GetCVar("zanrun_mph_doomguyheight"))
    {
        ConsoleCommand("set zanrun_mph_doomguyheight 72");
        ConsoleCommand("archivecvar zanrun_mph_doomguyheight");
    }

    // This controls how fast you have to be, in units/tic, to fill up the analog speedometer once.
    // Remember, it can wrap around forever!
    if (!GetCVar("zanrun_mph_barscale"))
    {
        ConsoleCommand("set zanrun_mph_barscale 20");
        ConsoleCommand("archivecvar zanrun_mph_barscale");
    }

    // How quickly can the bar transition between two radically different speeds?
    if (!GetCVar("zanrun_mph_barslide"))
    {
        ConsoleCommand("set zanrun_mph_barslide 300");
        ConsoleCommand("archivecvar zanrun_mph_barslide");
    }
}


script 424 ENTER clientside
{
    int pln = PlayerNumber();

    // f        - friction; zanrun_mph_barslide
    // s        - scale; zanrun_mph_barscale
    // x, y, z  - coordinates
    // m        - speed
    // c        - "close enough" threshold
    // mph      - what the digital speedometer shows; either km/h or mph
    // unitCm   - one cm in doom units; based on zanrun_mph_doomguyheight
    //
    // Why are all these variable names one letter?
    // It's so the calculation lines don't get too long.
    
    int i, f, s, c, x, y, z, m, mph, unitCm;
    int showmag = 0;

    // Remove these two lines, and "dead speedometer" bugs start cropping up.
    if (pln != ConsolePlayerNumber()) { terminate; }
    if (ClassifyActor(0) & ACTOR_WORLD) { terminate; }

    while (1)
    {
        // The below block will hide the speedometer when the HUD is hidden through screenblocks.
        // Feel free to add your own hiding clauses.
        if (GetCVar("screenblocks") >= 12)
        {
            // Hides the 'digital' part
            HudMessage(s:""; HUDMSG_PLAIN, 3500, 0, 0,0, 0);

            // Hides the 'analog' part
            for (x = 0; x < 50; x++) { HudMessage(s:""; HUDMSG_PLAIN, 3501+x, 0, 0,0, 0); }

            showmag = 0;
            Delay(1);
            continue;
        }

        // Get the speed for displaying
        x = GetActorVelX(0);
        y = GetActorVelY(0);
        z = cond(GetCVar("zanrun_mph_usez"), GetActorVelZ(0), 0);

        m = magnitudeThree_f(x, y, z);

        /*
         * Higher friction means slower slide - 100 or lower means instant,
         * 200 means 'half distance every tic', 400 means 'quarter distance every tic',
         * and so on and so forth
         */

        f = GetCVar("zanrun_mph_barslide");
        s = abs(GetCVar("zanrun_mph_barscale"));
        c = s * 0.01;

        if (f <= 100) { showmag = m; }                              // 2fast2slide
        else if (abs(m - showmag) < c) { showmag = m; }      // snap it to the actual speed, it's close enough
        else { showmag += FixedDiv(m - showmag, itof(f) / 100); }   // slide

        SetHudSize(800, 600, 1);
        drawSpeedometer(showmag, 3501, 700, 80, s, 50);
        
        // Pixels in doom are 20% taller than they are wide; account for that
        // Doomguy's sprite is 51 pixels tall starting at its origin, hence the 51
        unitCm = FixedDiv(itof(GetCVar("zanrun_mph_doomguyheight")) / 51, 1.2);
        unitCm = FixedMul(unitCm, 2.54); // inches/tic -> cm/tic

        mph = FixedMul(m, unitCm) / 100;    // cm/tic -> m/tic
        mph = FixedMul(mph, SECOND_TICS);   // m/tic  -> m/second

        if (GetCVar("zanrun_mph_km")) { mph /= 1000; } // m/second -> km/second  
        else { mph = FixedDiv(mph, 1609.344); } // m/second -> miles/second
        mph *= 3600;                        // second -> hour
        mph >>= 16;                         // we want an integer :(

        SetHudSize(640, 480, 1);
        SetFont("SMALLFONT");

        switch (GetCVar("zanrun_mph_km"))
        {
          case 3: HudMessage(f:m, s:"\c- units/tic"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5); break;
          case 2: HudMessage(d:round(m), s:"\c- units/tic"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5); break;
          case 1: HudMessage(d:mph, s:"\c- km/h"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5); break;
          case 0: HudMessage(d:mph, s:"\c- mph"; HUDMSG_FADEOUT, 3500, CR_BRICK, 552.2, 64.0, 0.5, 0.5); break;
        }

        Delay(1);
    }
}
