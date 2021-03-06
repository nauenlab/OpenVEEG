# OpenVEEG_video
For instructions relating to changing the default video codec, resolution, frame rate, and grayscale vs. color, look at the README in the OpenVEEG_video Folder

OpenVEEG is run on a four hour recurrent schedule that saves the files with the date, start time of the recording, and mouse number. This schedule can be set up through Windows Time Scheduler with batch files to run the video and EEG programs. 

# Suggested Hardware (Test Rig Specs):

Note, these are not the minimum specs required, but simply the rig we developed and tested on.

CPU: Intel Core i5-4460 Haswell Quad-Core Processor, 3.2GHz

Motherboard: At minimum two PCIex1 slots

RAM: 16GB

Memory: 2 x WD4000F9YZ 4TB HDD

PSU: Corsair 750W ATX12V Power Supply

# Licensing

OpenVEEG_video, READ_LSL_EEG (both versions), and PythonPlotter are provided under the MIT License

# Dependencies

In order to use OpenVEEG_video, the user must independently install OpenCV 3.1 from http://opencv.org/, and include opencv_world.dll in the file folder containing the OpenVEEG_video executable.

Boost 1.64 or later must also be downloaded to use OpenVEEG_video. You can download it here: http://www.boost.org/.

LabStreamingLayer is required. It can be downloaded at https://github.com/sccn/labstreaminglayer. The build dll must be included in the file folder containing the OpenVEEG_video (i.e. liblsl64.dll) executable.

# Credits:

The source code for LabStreamingLayer is included in these repositories. LabStreamingLayer is provided open-source by the Schwarz Center for Computational Neuroscience, UCSD. The original github can be found here: https://github.com/sccn/labstreaminglayer.