#include "LineBatch.h"

ok::graphics::LineBatch::Line ok::graphics::LineBatch::line;

ok::graphics::LineBatch::LineBatch(int size)
{
	glBO = new GLBufferObject(size * 4 * 3);
	glBO->AddVertexBuffer(GL_DYNAMIC_DRAW, 3); //position
	glBO->AddVertexBuffer(GL_STATIC_DRAW, 2); //texcoord
	glBO->AddVertexBuffer(GL_DYNAMIC_DRAW, 4); //color
	glBO->AddIndexBuffer(GL_STATIC_DRAW, size * 2 * 3);

	positions = glBO->GetVertexBuffer(0).vertices;
	texcoords = glBO->GetVertexBuffer(1).vertices;
	colors = glBO->GetVertexBuffer(2).vertices;
	indices = glBO->GetIndexBuffer(0).indices;

	int* ptr = nullptr;
	int index = 0;
	for (int i = 0; i < size * 3; i++)
	{
		index = i * 4;
		ptr = &(indices[i * quad_index_data_size]);
		ptr[0] = index;
		ptr[1] = index + 1;
		ptr[2] = index + 2;
		ptr[3] = index;
		ptr[4] = index + 2;
		ptr[5] = index + 3;
	}

	float* uv_ptr = nullptr;
	for (int i = 0; i < size; i++)
	{
		uv_ptr = &(texcoords[i * 3 * quad_texcoord_data_size]);

		uv_ptr[0] = 0.f;
		uv_ptr[1] = 0.f;
		uv_ptr[2] = 0.25f;
		uv_ptr[3] = 0.f;
		uv_ptr[4] = 0.25f;
		uv_ptr[5] = 1.f;
		uv_ptr[6] = 0.f;
		uv_ptr[7] = 1.f;

		uv_ptr[8] = 0.25f;
		uv_ptr[9] = 0.f;
		uv_ptr[10] = 0.75f;
		uv_ptr[11] = 0.f;
		uv_ptr[12] = 0.75f;
		uv_ptr[13] = 1.f;
		uv_ptr[14] = 0.25f;
		uv_ptr[15] = 1.f;

		uv_ptr[16] = 0.75f;
		uv_ptr[17] = 0.f;
		uv_ptr[18] = 1.f;
		uv_ptr[19] = 0.f;
		uv_ptr[20] = 1.0f;
		uv_ptr[21] = 1.f;
		uv_ptr[22] = 0.75f;
		uv_ptr[23] = 1.f;
	}

	glBO->ReloadVertexBuffer(1);
	glBO->ReloadIndexBuffer(0);

	default_material = ok::AssetsBasic::instance().GetMaterial("LineBatch.DefaultMaterial.xml");
	batch_lines_in_use = 0;
}

ok::graphics::LineBatch::~LineBatch()
{
	delete glBO;
}

void ok::graphics::LineBatch::BatchBegin()
{
	if (batch_lines_in_use != 0)
	{
		BatchEnd();
	}
}

void ok::graphics::LineBatch::BatchEnd()
{
	if (batch_lines_in_use == 0)
	{
		return;
	}

	ok::graphics::Material* current_material = default_material;

	//setup shader
	current_material->Bind(this);

	glBO->ReloadVertexBuffer(0); //reload positions
	glBO->ReloadVertexBuffer(2); //reload colors

	//bind buffers
	glBO->Bind();

	glDrawElements(GL_TRIANGLES, batch_lines_in_use * 3 * 6, GL_UNSIGNED_INT, 0);

	glBO->Unbind();

	batch_lines_in_use = 0;
}

void ok::graphics::LineBatch::LineAB(glm::vec3 a, glm::vec3 b)
{
	if (batch_lines_in_use == batch_size)
	{
		BatchEnd();
	}
	if (batch_lines_in_use == 0)
	{
		BatchBegin();
	}

	line.Build(a, b, brush_color_begin, brush_color_end, brush_thickness);

	PushLine();
}

void ok::graphics::LineBatch::MoveTo(glm::vec3 position)
{
	brush_position = position;
}

void ok::graphics::LineBatch::LineTo(glm::vec3 position)
{
	LineAB(brush_position, position);
	brush_position = position;
}

void ok::graphics::LineBatch::Circle(glm::vec3 center, glm::vec3 axis_x, glm::vec3 axis_y, float radius, float arc_step_degrees)
{
	ok::Transform root;
	ok::Transform circle;
	ok::Transform circle_arc_begin;
	ok::Transform circle_arc_end;

	circle_arc_begin.BeginTransform();
	circle_arc_begin.SetPosition(glm::vec3(radius, 0.f, 0.f));
	circle_arc_begin.EndTransform(false);

	circle_arc_end.BeginTransform();
	circle_arc_end.SetPosition(glm::vec3(radius, 0.f, 0.f));
	circle_arc_end.EndTransform(false);

	circle.AddChild(&circle_arc_begin);
	circle.BeginTransform();
	circle.SetRotation(glm::vec3(0.f, 0.f, -arc_step_degrees));
	circle.EndTransform(true);

	circle.AddChild(&circle_arc_end);

	circle.BeginTransform();
	circle.SetRotation(glm::vec3(0.f, 0.f, 0.f));
	circle.EndTransform(true);

	root.AddChild(&circle);

	root.BeginTransform();
	root.SetOrientation(axis_x, axis_y);
	root.SetPosition(center);
	root.EndTransform(true);

	int steps = static_cast<int>(glm::ceil(360.0f / arc_step_degrees));

	circle_arc_begin.BeginTransform(ok::TransformSpace::WorldSpace);
	MoveTo(circle_arc_begin.GetPosition());
	circle_arc_begin.EndTransform(false);

	circle_arc_begin.SetParent(nullptr);

	circle_arc_end.BeginTransform(ok::TransformSpace::WorldSpace);
	LineTo(circle_arc_end.GetPosition());
	circle_arc_end.EndTransform(false);

	for (int i = 1; i <= steps; i++)
	{
		circle_arc_end.BeginTransform(ok::TransformSpace::WorldSpace);
		LineTo(circle_arc_end.GetPosition());
		circle_arc_end.EndTransform(false);

		circle.BeginTransform();
		circle.SetRotation(glm::vec3(0.f, 0.f, arc_step_degrees*static_cast<float>(i)));
		circle.EndTransform(true);
	}
}

void ok::graphics::LineBatch::SetBrushColor(ok::Color color)
{
	brush_color_begin = color;
	brush_color_end = color;
}

void ok::graphics::LineBatch::SetBrushColor(ok::Color begin_color, ok::Color end_color)
{
	brush_color_begin = begin_color;
	brush_color_end = end_color;
}

void ok::graphics::LineBatch::SetBrushThickness(float thickness)
{
	brush_thickness = ((thickness < 1.f) ? 1.f : thickness);
}

void ok::graphics::LineBatch::SetBrushSoftness(float softness)
{
	brush_softness = softness;
}

glm::mat4 ok::graphics::LineBatch::DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
		case ok::graphics::ShaderAliasReference::ModelViewProjectionMatrix:
		{
			ok::graphics::Camera* camera = ok::graphics::Camera::GetCurrent();

			if (camera != nullptr)
				return camera->GetVPMatrix();
			else
				return glm::mat4(1.0f);
		}
		break;
	}

	unresolved_alias = true;
	return glm::mat4(1.0f);
}

float ok::graphics::LineBatch::DispatchAliasFloat(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
		case ok::graphics::ShaderAliasReference::Callback:
		{
			if (*callback_name_ptr == "softness")
				return brush_softness;
		}
		break;
	}

	unresolved_alias = true;
	return 0.0f;
}

void ok::graphics::LineBatch::PushLine()
{
	float* ptr = nullptr;

	ptr = &(positions[batch_lines_in_use * quad_position_data_size * 3]);
	for (int i = 0; i < 12; i++)
	{
		memcpy(ptr, glm::value_ptr(line.verts[i]), sizeof(float) * 3);
		ptr += 3;
	}

	ptr = &(colors[batch_lines_in_use * quad_color_data_size * 3]);

	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.begin_color_0)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.begin_color_1)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.begin_color_1)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.begin_color_0)), sizeof(float) * 4);
	ptr += 4;

	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.begin_color_1)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.end_color_0)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.end_color_0)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.begin_color_1)), sizeof(float) * 4);
	ptr += 4;

	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.end_color_0)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.end_color_1)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.end_color_1)), sizeof(float) * 4);
	ptr += 4;
	memcpy(ptr, glm::value_ptr(static_cast<glm::vec4>(line.end_color_0)), sizeof(float) * 4);

	batch_lines_in_use++;
}

void ok::graphics::LineBatch::Line::Build(glm::vec3 begin, glm::vec3 end, ok::Color begin_color, ok::Color end_color, float thickness)
{
	glm::vec3 direction = glm::normalize(end - begin);

	begin -= direction * thickness*0.5f;
	end += direction * thickness*0.5f;

	direction = end - begin;
	float direction_length = glm::length(direction);

	if (direction_length < 1.f) direction_length = 1.f;

	float knot_percentage = thickness / direction_length;
	direction /= direction_length;

	ok::graphics::Camera* current_camera = ok::graphics::Camera::GetCurrent();

	glm::vec3 camera_forward = current_camera->GetForward();
	glm::vec3 thickness_vector = glm::normalize(glm::cross(direction, camera_forward))*thickness*0.5f;
	
	verts[0] = begin + thickness_vector;
	verts[1] = begin + direction * thickness*0.5f + thickness_vector;
	verts[2] = begin + direction * thickness*0.5f - thickness_vector;
	verts[3] = begin - thickness_vector;

	verts[4] = verts[1];
	verts[5] = end - direction * thickness*0.5f + thickness_vector;
	verts[6] = end - direction * thickness*0.5f - thickness_vector;
	verts[7] = verts[2];

	verts[8] = verts[5];
	verts[9] = end + thickness_vector;
	verts[10] = end - thickness_vector;
	verts[11] = verts[6];

	
	begin_color_0 = begin_color;
	begin_color_1 = glm::lerp(begin_color, end_color, knot_percentage);
	end_color_0 = end_color;
	end_color_1 = glm::lerp(end_color, begin_color, knot_percentage);
}
