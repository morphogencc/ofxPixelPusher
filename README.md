# ofxPixelPusher

A C++ implementation of pixelpusher software.

## Requirements
This add-on requires [ofxAsio](http://github.com/morphogencc/ofxAsio.git) for networking.  See the repository for how to setup `ofxAsio`.

This implementation of the pixelpusher protocol attempts to be as framework-agnostic as possible; thus, it tries to avoid using openFrameworks-specific classes such as `ofLog` and `ofThread`, instead using the c++ equivalents.  `ofxAsio` follows a similar paradigm, so the entire project should be portable to any other C++ framework.

## Usage
In your application, create a `std::shared_ptr<DiscoveryService>` instance using `DiscoveryService::getInstance()`.  This creates a
separate thread that looks for PixelPusher announcements, and registers the PixelPushers with the Service.

Once a PixelPusher is detected, you can grab one with any of the three following methods:

- `DiscoveryService::getPushers()`
- `DiscoveryService::getGroup(long groupId)`
- `DiscoveryServiceit::getController(long groupId, long controllerId)`

The first two methods return a vector of shared pointers (`std::vector<shared_ptr<PixelPusher> >`), while the last one
returns either an empty pointer or a pointer to a PixelPusher.

Once a PixelPusher is selected, strips can be assigned values with
- `setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue)`
- `setStripValues(int stripNumber, std::vector<shared_ptr<Pixel> > pixels)`

The first method sets all pixels in the strip `stripNumber` to the same color, while the latter method copies the vector
of `Pixel` objects directly to the `Strip` objects.  See the header file `Strip.h` for a more exhaustive listing of ways to set pixels.

Alternatively, you can call `getStrips()` on a `PixelPusher` object to get a deque of strips that you can iterate
through, or `getStrip(int stripNumber)` to get a pointer to a particular strip.  Once you have a pointer to a `Strip`,
you can set the pixels with a variety of methods -- look at the header file to see a complete listing.

Each `PixelPusher` object automatically creates its own `CardThread` that manages sending data to the PixelPusher on
its own thread.  As long as you update the strips to reflect current data, everything else should run itself!

## Options
There are a variety of options that can be used to adjust the LEDs.  Currently implemented are:

* `PowerScale` -- this controls the overall brightness of the LEDs; use a value between 0 and 1.
* `ColorCorrection` -- this corrects the color ratios so that the tuple `(255, 255, 255)` produces a "true" white pixel. Look at `Pixel.h` for a full listing of options.
* `ColorTemperature` -- this further corrects the colors to reflect a specific light source.  Look at `Pixel.h` for a full listing.
* `Tint` -- you can also "tint" a strip by using the corresponding function call.  This effectively multiples the assigned color (set via `setPixels()`) by a constant tint -- this allows you to either manually correct the color to a setting non available with the presets, or allow for some neat tricks in animating the strips.


## Examples

* `example-colorFade` -- sets all known pixels to the same color, slowly cycling through hues.
* `example-colorChase` -- sets all strips to a classic rainbow chase sequence.
* `example-tileMapping` -- shows how to use map strips using textures, using a TwentySquared tile as an example.

## Contributions
This project uses the [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/) paradigm.  Before contributing, please make your own feature branch with your changes.

This project was originally developed at [Small Design Firm](http://www.smalldesignfirm.com/), before being open sourced on Github.

## More Information / Credits
For more info about PixelPusher visit:
- [Heroic Robotics](http://www.heroicrobotics.com)
- [PixelPusher Documentation](https://sites.google.com/a/heroicrobot.com/pixelpusher/home)

To purchase a PixelPusher (and BetterPixel products), go to:
- [Illumination Supply](http://www.illuminationsupply.com/)

Color Correction and Color Temperature presets were lifted straight from the [FastLED](https://github.com/FastLED/FastLED/) library.