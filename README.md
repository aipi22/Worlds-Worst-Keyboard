# Worlds-Worst-Keyboard
This is a keyboard that you definitely do not want to use. It features a limited key selection, difficult to navigate user interface and a very high operation cost.

![IMG_2240](https://github.com/user-attachments/assets/7569cf3e-cf19-479f-b1c5-a4cf661260fb)

**You will need:**
  1. HID capable Arduino
  2. An RFID Scanner
  3. Some buttons
  4. A Peizo buzzer
More information in requirements.txt

See schematic.png for a connection diagram. This project uses the SPI pins (MISO, MOSI, and SCK) on the Arduino to communicate with the RFID sensor and then the I2C pins (SDA, SCL) to connect with the I2C LCD Display and driver. I/O for the RFID sensor and for the buttons and buzzer can be filled in anywhere, but look at the schematic for how it's set up in the code.  

See keyboard.ino for Arduino code. Essentially, the keyboard is made to be as difficult as possible.  

See Apple Pay.stl for a fun 3D print that turns your ordinary key tag style scanner into "Apple Pay"  

# SPOILERS  
The first card you try to scan always declines. The second card will let you through. Then, the number of dollars you spend will be the number of characters you can type later. With the tip, if you give 0%, the system will refuse to help you. If you end up spending more than $110, the system will tell you that your card has declined this purchase. After that, if you type more characters than you purchased (excluding tip) then you will find the device getting mad and deleting the last 1000 characters you typed.  

This is a joke project that does not use real money.
