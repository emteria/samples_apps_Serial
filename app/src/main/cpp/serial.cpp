/*
 * Copyright (C) 2017-2021 emteria.OS project
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <jni.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SerialJNI", __VA_ARGS__)

// Save the file descriptor between multiple calls
int gSerialDescriptor = -1;

extern "C" void write_message(char message[])
{
    if (gSerialDescriptor < 0) { return; }

    ssize_t count = write(gSerialDescriptor, message, strlen(message));
    if (count < 0) { LOGI("ERR: failed to send data"); }
}

extern "C" JNIEXPORT void JNICALL Java_com_emteria_serial_SerialDriver_open(JNIEnv* env, jclass /* this */)
{
    if (gSerialDescriptor > 0) { return; }

    // open in non blocking read/write mode
    gSerialDescriptor = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (gSerialDescriptor == -1)
    {
        LOGI("ERR: unable to open serial port! wrong permissions?");
        return;
    }

    struct termios options;
    int config = tcgetattr(gSerialDescriptor, &options);
    if (config < 0)
    {
        LOGI("ERR: failed to get UART config");
        return;
    }

    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;

    tcflush(gSerialDescriptor, TCIFLUSH);
    tcsetattr(gSerialDescriptor, TCSANOW, &options);

    LOGI("Opened file descriptor");
}

extern "C" JNIEXPORT void JNICALL Java_com_emteria_serial_SerialDriver_logcat(JNIEnv* env, jclass /* this */)
{
    if (gSerialDescriptor < 0) { return; }

    char cleanup[] = { 12 };
    write_message(cleanup);

    char greeting[] = "Hello, there! I'll print to the logcat everything you type. Please exit with ESC.\n";
    write_message(greeting);

    char rx_buffer[128];
    unsigned int count = 0;
    ssize_t rx_length = 0;
    bool running = true;
    while (running)
    {
        rx_length = read(gSerialDescriptor, &rx_buffer, sizeof(rx_buffer));

        if (rx_buffer[0] == 27)
        {
            running = false;
            continue;
        }

        if (rx_length < 0)
        {
            running = count++ > 1000;
            usleep(100000);
            continue;
        }

        count = 0;
        if (rx_length == 0) { continue; }

        rx_buffer[rx_length] = '\0';
        LOGI("Received %d bytes: %s (%d)", (int) rx_length, rx_buffer, (int) rx_buffer[0]);
    }

    char goodbye[] = "\nGoodbye!";
    write_message(goodbye);
}

extern "C" JNIEXPORT void JNICALL Java_com_emteria_serial_SerialDriver_close(JNIEnv* env, jclass /* this */)
{
    if (gSerialDescriptor < 0) { return; }

    close(gSerialDescriptor);
    gSerialDescriptor = -1;

    LOGI("Closed file decriptor");
}
