# open hardware monitor must be running for this program to work!
import math

import time
import wmi
import serial

arduino = serial.Serial('COM6', 9600, timeout=.1)
w = wmi.WMI(namespace="root\OpenHardwareMonitor")

gpuLoad = 0
cpuLoad = 0
gpuTemp = 0
cpuTemp = 0
ramPerc = 0
ramLoad = 0

data = 0

while True:
    hardwareMon = w.Sensor()
    for sensor in hardwareMon:
        if sensor.Name == u'GPU Core' and sensor.SensorType == u'Load':
            gpuLoad = math.trunc(sensor.Value)
        if sensor.Name == u'CPU Total' and sensor.SensorType == u'Load':
            cpuLoad = math.trunc(sensor.Value)
        if sensor.Name == u'GPU Core' and sensor.SensorType == u'Temperature':
            gpuTemp = math.trunc(sensor.Value)
        if sensor.Name == u'CPU Package' and sensor.SensorType == u'Temperature':
            cpuTemp = math.trunc(sensor.Value)
        if sensor.Name == u'Used Memory' and sensor.SensorType == u'Data':
            ramPerc = math.trunc(sensor.Value)
        if sensor.Name == u'Memory' and sensor.SensorType == u'Load':
            ramLoad = math.trunc(sensor.Value)

    data = str(gpuLoad) + "|" + str(cpuLoad) + "|" + str(gpuTemp) + "|" + \
           str(cpuTemp) + "|" + str(ramPerc) + "|" + str(ramLoad) + "|0"
    arduino.write(bytes(data, 'utf-8'))
    time.sleep(2)
