import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np


running = True
 

def get_coords(bytestring):
    l = bytestring.split()
    x = float(l[0][2:])
    y = float(l[1][2:])
    z = float(l[2][2:])
    return [x,y,z]

with serial.Serial('/dev/ttyACM0', timeout=1) as ser:

    ax = plt.figure().add_subplot(projection='3d')
    x,y,z = [0,0,0] # Location of sun vector on 3d plot
    plt.ion()

    while running:
        try:
            coords = get_coords(ser.readline())
            print(coords)
            ax.cla()
            ax.set_xlabel("X")
            ax.set_ylabel("Y")
            ax.set_zlabel("Z")
            ax.set_title("Sun vector")
            ax.quiver(x, y, z, *coords, length=0.1, normalize=True)
            plt.draw()
            plt.pause(0.001)

        except Exception as e:
            print(e)