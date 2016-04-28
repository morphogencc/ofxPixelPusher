/*
 * Pixel
 * Dec 2014
 * nathan lachenmyer
 * http://morphogen.cc
 */

#pragma once

namespace ofxPixelPusher {

	//forward declare Strip so I don't get a Pixel<->Strip interdependency
	class Strip; 

	class Pixel {
	public:
		friend class Strip;

		enum ColorCorrection {
			UNCORRECTED = 0xFFFFFF,
			SMD5050 = 0xFFB0F0,
			LED_STRIP = 0xFFB0F0,
			PIXEL_STRING = 0xFFE08C
		};

		enum ColorTemperature {
			UNCORRECTED_TEMPERATURE = 0xFFFFFF,
			CANDLE = 0xFF9329,
			HALOGEN = 0xFFF1E0,
			OVERCAST = 0xC9E2FF,
			CLEAR_SKY = 0x409CFF,
			FLUORESCENT = 0xF4FFFA,
			WARM_FLUORESCENT = 0xFFF4E5,
			COOL_FLUORESCENT = 0xD4EBFF,
			GROWLIGHT = 0xFFEFF7,
			BLACKLIGHT = 0xA700FF
		};

		Pixel();
		Pixel(unsigned char r, unsigned char g, unsigned char b);
		Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
		~Pixel();
		void setColor(unsigned char r, unsigned char g, unsigned char b);
		void setColorHSB(float hue, float saturation, float brightness);
		void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char o, unsigned char w);
		void setColor(Pixel pixel);
		void setAntiLog(bool useAntiLog);
		void setManualCorrection(float red, float green, float blue);
		void setColorTemperature(ColorTemperature temperature);
		void setColorCorrection(ColorCorrection correction);
	protected:
		void calculateCorrection();
		ColorTemperature mColorTemperature;
		ColorCorrection mColorCorrection;
		bool mUseAntiLog;
		float mCorrection[3];
		unsigned char mRed;
		unsigned char mGreen;
		unsigned char mBlue;
		unsigned char mOrange;
		unsigned char mWhite;
		static unsigned char mLinearExp[256];

	};

}