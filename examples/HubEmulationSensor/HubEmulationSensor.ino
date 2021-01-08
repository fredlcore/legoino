/**
 * A Legoino example to emulate a train hub which directly sends
 * out the motor commands as IR signals to a power function IR receiver. 
 * With this setup you can upgrade your power function systems to powerup systems
 * 
 * For the setup the IR LED has to be connected on the OUTPUT PIN 12 of the 
 * ESP controller. This will work out of the Box with the M5 Atom matrix/light
 * esp32 board which has a build in IR LED on Port 12
 * 
 * Port A of the powered up hub is mapped to the RED power function port
 * Port B of the powered up hub is mapped to the BLUE power function port
 * 
 * Example video: https://www.youtube.com/watch?v=RTNexxT4-yQ&t=16s
 * 
 * (c) Copyright 2020 - Cornelius Munz
 * Released under MIT License
 * 
 */

#include "Lpf2HubEmulation.h"
#include "LegoinoCommon.h"

// create a hub instance
Lpf2HubEmulation myEmulatedHub("TrainHub", HubType::POWERED_UP_HUB);

int position = 0;

void writeValueCallback(byte port, byte value)
{
  Serial.println("writeValueCallback: ");
  Serial.println(port, HEX);
  Serial.println(value, HEX);

  if (port == 0x32)
  {
    Serial.print("Hub LED command received with color: ");
    Serial.println(LegoinoCommon::ColorStringFromColor(value).c_str());
  }
}

void setup()
{
  Serial.begin(115200);
  // define the callback function if a write message event on the characteristic occurs
  myEmulatedHub.setWritePortCallback(&writeValueCallback);
  myEmulatedHub.start();
}

// main loop
void loop()
{

  // if an app is connected, attach some devices on the ports to signalize
  // the app that values could be received/written to that ports
  if (myEmulatedHub.isConnected && !myEmulatedHub.isPortInitialized)
  {
    delay(2000);
    myEmulatedHub.attachDevice((byte)PoweredUpHubPort::A, DeviceType::MOVE_HUB_MEDIUM_LINEAR_MOTOR);
    delay(2000);
    myEmulatedHub.attachDevice((byte)PoweredUpHubPort::LED, DeviceType::HUB_LED);
    delay(2000);
    myEmulatedHub.attachDevice((byte)PoweredUpHubPort::B, DeviceType::TRAIN_MOTOR);
    delay(2000);
    myEmulatedHub.isPortInitialized = true;
  }

  if (!myEmulatedHub.isConnected && myEmulatedHub.isPortInitialized)
  {
    myEmulatedHub.isPortInitialized = false;
  }

  if (myEmulatedHub.isConnected && myEmulatedHub.isPortInitialized)
  {
    delay(1000);
    position++;
    std::string payload;
    Serial.print("position: ");
    Serial.println(position, DEC);

    byte *positionBytes = LegoinoCommon::Int32ToByteArray(position);
    payload.push_back((char)PoweredUpHubPort::A);
    payload.push_back((char)positionBytes[0]);
    payload.push_back((char)positionBytes[1]);
    payload.push_back((char)positionBytes[2]);
    payload.push_back((char)positionBytes[3]);

    myEmulatedHub.writeValue(MessageType::PORT_VALUE_SINGLE, payload);
  }

} // End of loop