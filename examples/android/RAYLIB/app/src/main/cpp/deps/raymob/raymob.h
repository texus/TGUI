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

#ifndef RAYMOB_H
#define RAYMOB_H

#include "android_native_app_glue.h"
#include "raylib.h"
#include "jni.h"

/* ENUMS */

typedef enum {
    SENSOR_ACCELEROMETER    = 0,
    SENSOR_GYROSCOPE        = 1,
} Sensor;

/* Callback define */

typedef void (*Callback)();

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Retrieves the Android application object.
 *
 * This function is defined in 'raylib/platforms/rcore_android.c'.
 * The function is not public by default, so we are making its declaration
 * public here now (instead of doing it in 'raylib.h'), in order to keep a
 * clean version of raylib.
 *
 * @return Pointer to the Android application object.
 */
struct android_app *GetAndroidApp(void);


/* Helper functions */

/**
 * @brief Attaches the current native thread to the Java VM environment.
 *
 * @return Pointer to the JNIEnv structure.
 */
JNIEnv* AttachCurrentThread(void);

/**
 * @brief Detaches the current native thread from the Java VM environment.
 */
void DetachCurrentThread(void);

/**
 * @brief Returns a pointer to the class that initiated the native activity.
 *
 * @return Pointer to the native loader instance.
 */
jobject GetNativeLoaderInstance(void);

/**
 * @brief Gets the cache directory path of the Android application.
 *
 * @warning This function returns a string allocated on the heap.
 * The responsibility for releasing the memory lies with the user.
 *
 * @return Pointer to the cache directory path string.
 */
char* GetCacheDir(void);

/**
 * @brief Read file from cache directory, the readFile from raylib is reserved to read in Assets folder.
 *
 * @param fileName to read
 * @return file content
 */
char* LoadCacheFile(const char* fileName);

/**
 * @brief Get localized string resource by name L10N.
 *
 * @see L10N: https://en.wikipedia.org/wiki/Internationalization_and_localization
 *
 * @warning This function returns a string allocated on the heap.
 * The responsibility for releasing the memory lies with the user.
 *
 * @param value string resource name
 * @return localized string
 */
char* GetL10NString(const char* value);


/* Vibrator functions */

/**
 * @brief Initiates device vibration for the specified duration in seconds.
 *
 * @param seconds Duration of vibration in seconds.
 */
void Vibrate(float seconds);

/**
 * @brief Initiates device vibration for the specified duration in milliseconds.
 *
 * @param ms Duration of vibration in milliseconds.
 */
void VibrateMS(uint64_t ms);

/**
 * @brief Initiates device vibration for the specified duration and intensity in seconds.
 *
 * @param seconds Duration of vibration in seconds.
 * @param intensity Intensity of the vibration (0.0 to 1.0).
 */
void VibrateEx(float seconds, float intensity);

/**
 * @brief Initiates device vibration for the specified duration and intensity in milliseconds.
 *
 * @param ms Duration of vibration in milliseconds.
 * @param intensity Intensity of the vibration (0.0 to 1.0).
 */
void VibrateExMS(uint64_t ms, float intensity);


/* Sensor functions */

/**
 * @brief Initializes the sensor manager for accessing device sensors.
 */
void InitSensorManager(void);

/**
 * @brief Enables the specified sensor.
 *
 * @param sensor The sensor to be enabled (e.g., Gyroscope, Accelerometer).
 */
void EnableSensor(Sensor sensor);

/**
 * @brief Disables the specified sensor.
 *
 * @param sensor The sensor to be disabled (e.g., Gyroscope, Accelerometer).
 */
void DisableSensor(Sensor sensor);

/**
 * @brief Retrieves the current accelerometer axis values.
 *
 * @return A Vector3 representing the accelerometer axis (x, y, z).
 */
Vector3 GetAccelerotmerAxis(void);

/**
 * @brief Retrieves the current gyroscope axis values.
 *
 * @return A Vector3 representing the gyroscope axis (x, y, z).
 */
Vector3 GetGyroscopeAxis(void);


/* Soft Keyboard functions */

/**
 * @brief Displays the soft keyboard on the screen.
 */
void ShowSoftKeyboard(void);

/**
 * @brief Hides the soft keyboard from the screen.
 */
void HideSoftKeyboard(void);

/**
 * @brief Returns the code of the last key pressed on the soft keyboard.
 *
 * @return Code of the last key pressed on the soft keyboard.
 */
int GetLastSoftKeyCode(void);

/**
 * @brief Returns the label of the last key pressed on the soft keyboard.
 *
 * @return Label of the last key pressed on the soft keyboard.
 */
unsigned short GetLastSoftKeyLabel(void);

/**
 * @brief Returns the Unicode value of the last key pressed on the soft keyboard.
 *
 * @return Unicode value of the last key pressed on the soft keyboard.
 */
int GetLastSoftKeyUnicode(void);

/**
 * @brief Returns the character value of the last key pressed on the soft keyboard.
 *
 * @return Character value of the last key pressed on the soft keyboard.
 */
char GetLastSoftKeyChar(void);

/**
 * @brief Clears the record of the last soft key pressed.
 */
void ClearLastSoftKey(void);

/**
 * @brief Allows editing the text displayed in the soft keyboard.
 *
 * @param text Pointer to the text to be edited.
 * @param size Size of the text buffer.
 */
void SoftKeyboardEditText(char* text, unsigned int size);

/**
 * @brief Controls whether the screen should remain on or not.
 *
 * @param keepOn If true, the screen should remain on; otherwise, it should not.
 */
void KeepScreenOn(bool keepOn);

/* Callback functions */

/**
 * @brief Initializes all callback functions to their default states.
 */
void InitCallBacks(void);

/**
 * @brief Sets the callback function to be called when the application starts.
 *
 * @param callback The callback function to be executed on start.
 */
void SetOnStartCallBack(Callback callback);

/**
 * @brief Sets the callback function to be called when the application resumes.
 *
 * @param callback The callback function to be executed on resume.
 */
void SetOnResumeCallBack(Callback callback);

/**
 * @brief Sets the callback function to be called when the application pauses.
 *
 * @param callback The callback function to be executed on pause.
 */
void SetOnPauseCallBack(Callback callback);

/**
 * @brief Sets the callback function to be called when the application is stopped.
 *
 * @param callback The callback function to be executed on destroy.
 */
void SetOnStopCallBack(Callback callback);

/**
 * @brief Get the app specific storage root path.
 * 
 * @warning This function returns a string allocated on the heap.
 * The responsibility for releasing the memory lies with the user.
 * Use free().
 * 
 * @return app specific storage path.
 */
char* GetAppStoragePath();

/**
 * @brief Read file in app specific storage.
 *
 * @param filepath Path of the file relative to app specific storage.
 * @param size Variable to store size of data read.
 * 
 * @return the data read.
 */
void* ReadFromAppStorage(const char *filepath, int *size);

/**
 * @brief Write file in app specific storage.
 *
 * @param filepath Path of the file relative to app specific storage.
 * @param data Pointer to the data.
 * @param size Size of the data.
 * 
 * @return true on success.
 */
bool WriteToAppStorage(const char *filepath, void *data, unsigned int size);

/**
 * @brief Check file exist or not in app specific storage.
 *
 * @param filepath Path of the file relative to app specific storage.
 *
 * @return true if file exists.
 */
bool IsFileExistsInAppStorage(const char *filepath);

/**
 * @brief Remove a file in app specific storage.
 *
 * @param filepath Path of the file relative to app specific storage.
 */
void RemoveFileInAppStorage(const char *filepath);

#if defined(__cplusplus)
}
#endif

#endif //RAYMOB_H
