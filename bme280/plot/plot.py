import serial
import numpy as np
from matplotlib import pyplot as plt
import json
from cmdlapp import CmdlApp

class BME280Plot(CmdlApp):
    def __init__(self):
        CmdlApp.__init__(self)

        self.configure(
            main_fct=self.plot,
            use_cfgfile=False,
            tool_name='bme280plot',
            tool_version='0.0-dev')

        self.num_points = 500

        self.pltdata = {
            't': {
                'buffer': [None] * self.num_points,
                'datakey': 't',
                'color': 'red',
            },
            
            'p': {
                'buffer': [None] * self.num_points,
                'datakey': 'p',
                'color': 'green',
            },
            
            'h': {
                'buffer': [None] * self.num_points,
                'datakey': 'h',
                'color': 'blue'
            }
        }
        

    def setup_arg_parser(self):
        CmdlApp.setup_arg_parser(self)

        self.parser.add_argument(
            '-p', '--port',
            help='Serial port',
            dest='port',
            required=True)

        
    def init_plot(self):
        # set plot to animated
        plt.ion() 
    
        f, axes = plt.subplots(
            nrows=3,
            ncols=1,
            num='sensordata',
            sharex=True)
        
        i = 0
        for k, v in self.pltdata.items():
            v['line'], = axes[i].plot(v['buffer'], color=v['color'])
            axes[i].autoscale(enable=True, axis='y')
            axes[i].set_title(label=v['datakey'])
            v['line'].set_xdata(np.arange(self.num_points))
            i += 1

            
    def get_data(self):
        if self.ser.inWaiting() > 0:
            sdata = self.ser.readline()
    
            try:
                data = json.loads(sdata)
                print(data)
    
                return data
            except Exception as e:
                print(sdata)
                print(e)
            
        return None

    
    def update_plot_data(self, data):
        for k, v in self.pltdata.items():
            del v['buffer'][0]
            v['buffer'].append(data[v['datakey']])
            
            #v['line'].set_xdata(np.arange(len(v['buffer'])))
            v['line'].set_ydata(v['buffer'])
            v['line'].axes.relim()
            v['line'].axes.autoscale_view(scaley=True)
            #set_ylim(0,50)

    
        
    def plot(self):
        self.ser = serial.Serial(self.args.port, 38400)

        self.init_plot()
        
        # start data collection
        while True:
            data = self.get_data()
            if data != None:
                self.update_plot_data(data)
                
            plt.pause(1)

            # Exit when the figure window is closed.
            if not plt.fignum_exists('sensordata'):
                break


if __name__ =='__main__':
    BME280Plot().run()
        
    

        
