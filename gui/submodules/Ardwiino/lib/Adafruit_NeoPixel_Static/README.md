# MODIFIED NeoPixel Library - removed use of digitalWrite/pinMode and all dynamic memory allocation. 

Adafruit_NeoPixel(uint16_t n, uint8_t p, neoPixelType t, int *pixels)

pixels must be a pointer to a uint8_t array big enough to fit all the pixels; bounds checking is not performed, that's your job. Example:

```

uint8_t pixels[6]; //buffer - 6 bytes = 2 LEDs * 3 color channels
pinMode(14,OUTPUT); //this throws away the advantage of eliminating pinMode - you might want to replace this with directly setting the DDRx register. 
Adafruit_NeoPixel leds = Adafruit_NeoPixel(2, 14, NEO_GRB + NEO_KHZ800, pixels);

```

Changes from basic neopixel library:
* Anything not-AVR has not been tested and may or may not work. 
* There is no begin() call. All it did was set an internal boolean to true once pinMode() was removed, ie, it just wasted flash. 
* You cannot change the length of the strip, or the type of neopixels, or the pin, at runtime. 
* You must create an uint8_t array to hold the buffer, and pass it to the constructor. 
* You must supply all four arguments to the constructor. 
* You must set the pin mode to OUTPUT (if you're trying to save flash by getting rid of pinMode, do it by setting the DDR registers, which you'd a;ready be doing)
* 400khz neopixels are not supported. 

By providing the option to eliminate calls to pinMode() from your sketch entirely, you can save the 250 or so bytes of overhead that pinMode() involves by replacing all pinMode() calls with direct port manipulation. Since it is not uncommon to not change the mode of any pins after startup, in these cases all calls to pinMode() can be replaced with a few writes to the DDR registers. 

By making the sketch pass a pointer to a statically allocated array, we can dispense with the use of all dynamic memory allocation within the library. This saves like 600-700 bytes of flash, assuming you haven't used malloc() and associated functions elsewhere in your code. 

Dropping 400khz neopixels saves about 100 bytes. 

# Adafruit NeoPixel Library

Arduino library for controlling single-wire-based LED pixels and strip such as the [Adafruit 60 LED/meter Digital LED strip][strip], the [Adafruit FLORA RGB Smart Pixel][flora], the [Adafruit Breadboard-friendly RGB Smart Pixel][pixel], the [Adafruit NeoPixel Stick][stick], and the [Adafruit NeoPixel Shield][shield].

After downloading, rename folder to 'Adafruit_NeoPixel' and install in Arduino Libraries folder. Restart Arduino IDE, then open File->Sketchbook->Library->Adafruit_NeoPixel->strandtest sketch.

[flora]:  http://adafruit.com/products/1060
[strip]:  http://adafruit.com/products/1138
[pixel]:  http://adafruit.com/products/1312
[stick]:  http://adafruit.com/products/1426
[shield]: http://adafruit.com/products/1430
