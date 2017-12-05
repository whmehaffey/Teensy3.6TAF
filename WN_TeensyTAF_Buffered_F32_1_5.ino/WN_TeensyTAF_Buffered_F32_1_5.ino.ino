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
float wn[WN_SIZE] = {28944, 25767, 34894, 33823, 36338, 21066, 60225, 53241, 21638, 50960, 27043, 34872, 42013, 61378, 42268, 29514, 57967, 36920, 36256, 54086, 52318, 24908, 56111, 61068, 47758, 37765, 37306, 43656, 25805, 39903, 38728, 22831, 33837, 26840, 57340, 57014, 33747, 49065, 61316, 60463, 31089, 32901, 32811, 52986, 57708, 39137, 30954, 53144, 20585, 37961, 55441, 28155, 59018, 53451, 54200, 53919, 21291, 24332, 40319, 50706, 36800, 59310, 51978, 45456, 58408, 60808, 46148, 35648, 37136, 31423, 56000, 38706, 49232, 37667, 44771, 31210, 33284, 46562, 47089, 59056, 56730, 33430, 54691, 56836, 39135, 32135, 28570, 44013, 47889, 41715, 58881, 41735, 43722, 44086, 52266, 28059, 61152, 54368, 36662, 23738, 27703, 56880, 28554, 38660, 26192, 40730, 58222, 56918, 59035, 26493, 36307, 22252, 52015, 40268, 26504, 49372, 35290, 58802, 30345, 39263, 43504, 42050, 53214, 60636, 43149, 47919, 27096, 35784, 27661, 54580, 20668, 49247, 54238, 60176, 57505, 24814, 42312, 48713, 30926, 46339, 26445, 20789, 32005, 32601, 52673, 30018, 22619, 21445, 47947, 29680, 31446, 21911, 39874, 38585, 48160, 22648, 23682, 54495, 35620, 50826, 30573, 54094, 41024, 25781, 30464, 30707, 54572, 50499, 39237, 22087, 21113, 39169, 57267, 50956, 29018, 27602, 35690, 40426, 53650, 28430, 27335, 39979, 55800, 44488, 40391, 53414, 49383, 34415, 32340, 32314, 23716, 32010, 47834, 23658, 20968, 56813, 48389, 59175, 32277, 56455, 31745, 52758, 33480, 38699, 41100, 40233, 52070, 26996, 26274, 56194, 60472, 45389, 29069, 31262, 59010, 30331, 38382, 45232, 61238, 48902, 32190, 36232, 52896, 31569, 57898, 54009, 53857, 43220, 33186, 56397, 37660, 24206, 46383, 21991, 45757, 53931, 60168, 32947, 56360, 28454, 33283, 21695, 56294, 22802, 59795, 56605, 52622, 25656, 34948, 36009, 35895, 50259, 34491, 45552, 34373, 42975, 24040, 45163, 39108, 46332, 53579, 60045, 33743, 43914, 23582, 57267, 30217, 56018, 34032, 41765, 26240, 51728, 40054, 28161, 46592, 48715, 56456, 36467, 55140, 52524, 46785, 29301, 45636, 33352, 49550, 25604, 50108, 41255, 39552, 55332, 41075, 28983, 35342, 55663, 58808, 34506, 58016, 25895, 59182, 42455, 24394, 44892, 58746, 48953, 38829, 34337, 57751, 38822, 25578, 54869, 43451, 27151, 30145, 32942, 44275, 30074, 30043, 42731, 55752, 53112, 54238, 30402, 30221, 21433, 36778, 25694, 24792, 23092, 60326, 40973, 53885, 53061, 30406, 23924, 44459, 36241, 29357, 22447, 58804, 44936, 50870, 28440, 21681, 36076, 42998, 45513, 29947, 57025, 38595, 43357, 30071, 33218, 41912, 59941, 55850, 21017, 28335, 20876, 55827, 23013, 55979, 41934, 45353, 34387, 25912, 35089, 36005, 23029, 57041, 57191, 30824, 43612, 55345, 55998, 57921, 45177, 60452, 44051, 28142, 36618, 43256, 29177, 29190, 31428, 53741, 46199, 54646, 29669, 26258, 26611, 58339, 57530, 30792, 59297, 40782, 32867, 34637, 26484, 24908, 44661, 20910, 34929, 21400, 41248, 21143, 46931, 22827, 25388, 45780, 27270, 38541, 22488, 36232, 49051, 44789, 35438, 47072, 31683, 39699, 54857, 25703, 58754, 55300, 28953, 47630, 35590, 47263, 21108, 58190, 51781, 56045, 58677, 21084, 44184, 30342, 58330, 31336, 57833, 41649, 23681, 39264, 60597, 61376, 48360, 51420, 54790, 48714, 45956, 42286, 53194, 55569, 40619, 41974, 57004, 22228, 25744, 30264, 29881, 35774, 31442, 50562, 60494, 26870, 22996, 52272, 52029, 60216, 60354, 40885, 49277, 53900, 58095, 22870, 34591, 38433, 55753, 52140, 46479, 29511, 58661, 23148, 52031, 51096, 32715, 21203, 55156, 34391, 57960, 25570, 23930, 46915, 53717, 22706, 34092, 52909, 45552, 37435, 21762, 51965, 25454, 28806, 40113, 34570, 30403, 39132, 52696, 36093, 46814, 24082, 24704, 21420, 20962, 31222, 42195, 41514, 23937, 42139, 22739, 26220, 53530, 49042, 40392, 22887, 32384, 36233, 55175, 51874, 29845, 54926, 40290, 30311, 25612, 33945, 38560, 30696, 58693, 60151, 55381, 42108, 53057, 50769, 25148, 47874, 47156, 60418, 27692, 41404, 55779, 60798, 30067, 27664, 58340, 47885, 41474, 38643, 38479, 57108, 39277, 26299, 32146, 22498, 20542, 32325, 42707, 29635, 57403, 43261, 28282, 46891, 23897, 46900, 45857, 37458, 44068, 49305, 21421, 41215, 60474, 46249, 45853, 41542, 38588, 46410, 33876, 34425, 35684, 55067, 27624, 58866, 38857, 20540, 35674, 58663, 50745, 31315, 39095, 53522, 45995, 57181, 57911, 39991, 21834, 59175, 44588, 48042, 34807, 28606, 47320, 31854, 56191, 47822, 34096, 49547, 55514, 28157, 37072, 34319, 32713, 25045, 27577, 36774, 26626, 43931, 30340, 37664, 57767, 43435, 42178, 40210, 24935, 52591, 59027, 47615, 26448, 45507, 25583, 33277, 49385, 32874, 46666, 49224, 54814, 25767, 33933, 47916, 44505, 33943, 42850, 61420, 48800, 27321, 37398, 49551, 36117, 53659, 45292, 32825, 23363, 44715, 53731, 38897, 39621, 20852, 44352, 41363, 55573, 35212, 57253, 44517, 57301, 31784, 47850, 47087, 38205, 36017, 36527, 57243, 42177, 23183, 49340, 46589, 51707, 29337, 37471, 41204, 39694, 44383, 49406, 24968, 32123, 23565, 52891, 32360, 32498, 55424, 46508, 28469, 36260, 24711, 30635, 56453, 44361, 49635, 21437, 28516, 30410, 29835, 55344, 42220, 54673, 33828, 35649, 20829, 48211, 55579, 30178, 40889, 36972, 55019, 43059, 60407, 56582, 37820, 50465, 24700, 23875, 45268, 20817, 47153, 37978, 56463, 49625, 29987, 45759, 45383, 46548, 44685, 20739, 24369, 40594, 23563, 51909, 29001, 53125, 59425, 24771, 44886, 42959, 44035, 54246, 46333, 50577, 43622, 43360, 53436, 52971, 44638, 53410, 37662, 32650, 43570, 39601, 29339, 43331, 43420, 50350, 23883, 56241, 43964, 36903, 33738, 50045, 42404, 35904, 29330, 24424, 27471, 59763, 41287, 49794, 58346, 42104, 52690, 23820, 60030, 27005, 25000, 44637, 22746, 56476, 52831, 47389, 43396, 58308, 57317, 60733, 30556, 47667, 49133, 45658, 36654, 50435, 26715, 30138, 40908, 33323, 37022, 39600, 26688, 20863, 37301, 34615, 38520, 50767, 45065, 27032, 33773, 32899, 39291, 30988, 24554, 60721, 61015, 55561, 55559, 47872, 57013, 45438, 41077, 54715, 56457, 38906, 44039, 30548, 31341, 52990, 24350, 49166, 49324, 25468, 44959, 41212, 24593, 47154, 27029, 45327, 30133, 54022, 59130, 57032, 24502, 21293, 27615, 47788, 58861, 41166, 24860, 50380, 31603, 31893, 32418, 22850, 28131, 26121, 55531, 61158, 38843, 36913, 41046, 34427, 33052, 35697, 52193, 29820, 26250, 24077, 32174, 46151, 52449, 31151, 36417, 42281, 46215, 23797, 59976, 24801, 56668, 39617, 51252, 39289, 23233, 39060, 43117, 56777, 25933, 55239, 42603, 61402, 21159, 59136, 32118, 61048, 37990, 23699, 20827, 58658, 56287, 53221, 51109, 24107, 45297, 35821, 56765, 45637, 48654, 36559, 30912, 48606, 47597, 57885, 53528, 46123, 30179, 21701, 29391, 26699, 35267, 39025, 39206, 34282, 60432, 54392, 31526, 50395, 53098, 28709, 44849, 25279, 23061, 37650, 23224, 59088, 23065, 42012, 56198, 25299, 46072, 20528, 33660, 59362, 45866, 56027, 50522, 58089, 45828, 47472, 41009, 49935, 31156, 50516, 42419, 50631, 57421, 53102, 35413, 29484, 21011, 29816, 46882, 39056, 55395, 36671, 41822, 27595, 50035, 46478, 28354, 32302, 40213, 57138, 53745, 55278, 54249, 53369, 46350, 29114, 36964, 31169, 26166, 49980, 48571, 39856, 44963, 29041, 61063, 33056, 28505, 31163, 27507, 25014, 50471, 57250, 42101, 45436, 49880, 44884, 37462, 39477, 34453, 41943, 36146, 58678, 53318, 46639, 31248, 53815, 48420, 22505, 22905, 46106, 36305, 25246, 24354, 43729};

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

    //export_mags(); For testing of FFT- will sent out 0:FFT_SIZE/2 from the calculated FFT magnitudes. 
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
    arm_cmplx_mag_squared_f32(complexmagnitudes, magnitudes , FFT_SIZE); // doesnt have to be squared. ampl_cmplx_mag_f32 would probably work fine too. 

    
  }
  else {  ////////support for 256 and 1024 sample. 
    arm_cfft_radix4_instance_f32 fft_inst; // create structure on ARM processor
    arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1); // initialize,
    if (status == 0){
        arm_cfft_radix4_f32(&fft_inst, fftbuffer); // and calculate the FFT.
    }
   
    // Calculate magnitude of complex numbers output by the FFT.
    arm_cmplx_mag_squared_f32(fftbuffer, magnitudes, FFT_SIZE);
  }
  
  //
  for (int i = 0; i <= FF_discardIDX; i++) { /// Get rid of low-frequency stuff.... 
    magnitudes[i] = 0;
  }

}

float ScaleAndCompareToTemplate() {

  float scaledmags[TEMPLT_SIZE];
  float peakmag;
  float scalefactor;
  uint32_t peakmagidx;
  float dotprod;

  arm_max_f32(magnitudes,TEMPLT_SIZE,&scalefactor,&peakmagidx);//find the max; 
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

       for (int i=0; i <=TEMPLT_SIZE-1; i++) { 
              if (magnitudes[i] > fftmax) {
           fftmax=magnitudes[i];
          }        
       }
   
       for (int i=0; i<=TEMPLT_SIZE-1; i++) {
            scaledOut=(float)magnitudes[i];
            scaledOut=scaledOut/fftmax;
            Serial.print(scaledOut);                    
            Serial.print(',');
       }

   Serial.println();    
     
}
