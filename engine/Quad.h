#pragma once
#include "Color.h"

namespace ok
{
	class Quad
	{
	public:
		glm::vec2 vertices[4];
		glm::vec2 uvs[4];
		glm::vec4 colors[4];
		float colors_stride[16];

		Quad();
		void SetCenter(glm::vec2 center);
		void SetSize(glm::vec2 size);
		void SetTransform(glm::vec3 translation, float rotation_degrees, glm::vec3 scale);
		void SetTransform(glm::vec2 translation, glm::vec2 scale);
		void SetTransform(glm::mat3& matrix);
		void Place(int x, int y, int w, int h);
		void SetUVRect(glm::vec2 rect_left_top_corner, glm::vec2 rect_size, glm::vec2 texture_size);
		void SetUVRect(glm::vec4 uv_rect);
		void SetUVRectFlipXY(glm::vec4 uv_rect);
		void SetUVRectFlipX(glm::vec4 uv_rect);
		void SetUVRectFlipY(glm::vec4 uv_rect);
		void SetColors(ok::Color color);
		void SetColors(ok::Color left_top, ok::Color right_top, ok::Color left_bottom, ok::Color right_bottom);
	protected:
		static glm::mat4 transform_operator;
	private:
		glm::vec2 _center;
		glm::vec2 _size;
	};
}