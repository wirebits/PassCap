# PassCap
It captures the wifi password by using evil twin attack.

# Key Features
- Simple Setup.<br>
- LED Status for Right and Wrong Passwords.<br>

# Supported Board
- It supports NodeMCU ESP8266 Boards.
- If possible, use those NodeMCU ESP8266 boards which contain `CP2102` driver chipset.

# Setup
1. Download Arduino IDE from <a href="https://www.arduino.cc/en/software">here</a> according to your Operating System.<br>
2. Install it.<br>
3. Go to `File` and then `Preferences`.
4. Go to `Additional Boards Manager URLs` and paste the following link :
   
   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
5. Click on `OK`.
6. Go to `Tools` then `Board` and then `Board Manager`.
7. Wait for sometimes and search `esp8266` by `ESP8266 Community`.
8. Simply install it.
9. Wait for sometime and after that it is installed.
10. Restart the Arduino IDE.
11. Done!

# Install and Run
1. Download or Clone the Repository.
2. Open the folder and just double click on `PassCap.ino` file.
3. It opens in Arduino IDE.
4. Compile the code.
5. Select the correct board from the `Tools` then `Board` and then `ESP8266 Boards`.
6. It is generally `NodeMCU 1.0 (ESP-12E Module)`.
7. Select the port number of that board.
8. Upload the code.
9. After 1-2 minutes, an Access Point is created named `PassCap` whose password is `wpasscap`.
10. Connect to it.
11. After few seconds, a page automatically opens where all nearby wifi are shown.
12. Select any wifi and click on `Start Capture` button.
13. After that, it disconnects the access point and as well as an open wifi with the select wifi is created.
14. Connect to that open wifi.
15. It show a page where it ask for password.
16. If password is right, then led of the board blink 3 times.
17. If password is wrong, then led of the board blink 2 times.
18. Connnct again to that `PassCap` access point.
19. At the bottom, it shows the password of the selected wifi.
