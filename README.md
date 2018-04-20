# OpenVEEG 
OpenVEEG is an open-soure program that allows synchronous electroencephalogram and video recording for seizure-induced animal models.

OpenVEEG is run on a four hour recurrent schedule that saves the files with the date, start time of the recording, and mouse number. This schedule can be set up through Windows Time Scheduler with batch files to run the video and EEG programs. 


# Dependencies
In order to use OpenVEEG_Video, the user must independently install OpenCV 3.1 from http://opencv.org/, and include opencv_world.dll in the file folder containing the OpenVEEG_video executable.
Boost 1.64 or later must also be downloaded to use OpenVEEG_video. You can download it here: http://www.boost.org/.
LabStreamingLayer can be found at https://github.com/sccn/labstreaminglayer.