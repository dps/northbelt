Notes
-----

adafruit buzzer draws:
  - 106mA at 5V ~= 50 Ohms
  - will not activate with series 220 Ohm resistor.
  - ATmega32U4 can handle 40mA output current per digital pin => 125 Ohm @ 5V -> series 75 Ohm required.  Choose 82 for +/- 5% safety.
   - non linear current response :-(

draws 60mA with series LED
