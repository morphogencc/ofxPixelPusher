# ofxPixelPusher

A C++ implementation of pixelpusher software.

## Requirements
This add-on requires [ofxAsio](http://github.com/morphogencc/ofxAsio.git) for networking.  See the repository for how to setup `ofxAsio`.

This implementation of the pixelpusher protocol attempts to be as framework-agnostic as possible; thus, it tries to avoid using openFrameworks-specific classes such as `ofLog` and `ofThread`, instead using the c++ equivalents.  `ofxAsio` follows a similar paradigm, so the entire project should be portable to any other C++ framework.

## Usage
In your application, create a `std::shared_ptr<DiscoveryListener>` instance using `DiscoveryListener::getInstance()`.  This creates a
separate thread that looks for PixelPusher announcements, and registers the PixelPushers with the Listener.

Once a PixelPusher is detected, you can grab one with any of the three following methods:

- `DiscoveryListener::getPushers()`
- `DiscoveryListener::getGroup(long groupId)`
- `DiscoveryListener::getController(long groupId, long controllerId)`

The first two methods return a vector of shared pointers (`std::vector<shared_ptr<PixelPusher> >`), while the last one
returns either an empty pointer or a pointer to a PixelPusher.

Once a PixelPusher is selected, strips can be assigned values with
- `setStripValues(int stripNumber, unsigned char red, unsigned char green, unsigned char blue)`
- `setStripValues(int stripNumber, std::vector<shared_ptr<Pixel> > pixels)`

The first method sets all pixels in the strip `stripNumber` to the same color, while the latter method copies the vector
of `Pixel` objects directly to the `Strip` objects.  See the header file `Strip.h` for a more exhaustive listing of ways to set pixels.

Alternatively, you can call `getStrips()` on a `PixelPusher` object to get a deque of strips that you can iterate
through, or `getStrip(int stripNumber)` to get a pointer to a particular strip.  Once you have a pointer to a `Strip`,
you can set the pixels with one of four methods:

- `setPixel(int position, unsigned char r, unsigned char g, unsigned char b)`
- `setPixel(int position, shared_ptr<Pixel> pixel)`
- `setPixels(unsigned char r, unsigned char g, unsigned char b)`
- `setPixels(std::vector<shared_ptr<Pixel> > p)`

Each `PixelPusher` object automatically creates its own `CardThread` object manages sending data to the PixelPusher on
its own thread.  As long as you update the strips to reflect current data, everything else should run itself!

## Examples

* `example-colorFade`

## Contributions
This project uses the [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/) paradigm.  Before contributing, please make your own feature branch with your changes.

This project was originally developed at [Small Design Firm](http://www.smalldesignfirm.com/), before being open sourced on Github.

## More Information
For more info about PixelPusher visit:
- [Heroic Robotics](http://www.heroicrobotics.com)
- [PixelPusher Documentation](https://sites.google.com/a/heroicrobot.com/pixelpusher/home)

To purchase a PixelPusher (and BetterPixel products), go to:
- [Illumination Supply](http://www.illuminationsupply.com/)
