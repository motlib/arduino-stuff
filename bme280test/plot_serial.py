import serial
import numpy as np
from matplotlib import pyplot as plt
import json

num_points = 500

pltdata = {
    't': {
        'buffer': [25.0] * num_points,
        'datakey': 'temperature',
    },
    
    'p': {
        'buffer': [1000.0] * num_points,
        'datakey': 'pressure',
    },
    
    'h': {
        'buffer': [60.0] * num_points,
        'datakey': 'humidity',
    }
}

ser = serial.Serial('COM8', 38400)

def init_plot():
    # set plot to animated
    plt.ion() 

    f, axes = plt.subplots(3, 1)
    
    i = 0
    for k, v in pltdata.items():
        v['line'], = axes[i].plot(v['buffer'])
        axes[i].autoscale(enable=True, axis='y')
        axes[i].set_title(label=v['datakey'])
        v['line'].set_xdata(np.arange(num_points))
        i += 1


def get_data():
    if ser.inWaiting() > 0:
        sdata = ser.readline()

        try:
            data = json.loads(sdata)
            print(data)

            return data
        except Exception as e:
            print(sdata)
            print(e)
        
    return None



def update_plot_data(data):
    for k, v in pltdata.items():
        del v['buffer'][0]
        v['buffer'].append(data[v['datakey']])
        
        #v['line'].set_xdata(np.arange(len(v['buffer'])))
        v['line'].set_ydata(v['buffer'])
        v['line'].axes.relim()
        v['line'].axes.autoscale_view(scaley=True)
        #set_ylim(0,50)
    
    
init_plot()    

# start data collection
while True:
    data = get_data()
    if data != None:
        update_plot_data(data)

    plt.pause(1)
        
    

        
