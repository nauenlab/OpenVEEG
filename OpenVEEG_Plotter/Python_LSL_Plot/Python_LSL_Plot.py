from pylsl import StreamInlet, resolve_stream
import numpy as np
import matplotlib.pyplot as plt

# first resolve an EEG stream on the lab network
print("Resolving stream...")
streams = resolve_stream('type', "EEG")

# create a new inlet to read from the stream
inlet = StreamInlet(streams[0])
data = []
time = []
print("Inlet opened.")
while True:
    try:
        chunk, timestamps = inlet.pull_chunk()
        if timestamps:
            np.append(data, chunk)
#            np.append(time, timestamps)
            plt.plot(data)
            plt.show()
    except KeyboardInterrupt:
        print("Program stopped.")
        break
