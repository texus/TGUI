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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Static variables */

static jobject featuresInstance = NULL;

/* Functions definition */

JNIEnv* AttachCurrentThread(void)
{
    JavaVM *vm = GetAndroidApp()->activity->vm;
    JNIEnv *env = NULL;

    (*vm)->AttachCurrentThread(vm, &env, NULL);
    return env;
}

void DetachCurrentThread(void)
{
    JavaVM *vm = GetAndroidApp()->activity->vm;
    (*vm)->DetachCurrentThread(vm);
}

jobject GetNativeLoaderInstance(void)
{
    return GetAndroidApp()->activity->clazz;
}

jobject GetFeaturesInstance(void)
{
    if (featuresInstance == NULL)
    {
        JNIEnv *env = AttachCurrentThread();
        jobject nativeLoaderInstance = GetNativeLoaderInstance();

        jclass nativeLoaderClass = (*env)->GetObjectClass(env, nativeLoaderInstance);
        jmethodID getFeaturesMethod = (*env)->GetMethodID(env, nativeLoaderClass, "getFeatures", "()Lcom/raylib/raymob/Features;");

        if (getFeaturesMethod == NULL) return NULL; // Handle the case where the method is not found

        jobject localFeaturesInstance = (*env)->CallObjectMethod(env, nativeLoaderInstance, getFeaturesMethod);
        featuresInstance = (*env)->NewGlobalRef(env, localFeaturesInstance);

        DetachCurrentThread();
    }

    return featuresInstance;
}

char* GetCacheDir(void)
{
    struct android_app *app = GetAndroidApp();

    JavaVM* vm = app->activity->vm;
    JNIEnv* env = NULL;
    (*vm)->AttachCurrentThread(vm, &env, NULL);

    // Get the activity object and its class
    jobject activity = app->activity->clazz;
    jclass activityClass = (*env)->GetObjectClass(env, activity);

    // Get the method ID for the getCacheDir() method of the activity
    jmethodID getCacheDirMethod = (*env)->GetMethodID(env, activityClass, "getCacheDir", "()Ljava/io/File;");

    // Call the getCacheDir() method to get the cache directory
    jobject cacheDir = (*env)->CallObjectMethod(env, activity, getCacheDirMethod);

    // Get the class object for java.io.File
    jclass fileClass = (*env)->GetObjectClass(env, cacheDir);

    // Get the method ID for the getPath() method of java.io.File
    jmethodID getPathMethod = (*env)->GetMethodID(env, fileClass, "getPath", "()Ljava/lang/String;");

    // Call the getPath() method to get the path of the cache directory
    jstring pathString = (jstring)(*env)->CallObjectMethod(env, cacheDir, getPathMethod);

    // Get the UTF-8 encoded string from the Java string
    const char *pathChars = (*env)->GetStringUTFChars(env, pathString, NULL);

    // Allocate memory for the cache path
    size_t len = strlen(pathChars) + 1; // NOTE: +1 for the null terminator
    char* cachePath = RL_MALLOC(len);

    // Copy the string to the allocated memory
    if (cachePath)
    {
        strncpy(cachePath, pathChars, len);
        cachePath[len - 1] = '\0'; // NOTE: just for security
    }

    // Release the UTF-8 encoded string
    (*env)->ReleaseStringUTFChars(env, pathString, pathChars);

    // Clean up local references
    (*env)->DeleteLocalRef(env, pathString);
    (*env)->DeleteLocalRef(env, fileClass);
    (*env)->DeleteLocalRef(env, cacheDir);
    (*env)->DeleteLocalRef(env, activityClass);

    // Detach the current thread from the JavaVM
    (*vm)->DetachCurrentThread(vm);

    // Return the cache path
    return cachePath;
}

char* LoadCacheFile(const char* fileName)
{
    char *text = NULL;
    char *cacheDir = GetCacheDir();
    size_t len1 = strlen(cacheDir);
    size_t len2 = strlen(fileName);
    size_t len = len1 + len2 + 1;
    char *filePath = RL_MALLOC(len);
    strncpy(filePath, cacheDir, len1);
    filePath[len1] = '/';
    strncpy(filePath + len1 + 1, fileName, len2);
    filePath[len] = '\0';

    FILE * file = fopen(filePath, "rt");
    if (file != NULL)
    {
        // WARNING: When reading a file as 'text' file,
        // text mode causes carriage return-linefeed translation...
        // ...but using fseek() should return correct byte-offset
        fseek(file, 0, SEEK_END);
        unsigned int size = (unsigned int)ftell(file);
        fseek(file, 0, SEEK_SET);

        if (size > 0)
        {
            text = (char *)RL_MALLOC((size + 1)*sizeof(char));

            if (text != NULL)
            {
                unsigned int count = (unsigned int)fread(text, sizeof(char), size, file);

                // WARNING: \r\n is converted to \n on reading, so,
                // read bytes count gets reduced by the number of lines
                if (count < size) text = RL_REALLOC(text, count + 1);

                // Zero-terminate the string
                text[count] = '\0';

                TraceLog(LOG_INFO, "FILEIO: [%s] Text file loaded successfully", fileName);
            }
            else TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", fileName);
        }
        else TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to read text file", fileName);

        fclose(file);
    }
    else TraceLog(LOG_WARNING, "FILEIO: File name provided is not valid");

    RL_FREE(filePath);
    RL_FREE(cacheDir);

    return text;
}

char* GetL10NString(const char* value)
{
    jobject nativeInstance = GetNativeLoaderInstance();

    if (nativeInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();

        // Get the native context class (nativeInstance)
        jclass nativeClass = (*env)->GetObjectClass(env, nativeInstance);

        // Get the native instance's getResources method
        jmethodID getResourcesMethod = (*env)->GetMethodID(env, nativeClass, "getResources", "()Landroid/content/res/Resources;");
        jobject resources = (*env)->CallObjectMethod(env, nativeInstance, getResourcesMethod);

        // Get the getIdentifier method of the Resources class
        jclass resourcesClass = (*env)->GetObjectClass(env, resources);
        jmethodID getIdentifierMethod = (*env)->GetMethodID(env, resourcesClass, "getIdentifier",
                                                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");

        // Convert string name passed as parameter to jstring
        jstring resourceName = (*env)->NewStringUTF(env, value);
        jstring defType = (*env)->NewStringUTF(env, "string");

        // Get the package name of the native instance
        jmethodID getPackageNameMethod = (*env)->GetMethodID(env, nativeClass, "getPackageName", "()Ljava/lang/String;");
        jstring packageName = (jstring)(*env)->CallObjectMethod(env, nativeInstance, getPackageNameMethod);

        // Call getIdentifier to get the resource identifier
        jint resId = (*env)->CallIntMethod(env, resources, getIdentifierMethod, resourceName, defType, packageName);

        // Clean up used local references
        (*env)->DeleteLocalRef(env, resourceName);
        (*env)->DeleteLocalRef(env, defType);

        if (resId == 0) {
            // No identifier found for this resource
            DetachCurrentThread();
            return NULL;
        }

        // Call getString with the obtained identifier
        jmethodID getStringMethod = (*env)->GetMethodID(env, nativeClass, "getString", "(I)Ljava/lang/String;");
        jstring rv = (jstring)(*env)->CallObjectMethod(env, nativeInstance, getStringMethod, resId);

        if (rv == NULL) {
            DetachCurrentThread();
            return NULL;
        }

        // Convert jstring to char*
        const char* strReturn = (*env)->GetStringUTFChars(env, rv, NULL);

        // Allocate memory for returned string
        size_t len = strlen(strReturn) + 1;
        char* stringValue = RL_MALLOC(len);

        if (stringValue) {
            strncpy(stringValue, strReturn, len);
            stringValue[len - 1] = '\0'; // Just for security: end with '\0'
        }

        // Free UTF string and clean local references
        (*env)->ReleaseStringUTFChars(env, rv, strReturn);
        (*env)->DeleteLocalRef(env, rv);

        DetachCurrentThread();
        return stringValue;
    }

    return NULL;
}

char* GetAppStoragePath(){

    jobject nativeInstance = GetNativeLoaderInstance();

    if (nativeInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
        
        // Get the native context class (nativeInstance)
        jclass nativeClass = (*env)->GetObjectClass(env, nativeInstance);

        // Get the getExternalFilesDir method ID
        jmethodID getExternalFilesDirMethod = (*env)->GetMethodID(env, nativeClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");

        // Call getExternalFilesDir(null) to get the root external files directory
        jobject fileObj = (*env)->CallObjectMethod(env, nativeInstance, getExternalFilesDirMethod, NULL);

        // Get the java.io.File class
        jclass fileClass = (*env)->GetObjectClass(env, fileObj);

        // Get the getAbsolutePath() method ID
        jmethodID getAbsolutePathMethod = (*env)->GetMethodID(env, fileClass, "getAbsolutePath", "()Ljava/lang/String;");

        // Call getAbsolutePath() to get the Java string
        jstring jFilePath = (jstring)(*env)->CallObjectMethod(env, fileObj, getAbsolutePathMethod);

        // Convert Java string to C string
        const char *cFilePath = (*env)->GetStringUTFChars(env, jFilePath, NULL);

        char *filepath = strdup(cFilePath);

        (*env)->ReleaseStringUTFChars(env, jFilePath, cFilePath);
        (*env)->DeleteLocalRef(env, jFilePath);
        (*env)->DeleteLocalRef(env, fileClass);
        (*env)->DeleteLocalRef(env, fileObj);
        (*env)->DeleteLocalRef(env, nativeClass);
        
        DetachCurrentThread();

        return filepath;
    }

    return NULL;
}

void* ReadFromAppStorage(const char *filepath, int *dataSize){

    char *appStoragePath = GetAppStoragePath();

    size_t pathLen = strlen(appStoragePath) + strlen(filepath) + 2;
    char *path = RL_MALLOC(sizeof(char)*pathLen);
    snprintf(path, pathLen, "%s/%s", appStoragePath, filepath);

    unsigned char *data = NULL;
    *dataSize = 0;

    FILE *file = fopen(path, "rb");

    if (file == NULL){
        TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to open file", path);
        RL_FREE(appStoragePath);
        RL_FREE(path);
        return NULL;
    }

    // WARNING: On binary streams SEEK_END could not be found,
    // using fseek() and ftell() could not work in some (rare) cases
    fseek(file, 0, SEEK_END);
    int size = ftell(file);     // WARNING: ftell() returns 'long int', maximum size returned is INT_MAX (2147483647 bytes)
    fseek(file, 0, SEEK_SET);

    if (size > 0)
    {
        data = RL_MALLOC(size*sizeof(unsigned char));

        if (data != NULL)
        {
            // NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, size elements]
            size_t count = fread(data, sizeof(unsigned char), size, file);

            // WARNING: fread() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
            // dataSize is unified along raylib as a 'int' type, so, for file-sizes > INT_MAX (2147483647 bytes) we have a limitation
            if (count > 2147483647)
            {
                TraceLog(LOG_WARNING, "FILEIO: [%s] File is bigger than 2147483647 bytes, avoid using LoadFileData()", path);

                RL_FREE(data);
                data = NULL;
            }
            else
            {
                *dataSize = (int)count;

                if ((*dataSize) != size) TraceLog(LOG_WARNING, "FILEIO: [%s] File partially loaded (%i bytes out of %i)", path, dataSize, count);
                else TraceLog(LOG_INFO, "FILEIO: [%s] File loaded successfully", path);
            }
        }
        else TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to allocated memory for file reading", path);
    }
    else TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to read file", path);

    fclose(file);

    RL_FREE(appStoragePath);
    RL_FREE(path);

    return data;
}

bool WriteToAppStorage(const char *filepath, void *data, unsigned int dataSize){

    char *appStoragePath = GetAppStoragePath();

    size_t pathLen = strlen(appStoragePath) + strlen(filepath) + 2;
    char *path = RL_MALLOC(sizeof(char)*pathLen);
    snprintf(path, pathLen, "%s/%s", appStoragePath, filepath);

    bool success = false;

    FILE *file = fopen(path, "wb");

    if (file != NULL)
    {
        // WARNING: fwrite() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
        // and expects a size_t input value but as dataSize is limited to INT_MAX (2147483647 bytes), there shouldn't be a problem
        int count = (int)fwrite(data, sizeof(unsigned char), dataSize, file);

        if (count == 0) TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to write file", path);
        else if (count != dataSize) TraceLog(LOG_WARNING, "FILEIO: [%s] File partially written", path);
        else TraceLog(LOG_INFO, "FILEIO: [%s] File saved successfully", path);

        int result = fclose(file);
        if (result == 0) success = true;
    }
    else TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to open file", path);

    RL_FREE(appStoragePath);
    RL_FREE(path);

    return success;
}

bool IsFileExistsInAppStorage(const char *filepath){

    char *appStoragePath = GetAppStoragePath();

    size_t pathLen = strlen(appStoragePath) + strlen(filepath) + 2;
    char *path = RL_MALLOC(sizeof(char)*pathLen);
    snprintf(path, pathLen, "%s/%s", appStoragePath, filepath);

    bool success = (access(path, F_OK) != -1);

    RL_FREE(appStoragePath);
    RL_FREE(path);

    return success;
}

void RemoveFileInAppStorage(const char *filepath){

    char *appStoragePath = GetAppStoragePath();

    size_t pathLen = strlen(appStoragePath) + strlen(filepath) + 2;
    char *path = RL_MALLOC(sizeof(char)*pathLen);
    snprintf(path, pathLen, "%s/%s", appStoragePath, filepath);

    remove(path);

    RL_FREE(appStoragePath);
    RL_FREE(path);
}
