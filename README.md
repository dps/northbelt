northbelt
=========

A belt that buzzes north

This repo contains Arduino code for a wearable compass, consisting of a belt with a number of mobile phone vibration motors mounted along its circumference.  As the wearer moves around, the motor closest to magnetic north buzzes.  According to [this WIRED article](http://archive.wired.com/wired/archive/15.04/esp.html), through the miracle of [brain plasticity](http://en.wikipedia.org/wiki/Neuroplasticity), this will give the gift of an amazing sixth sense of direction.

Hardware
========

Prototype
---------
 * [A-Star 32U4 Mini ULV](http://www.pololu.com/product/3102)
 * [LSM303D 3D Compass and Accelerometer breakout](http://www.pololu.com/product/2127)
 * [LiPoly charger](http://www.adafruit.com/products/259)
 * [Lithium Ion Polymer Battery - 3.7v 500mAh](http://www.adafruit.com/products/1578)
 * [Vibrating Mini Motor Disc](http://www.adafruit.com/products/1201) x 16
 * Motor board x16
 * 20 way ribbon cable
 * [Belt](http://amzn.com/B001FOR2CY)

Pololu A-Star
-------------

[A* Pinout](http://www.pololu.com/file/0J784/a-star-32u4-mini-pinout.pdf)

    A*  <---> LS303
    -----------------------
    5V  <---> 5V
    GND <---> GND
    2   <---> SDA
    3   <---> SCL
    4   <---> 0 degree vibe
    5   <---> 22.5
    6   <---> 45
    7   <--->
    8   <---> 90
    9   <--->
    10  <---> 135
    11  <--->
    12  <---> 180
    13  <--->
    14  <---> 225
    15  <--->
    16  <---> 270
    17  <--->
    18  <---> 315
    19  <---> 337.5

