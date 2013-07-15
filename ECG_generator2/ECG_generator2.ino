/**************************************************************************/
/*!
    @file     ECG_generator.ino
    @author   Thijs Elenbaas

    Displays a ECG signal 

*/
/**************************************************************************/
#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

// Set this value to 9, 8, 7, 6 or 5 to adjust the resolution
#define DAC_RESOLUTION    (8)

// IF DAC_RESOLUTION 0 is used values are calculated on the fly
const int PERIOD = 512;
const float INV_PERIOD = 1.0/(float)PERIOD;
const int AMPLITUDE = 4095;

const float B = 4/PI;
const float C = -4/(PI*PI);

/* Note: If flash space is tight a quarter sine wave is enough
   to generate full sine and cos waves, but some additional
   calculation will be required at each step after the first
   quarter wave.                                              */

PROGMEM uint16_t DACLookup_ECG[842] =
{
    393, 392, 394, 395, 398, 401, 404, 407, 409, 410, 410, 409, 408, 405, 401, 398, 394, 391, 390, 390, 389, 390, 395, 402, 411, 418, // Items 0 to 26
    426, 434, 444, 451, 454, 457, 461, 464, 467, 471, 476, 486, 495, 503, 512, 522, 534, 543, 549, 552, 555, 561, 564, 564, 566, 570, // Items 26 to 52
    578, 586, 592, 602, 614, 627, 638, 646, 652, 659, 665, 669, 667, 665, 667, 670, 671, 669, 671, 678, 684, 690, 693, 696, 701, 702, // Items 52 to 78
    701, 693, 687, 682, 674, 668, 659, 655, 653, 650, 646, 643, 642, 642, 638, 631, 624, 614, 606, 593, 578, 564, 551, 540, 528, 517, // Items 78 to 104
    507, 501, 495, 487, 479, 473, 467, 460, 451, 439, 431, 424, 416, 408, 398, 392, 389, 393, 384, 382, 383, 386, 390, 387, 385, 384, // Items 104 to 130
    382, 378, 371, 365, 362, 360, 358, 356, 354, 357, 361, 364, 364, 363, 362, 362, 358, 350, 342, 334, 329, 324, 318, 315, 316, 322, // Items 130 to 156
    329, 338, 351, 368, 387, 408, 436, 469, 506, 551, 604, 666, 737, 817, 906,1006,1110,1222,1342,1466,1594,1720,1847,1974,2099,2222, // Items 156 to 182
   2339,2451,2558,2658,2742,2810,2866,2908,2923,2913,2879,2825,2748,2651,2538,2406,2266,2119,1967,1814,1659,1509,1366,1229,1100, 979, // Items 182 to 208
    868, 766, 674, 590, 513, 446, 368, 338, 294, 254, 224, 204, 189, 175, 168, 169, 175, 182, 186, 194, 202, 212, 219, 222, 228, 234, // Items 208 to 234
    241, 246, 250, 258, 268, 279, 288, 298, 307, 317, 322, 324, 323, 322, 323, 320, 318, 313, 312, 316, 318, 322, 325, 332, 339, 343, // Items 234 to 260
    345, 344, 345, 347, 345, 341, 338, 338, 340, 342, 345, 350, 359, 369, 376, 382, 387, 391, 394, 396, 395, 396, 395, 396, 398, 398, // Items 260 to 286
    402, 408, 416, 424, 432, 438, 445, 450, 455, 458, 458, 459, 459, 460, 460, 461, 464, 467, 474, 478, 483, 489, 495, 502, 505, 506, // Items 286 to 312
    510, 510, 510, 506, 502, 499, 497, 498, 495, 496, 502, 509, 515, 522, 528, 536, 543, 547, 549, 548, 550, 552, 552, 553, 557, 565, // Items 312 to 338
    572, 579, 588, 601, 612, 622, 627, 632, 640, 647, 651, 653, 656, 662, 671, 680, 686, 695, 708, 722, 733, 740, 749, 760, 770, 775, // Items 338 to 364
    778, 782, 789, 797, 805, 810, 818, 833, 847, 860, 870, 881, 896, 908, 915, 921, 926, 936, 945, 950, 957, 966, 982, 995,1008,1022, // Items 364 to 390
   1038,1054,1067,1078,1087,1099,1109,1116,1120,1126,1136,1143,1151,1156,1166,1178,1189,1200,1207,1215,1226,1233,1237,1238,1241,1245, // Items 390 to 416
   1245,1242,1242,1245,1249,1252,1255,1259,1266,1273,1276,1278,1278,1278,1275,1268,1261,1250,1241,1232,1222,1212,1202,1193,1186,1176, // Items 416 to 442
   1163,1148,1133,1116,1097,1075,1052,1029,1006, 985, 962, 942, 922, 904, 886, 867, 849, 829, 810, 790, 767, 745, 721, 698, 675, 650, // Items 442 to 468
    627, 608, 590, 574, 558, 542, 530, 517, 502, 489, 466, 451, 434, 418, 398, 382, 368, 356, 344, 331, 324, 318, 312, 302, 293, 286, // Items 468 to 494
    281, 272, 261, 250, 240, 234, 226, 218, 211, 207, 206, 206, 203, 202, 205, 206, 206, 203, 201, 202, 199, 194, 186, 179, 178, 177, // Items 494 to 520
    174, 173, 174, 178, 182, 186, 187, 191, 194, 194, 191, 189, 186, 185, 182, 179, 177, 178, 181, 182, 186, 189, 192, 195, 196, 196, // Items 520 to 546
    194, 192, 190, 188, 186, 185, 185, 188, 191, 194, 201, 206, 213, 215, 218, 221, 221, 222, 219, 218, 216, 216, 217, 217, 218, 222, // Items 546 to 572
    230, 236, 242, 246, 250, 254, 255, 256, 254, 251, 250, 249, 247, 246, 246, 249, 254, 258, 259, 262, 267, 272, 274, 272, 270, 268, // Items 572 to 598
    268, 267, 264, 263, 264, 267, 269, 271, 275, 282, 289, 292, 294, 297, 301, 302, 301, 296, 292, 291, 290, 288, 286, 287, 291, 294, // Items 598 to 624
    298, 302, 307, 312, 314, 311, 310, 310, 308, 305, 299, 298, 298, 299, 302, 303, 308, 314, 317, 318, 317, 317, 316, 312, 305, 298, // Items 624 to 650
    295, 294, 291, 289, 288, 291, 298, 302, 304, 304, 306, 308, 306, 301, 296, 292, 290, 287, 283, 282, 285, 288, 291, 294, 298, 299, // Items 650 to 676
    301, 300, 299, 298, 295, 293, 290, 286, 283, 283, 284, 286, 288, 290, 294, 297, 301, 303, 303, 302, 301, 298, 295, 291, 288, 283, // Items 676 to 702
    281, 278, 278, 279, 281, 284, 286, 288, 292, 294, 295, 294, 291, 290, 287, 286, 282, 282, 282, 283, 286, 289, 294, 299, 303, 306, // Items 702 to 728
    307, 307, 308, 306, 303, 299, 296, 297, 298, 298, 301, 305, 311, 318, 318, 320, 320, 319, 318, 314, 307, 304, 302, 299, 297, 295, // Items 728 to 754
    298, 302, 306, 308, 310, 312, 314, 313, 309, 304, 302, 299, 297, 291, 288, 288, 291, 294, 297, 301, 306, 310, 311, 310, 308, 310, // Items 754 to 780
    308, 302, 298, 293, 292, 293, 291, 291, 292, 298, 302, 304, 306, 307, 312, 313, 310, 309, 310, 310, 309, 305, 302, 305, 307, 310, // Items 780 to 806
    311, 314, 319, 326, 330, 330, 332, 334, 335, 334, 330, 329, 332, 333, 334, 333, 337, 344, 350, 357, 360, 366, 373, 377, 381, 381, // Items 806 to 832
    383, 387, 390, 393, 398, 406, 402, 400, 399   // Items 832 to 841

};

void setup(void) {
  Serial.begin(9600);
  Serial.println("Starting ");

  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  dac.begin(0x62);

  Serial.println("Generating a ECG wav");
}

void loop(void) {
    // Push out the right lookup table, depending on the selected resolution
     int startTime = millis();
     runScaledECG();
     int stopTime = millis();
     Serial.println(stopTime-startTime);
     
}

void runUnscaledECG()
{
     for (uint16_t i = 0; i < 841; i++)
     {
        dac.setVoltage(pgm_read_word(&(DACLookup_ECG[i])), false);
        delayMicroseconds(1000);
     }
}

void runScaledECG()
{
   double Slope;
   int Offset;
   int bps;
  
   // P
   Slope = (float)random(500,1500)/1000.0;
   Offset = 393;
   bps = 1000;
   for (uint16_t i = 0; i < 121; i++)
   {     
     int value = (int)((double)((double)pgm_read_word(&(DACLookup_ECG[i])) - (double)Offset) * Slope) + Offset;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   // in between P- R
   bps = 1000;
   for (uint16_t i = 121; i < 159; i++)
   {
      int value = pgm_read_word(&(DACLookup_ECG[i])) ;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   
   // R
   Slope = (float)random(500,1300)/1000.0;
   Offset = 368;
   bps = 1000;
   for (uint16_t i = 159; i < 214; i++)
   {
      int value = (int)((double)((double)pgm_read_word(&(DACLookup_ECG[i])) - (double)Offset) * Slope) + Offset;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   
   // in between R- T
   bps = 1000;
   for (uint16_t i = 214; i < 307; i++)
   {
      int value = pgm_read_word(&(DACLookup_ECG[i])) ;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   
   // T
   Slope = (float)random(300,1200)/1000.0;
   Offset = 489;
   bps = 1000;
   for (uint16_t i = 307; i < 477; i++)
   {
      int value = (int)((double)((double)pgm_read_word(&(DACLookup_ECG[i])) - (double)Offset) * Slope) + Offset;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   
   // in between T - diastole
   bps = 1000;
   for (uint16_t i = 477; i < 539; i++)
   {
      int value = pgm_read_word(&(DACLookup_ECG[i])) ;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   
   // Diastole
   bps = random(750,2500);
   for (uint16_t i = 539; i < 788; i++)
   {
      int value = pgm_read_word(&(DACLookup_ECG[i])) ;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
   
   // in between Diastole - end
   bps = 1000;
   for (uint16_t i = 788; i < 841; i++)
   {
      int value = pgm_read_word(&(DACLookup_ECG[i])) ;
      dac.setVoltage(value, false);
      delayMicroseconds(bps);
   }
}

