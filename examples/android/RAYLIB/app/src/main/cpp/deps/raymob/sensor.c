/*
 *  raymob License (MIT)
 *
 *  Copyright (c) 2023-2024 Le Juez Victor
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "raymob.h"

#include <android/sensor.h>
#include <stdlib.h>

/* GLOBAL VARIABLES */

struct SensorInputs {
    Vector3 accelerometerAxis;
    Vector3 gyroscopeAxis;
};

static struct {

    ASensorManager* manager;
    ASensorEventQueue* eventQueue;
    const ASensor *sensors[2];
    int looperID;

    struct SensorInputs inputs;

} State = { 0 };

/* INTERNAL FUNCTIONS */

static const char* GetSensorName(Sensor sensor)
{
    const char *sensorName = "UNKNOWN";
    switch (sensor) {
        case SENSOR_ACCELEROMETER: sensorName = "SENSOR_ACCELEROMETER"; break;
        case SENSOR_GYROSCOPE: sensorName = "SENSOR_GYROSCOPE"; break;
        default: break;
    }
    return sensorName;
}

static int SensorCallback(int fd, int events, void* data)
{
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(State.eventQueue, &event, 1) > 0) {
        switch (event.type) {
            case ASENSOR_TYPE_ACCELEROMETER:
                State.inputs.accelerometerAxis.x = event.acceleration.x;
                State.inputs.accelerometerAxis.y = event.acceleration.y;
                State.inputs.accelerometerAxis.z = event.acceleration.z;
                break;
            case ASENSOR_TYPE_GYROSCOPE:
                State.inputs.gyroscopeAxis.x = event.gyro.x;
                State.inputs.gyroscopeAxis.y = event.gyro.y;
                State.inputs.gyroscopeAxis.z = event.gyro.z;
                break;
            default:
                break;
        }
    }
    return 1;
}

/* PUBLIC API */

void InitSensorManager(void)
{
    State.manager = ASensorManager_getInstance();

    // Get all available sensors

    ASensorList sensorList;
    int sensorCount = ASensorManager_getSensorList(State.manager, &sensorList);

    for (int i = 0 ; i < sensorCount ; i++) {
        const char *type = ASensor_getStringType(sensorList[i]);
        const char *vendor = ASensor_getVendor(sensorList[i]);
        const char *name = ASensor_getName(sensorList[i]);
        bool supported = true;
        switch (ASensor_getType(sensorList[i])) {
            case ASENSOR_TYPE_ACCELEROMETER:
                State.sensors[SENSOR_ACCELEROMETER] = sensorList[i];
                break;
            case ASENSOR_TYPE_GYROSCOPE:
                State.sensors[SENSOR_GYROSCOPE] = sensorList[i];
                break;
            default:
                supported = false;
                break;
        }
        TraceLog(LOG_INFO, "Sensor %s:\n    > Name: %s\n    > Vendor: %s\n    > Supported: %s",
                 type, name, vendor, supported ? "YES" : "NO");
    }

    // Create event queue

    State.looperID = 1;
    State.eventQueue = ASensorManager_createEventQueue(
            State.manager, ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS),
            State.looperID, SensorCallback, NULL);

    if (State.eventQueue == NULL) {
        TraceLog(LOG_ERROR, "Cannot create sensor event queue");
    }
}

void EnableSensor(Sensor sensor)
{
    if (State.sensors[sensor] == NULL) {
        TraceLog(LOG_WARNING, "Cannot enable unsupported sensor: %s", GetSensorName(sensor));
        return;
    }
    if (ASensorEventQueue_enableSensor(State.eventQueue, State.sensors[sensor]) != 0) {
        TraceLog(LOG_ERROR, "Cannot enable sensor: %s", GetSensorName(sensor));
    }
}

void DisableSensor(Sensor sensor)
{
    if (State.sensors[sensor] == NULL) {
        TraceLog(LOG_WARNING, "Cannot disable unsupported sensor: %s", GetSensorName(sensor));
        return;
    }
    if (ASensorEventQueue_disableSensor(State.eventQueue, State.sensors[sensor]) != 0) {
        TraceLog(LOG_ERROR, "Cannot disable sensor: %s", GetSensorName(sensor));
    }
}

bool IsSensorAvailable(Sensor sensor)
{
    return State.sensors[sensor] != NULL;
}

Vector3 GetAccelerotmerAxis(void)
{
    return State.inputs.accelerometerAxis;
}

Vector3 GetGyroscopeAxis(void)
{
    return State.inputs.gyroscopeAxis;
}
