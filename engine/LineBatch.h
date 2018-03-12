#pragma once

#include "AssetsBasic.h"
#include "GLBufferObject.h"
#include "Camera.h"
#include "Transform.h"

namespace ok
{
	namespace graphics
	{
		class LineBatch : public ok::graphics::ShaderAliasDispatcher
		{
		public:
			LineBatch(int size = 1000);
			~LineBatch();

			void BatchBegin();
			void BatchEnd();

			void LineAB(glm::vec3 a, glm::vec3 b);
			void MoveTo(glm::vec3 position);
			void LineTo(glm::vec3 position);
			void Circle(glm::vec3 center, glm::vec3 axis_x, glm::vec3 axis_y, float radius, float arc_step_degrees);

			void SetBrushColor(ok::Color color);
			void SetBrushColor(ok::Color begin_color, ok::Color end_color);
			void SetBrushThickness(float thickness);
			void SetBrushSoftness(float softness);

			glm::mat4 DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type);
			float DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type);
		protected:
		private:
			GLBufferObject* glBO;

			int batch_lines_in_use;
			int batch_size;

			static const unsigned int quad_position_data_size = 4 * 3; //4 vertices, 3 floats XY per vertex
			static const unsigned int quad_texcoord_data_size = 4 * 2; //4 vertices, 2 floats UV per vertex
			static const unsigned int quad_color_data_size = 4 * 4; //4 vertices, 4 floats RGBA per vertex
			static const unsigned int quad_index_data_size = 3 * 2; //2 triangles, 3 vertices per triangle, 1 ints INDEX per vertex

			float* positions;
			float* texcoords;
			float* colors;
			int* indices;

			ok::graphics::Material* default_material;

			ok::Color brush_color_begin;
			ok::Color brush_color_end;
			float brush_thickness;
			glm::vec3 brush_position;
			float brush_softness;

			class Line
			{
			public:
				glm::vec3 verts[4*3];
				ok::Color begin_color_0;
				ok::Color begin_color_1;
				ok::Color end_color_0;
				ok::Color end_color_1;

				void Build(glm::vec3 begin, glm::vec3 end, ok::Color begin_color, ok::Color end_color, float thickness);
			};

			static ok::graphics::LineBatch::Line line;
			void PushLine();
		};
	}
}