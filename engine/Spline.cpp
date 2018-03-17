#include "Spline.h"

ok::Spline & ok::Spline::instance()
{
	static ok::Spline s;
	return s;
}

ok::Spline & ok::Spline::o()
{
	return instance();
}

glm::vec3 ok::Spline::HermitPick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick)
{
	float t = pick;
	float t2 = t*t;
	float t3 = t2*t;

	return (
	pos0  * (2.f*t3 - 3.f*t2 + 1.f) +
	tang0 * (t3 - 2.f*t2 + t) +
	pos1  * (-2.f*t3 + 3.f*t2) +
	tang1 * (t3 - t2)
	);
}

glm::vec3 ok::Spline::HermitTangentPick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick)
{
	float t = pick;
	float t2 = t*t;
	float t3 = t2*t;


	return (
	pos0  * (6.f*t2 - 6.f*t) +
	tang0 * (3.f*t2 - 4.f*t + 1) +
	pos1  * (-6.f*t2 + 6.f*t) +
	tang1 * (3.f*t2 - 2.f*t)
	);
}

glm::vec3 ok::Spline::Hermit1stDerivativePick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick)
{
	float t = pick;
	float t2 = t*t;
	float t3 = t2*t;


	return (
	pos0  * (6.f*t2 - 6.f*t) +
	tang0 * (3.f*t2 - 4.f*t + 1) +
	pos1  * (-6.f*t2 + 6.f*t) +
	tang1 * (3.f*t2 - 2.f*t)
	);
}

glm::vec3 ok::Spline::Hermit2ndDerivativePick(glm::vec3 pos0, glm::vec3 tang0, glm::vec3 pos1, glm::vec3 tang1, float pick)
{
	float t = pick;
	float t2 = t*t;
	float t3 = t2*t;


	return (
	pos0  * (12.f*t - 6.f) +
	tang0 * (6.f*t - 4.f) +
	pos1  * (-12.f*t + 6.f) +
	tang1 * (6.f*t - 2.f)
	);
}
