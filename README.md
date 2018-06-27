# Teensy3.6TAF
, BirdTAF for Teensy 3.6.
A handful of notes:

You can order boards at OSH Park:

https://oshpark.com/shared_projects/d7Odnek7

for ~10$ea.

or submit the eagle .brd file included here yourself. 




for 44.1Khz, I recomment overclocking- otherwise it could have issues from time to time. 216MHz seems to work well. To get at it, you may have to edit boards.txt (arduino/hardware/teensy/avr/boards.txt) and uncomment the higher overclock speeds. 

To get spectrogram out: uncomment 'export_mags', and comment out anything else that prints to a serial port- you'll be able to get the PSD calculated by the teensy whenever the audio input is above AMP_THRESHOLD. This is also useful for testing this variable.
Let the bird sing, copy and paste out of the serial monitor, and have a look for something you can target. You can make a template from the PSD, or something more arbitrary- if you're getting hits in non-target syllables consider negative values in the template to penalize regions of the syllable that have power the target syllable does not.

Once you have a template, copy it in, recomment export_mags, and uncomment Serial.println(dp). Now it'll kick out the template(x)psd distance every time sound is above threshold. 

You can use Serial Monitor or Serial Plotter, but as long as you see a single peak for each song, you have a good template- pick a threshold where it crosses and avoids everything else, and see how it goes. 


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

