%Designed to use national instruments data to send to lsl - Arun R.
daq.getDevices
disp('Loading LSL library...');
addpath(genpath('liblsl-Matlab'));
lib = lsl_loadlib();
str = sprintf('Please enter the sampling rate in Hz.\n');
x = input(str);


disp('Opening National Instruments - Epoch Device');
daq.getDevices
str = sprintf('Please enter the ID of the device that you wish to open.\n');
device = input(str, 's');
str = sprintf('Please enter the number of channels to use.\n');
num = input(str);

disp('Creating new stream outlet...');
info = lsl_streaminfo(lib, 'NI - Epoch', 'EEG', num, x, 'cf_float32', 'sdfwerr32432');

disp('Opening outlet...');
outlet = lsl_outlet(info);
data = 0;
%save('data.mat', 'data');

s = daq.createSession('ni');
for i = 1:num;
    addAnalogInputChannel(s, device, (i-1), 'Voltage');
end
s.Rate = x;
findat = 0;
s.IsNotifyWhenDataAvailableExceedsAuto = true;
%lh = addlistener(s, 'DataAvailable', @saveData);
lh2 = addlistener(s, 'DataAvailable', @(src, event) outlet.push_chunk(event.Data'));
lh3 = addlistener(s, 'DataAvailable', @(src, event) plot(event.TimeStamps, event.Data));
s.IsContinuous = true;
disp('Device opened.');
disp('Sending data...');
FS = stoploop('Stop Data Transfer');
s.startBackground();
while s.IsRunning
    if FS.Stop()
        s.stop();
    end
end
disp('Closing System.');
FS.Clear();
clear FS;



