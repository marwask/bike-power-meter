Bicycle power meter based on Arduino platform with Bluetooth Low Energy support.

# Specifications
- it supports various bike computers, sport watches and smartphone applications with Bluetooth LE and power meter support
- ~10 hours operation time with 220mAh battery (can be optimized)
- strain gauges mounted on left crank

# Hardware
- Adafruit Feather Nrf52832 - module based on nRF52832 system with Bluetooth Low Energy and rechargeable LiPol battery support
- SparkFun MPU-9250 9DoF - 3-axis accelometer, gyroscope and magnetometer
- HX711 (red) - operational amplifier to handle strain beams
- strain gauges - 120 ohm version to measure forces
- 3.7V LiPol rechargeable battery

There are two different versions of HX711 module - a green and a red one. Both can operate with 10 and 80Hz update frequency, but the green has 10Hz and the red has 80Hz by default. Furthermore, red version has a shield to prevent interference from the environment. 

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/hx711.jpg" height="120" />

The green and the red version have also slightly different pins layout. During testing it turned out that the red version has much more stable measurements.

# Schematics
Connections between HX711 and strain gauges have to be soldered continuously without any "weak wires". HX711 module is very sensitive for any noises and every not so solid connection can add some errors in measurements. 

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/wiring.png" height="400" />
Wheatstone bridge on figure will be corrected.

It is recommended to have a detachable main part, then the best way is to connect it with four-wire connector similar to Groove.

# Assembly
Before soldering strain gauges, the place on the crank must be cleaned off paint. This should be done with sandpapers with an increasingly higher value. Sticking strain gauges omitting this step will result in much less sensitivity.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/1.jpg" height="200" />

It is time to glue strain gauges after clearing place by IPA. The best way to do this is to use colorless an adhesive tape and an instant glue (cyanoacrylate).

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/2.jpg" height="200" />

When strain gauge has been glued then slowly peel off the tape.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/3b.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/3.jpg" height="200" />

Strain gauges have to work in Wheatstone bridge layout, to measure values by operational amplifier HX711. In this case half bridge layout with temperature compensation has been implemented.

img

Two strain gauges are connected as shown on the figure above. Due to temperature variation, both of them should have resistance change in the same way. Resistors have equal resistance and strain gauges also have to be identical to each other.

Resistors, wires to strain gauges and a connector to the main part have been placed on an additional PCB. This board is not detachable.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/4.jpg" height="200" />

Base parts can be assembled on a dedicated Adafruit Feather PCB.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/5.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/6.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/7.jpg" height="200" />

After placing an Adafruit Feather Nrf52832 and a PCB on each other the main part has quite a small size. A battery should be placed on a crank and connected to the base module.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/8.jpg" height="200" />

Unfortunately, in this case, the whole base module is very sensitive to dust and dirt. The best way to prevent it from environmental factors is to create and print a dedicated case. 

The first version of the integrated base module (with 250 mAh battery) has dimensions 101x30x10mm

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/9.jpg" height="200" />

The current version has more optimized dimensions 72x33x11mm but with a slightly smaller battery - 220mAh.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/15.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/14.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/13.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/11.jpg" height="200" />
<img src="https://github.com/marwask/bike-power-meter/blob/master/images/12.jpg" height="200" />

The final version has to have a cover for strain gauges and for an additional printed circuit board.

# Calibration
There are two different stages of calibration. The first is to set a conversion factor from scale points to kg, the second is to set current offset to measure force. The initial calibration must be carried out before the first use. In "config.h" file is “initialConfiguration” variable. When is to “true”, then the power meter is in the initial configuration stage and set “knownWeight” variable in next variable. Probably the best way to properly measure “scaleValueToKgEquivalent” value is to hang a heavy thing (with a known weight) on a crank in horizontal position and then read the value from logs. It is obvious that a heavier thing can give more accurate factor. The second calibration step is always performed after the power meter is started. Each time, for the first 30 seconds, do not touch the crank for the calibration to be succeeded. During this process a red led is blinking.

# Compatibility
The power meter should work with any bike computer, sport watch or smartphone application with Bluetooth Low Energy and bike power meter support. The power meter was tested with:
- Polar V650
- Wahoo Elemnt Bolt
- Garmin Fenix 5
- Trainer Road (Android application)
- Ride With Gps (Android application)
- Wahoo Fitness (Android application)
- The Sufferfest (Windows application)

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/10.jpg" height="200" />

# Known Issues
Strain gauges with HX711 are very sensitive for temperature changes. Their behavior is similar to thermometer and during some time overall offset should be corrected or recalculated. Unfortunately there isn't obvious correlation and we couldn't increase or decrease offset with exact temperature change.

# Tests
According to stationary tests based on transmission over USB cable there is around 70-80Hz sampling rate. Debug transmission over Bluetooth is slower and has around 40Hz. In order to analyze and debug live data, there is additional software which allows recording and showing results. These python scripts can operate on any Linux based machine (computer, Raspberry Pi) with Bluetooth LE support.

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/live_data.png" height="200" />

After recording the session, another script can create charts with various information:
- crank angle
- measured weight (which allows calculating force and power)
- cadence
- calculated power
- calculation range (between vertical gray lines)

<img src="https://github.com/marwask/bike-power-meter/blob/master/images/training_chart.png" height="400" />

Currently cadence and power values, in order to increase stability, come from two crank rounds.

In normal usage 220mAh LiPol battery lasts in around 10 hours.

# Accuracy
Unfortunately, the only way to measure accuracy is to make direct comparison with a real power meter. I currently only have a Elite Turbo Muin trainer with power meter support, but its measurements are not so accurate.

# Todo
- dealing with huge strain gauges drift
- autozero / autocalibration
- manual calibration from bike computer / sport watch
- battery optimization
- ANT+ support
