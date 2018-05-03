#pragma once

namespace ok
{
	enum class ColorBlendMode
	{
		Normal = 0,
		Additive,
		Subtract,
		Multiply,
		Divide,
		DivideSafe,
		Average,
		Max,
		Min
	};

	class Color : public glm::vec4
	{
	public:
		Color();
		Color(float r, float g, float b, float a);
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		Color(int r, int g, int b, int a);
		void operator=(const glm::vec4& color);
		void operator=(const ok::Color& color);

		static ok::Color Red;
		static ok::Color Green;
		static ok::Color Blue;
		static ok::Color Yellow;
		static ok::Color Magenta;
		static ok::Color Cyan;
		static ok::Color White;
		static ok::Color Black;
		static ok::Color Gray;
		static ok::Color Zero;
	protected:
	private:
	};
}