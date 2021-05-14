# IoTrash

IoTrash is a ESP-32 powered smart-bin that gathers sensor data and publishes the sensor data to a MQTT broker to help waste management more efficient.

## Prerequisites

### Hardware
- ESP32 Microcontroller
- US-100 Ultrasonic Sensor

### Software
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO](https://platformio.org/), a cross-platform VSCode-based IDE for embedded system development

## Compiling & Uploading the Code

1. First, clone the repo by using the command 
```bash
git clone https://github.com/patrickanguyen/IoTrash.git
```

2. In the ```include``` folder, create a header file called ```secrets.h```, which will hold your secret information including your wifi information and the details of your MQTT broker. Copy the contents of ```secrets_example.txt``` into ```secrets.h``` and fill out the header file with the appropriate information.

4. Then, finally, the last step is to upload the code by going to the PlatformIO tab on the left and clicking on the ```Upload``` button which will compile and upload the source code to the ESP-32.

If anything goes well, the application should be uploaded to your microcontroller and the smart sensor should be publishing the sensor data to your MQTT broker!
