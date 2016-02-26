#pragma once

#include <algorithm>

namespace ofxPixelPusher {

	struct ColorHSB {
		float mHue; // hue from 0 - 360
		float mSaturation; // 0 - 1
		float mBrightness; // 0 - 1
	};

	struct ColorRGB {
		unsigned char mRed;
		unsigned char mGreen;
		unsigned char mBlue;
	};

	static ColorHSB rgb2hsb(ColorRGB inputColor)
	{
		ColorHSB outputColor;
		double min, max, delta;

		min = inputColor.mRed < inputColor.mGreen ? inputColor.mRed : inputColor.mGreen;
		min = min  < inputColor.mBlue ? min : inputColor.mBlue;

		max = inputColor.mRed > inputColor.mGreen ? inputColor.mRed : inputColor.mGreen;
		max = max > inputColor.mBlue ? max : inputColor.mBlue;

		outputColor.mBrightness = max;
		delta = max - min;
		if (delta < 0.00001)
		{
			outputColor.mSaturation = 0;
			outputColor.mHue = 0;
			return outputColor;
		}
		if (max > 0.0) {
			outputColor.mSaturation = (delta / max);
		}
		else {
			outputColor.mSaturation = 0.0;
			outputColor.mHue = NAN;
			return outputColor;
		}
		if (inputColor.mRed >= max)
			outputColor.mHue = (inputColor.mGreen - inputColor.mBlue) / delta;
		else
			if (inputColor.mGreen >= max)
				outputColor.mHue = 2.0 + (inputColor.mBlue - inputColor.mRed) / delta;
			else
				outputColor.mHue = 4.0 + (inputColor.mRed - inputColor.mGreen) / delta;

		outputColor.mHue *= 60.0;

		if (outputColor.mHue < 0.0)
			outputColor.mHue += 360.0;

		return outputColor;
	}

	static ColorHSB rgb2hsb(unsigned char red, unsigned char green, unsigned char blue) {
		ColorRGB inputColor;
		inputColor.mRed = red;
		inputColor.mGreen = green;
		inputColor.mBlue = blue;

		return rgb2hsb(inputColor);
	}

	static ColorRGB hsb2rgb(ColorHSB inputColor)
	{
		double hh, p, q, t, ff;
		long i;
		ColorRGB outputColor;

		if (inputColor.mSaturation <= 0.0) {
			outputColor.mRed = 255.0*inputColor.mBrightness;
			outputColor.mGreen = 255.0*inputColor.mBrightness;
			outputColor.mBlue = 255.0*inputColor.mBrightness;
			return outputColor;
		}

		hh = inputColor.mHue;
		if (hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = 255.0*inputColor.mBrightness * (1.0 - inputColor.mSaturation);
		q = 255.0*inputColor.mBrightness * (1.0 - (inputColor.mSaturation * ff));
		t = 255.0*inputColor.mBrightness * (1.0 - (inputColor.mSaturation * (1.0 - ff)));

		switch (i) {
		case 0:
			outputColor.mRed = 255.0*inputColor.mBrightness;
			outputColor.mGreen = t;
			outputColor.mBlue = p;
			break;
		case 1:
			outputColor.mRed = q;
			outputColor.mGreen = 255.0*inputColor.mBrightness;
			outputColor.mBlue = p;
			break;
		case 2:
			outputColor.mRed = p;
			outputColor.mGreen = 255.0*inputColor.mBrightness;
			outputColor.mBlue = t;
			break;

		case 3:
			outputColor.mRed = p;
			outputColor.mGreen = q;
			outputColor.mBlue = 255.0*inputColor.mBrightness;
			break;
		case 4:
			outputColor.mRed = t;
			outputColor.mGreen = p;
			outputColor.mBlue = 255.0*inputColor.mBrightness;
			break;
		case 5:
		default:
			outputColor.mRed = 255.0*inputColor.mBrightness;
			outputColor.mGreen = p;
			outputColor.mBlue = q;
			break;
		}

		return outputColor;
	}

	static ColorRGB hsb2rgb(float hue, float saturation, float brightness) {
		ColorHSB inputColor;
		inputColor.mHue = hue;
		inputColor.mSaturation = saturation;
		inputColor.mBrightness = brightness;

		return hsb2rgb(inputColor);
	}

	static float clamp(float n, float lower, float upper) {
		return std::max(lower, std::min(n, upper));
	}



}