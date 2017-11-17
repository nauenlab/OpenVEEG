from pylsl import StreamInlet, resolve_stream
from numpy import *
import matplotlib
matplotlib.use('WXAgg')
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg
from matplotlib.figure import Figure
import wx



# noinspection PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,
# PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,
# PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,
# PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,PyAttributeOutsideInit,
# PyAttributeOutsideInit,PyAttributeOutsideInit,PyPep8Naming,PyPep8Naming
class MyFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'NI_EEG',
                          style=wx.DEFAULT_FRAME_STYLE ^ wx.RESIZE_BORDER,
                          size=(1200, 800))
        self.panel = wx.Panel(self, -1)
        self.zoom_level = 0.5
        self.fig = Figure((5, 4), 75)
        self.canvas = FigureCanvasWxAgg(self.panel, -1, self.fig)
        self.scroll_range = 400
        self.canvas.SetScrollbar(wx.HORIZONTAL, self.scroll_range, 5,
                                 self.scroll_range)

        self.redrawTimer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.onRedrawTimer, self.redrawTimer)
        self.zoomIn = wx.Button(self.panel, -1, "Zoom In", (10, 10))
        self.zoomOut = wx.Button(self.panel, -1, "Zoom Out")
        self.Reset = wx.Button(self.panel, -1, "Reset Zoom")
        self.zoomIn.Bind(wx.EVT_BUTTON, self.onZoomIn)
        self.zoomOut.Bind(wx.EVT_BUTTON, self.onZoomOut)
        self.Reset.Bind(wx.EVT_BUTTON, self.onReset)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.canvas, -1, wx.EXPAND)
        sizer.Add(self.zoomIn, 0, wx.ALL, 5)
        sizer.Add(self.zoomOut, 0, wx.ALL, 5)
        sizer.Add(self.Reset, 0, wx.ALL, 5)

        self.panel.SetSizer(sizer)
        self.panel.Fit()

        self.init_data()
        self.init_plot()
        self.redrawTimer.Start(100)

        self.canvas.Bind(wx.EVT_SCROLLWIN, self.OnScrollEvt)

    def onZoomIn(self, event):
        temp = float(self.zoom_level / 10)
        self.zoom_level = self.zoom_level - temp
        self.draw_plot()

    def onZoomOut(self, event):
        temp = float(self.zoom_level/10)
        self.zoom_level = self.zoom_level + temp
        self.draw_plot()

    def onReset(self, event):
        self.zoom_level = 0.5
        self.draw_plot()

    def onRedrawTimer(self, event):
        self.chunk, self.timestamps = self.inlet.pull_chunk()
        if self.timestamps:
            tempx = self.x
            tempx2 = self.x2
            print(shape(self.chunk))
            chunky = array(self.chunk, dtype=float)
            data1 = chunky[:, 0]
            data2 = chunky[:, 1]
            self.x[(self.length - len(self.chunk)):, 0] = data1
            self.x[:self.length - len(self.chunk), 0] = tempx[(len(self.chunk)):, 0]
            self.x2[(self.length - len(self.chunk)):, 0] = data2
            self.x2[:self.length - len(self.chunk), 0] = tempx2[(len(self.chunk)):, 0]
            print(self.x)
            self.draw_plot()


    def init_data(self):

        # Initalize LSL
        print("Resolving Streams...")
        self.streams = resolve_stream('type', "EEG")
        if self.streams:
            self.inlet = StreamInlet(self.streams[0])

        # Generate some data to plot:
        self.rate = float(raw_input("Please enter the sample rate of the EEG system for correct plotting.\n"))
        print(self.rate)
        self.t = arange(0, 5, 1/self.rate)
        size = 5*self.rate
        self.x = zeros((int(size), 1))
        self.x2 = zeros((int(size), 1))
        print(shape(self.x))
        self.length = len(self.x)

        # Extents of data sequence:
        self.i_min = 0
        self.i_max = len(self.t)

        # Size of plot window:
        self.i_window = 2000

        # Indices of data interval to be plotted:
        self.i_start = 0
        self.i_end = self.i_start + self.i_window

    def init_plot(self):
        self.axes = self.fig.add_subplot(2, 1, 1)
        self.axes2 = self.fig.add_subplot(2, 1, 2)
        self.plot_data = \
            self.axes.plot(self.t[self.i_start:self.i_end],
                           self.x[self.i_start:self.i_end])[0]
        self.plot_data2 = \
            self.axes2.plot(self.t[self.i_start:self.i_end], self.x2[self.i_start:self.i_end], 'r')[0]
    def draw_plot(self):

        # Update data in plot 1:
        self.plot_data.set_xdata(self.t[self.i_start:self.i_end])
        self.plot_data.set_ydata(self.x[self.i_start:self.i_end])

        # Update data in plot 2:
        self.plot_data2.set_xdata(self.t[self.i_start:self.i_end])
        self.plot_data2.set_ydata(self.x2[self.i_start:self.i_end])

        # Adjust plot limits:
        self.axes.set_xlim((min(self.t[self.i_start:self.i_end]),
                            max(self.t[self.i_start:self.i_end])))
        level = -1 * self.zoom_level
        self.axes.set_ylim((level, self.zoom_level))

        self.axes2.set_xlim((min(self.t[self.i_start:self.i_end]),
                            max(self.t[self.i_start:self.i_end])))
        self.axes2.set_ylim((level, self.zoom_level))

        # Redraw:
        self.canvas.draw()

    def OnScrollEvt(self, event):

        # Update the indices of the plot:
        self.i_start = self.i_min + event.GetPosition()
        self.i_end = self.i_min + self.i_window + event.GetPosition()
        self.draw_plot()


# noinspection PyAttributeOutsideInit,PyPep8Naming
class MyApp(wx.App):
    def OnInit(self):
        self.frame = MyFrame(parent=None, id=-1)
        self.frame.Show()
        self.SetTopWindow(self.frame)
        return True

if __name__ == '__main__':
    app = MyApp()
    app.MainLoop()
