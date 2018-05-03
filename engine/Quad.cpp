#include "Quad.h"

glm::mat4 ok::Quad::transform_operator;

ok::Quad::Quad()
{
	vertices[0] = glm::vec2(0.0f, 0.0f);
	vertices[1] = glm::vec2(1.0f, 0.0f);
	vertices[2] = glm::vec2(1.0f, 1.0f);
	vertices[3] = glm::vec2(0.0f, 1.0f);

	uvs[0] = glm::vec2(0.0f, 0.0f);
	uvs[1] = glm::vec2(1.0f, 0.0f);
	uvs[2] = glm::vec2(1.0f, 1.0f);
	uvs[3] = glm::vec2(0.0f, 1.0f);
	
	_center = glm::vec2(0.5f, 0.5f);
	_size = glm::vec2(1.0f, 1.0f);

	SetColors(ok::Color(1.0f, 1.0f, 1.0f, 0.0f));
}

void ok::Quad::SetCenter(glm::vec2 center)
{
	_center = center;
}

void ok::Quad::SetSize(glm::vec2 size)
{
	_size = size;
}

void ok::Quad::SetTransform(glm::vec3 translation, float rotation_degrees, glm::vec3 scale)
{
	transform_operator = glm::mat4(1.0f);

	transform_operator = glm::translate(transform_operator, translation) * glm::rotate(transform_operator, glm::radians(rotation_degrees), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), scale);

//	transform_operator = glm::mat4(1.0f);
	//possibly here is error, because glm::rotate will erase info about previous scale (use op = scale * rotation, not op = rotation(scaled_op))
	//transform_operator = glm::scale(glm::mat4(1.0f), scale);
	//transform_operator = glm::rotate(transform_operator, glm::radians(rotation_degrees), glm::vec3(0.0f, 0.0f, 1.0f));
	//transform_operator = glm::translate(transform_operator, translation);

	vertices[0] = glm::vec2(transform_operator * glm::vec4((glm::vec2(0.0f, 0.0f) - _center) * _size, 0.0f, 1.0f));
	vertices[1] = glm::vec2(transform_operator * glm::vec4((glm::vec2(1.0f, 0.0f) - _center) * _size, 0.0f, 1.0f));
	vertices[2] = glm::vec2(transform_operator * glm::vec4((glm::vec2(1.0f, 1.0f) - _center) * _size, 0.0f, 1.0f));
	vertices[3] = glm::vec2(transform_operator * glm::vec4((glm::vec2(0.0f, 1.0f) - _center) * _size, 0.0f, 1.0f));
}

void ok::Quad::SetTransform(glm::vec2 translation, glm::vec2 scale)
{
	vertices[0] = (((glm::vec2(0.0f, 0.0f) - _center) * _size * scale + translation));
	vertices[1] = (((glm::vec2(1.0f, 0.0f) - _center) * _size * scale + translation));
	vertices[2] = (((glm::vec2(1.0f, 1.0f) - _center) * _size * scale + translation));
	vertices[3] = (((glm::vec2(0.0f, 1.0f) - _center) * _size * scale + translation));
}

void ok::Quad::SetTransform(glm::mat3 & matrix)
{
	vertices[0] = glm::vec2(matrix * glm::vec3((glm::vec2(0.0f, 0.0f) - _center) * _size, 1.0f));
	vertices[1] = glm::vec2(matrix * glm::vec3((glm::vec2(1.0f, 0.0f) - _center) * _size, 1.0f));
	vertices[2] = glm::vec2(matrix * glm::vec3((glm::vec2(1.0f, 1.0f) - _center) * _size, 1.0f));
	vertices[3] = glm::vec2(matrix * glm::vec3((glm::vec2(0.0f, 1.0f) - _center) * _size, 1.0f));
}

void ok::Quad::Place(int x, int y, int w, int h)
{
	vertices[0] = glm::vec2(static_cast<float>(x), static_cast<float>(y));
	vertices[1] = glm::vec2(static_cast<float>(x + w), static_cast<float>(y));
	vertices[2] = glm::vec2(static_cast<float>(x + w), static_cast<float>(y + h));
	vertices[3] = glm::vec2(static_cast<float>(x), static_cast<float>(y + h));
}

void ok::Quad::SetUVRect(glm::vec2 rect_left_top_corner, glm::vec2 rect_size, glm::vec2 texture_size)
{
	uvs[0] = rect_left_top_corner / texture_size;
	uvs[1] = (rect_left_top_corner + glm::vec2(rect_size.x, 0.0f)) / texture_size;
	uvs[2] = (rect_left_top_corner + glm::vec2(rect_size.x, rect_size.y)) / texture_size;
	uvs[3] = (rect_left_top_corner + glm::vec2(0.0f, rect_size.y)) / texture_size;
}

void ok::Quad::SetUVRect(glm::vec4 uv_rect)
{
	uvs[0].x = uv_rect.x;				uvs[0].y = uv_rect.y;
	uvs[1].x = uv_rect.x + uv_rect.z;	uvs[1].y = uv_rect.y;
	uvs[2].x = uv_rect.x + uv_rect.z;	uvs[2].y = uv_rect.y + uv_rect.w;
	uvs[3].x = uv_rect.x;				uvs[3].y = uv_rect.y + uv_rect.w;
}

void ok::Quad::SetUVRectFlipXY(glm::vec4 uv_rect)
{
	uvs[3].x = uv_rect.x;				uvs[3].y = uv_rect.y;
	uvs[2].x = uv_rect.x + uv_rect.z;	uvs[2].y = uv_rect.y;
	uvs[1].x = uv_rect.x + uv_rect.z;	uvs[1].y = uv_rect.y + uv_rect.w;
	uvs[0].x = uv_rect.x;				uvs[0].y = uv_rect.y + uv_rect.w;
}

void ok::Quad::SetUVRectFlipX(glm::vec4 uv_rect)
{
	uvs[3].x = uv_rect.x;				uvs[0].y = uv_rect.y;
	uvs[2].x = uv_rect.x + uv_rect.z;	uvs[1].y = uv_rect.y;
	uvs[1].x = uv_rect.x + uv_rect.z;	uvs[2].y = uv_rect.y + uv_rect.w;
	uvs[0].x = uv_rect.x;				uvs[3].y = uv_rect.y + uv_rect.w;
}

void ok::Quad::SetUVRectFlipY(glm::vec4 uv_rect)
{
	uvs[0].x = uv_rect.x;				uvs[3].y = uv_rect.y;
	uvs[1].x = uv_rect.x + uv_rect.z;	uvs[2].y = uv_rect.y;
	uvs[2].x = uv_rect.x + uv_rect.z;	uvs[1].y = uv_rect.y + uv_rect.w;
	uvs[3].x = uv_rect.x;				uvs[0].y = uv_rect.y + uv_rect.w;
}

void ok::Quad::SetColors(ok::Color color)
{
	for (int i = 0; i < 4; i++)
	{
		colors[i] = color;
	}
	colors_stride[0] = color.r;
	colors_stride[1] = color.g;
	colors_stride[2] = color.b;
	colors_stride[3] = color.a;
	memcpy(&(colors_stride[4]), colors_stride, sizeof(float) * 4);
	memcpy(&(colors_stride[8]), colors_stride, sizeof(float) * 8);
}

void ok::Quad::SetColors(ok::Color left_top, ok::Color right_top, ok::Color left_bottom, ok::Color right_bottom)
{
	//FIX ORDER HERE!
	colors[0] = left_top;
	colors[1] = right_top;
	colors[2] = left_bottom;
	colors[3] = right_bottom;

	colors_stride[0] = left_top.r;		colors_stride[1] = left_top.g;		colors_stride[2] = left_top.b;		colors_stride[3] = left_top.a;
	colors_stride[4] = right_top.r;		colors_stride[5] = right_top.g;		colors_stride[6] = right_top.b;		colors_stride[7] = right_top.a;
	colors_stride[8] = left_bottom.r;	colors_stride[9] = left_bottom.g;	colors_stride[10] = left_bottom.b;	colors_stride[11] = left_bottom.a;
	colors_stride[12] = right_bottom.r; colors_stride[13] = right_bottom.g; colors_stride[14] = right_bottom.b; colors_stride[15] = right_bottom.a;
}
