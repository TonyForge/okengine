#include "SmoothPath.h"

void Zoner::SmoothPath::Build(glm::vec3 from, glm::vec3 from_direction, glm::vec3 to, std::vector<Zoner::SmoothPathObstacle*>& obstacles, ok::graphics::LineBatch& batch)
{
	bool mirrored = false;

	glm::vec3 to_memory = to;

	glm::vec2 from_to_direction = glm::vec2(to - from);
	glm::vec2 from_direction_nrm = glm::vec2(from_direction.y, -from_direction.x);

	if (glm::dot(from_direction_nrm, from_to_direction) > 0.f)
	{
		mirrored = true;

		float dot_x = glm::dot(glm::vec2(from_direction), from_to_direction);
		float dot_y = glm::dot(-from_direction_nrm, from_to_direction);

		from_to_direction = glm::vec2(from_direction) * dot_x + from_direction_nrm * dot_y;
		to = from + glm::vec3(from_to_direction, 0.f);
	}


	float from_to_distance = glm::length(from_to_direction);
	from_to_direction = glm::normalize(from_to_direction);

	float rotation_radius = glm::clamp(from_to_distance, 1.f, 50.f);
	float oriented_angle = glm::degrees(glm::orientedAngle(glm::vec2(from_direction), glm::vec2(from_to_direction)));
	float oriented_angle_delta = glm::sign(-oriented_angle)*(180.0f - glm::abs(oriented_angle));
	
	glm::vec2 rotation_center = glm::vec2(from) - glm::vec2(from_direction.y, -from_direction.x) * rotation_radius;

	float c = glm::length(rotation_center - glm::vec2(to));
	float a = rotation_radius;
	float b = glm::sqrt(c*c - a*a);

	float sina = a / c;
	float ang_a = glm::degrees(glm::asin(sina));
	float ang_b = 90.0f - ang_a;

	glm::vec2 arc_end = (glm::vec2(to) + b*glm::normalize(glm::rotate(rotation_center - glm::vec2(to), glm::radians(ang_a)))) - glm::vec2(from);

	if (mirrored)
	{
		glm::vec2 nrm = glm::vec2(from_direction.y, -from_direction.x);

		float dot_x = glm::dot(glm::vec2(from_direction), arc_end);
		float dot_y = glm::dot(-nrm, arc_end);

		arc_end = glm::vec2(from_direction) * dot_x + nrm * dot_y;

		to = to_memory;
		rotation_center = glm::vec2(from) - (rotation_center - glm::vec2(from));
	}

	arc_end += glm::vec2(from);


	batch.SetBrushThickness(2.f);
	batch.SetBrushSoftness(0.0001f);
	
	batch.BatchBegin();

	batch.SetBrushColor(ok::Color::Gray);
	batch.Circle(glm::vec3(rotation_center,0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), rotation_radius, 5.f);

	batch.SetBrushColor(ok::Color::Red);
	batch.MoveTo(from);
	batch.LineTo(glm::vec3(rotation_center,0.f));

	batch.SetBrushColor(ok::Color::Green);
	batch.LineTo(glm::vec3(arc_end, 0.f));

	batch.SetBrushColor(ok::Color::Blue);
	batch.LineTo(to);

	batch.BatchEnd();
}
