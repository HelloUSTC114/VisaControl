#include "mainwindow.h"

#include <QApplication>

// C++ STL
#include <stdio.h>
#include <iostream>
#include <memory.h>
#include <string>

// NI-VISA
#include <visa.h>

int ProcessError(ViSession &defaultRM, ViSession &device, ViStatus status, ViChar *buffer)
{
    // Report error and clean up
    viStatusDesc(device, status, buffer);
    fprintf(stderr, "failure: %s\n", buffer);
    if (defaultRM != VI_NULL)
    {
        viClose(defaultRM);
    }
    return 1;
}

int TestOpen()
{
    ViSession defaultRM = VI_NULL, rm1 = VI_NULL, rm2 = VI_NULL, rm3 = VI_NULL, device = VI_NULL;
    ViStatus status;
    ViChar buffer[256];
    ViUInt32 sendCharCount;

    // Open a default Session
    status = viOpenDefaultRM(&defaultRM);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);
    std::cout << status << '\t' << VI_SUCCESS << std::endl;

    // Open the gpib device at primary address 1, gpib board 8
    //    status = viOpen(defaultRM, "TCPIP::localhost::4001::SOCKET", VI_NULL, VI_NULL, &device);
    status = viOpen(defaultRM, "TCPIP::192.168.1.177::INSTR", VI_NULL, VI_NULL, &device);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);

    // Send an ID query.
    status = viWrite(device, (ViBuf) "*idn?", 5, &sendCharCount);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);

    // Clear the buffer and read the response
    memset(buffer, 0, sizeof(buffer));
    status = viRead(device, (ViBuf)buffer, sizeof(buffer), &sendCharCount);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);

    // Print the response
    printf("id: %s\n", buffer);

    status = viWrite(device, (ViBuf) "OUTPut1:STATe ON", 17, &sendCharCount);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);

    // Clean up
    viClose(device); // Not needed, but makes things a bit more understandable
    viClose(defaultRM);
}

int TestFind()
{
    ViSession defaultRM = VI_NULL, device = VI_NULL;
    ViStatus status;
    ViChar deviceName[256], id[256], buffer[256];
    ViUInt32 sendCharCount, itemCnt;
    ViFindList deviceList;
    ViUInt32 i;

    // Open a default Session
    status = viOpenDefaultRM(&defaultRM);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);

    // Find all GPIB devices
    status = viFindRsrc(defaultRM, "GPIB?*INSTR", &deviceList, &itemCnt, deviceName);
    if (status < VI_SUCCESS)
        return ProcessError(defaultRM, device, status, buffer);

    for (i = 0; i < itemCnt; i++)
    {
        // Open resource found in rsrc deviceList
        status = viOpen(defaultRM, deviceName, VI_NULL, VI_NULL, &device);
        if (status < VI_SUCCESS)
            return ProcessError(defaultRM, device, status, buffer);

        // Send an ID query.
        status = viWrite(device, (ViBuf) "*idn?", 5, &sendCharCount);
        if (status < VI_SUCCESS)
            return ProcessError(defaultRM, device, status, buffer);

        // Clear the buffer and read the response
        memset(id, 0, sizeof(id));
        status = viRead(device, (ViBuf)id, sizeof(id), &sendCharCount);
        if (status < VI_SUCCESS)
            return ProcessError(defaultRM, device, status, buffer);

        // Print the response
        printf("id: %s: %s\n", deviceName, id);

        // We're done with this device so close it
        viClose(device);

        // Get the next item
        viFindNext(deviceList, deviceName);
    }

    // Clean up
    viClose(defaultRM);

    return 0;
}

ViSession defaultRM = VI_NULL, device = VI_NULL;
ViFindList deviceList = 0;
ViUInt32 sendCharCount, itemCnt;
ViChar deviceName[256], buffer[256];
void OpenDevice();
void CloseDevice();

int SetAmp(double amp)
{

    auto status = viOpenDefaultRM(&defaultRM);
    status = viFindRsrc(defaultRM, "TCPIP?*INSTR", &deviceList, &itemCnt, deviceName);
    std::cout << deviceList << '\t' << itemCnt << '\t' << deviceName << std::endl;

    status = viFindRsrc(defaultRM, "TCPIP?*INSTR", &deviceList, &itemCnt, deviceName);

    for (int i = 0; i < 1; i++)
    {
        std::cout << "the first Device: " << i << '\t' << itemCnt << std::endl;
        // Open resource found in rsrc deviceList
        status = viOpen(defaultRM, deviceName, VI_NULL, VI_NULL, &device);

        int errorCount = 0;
        std::string sCmd;
        sCmd = "source1:voltage:level:immediate:amplitude 50mVpp";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;

        // Send an ID query.
        status = viWrite(device, (ViBuf) "*idn?", 5, &sendCharCount);
        viClose(device);
    }
    viClose(defaultRM);
    return 0;
}

int TestInit(int argc, char *argv[])
{
    // QApplication a(argc, argv);
    // MainWindow w;

    // w.show();
    // return a.exec();

    ViSession defaultRM = VI_NULL, device = VI_NULL;
    ViFindList deviceList = 0;
    ViUInt32 sendCharCount, itemCnt;
    ViChar deviceName[256], buffer[256];

    auto status = viOpenDefaultRM(&defaultRM);
    status = viFindRsrc(defaultRM, "TCPIP?*INSTR", &deviceList, &itemCnt, deviceName);
    std::cout << deviceList << '\t' << itemCnt << '\t' << deviceName << std::endl;

    for (int i = 0; i < 1; i++)
    {
        std::cout << "the first Device: " << i << '\t' << itemCnt << std::endl;
        // Open resource found in rsrc deviceList
        status = viOpen(defaultRM, deviceName, VI_NULL, VI_NULL, &device);

        // Send an ID query.
        status = viWrite(device, (ViBuf) "*idn?", 5, &sendCharCount);

        // Clear the buffer and read the response
        memset(buffer, 0, sizeof(buffer));
        status = viRead(device, (ViBuf)buffer, sizeof(buffer), &sendCharCount);
        // Print the response
        std::cout << "id: " << deviceName << ": " << buffer << std::endl;
        _sleep(1000);

        // Core write read
        int errorCount = 0;
        std::string sCmd;

        // Oscilllator setting
        sCmd = "source:roscillator:source internal";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        // sCmd = "source1:function:shape edecay";
        sCmd = "source1:function:shape user1";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:frequency:mode CW";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:frequency:CW 1.5kHz";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:voltage:level:immediate:amplitude 50mVpp";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:voltage:level:immediate:offset 0mV";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        // Output settings
        sCmd = "output1:impedance 50";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "output1:polarity normal";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "output:trigger:mode trigger";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        // Burst mode setting
        sCmd = "source1:burst:mode triggered";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:burst:ncycles 1";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:burst:state ON";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        sCmd = "source1:burst:tdelay 0ns";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        //  on
        sCmd = "output1:state 1";
        status = viWrite(device, (ViBuf)sCmd.c_str(), sCmd.size(), &sendCharCount);
        std::cout << errorCount++ << '\t' << "Error? " << (status) << '\t' << sCmd.size() << '\t' << sendCharCount << std::endl;
        _sleep(1000);

        // We're done with this device so close it
        viClose(device);

        // Get the next item
        viFindNext(deviceList, deviceName);
    }

    viClose(defaultRM);
    return 1;
}

#include "visaapi.h"
int main(int argc, char *argv[])
{
    VisaAPI a;

    for (int i = 0; i < 2; i++)
    {
        std::cout << i << std::endl;
        // VisaAPI a1;
        a.SetAmp(50 + i * 5);
        // _sleep(10000);
    }
}
