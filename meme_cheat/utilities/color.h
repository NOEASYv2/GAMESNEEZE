#pragma once

#define NOCOLOR CColor(0, 0, 0, 0)
#define WHITE CColor(255, 255, 255, 255)
#define BLACK CColor(0, 0, 0, 255)
#define RED CColor(255, 0, 0, 255)
#define LIGHTRED CColor(255, 100, 100, 255)
#define DARKRED CColor(120, 0, 0, 255)
#define GREEN CColor(0, 255, 0, 255)
#define BLUE CColor(0, 0, 255, 255)
#define DARKGREY CColor(55, 55, 55, 255)
#define GREY CColor(70, 70, 70, 255)
#define LIGHTGREY CColor(150, 150, 150, 255)
#define HOTPINK CColor(255,20,147, 255)
#define CYAN CColor(0,255,255, 255)
#define YELLOW CColor(255, 255, 0)


class CColor
{
public:
	unsigned char RGBA[4];

	CColor()
	{
		RGBA[0] = 255;
		RGBA[1] = 255;
		RGBA[2] = 255;
		RGBA[3] = 255;
	}
	CColor(int r, int g, int b, int a = 255)
	{
		RGBA[0] = r;
		RGBA[1] = g;
		RGBA[2] = b;
		RGBA[3] = a;
	}


	bool operator!=(CColor color) { return RGBA[0] != color.RGBA[0] || RGBA[1] != color.RGBA[1] || RGBA[2] != color.RGBA[2] || RGBA[3] != color.RGBA[3]; }
	bool operator==(CColor color) { return RGBA[0] == color.RGBA[0] && RGBA[1] == color.RGBA[1] && RGBA[2] == color.RGBA[2] && RGBA[3] == color.RGBA[3]; }


	// returns the color from 0.f - 1.f
	static float Base(const unsigned char col)
	{
		return col / 255.f;
	}
	static CColor Inverse(const CColor color)
	{
		return CColor(255 - color.RGBA[0], 255 - color.RGBA[1], 255 - color.RGBA[2]);
	}
	float Difference(const CColor color) const // from 0.f - 1.f with 1.f being the most different
	{
		float red_diff = fabs(Base(RGBA[0]) - Base(color.RGBA[0]));
		float green_diff = fabs(Base(RGBA[1]) - Base(color.RGBA[1]));
		float blue_diff = fabs(Base(RGBA[2]) - Base(color.RGBA[2]));
		float alpha_diff = fabs(Base(RGBA[3]) - Base(color.RGBA[3]));

		return (red_diff + green_diff + blue_diff + alpha_diff) * 0.25f;
	}

	// RGB -> HSB
	static float Hue(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		float mx = max(R, max(G, B));
		float mn = min(R, min(G, B));
		if (mx == mn)
			return 0.f;

		float delta = mx - mn;

		float hue = 0.f;
		if (mx == R)
			hue = (G - B) / delta;
		else if (mx == G)
			hue = 2.f + (B - R) / delta;
		else
			hue = 4.f + (R - G) / delta;

		hue *= 60.f;
		if (hue < 0.f)
			hue += 360.f;

		return hue / 360.f;
	}
	static float Saturation(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		float mx = max(R, max(G, B));
		float mn = min(R, min(G, B));

		float delta = mx - mn;

		if (mx == 0.f)
			return delta;

		return delta / mx;
	}
	static float Brightness(const CColor color)
	{
		float R = Base(color.RGBA[0]);
		float G = Base(color.RGBA[1]);
		float B = Base(color.RGBA[2]);

		return max(R, max(G, B));
	}

	float Hue() const
	{
		return Hue(*this);
	}
	float Saturation() const
	{
		return Saturation(*this);
	}
	float Brightness() const
	{
		return Brightness(*this);
	}

	// HSB -> RGB
	static CColor HSBtoRGB(float hue /* 0.f - 1.f*/,
		float saturation /* 0.f - 1.f */,
		float brightness /* 0.f - 1.f */,
		int alpha = 255)
	{
		hue = UTILS::clamp(hue, 0.f, 1.f);
		saturation = UTILS::clamp(saturation, 0.f, 1.f);
		brightness = UTILS::clamp(brightness, 0.f, 1.f);

		float h = (hue == 1.f) ? 0.f : (hue * 6.f);
		float f = h - static_cast<int>(h);
		float p = brightness * (1.f - saturation);
		float q = brightness * (1.f - saturation * f);
		float t = brightness * (1.f - (saturation * (1.f - f)));

		if (h < 1.f)
			return CColor(brightness * 255, t * 255, p * 255, alpha);
		else if (h < 2.f)
			return CColor(q * 255, brightness * 255, p * 255, alpha);
		else if (h < 3.f)
			return CColor(p * 255, brightness * 255, t * 255, alpha);
		else if (h < 4)
			return CColor(p * 255, q * 255, brightness * 255, alpha);
		else if (h < 5)
			return CColor(t * 255, p * 255, brightness * 255, alpha);
		else
			return CColor(brightness * 255, p * 255, q * 255, alpha);

	}

	/*
	static CColor WHITE() { return CColor(255, 255, 255); }
	static CColor BLACK() { return CColor(0, 0, 0); }
	static CColor RED() { return CColor(255, 0, 0); }
	static CColor GREEN() { return CColor(0, 255, 0); }
	static CColor BLUE() { return CColor(0, 0, 255); }
	*/
};
/*
class CColor
{
public:

	unsigned char RGBA[4];

	CColor()
	{
		RGBA[0] = (unsigned char)255;
		RGBA[1] = (unsigned char)255;
		RGBA[2] = (unsigned char)255;
		RGBA[3] = (unsigned char)255;
	}
	CColor(int R, int G, int B, int A)
	{
		RGBA[0] = (unsigned char)R;
		RGBA[1] = (unsigned char)G;
		RGBA[2] = (unsigned char)B;
		RGBA[3] = (unsigned char)A;
	}
	CColor(int R, int G, int B)
	{
		RGBA[0] = (unsigned char)R;
		RGBA[1] = (unsigned char)G;
		RGBA[2] = (unsigned char)B;
		RGBA[3] = (unsigned char)255;
	}

	bool operator==(const CColor& c) { return (c.RGBA[0] == RGBA[0] && c.RGBA[1] == RGBA[1] && c.RGBA[2] == RGBA[2]); }
	bool operator!=(const CColor& c) { return (c.RGBA[0] != RGBA[0] || c.RGBA[1] != RGBA[1] || c.RGBA[2] != RGBA[2]); }

	float* Base()
	{
		float tmp[4];
		for (int i = 0; i < 4; i++)
		{
			tmp[i] = ToFColor(RGBA[i]);
		}

		return tmp;
	}
	float ToFColor(float i)
	{
		if (i != 0)
		{
			return i / 255;
		}
		else
		{
			return 0;
		}
	}
	CColor inverse()
	{
		return CColor(255 - RGBA[0], 255 - RGBA[1], 255 - RGBA[2], RGBA[3]);
	}

	static CColor HSBtoRGB(float hue, float saturation, float brightness, int alpha)
	{
		hue = UTILS::clamp(hue, 0.f, 1.f);
		saturation = UTILS::clamp(saturation, 0.f, 1.f);
		brightness = UTILS::clamp(brightness, 0.f, 1.f);

		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return CColor{ (int)(brightness * 255), (int)(t * 255), (int)(p * 255), alpha };
		}
		else if (h < 2)
		{
			return CColor{ (int)(q * 255), (int)(brightness * 255), (int)(p * 255), alpha };
		}
		else if (h < 3)
		{
			return CColor{ (int)(p * 255), (int)(brightness * 255), (int)(t * 255), alpha };
		}
		else if (h < 4)
		{
			return CColor{ (int)(p * 255), (int)(q * 255), (int)(brightness * 255), alpha };
		}
		else if (h < 5)
		{
			return CColor{ (int)(t * 255), (int)(p * 255), (int)(brightness * 255), alpha };
		}
		else
		{
			return CColor{ (int)(brightness * 255), (int)(p * 255), (int)(q * 255), alpha };
		}
	}

	float Hue() const
	{
		if (RGBA[0] == RGBA[1] && RGBA[1] == RGBA[2] && RGBA[0] == RGBA[2])
			return 0.f;

		float R = RGBA[0] / 255.f;
		float G = RGBA[1] / 255.f;
		float B = RGBA[2] / 255.f;

		float max = max(B, max(R, G));
		float min = min(B, min(R, G));
		float delta = max - min;

		float hue = 0.f;
		if (max == R)
			hue = (G - B) / delta;
		else if (max == G)
			hue = 2.f + (B - R) / delta;
		else if (max == B)
			hue = 4.f + (R - G) / delta;

		hue *= 60.f;
		if (hue < 0.f)
			hue += 360.f;

		return hue / 360.f;
	}
	float Saturation() const
	{
		float R = RGBA[0] / 255.f;
		float G = RGBA[1] / 255.f;
		float B = RGBA[2] / 255.f;

		float max = max(B, max(R, G));
		float min = min(B, min(R, G));
		float delta = max - min;

		if (!max)
			return 0.f;

		return delta / max;
	}
	float Brightness() const
	{
		float R = RGBA[0] / 255.f;
		float G = RGBA[1] / 255.f;
		float B = RGBA[2] / 255.f;

		float max = max(B, max(R, G));
		float min = min(B, min(R, G));
		float delta = max - min;

		return max;
	}


private:
};
*/