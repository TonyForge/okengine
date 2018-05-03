#include "Color.h"

ok::Color ok::Color::Red(1.f,0.f,0.f,1.f);
ok::Color ok::Color::Green(0.f, 1.f, 0.f, 1.f);
ok::Color ok::Color::Blue(0.f, 0.f, 1.f, 1.f);
ok::Color ok::Color::Yellow(1.f, 1.f, 0.f, 1.f);
ok::Color ok::Color::Magenta(1.f, 0.f, 1.f, 1.f);
ok::Color ok::Color::Cyan(0.f, 1.f, 1.f, 1.f);
ok::Color ok::Color::White(1.f, 1.f, 1.f, 1.f);
ok::Color ok::Color::Black(0.f, 0.f, 0.f, 1.f);
ok::Color ok::Color::Gray(0.5f, 0.5f, 0.5f, 1.f);
ok::Color ok::Color::Zero(0.f, 0.f, 0.f, 0.f);

ok::Color::Color() : glm::vec4()
{
}

ok::Color::Color(float r, float g, float b, float a) : glm::vec4(r,g,b,a)
{
}

ok::Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f)
{
}

ok::Color::Color(int r, int g, int b, int a) : glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f)
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
