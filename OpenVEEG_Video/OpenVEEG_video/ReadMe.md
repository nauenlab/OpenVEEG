# VideosStream Console Application:

This program is written to record video from the BlackMagic Decklink Video Acquisition Card and sync with LabStreamingLayer (https://github.com/sccn/labstreaminglayer). It will only work with cameras that work as BMD streaming devices, as specified in the BlackMagic API manual. (http://hackipedia.org/Software/Libraries/Black%20Magic%20Design/Decklink%20SDK/Blackmagic%20Decklink%20SDK%20(December%202012).pdf)

To build this program three separate packages are required: Boost, LSL, and the DeckLink SDK. Boost can be downloaded at http://www.boost.org/, LSL can be downloaded from the github linked above, and the DeckLink SDK can be retrieved from BlackMagic Design's website https://www.blackmagicdesign.com/.
Once downloaded, the locations of these packages must be specifed in the properties page of the VideosStream progject via File->Project Properties. 

The folder labelled precompiled contains a built version of the program made for Windows 7. This folder is meant for those wishing to either edit or adapt the code for their own purposes.


# Customizing Settings

### IMPORTANT NOTE: To use with your own camera, first determine the resolution, frame rate, and codec of your camera. For a blackmagic card, make sure you can see the video in BlackMagic Media Express. In the properties tab you can see the name of the correct video format. In DeckLinkCapture.cpp, line 162 must be edited to match this input format (in our case it is BMD YUV 8bit) 

### Defaults:

Resolution: 1080 X 1920

AVI Codec: DivX

Color vs. Grayscale: Color

Frame Rate: 30 frames/sec

### Changing the Default Settings

Upon initializing VideoStream, you will be given the option tot change the frame rate and the resolution, or remain with the default settings.

In VideosStream.cpp you may edit line 98 to specify a different recording codec other than DIVX. DeckLinkInputCallback.cpp line 44 must be edited to match the codec specified in line 162 of DeckLinkCapture.cpp.

To change the recording to grayscale, uncomment lines 115 and 116 in VideosStream.cpp, and change 'frame' in line 117 to 'greymat'.

