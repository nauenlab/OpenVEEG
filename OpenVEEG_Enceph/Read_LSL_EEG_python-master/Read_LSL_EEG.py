from PyDAQmx import *
from nidaqmx import system
from numpy import zeros
from pylsl import *

class CallbackTask(Task):
    def __init__(self):
        Task.__init__(self)
        print('Following is the list of NI devices attachedto this system:\n')
        local = system.System.local()
        for i in range(local.devices.__len__()):
            print(str(local.devices[i]._name) + '\n')
        name = str(raw_input('Please enter the name of the device you wish to use, exactly as printed above.\n'))
        self.sf = int(raw_input('Please enter the sampling frequency in Hz.\n'))
        self.numchannels = int(raw_input('Please enter the number of channels to record.\n'))
        self.info = StreamInfo('EEGStream', 'EEG', 2, 1000, 'float32', 'myuid1657')
        self.outlet = StreamOutlet(self.info)
        self.data = zeros((self.sf, self.numchannels))
        self.a = []
        for i in range(self.numchannels):
            currname = name + '/ai' + str(i)
            self.CreateAIVoltageChan(currname, "", DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, None)
        self.CfgSampClkTiming("", self.sf, DAQmx_Val_Rising ,DAQmx_Val_ContSamps, 10*self.sf)
        self.AutoRegisterEveryNSamplesEvent(DAQmx_Val_Acquired_Into_Buffer, self.sf, 0)
        self.AutoRegisterDoneEvent(0)
    def EveryNCallback(self):
        read = int32()
        self.ReadAnalogF64(self.sf, 10.0, DAQmx_Val_GroupByScanNumber, self.data, self.numchannels*self.sf ,byref(read),None)
        print(self.data[0])
        print(read)
        self.outlet.push_chunk(self.data.T.tolist())
        return 0 # The function should return an integer
    def DoneCallback(self, status):
        print "Status",status.value
        return 0 # The function should return an integer


task=CallbackTask()
task.StartTask()

raw_input('Acquiring samples continuously. Press Enter to interrupt\n')

task.StopTask()
task.ClearTask()