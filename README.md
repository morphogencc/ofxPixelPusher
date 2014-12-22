# ofxPixelPusher

c++ implementation of pixelpusher software using openframeworks

## Usage
`DiscoveryListener` is the entry point for this addon.  In your application, create a `DiscoveryListener` instance using
`DiscoveryListener::getInstance()`.  This creates a separate thread that looks for PixelPusher announcements, and
registers the PixelPushers with the Listener.

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
of `Pixel` objects directly to the `Strip` objects.

Alternatively, you can call `getStrips()` on a `PixelPusher` object to get a deque of strips that you can iterate
through, or `getStrip(int stripNumber)` to get a pointer to a particular strip.  Once you have a pointer to a `Strip`,
you can set the pixels with one of four methods:

- `setPixel(int position, unsigned char r, unsigned char g, unsigned char b)`
- `setPixel(int position, shared_ptr<Pixel> pixel)`
- `setPixels(unsigned char r, unsigned char g, unsigned char b)`
- `setPixels(std::vector<shared_ptr<Pixel> > p)`

Each `PixelPusher` object automatically creates its own `CardThread` object manages sending data to the PixelPusher on
its own thread.  As long as you update the strips to reflect current data, everything else should run itself!

## Useful Abstractions

## Examples

