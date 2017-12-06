//BIRDTAF variant running on Teensy 3.6
//Hamish Mehaffey,  Dec. 2017. 
//as always, a work in progress. 
//
// William.Mehaffey@ucsf.edu
// Distributed as-is. No warranty given. 
//
// 

// ARM library.....
#define ARM_MATH_CM4
#include <arm_math.h>

arm_status status;
////////////////////////////////////////////////////////////////////////////////
// CONIFIGURATION
// Values controlling the acquisition, template matching, amplitude thresholds, etc.
////////////////////////////////////////////////////////////////////////////////

///////////// LEDs. Probably not necessary.
const int POWER_LED_PIN = 13;          // Output pin for power LED (pin 13 = onboard LED).

///////////////
// Analog Input Settings for Sound. Note that this does not effect AUDIO_MIRROR_OUT (on PCB), which is hardware w/ 0 lag.
//////////////
int SAMPLE_RATE_HZ = 44100;            // Sample rate of the audio in Hz. 221000 is pretty accurate. There can be some aliasing w/ a 15khz mic though. I've run it a 44khz successfully.
const int FFT_SIZE = 512;              // Size of the FFT. 256/1024 use cfft, 512 uses rfft, could be simplified by upgrading to CMSIS4.5. 256 was the evtaf standard and is pretty fast. 
const int BUFFER_SIZE = FFT_SIZE*4;
const int TEMPLT_SIZE = FFT_SIZE / 2;
const int AUDIO_INPUT_PIN = A1;        // Input ADC pin for audio data. Same for Teensy 3.1/3.2/3.5/3.6
const int ANALOG_READ_RESOLUTION = 16; // Bits of resolution for the ADC. Realisticity it's more like 12-14 due to noise, the last few bits of precision aren't all that informative nehow
const int ANALOG_READ_AVERAGING = 1;   // Number of samples to average with each ADC reading.

///////////////// White Noise Output Stuff.
const int ANALOG_WRITE_RESOLUTION = 12; // DAC
const int AUDIO_OUTPUT_PIN = A21;     // DAC0 on Teensy 3.6 A14 for DAC0 on Teensy 3.1/2
const int BNC_TRIGGER_OUTPUT_PIN = 39;    // Pin 39 for Teensy3.6. --> BNC
const int TRIGGER_OUTPUT_PIN = 21;    // Pin 21 -> soundcard channel 2. 


// Array for WN playback. You could also put filtered WN here....
// WN should be whatever you need for whatever duration @ yr chosen sampling frequency.

const int WN_SIZE = 1024; //I should make this random and variable at some point. As for now, it's 1024 samples @ whatever sampling.
int16_t wn[WN_SIZE] = {24442,27174,3810,27401,18971,2926,8355,16406,28725,28947,4728,29118,28715,14561,24008,4257,12653,27472,23766,28785,19672,1071,25474,28020,20362,22732,22294,11767,19664,5136,21181,955,8308,1385,2914,24704,20845,9513,28507,1033,13162,11447,22966,23856,5606,14693,13368,19389,21281,22641,8281,20391,19653,4878,3570,14951,28792,10212,17558,6714,22538,7653,15179,20972,26727,28779,16416,4159,4479,7725,25222,7628,24429,7306,27878,10500,5898,7533,18481,14199,10550,24925,17558,16492,27516,8575,22716,22612,11413,17035,2276,1619,15924,23375,28020,3897,17065,14082,357,10114,4865,23829,9336,15856,4969,18059,7889,19622,20676,22445,13516,2515,6869,27400,4571,24775,16150,29884,2345,13280,3200,28857,139,23247,24519,26061,2533,11993,7796,24002,12942,27319,5455,7914,4366,4082,26079,17391,16496,4349,25591,18662,10529,15397,12054,2279,7197,3700,5517,7199,12518,1490,27081,28344,14726,14678,10132,27002,11077,3336,23408,11692,7251,12117,2894,3959,28262,28684,17256,1793,7043,10595,24636,462,1291,5070,19473,21952,19432,13528,16410,8890,22341,5669,20603,5505,11055,18769,23407,2434,27882,23271,14604,13076,13404,9190,15255,15323,24529,23845,19330,11358,24347,15985,10522,28170,26278,16505,18674,17611,6232,9037,14128,6915,25329,5843,6778,5121,6830,13071,9333,27701,12906,5544,27146,29392,13166,3334,7742,12262,17847,7866,18085,21336,6652,3523,8900,9563,12725,15236,2565,7874,24030,877,27866,21910,14658,17356,7119,13765,28893,16404,15634,6948,14667,18722,20374,11865,11023,29639,1132,26555,27399,23886,2961,7856,10061,20392,4097,21637,3203,19613,14825,23372,21451,27112,26728,10025,20962,5934,916,22322,15001,14398,27142,18296,18530,25783,24165,17302,5488,7198,26595,860,14697,5038,29360,21381,15014,14133,1789,20459,1273,2143,15649,2902,24544,24526,21673,4496,19788,15558,29189,19470,24010,13614,12972,24759,2504,3995,5202,11728,24941,24101,1814,11978,15806,12504,19706,18839,8760,12950,465,29522,5015,3186,11172,5944,14691,10185,28549,27610,1580,22136,8074,12685,16436,28282,12532,29492,9044,21033,19990,16174,20943,19996,5344,3840,29972,5134,978,16836,26456,20075,5713,11067,13822,29449,4692,25666,19343,11288,5728,12848,14461,3618,17685,6786,11539,17490,7554,8713,18513,7958,24731,29480,21907,10316,17522,3233,27189,26390,24533,7822,17831,675,12758,9382,4845,5363,12687,2827,17956,14128,20878,20997,19156,1008,2064,9588,15926,19633,12229,24599,21551,29059,15940,9754,3169,18329,23364,12704,2725,7994,4610,8430,13203,15814,13723,26261,15542,28309,19131,28731,7221,20284,8672,20154,20854,2040,7644,6721,20035,25332,10334,23416,20260,201,18065,11603,27480,35,13873,12730,13827,23105,9674,23542,14141,1073,5276,21653,14205,4582,10234,18222,5752,22153,7285,27523,8072,22965,5660,8625,2733,17286,20501,16398,12772,19333,19429,20371,19074,28355,6268,21278,7087,3582,18219,13504,13762,19858,23109,10507,19860,12485,25258,24988,7693,18404,17467,16222,26098,7943,9542,3576,28195,19367,14384,19180,16341,19419,16317,21631,15675,29811,6560,3174,3291,1908,12137,13451,10974,22905,18837,23159,27986,29182,5761,4166,20888,2815,15762,15910,25834,14546,11804,20143,22238,15602,10431,4500,17583,7864,1334,22648,7284,13272,20634,10777,22090,11841,20502,21121,13269,587,9926,12729,8108,5912,24652,12898,26633,11735,23073,11904,24255,22652,11322,6481,23712,28479,9827,20138,13159,25005,23066,5018,25859,29696,15433,26528,17641,4643,5996,12209,22461,24768,23699,9556,16022,2699,3351,4089,20360,14855,5691,14850,4428,1649,25521,16817,27888,20900,17484,24462,26370,29667,16,25963,18377,29699,15830,14386,24040,6835,14943,27026,17240,25355,22159,17580,7402,19992,2504,18779,19828,21893,26723,29469,23071,17443,27849,17403,509,3626,25881,14529,25346,6282,16569,18897,960,18441,10872,1486,14687,5775,3693,6165,4395,5672,1280,19056,8456,16158,20855,14973,16074,13355,3718,14711,25590,26218,8109,6254,16949,19209,12511,6179,28438,2462,3171,4261,4994,18629,17211,1562,27936,21860,22135,1902,25813,28032,29532,25768,23567,15401,5328,11958,4018,927,28174,9039,8866,9988,14012,19446,757,25266,16771,25623,10436,13381,1627,5313,19884,9925,26955,3545,29653,16199,21208,29985,8635,12436,13945,22919,24546,3007,5344,10789,1701,15657,10075,5270,6268,27155,20262,14054,27364,3120,22366,22088,16856,5526,17916,8998,4024,6378,26848,2144,7275,1613,13252,398,26916,5900,2801,9221,13682,3050,29862,9963,8920,1861,8947,1391,15163,22843,18932,2697,2426,23317,27154,16013,3275,24774,10143,8819,22389,310,1453,20037,18104,15783,21891,21218,23441,8639,20776,16700,11896,1848,23405,10128,18236,22238,3144,3837,16486,14557,26714,23969,22030,1540,2187,2656,23951,28290,20511,3962,21682,3311,3525,19222,9864,19614,22474,17496,22201,7045,22049,29118,26008,2587,10993,11076,20551,17938,23681,11030,6181,2600,23158,6170,11648,16553,6869,19258,14534,4555,23458,3018,8822,7121,15926,2745,12159,3145,3369,23533,8747,18106,28933,12975,20843,22743,12979,19665,3293,28013,5624,7985,23935,14628,23069,11880,8188,1117,20199,12887,13552,18296,1782,9474,23182,20893,3760,3905,2771,235,12693,19667,21688,15936,3265,18953,3795,4029,2958,4261,5048,5887,9524,9493,6527,7531,26788,21097,16672,5533,6361,2320,27414,21201,16734,9403,4986,18675,29638,5113,7734,11904,2220,20523,12072,29485,12066,18620,4631,11440,4834,22743,26133,10523,20566,8824,15919,24973,17925,10059,8977,13578,12679,10788,16750,22276,12730,12881,3746,733,8706,9526,19611,28708,28072,13737,7214,22917,22780,22219,22311,3178,20447,13898,6365,2956,24707,5250,4907,19980,26832,15497,21081,4608,28604,16227,20392,1097,24276,22459,3606,15751,9775,16393,11966,12453,5422,7662,616,27710,19611,27978,4905,27633,23840,17322,13201,7728,22558,6860,1926,23020,20136,21456,19262,12571,11723,24484,9523,24436,23672,25568,15169,19070,28527,13319,1801,26002,18936,10652,29910,6725,19574,18150,11617,4266,754,12633,5523,21773,11111,25247,22027,17131,5306,28722,7960,27737,6713,11207,2625,19203};;

///////////////////////Templates and Triggers..

//Template for matching.... made using the associated MatLab scripts....

//float templt[512]= {-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.00076923077,-0.10000000,0.0015384615,0.0015384615,0.0046153846,0.0076923077,0.0046153846,0.0053846152,0.0061538462,0.028461538,0.025384616,0.0092307692,0.093846157,0.19307692,0.32076922,0.066153847,0.57076925,0.059230771,0.31846154,0.49076924,0.40384614,0.12307692,0.27076924,0.32461539,0.49000001,0.037692308,0.15307692,0.066923074,0.026923077,0.020769231,0.0038461538,0.0099999998,0.012307692,0.0084615387,0.0084615387,0.0061538462,0.0069230767,0.0023076923,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.00076923077,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.00076923077,-0.10000000,0.00076923077,0.0046153846,-0.10000000,0.00076923077,0.0015384615,0.031538460,0.025384616,0.011538462,0.030769231,0.029230770,0.11923077,0.11153846,0.079230770,0.31000000,0.43000001,0.35076922,0.12538461,0.096153848,0.20076923,0.32923076,0.17692308,0.082307689,0.023076924,0.017692307,0.23153846,0.17076923,0.19153847,0.43692309,0.20538461,0.23692308,0.036923077,0.17307693,0.026153846,0.035384614,0.037692308,0.033846155,0.040769231,0.017692307,0.0092307692,0.013846153,0.011538462,0.0069230767,0.0069230767,0.0084615387,0.0084615387,0.0092307692,0.010769230,0.0092307692,0.0069230767,0.0015384615,0.0030769231,0.0038461538,0.00076923077,-0.10000000,-0.10000000,0.00076923077,-0.10000000,-0.10000000,-0.10000000,0.0030769231,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.0015384615,-0.10000000,-0.10000000,0.0038461538,0.00076923077,-0.10000000,-0.10000000,-0.10000000,0.0030769231,0.0046153846,0.0076923077,-0.10000000,-0.10000000,0.0030769231,0.0053846152,0.0030769231,0.0084615387,0.014615385,0.018461538,0.025384616,0.0092307692,0.016923077,0.026153846,0.067692310,0.023076924,0.11076923,0.13692307,0.20384616,0.18692307,0.023076924,0.11153846,0.37461537,0.31000000,0.24384615,0.21384615,0.10538462,0.026153846,0.0084615387,0.031538460,0.0084615387,0.023846153,0.059999999,0.020000000,0.020000000,0.037692308,0.018461538,0.0046153846,0.0069230767,0.0023076923,0.0015384615,-0.10000000,0.00076923077,0.014615385,0.011538462,0.0038461538,0.0076923077,0.0046153846,0.0046153846,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.0015384615,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.0030769231,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.0015384615,0.00076923077,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.0023076923,0.0046153846,0.0038461538,0.0038461538,0.0038461538,0.0076923077,0.0076923077,0.013076923,0.018461538,0.013846153,0.0046153846,0.010769230,0.010769230,0.0046153846,0.0038461538,0.0046153846,0.00076923077,-0.10000000,-0.10000000,-0.10000000,-0.10000000,0.0015384615,0.0038461538,-0.10000000,-0.10000000,0.0046153846,0.0046153846,0.0046153846,0.0038461538,-0.10000000,0.0046153846,0.0038461538,0.0038461538,0.0030769231,0.0046153846,0.0099999998,0.014615385,0.012307692,0.0023076923,0.0038461538,-0.10000000,0.0030769231,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000,-0.10000000};
float templt[TEMPLT_SIZE] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
// Thresholding Variables...
int AMP_THRESHOLD = 5000; // Amplitute Threshold (below this value, we don't do template matching.... Otherwise background noise can trigger a template match by chance once it's normalized


float DPTHRESH = 1;      // Value for Template Match Trigger.
int AMPLITUDE_THRESHOLD_DURATION = 4; //how many samples in a row should be above threshold. value depends on other parameters.
int TEMPLATE_MATCH_DURATION = 1; // How long should the template be above threshold? Number of FFTs in a row that need to match....
float FF_DISCARD = 300; // Ignore parts of the FF below this (200-300Hz usually fine).
float FF_MIN = 3200; //  Lower Limit for frequency bins to find peak.
float FF_MAX = 4000; //. Upper Limit to look at frequencies to find peak.
int FREQTHRESH = 3800 ;  /////////// Frequency Limits
int PercentHits = 100; /////////  Maintain this percent hits.

/////// Useful variables defined.
float fftmax = 0; //Highest value of FFT used for normalization
float dp;  // dot product for template matching.
float scaledMag;
float FF; //estimated FF
int peakIDX;
int PlayBackCounter;
boolean PLAYWN = true; // WN or or off? Used for testing.
boolean FREQDIR = true; // 1=up, 0=down (true=up, false=down)
float FFT_Bin = (float)((SAMPLE_RATE_HZ / 2) / (FFT_SIZE / 2)); // Binned Frequencies for Calculation, cast to FLOAT!  e.g. How many Hz is each FFT bin.
int FF_maxIDX = (int)(FF_MAX / FFT_Bin); //float to int! ////////// Indexes for finding peak frequency after template matesk.
int FF_minIDX = (int)(FF_MIN / FFT_Bin);                  /////// Lower bound for detection.
int FF_discardIDX = (int)(FF_DISCARD / FFT_Bin);
int ThreshRand = 0; // Random variable for generating Hit/Catch trials

////////////////////////////////////////////////////////////////////////////////
// INTERNAL STATE
// These shouldn't be modified unless you know what you're doing.
////////////////////////////////////////////////////////////////////////////////

IntervalTimer samplingTimer;
volatile float samples[FFT_SIZE * 4];
float magnitudes[FFT_SIZE];
float fftbuffer[FFT_SIZE * 2];
volatile int sampleCounter = 0;
volatile int PlaybackCounter = 0;

int AboveThresh = 0;
int AboveThreshold = 0;
int fft_threshcounter = 0;
int dpold = 0;
boolean HIT = 1;
float RUNNING_AMP = 0;

////////////////////////////////////////////////////////////////////////////////
// MAIN SKETCH FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void setup() {
  // Set up serial port

  Serial.begin(9600);

  //output LEDs for playback observations.
  pinMode(TRIGGER_OUTPUT_PIN, OUTPUT);
  digitalWrite(TRIGGER_OUTPUT_PIN, LOW);
  pinMode(POWER_LED_PIN, OUTPUT);
  digitalWrite(POWER_LED_PIN, HIGH);
  pinMode(BNC_TRIGGER_OUTPUT_PIN, OUTPUT);
  digitalWrite(BNC_TRIGGER_OUTPUT_PIN, LOW);


  // Set up ADC and audio input.
  pinMode(AUDIO_INPUT_PIN, INPUT);
  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogReadAveraging(ANALOG_READ_AVERAGING);
  // Set up DAC for audio putput.
  analogWriteResolution(14);

  // Begin sampling audio
  samplingBegin();
  delay(250); ///////////wait to fill the buffer....
}


//Main Loop  should run every ~1ms for 1024 float_32
void loop() {

  float timea = micros();

  getsamples(); // not perfect, but I can't point to a volatile float, and I don't want to make the samples non-volatile.
  arm_rms_f32(fftbuffer, FFT_SIZE, &RUNNING_AMP); //RMS of the buffer. Since there's a 'complex' component of all zeros, it still works fine.

  //Serial.println(RUNNING_AMP);
  if (RUNNING_AMP > AMP_THRESHOLD) {
    AboveThresh++;
  }
  else {
    AboveThresh = 0;
  }

  if (AboveThresh > AMPLITUDE_THRESHOLD_DURATION) {   // e.g. has there been sound?

    CalculateFFT(); // Galculate the FFT, results are cast to the magnitudes variable.

//    export_mags(); //For testing of FFT- will sent out 0:FFT_SIZE/2 from the calculated FFT magnitudes. 
    dp = ScaleAndCompareToTemplate();
    
    if ((dp >= DPTHRESH)) { //// Are we above the template match threshold?
      fft_threshcounter++; // if we are, increment the counter so we can decide how many sequenctial template matches we went before we trigger.
    }
    else {
      fft_threshcounter = 0; // reset the FFT_threshold counter.
    }

    
    if (fft_threshcounter >= TEMPLATE_MATCH_DURATION) { ////////// How long has the template match been? If it's been enough samples above the FFT match threshold, then trigger!

      peakIDX = get_FF_Peaks();     ////// Find the peak IDX in the window of interest defined by FF_MIN and FF_MAX
      FF = PinterP(magnitudes[peakIDX - 1], magnitudes[peakIDX], magnitudes[peakIDX + 1], peakIDX, FFT_Bin); // Parabolic Interpolation for more accuate FF calculation.

      if (FF < FF_MAX && FF > FF_MIN) { // If the result isn't gibberish (e.g. as long as the FF returned is inside of the window it should be in, the peak isn't the first or last or anything stupid. 
        Serial.print(FF);
        Serial.print(','); // print FF to serial so it can be written out. 
        //   if (FF<FREQTHRESH) { /////// Are we below the Frequency Trials?       Upshifts... Comment/Uncomment these two lines to choose Up or Downshifts.
        if (FF > FREQTHRESH  ) { /////// Are we above the Frequency Trials       Downshifts...

                 play_wn();
          //     delay(150);         // optional for slow syllables that you don't want to hit twice.

        } // end of FF thresholding;
      } // end of sanity check for peak FF.
    } //end of template match triggered portions
  } // end of amplitude triggered portion

  //float timeb = micros();
  //Serial.println(timeb - timea);  // benchmarking output. 


}


////////////////////////////////////////////////////////////////////////////////
// FFT/SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


void play_wn() {

  PlayBackCounter = 0;
  digitalWrite(POWER_LED_PIN,HIGH);
  digitalWrite(TRIGGER_OUTPUT_PIN, HIGH);  // Trigger the output to be recorded on the second audio channel
  digitalWrite(BNC_TRIGGER_OUTPUT_PIN, HIGH);
    
  samplingStop();                          //stop recording

  if (PLAYWN == true && HIT >= 1) {
    playbackBegin();                         // trigger WN
  }

  delay(50);                               // wait for WN to end- also how long the BNC trigger is high for, for e.g. optogenetics or whatever. 
  samplingBegin();                         // restart the buffering....
  //
  digitalWrite(BNC_TRIGGER_OUTPUT_PIN, LOW);

  if (HIT == 0) {
    delay(50); // catch trials are 50ms longer on the audio channel to help tell catch, from hit, from total miss.   
  }

  digitalWrite(TRIGGER_OUTPUT_PIN, LOW); 
  digitalWrite(POWER_LED_PIN, LOW);        // And power everything else down too. 
 

  Serial.println(HIT);         // Output Catch (0) or Hit (1) trial.
  ThreshRand = random(1, 100); // choose a random variable

  if (ThreshRand < PercentHits) { //Decide if the next trial will be a hit, or a catch.
    HIT = 1;
  }
  else {
    HIT = 0;
  }

}

//////////////////////////////////////
// Performs the parabolic interpolation.
// takes in PeakIDX-1, PeakIDX, PeakIDX+1 magnitudes, the peakIDX,
// and the value in F for each integer idx;
///////////////////////////////////////

float PinterP(float a, float b, float c, int idx, float FBin) {

  float InterP_FF;
  float interpidx;

  interpidx = 0.5 * ((a - c) / (a - (2 * b) + c));
  idx = idx + 1;
  InterP_FF = (idx * FBin) + (interpidx * FBin);

  return InterP_FF;

}

//////////////////
// Calculated Peak FF index
//////////////////

int get_FF_Peaks() {

  int magmax = 0;
  int idxmax = 0;

  // probably not worth arm_max_f32 here.....
  for (int i = FF_minIDX; i <= FF_maxIDX; i++) { //find peak index within the FF window delineated
    if (magnitudes[i] >= magmax) { //find the biggest value
      magmax = magnitudes[i];
      idxmax = i;
    }
  }
  return idxmax; // returns actual index mind you. 
}

// Calcutes the FFT, in this case 32bit floating point (given 12-14 bit data, possibly overkill, but int16 is definately a smoother FFT, especially once normalized, and it's nice to have everything be floats for convenience. 
void CalculateFFT() {

  //    // Run FFT on sample data.

  if (FFT_SIZE == 512) {  ///////////////////////////Lets me use 512, since its not available in cfft_radix4. 512 is a nice mix of accurate and fast..... 
    float complexmagnitudes[FFT_SIZE*2];                // Twice the size of the FFT, since every 2nd number is gonna be complex, but uninportant. 
    arm_cfft_radix4_instance_f32 cfft_inst;             //
    arm_rfft_instance_f32 rfft_inst; // create structure on ARM processor
    arm_rfft_init_f32(&rfft_inst, &cfft_inst, FFT_SIZE, 0, 1); // initialize,
    if (status == 0){      
          arm_rfft_f32(&rfft_inst, fftbuffer, complexmagnitudes); // and calculate the FFT.
    }
    arm_cmplx_mag_f32(complexmagnitudes, magnitudes , FFT_SIZE); // doesnt have to be squared. ampl_cmplx_mag_f32 would probably work fine too. 

    
  }
  else {  ////////support for 256 and 1024 sample. 
    arm_cfft_radix4_instance_f32 fft_inst; // create structure on ARM processor
    arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1); // initialize,
    if (status == 0){
        arm_cfft_radix4_f32(&fft_inst, fftbuffer); // and calculate the FFT.
    }
   
    // Calculate magnitude of complex numbers output by the FFT.
    arm_cmplx_mag_f32(fftbuffer, magnitudes, FFT_SIZE);
  }
  
  //
  for (int i = 0; i <= FF_discardIDX; i++) { /// Get rid of low-frequency stuff.... 
    magnitudes[i] = 0;
  }

}

float ScaleAndCompareToTemplate() {

  float scaledmags[TEMPLT_SIZE];
  float32_t peakmag;
  float32_t scalefactor;
  uint32_t peakmagidx;
  float dotprod;

  arm_max_f32(magnitudes,FFT_SIZE,&scalefactor,&peakmagidx);//find the max; 
  scalefactor=1/scalefactor; // for vector multiplation below 

  arm_scale_f32(magnitudes, scalefactor, scaledmags, TEMPLT_SIZE); // and normalize.
  arm_dot_prod_f32(scaledmags, templt, TEMPLT_SIZE, &dotprod); // and calculate the dot product

  return dotprod;
}

////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// Get the last FFT_SIZE samples, and format them for the FFT. 
void getsamples() {
  int currentidx = sampleCounter - (FFT_SIZE - 1); // Go back FFT_Size from where we are now. (array starts at 0, so 0-1023 for 1024 samples).

  for (int i = 0; i <= ((FFT_SIZE * 2) - 2); i = i + 2) { // go through, get the last FFT_Size samples, and put them into a structure for ARM FFT calculation.
    fftbuffer[i] = samples[currentidx + (i / 2)]; //Samples in real idx, FFT interleaved with 0s.
    fftbuffer[i + 1] = 0; // Sampled in place, so reset every time.
  }

  if (FFT_SIZE==512) {
    for (int i = 0; i <= (FFT_SIZE-1); i++) { // go through, get the last FFT_Size samples, and put them into a structure for ARM FFT calculation.
     fftbuffer[i] = samples[currentidx + (i)]; //Samples in real idx, FFT interleaved with 0s.
    //  fftbuffer[i + 1] = 0; // Sampled in place, so reset every time.
  }
  }
  
}

// This is called at the sampling frequency in order to continously sample, and write to a buffer.
// for whatever reason, I couldn't get a simple circular buffer to work- but this seems to be stable and functional. 
void samplingCallback() {

  // Read from the ADC and store the sample data
  samples[sampleCounter] = (float)(analogRead(AUDIO_INPUT_PIN));
  samples[sampleCounter] = samples[sampleCounter] - 32768;

  sampleCounter += 1;

  // Keep FFT_SIZE worth of data at all times.
  if (sampleCounter >= BUFFER_SIZE) {    /// if the buffer is full....
    for (int i = 0; i <= FFT_SIZE - 1; i++) { /// Rewrite the first however many for continuity. This seems to work just fine, as long at it's fast.
      samples[i] = samples[i + FFT_SIZE - 1]; /// move the end to the start, and continue writing from there.
      sampleCounter = FFT_SIZE;
    }
  }
}

// Starts the timer. If you run things that conflict with this interrupt, you'll crash the processor.
void samplingBegin() {
  // Reset sample buffer position and start callback at necessary rate.
  sampleCounter = 0;
  samplingTimer.begin(samplingCallback, 1000000 / SAMPLE_RATE_HZ); ///// This never really stops, except for WN playback....
}

// pretty self-evident.
void samplingStop() {
  samplingTimer.end();
}

boolean samplingIsDone() {
  return sampleCounter >= FFT_SIZE * 2;
}

// plays WN to the DAC output.
void playbackCallBack() {

  analogWrite(AUDIO_OUTPUT_PIN, wn[PlayBackCounter]);
  PlayBackCounter++;
  if (PlayBackCounter > 1024) {
    samplingStop();
  }
}

// DO playback at Sampling_Frequency in case you want notched noise or something....
void playbackBegin() {
  // Reset sample buffer position and start callback at necessary rate.
  // Allows for notched white noise if necessary...
  PlaybackCounter = 0;
  samplingTimer.begin(playbackCallBack, 1000000 / SAMPLE_RATE_HZ);
}





//////////////////////////////////////////////////////////////////////////////////
//// UTILITY FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////
void export_mags() {

       float scaledOut;
       int fftmax = 0;
         float scaledmags[TEMPLT_SIZE];
      float32_t peakmag;
      float32_t scalefactor;
      uint32_t peakmagidx;
      float dotprod;

      arm_max_f32(magnitudes,FFT_SIZE,&scalefactor,&peakmagidx);//find the max; 
      scalefactor=1/scalefactor; // for vector multiplation below 

      arm_scale_f32(magnitudes, scalefactor, scaledmags, TEMPLT_SIZE); // and normalize.
   
       for (int i=0; i<=TEMPLT_SIZE-1; i++) {
            //scaledOut=(float)magnitudes[i];
            //scaledOut=scaledOut/fftmax;
            Serial.print(scaledmags[i]);                    
            Serial.print(',');
       }

   Serial.println();    
     
}
