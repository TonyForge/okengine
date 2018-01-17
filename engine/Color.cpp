#include "Color.h"

ok::Color::Color() : glm::vec4()
{
}

ok::Color::Color(float r, float g, float b, float a) : glm::vec4(r,g,b,a)
{
}

ok::Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f)
{
}

void ok::Color::operator=(const glm::vec4 & color)
{
	glm::vec4::operator=(color);
}

void ok::Color::operator=(const ok::Color & color)
{
	glm::vec4::operator=(color);
}
