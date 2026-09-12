#pragma once

#include <cmath>
#include <algorithm>

#ifndef PI
#define PI 3.14159265358979323846
#endif

class CGlowingButtonInternal
{
public:
	CGlowingButtonInternal() = default;
	~CGlowingButtonInternal() = default;

	CGlowingButtonInternal(const CGlowingButtonInternal&) = delete;
	CGlowingButtonInternal& operator=(const CGlowingButtonInternal&) = delete;
	CGlowingButtonInternal(CGlowingButtonInternal&&) = delete;
	CGlowingButtonInternal& operator=(CGlowingButtonInternal&&) = delete;

protected:
	COLORREF HSVtoRGB(double h, double s, double v)
	{
		h = fmod(h, 360.0);
		if (h < 0.0) h += 360.0;

		double c = v * s;
		double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
		double m = v - c;
		double r = 0, g = 0, b = 0;

		if      (h < 60)  { r = c; g = x; }
		else if (h < 120) { r = x; g = c; }
		else if (h < 180) { g = c; b = x; }
		else if (h < 240) { g = x; b = c; }
		else if (h < 300) { r = x; b = c; }
		else              { r = c; b = x; }

		return RGB(
			static_cast<BYTE>((r + m) * 255.0),
			static_cast<BYTE>((g + m) * 255.0),
			static_cast<BYTE>((b + m) * 255.0));
	}

	COLORREF InternalLerpColor(COLORREF c1, COLORREF c2, double t)
	{
		BYTE r = static_cast<BYTE>(GetRValue(c1) + (GetRValue(c2) - GetRValue(c1)) * t);
		BYTE g = static_cast<BYTE>(GetGValue(c1) + (GetGValue(c2) - GetGValue(c1)) * t);
		BYTE b = static_cast<BYTE>(GetBValue(c1) + (GetBValue(c2) - GetBValue(c1)) * t);
		return RGB(r, g, b);
	}

	void DrawRainbowRoundedRect(HDC hdc, int left, int top, int right, int bottom,
		int radius, double offset, int thickness = 1,
		bool bOverride = false, COLORREF overrideClr = RGB(255, 0, 0), BYTE alpha = 255)
	{
		const int width  = right - left;
		const int height = bottom - top;
		if (width <= 0 || height <= 0) return;

		if (radius <= 0)
		{
			// Simple rectangular border path
			const double peri = 2.0 * (width + height);
			const int steps = static_cast<int>(peri * 2) + 2;
			for (int i = 0; i <= steps; ++i)
			{
				const double t = static_cast<double>(i) / steps;
				double rem = t * peri;
				int x = 0, y = 0;

				if (rem <= width) { x = left + static_cast<int>(rem); y = top; }
				else {
					rem -= width;
					if (rem <= height) { x = right - 1; y = top + static_cast<int>(rem); }
					else {
						rem -= height;
						if (rem <= width) { x = right - 1 - static_cast<int>(rem); y = bottom - 1; }
						else { rem -= width; x = left; y = bottom - 1 - static_cast<int>(rem); }
					}
				}

				x = std::clamp(x, left, right - 1);
				y = std::clamp(y, top, bottom - 1);

				const double hue = fmod(t * 360.0 + offset, 360.0);
				const COLORREF color = bOverride ? overrideClr : HSVtoRGB(hue, 1.0, 1.0);
				SetPixelV(hdc, x, y, color);
			}
			return;
		}

		// Clamp radius
		radius = (std::min)(radius, (std::min)(width, height) / 2);

		const double straightW = width  - 2.0 * radius;
		const double straightH = height - 2.0 * radius;
		const double peri = 2.0 * straightW + 2.0 * straightH + 2.0 * PI * radius;
		if (peri <= 0.0) return;

		const int steps = static_cast<int>(peri * 2.0) + 4;

		for (int i = 0; i <= steps; ++i)
		{
			const double t = static_cast<double>(i) / steps;
			double rem = t * peri;
			int x = 0, y = 0;

			if (rem <= straightW)
			{
				x = left + radius + static_cast<int>(rem + 0.5);
				y = top;
			}
			else
			{
				rem -= straightW;
				if (rem <= 0.5 * PI * radius)
				{
					const double a = rem / radius;
					x = right - radius + static_cast<int>(radius * sin(a) + 0.5);
					y = top + radius - static_cast<int>(radius * cos(a) + 0.5);
				}
				else
				{
					rem -= 0.5 * PI * radius;
					if (rem <= straightH)
					{
						x = right;
						y = top + radius + static_cast<int>(rem + 0.5);
					}
					else
					{
						rem -= straightH;
						if (rem <= 0.5 * PI * radius)
						{
							const double a = rem / radius;
							x = right - radius + static_cast<int>(radius * cos(a) + 0.5);
							y = bottom - radius + static_cast<int>(radius * sin(a) + 0.5);
						}
						else
						{
							rem -= 0.5 * PI * radius;
							if (rem <= straightW)
							{
								x = right - radius - static_cast<int>(rem + 0.5);
								y = bottom;
							}
							else
							{
								rem -= straightW;
								if (rem <= 0.5 * PI * radius)
								{
									const double a = rem / radius;
									x = left + radius - static_cast<int>(radius * sin(a) + 0.5);
									y = bottom - radius + static_cast<int>(radius * cos(a) + 0.5);
								}
								else
								{
									rem -= 0.5 * PI * radius;
									if (rem <= straightH)
									{
										x = left;
										y = bottom - radius - static_cast<int>(rem + 0.5);
									}
									else
									{
										rem -= straightH;
										const double a = rem / radius;
										x = left + radius - static_cast<int>(radius * cos(a) + 0.5);
										y = top + radius - static_cast<int>(radius * sin(a) + 0.5);
									}
								}
							}
						}
					}
				}
			}

			x = std::clamp(x, left, right - 1);
			y = std::clamp(y, top, bottom - 1);

			const double hue = fmod(t * 360.0 + offset, 360.0);
			const COLORREF color = bOverride ? overrideClr : HSVtoRGB(hue, 1.0, 1.0);

			for (int dx = -thickness / 2; dx <= thickness / 2; ++dx)
				for (int dy = -thickness / 2; dy <= thickness / 2; ++dy)
					SetPixelV(hdc, x + dx, y + dy, color);
		}
	}
};
