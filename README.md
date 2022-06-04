# Feather-M0-Altimeter
Low-form factor altimeter for DU Spaceflight rocket and balloon launches 


## The Circuit 

![circuit diagram](images/image1.jpg)

Components: 
- [Adafruit Feather M0](https://www.adafruit.com/product/2772)
- [DPS310 Sensor](https://www.adafruit.com/product/4494) 
- 3.7V 400mAh LiPo

## The Code

The code uses the [Flash Storage library](https://github.com/cmaglie/FlashStorage) and the [Adafruit DPS310 library](https://github.com/adafruit/Adafruit_DPS310) to collect temperature, pressure and altitude data and store the maximum altitude in the onboard flash memory. 

The max values is then retained and can be printed via serial using the command "M". To get all of the data currently being held on the chip use the command "A". 

## Usage

To use the device plug in the battery. The green light on the DPS310 sensor will illuminate to indicate power. The red LED will flash to indicate the sensor is working. The device will then continuosly collect pressure, temperature and altitude data and update the `max_alt` parameter as necessary. To read data off the device, insert the serial USB connection and open the Serial monitor in the Arduino IDE. Send the command "M" to print max values, and the command "A" to print all data being held on the device. In the event of a power outage this data will not be retained (as it is stored in the volatile memory), however, the maximum values will. 

** Note: Flashing a new program to the board will reset the flash memory to zero and the max values will be lost **

With `SMOOTHNESS` set to 3, the sample rate is ~1.6Hz. With the `RETAIN` parameter set to 2^11, this gives a total usage time of (1/1.6) x 2^11 = 21 minutes. The data is stored on a rolling basis with the "Loop index" indicating the end of the list. 


### ToDos

- Low-battery indictor
- Remove filtering for max_alt if inbuilt filter used
- Checks for valid data incase I2C connection becomes erroneous 
