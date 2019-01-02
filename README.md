# Teensy3.6TAF
, BirdTAF for Teensy 3.6.

1/2/2019- Version with GUI now at: 
https://github.com/whmehaffey/TeensyTAF_GUI/tree/master

This version is now mostly for archival purposes

A handful of notes:

You can order boards at OSH Park:

https://oshpark.com/shared_projects/d7Odnek7

for ~10$ea.

or submit the eagle .brd file included here yourself. 




for 44.1Khz, I recomment overclocking- otherwise it could have issues from time to time. 216MHz seems to work well. To get at it, you may have to edit boards.txt (arduino/hardware/teensy/avr/boards.txt) and uncomment the higher overclock speeds. 

If you want to build a template- I would uncomment the 'exports_mags' line, and comment out any other Serial.println. Then, whenever the sound is above threshold, you'll start to export the PSD estimate. This will give you the PSDs the Teensy is calculating during song (and also help make sure your amplitude threshold is right- it should be calcuating when the bird is singing, but not when it's quiet

Once you have a template, you can recomment export_mags, and uncomment Serial.println(dp) which will export the distance between the template and the current PSD. If you're well-targeted, you should see a sharp peak, which makes it easy to set a threshold (DPTHRESH).

You can also use negative values in the template if you want to penalize spectral information that's present in a syllable you want to avoid, but absent in the target syllable. 


------------------------------
Variables for TAF:

templt is an array of 1:FFT_SIZE/2 containing the template for matching. This should be normalized to one generally, and the FFTs calculated are normalized as well.

AMP_THRESHOLD sets the RMS value above which template comparisons begin- this ensure quiet, random events don't set off template matches.

DPTHRESH is the threshold above which a match is triggered. 

AMPLITUDE_THRESHOLD_DURATION is how many times in a row the templates should be matched before a trigger. 

FF_DISCARD throws out FFs below this in the comparison to get rid of low frequency garbage in the FFT

FF_MIN and FF_MAX define the range for looking for the FF- this helps keep the estimate at the FF, even if the maximum power is in a higher harmonic for a stacky syllable. 

FREQTHRESH sets the threshold, above or below which WN is triggered. 

PercentHits sets the hit/catch trial ratio. 

wn[WN_SIZE] is an array containing WN to play back through the DAC. 


-----------------------------

On trigger, a few things happen - WN is played, and the the BNC is set high, as is the second audio channel on the PCB. 
This lets you record audio with a record of when things were triggered if you want. 
It also estimates the FF, and writes it to the serial port, along with whether the trial was a hit or a catch. 

FF's are estimated by parabolic interpolation around the peak. 

