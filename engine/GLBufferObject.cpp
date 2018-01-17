#include "GLBufferObject.h"

GLBufferObject::GLBufferObject(int vertex_count)
{
	vb_count = 0;
	ind_count = 0;
	vb = nullptr;
	ind = nullptr;
	this->vertex_count = vertex_count;
	current_index_buffer_id = 0;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_buffer_count);

	glGenVertexArrays(1, &VAO);
}

GLBufferObject::~GLBufferObject()
{
	Dispose();
	glDeleteVertexArrays(1, &VAO);
}

void GLBufferObject::AddVertexBuffer(GLenum mode, int vertex_size, bool allocate)
{
	int new_vb_count = vb_count + 1;
	GLBufferInfo* new_vb = new GLBufferInfo[new_vb_count];

	for (int i = 0; i < vb_count; i++)
	{
		new_vb[i] = vb[i];
	}
	delete[] vb;

	vb = new_vb;
	vb_count = new_vb_count;

	GLBufferInfo info;
	if (allocate)
	{
		info.vertices = new float[vertex_count*vertex_size];
		memset(info.vertices, 0, sizeof(float) * vertex_count * vertex_size);
	}
	else
	{
		info.vertices = nullptr;
	}
	info.indices = nullptr;
	info.vertex_count = vertex_count;
	info.vertex_size = vertex_size;
	info.triangles_count = -1;

	glGenBuffers(1, &(info.object_id));

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, info.object_id);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_count * vertex_size, info.vertices, mode);
	glVertexAttribPointer(vb_count - 1, vertex_size, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vb_count - 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vb[vb_count - 1] = info;
}

GLBufferInfo GLBufferObject::GetVertexBuffer(int index)
{
	return vb[index];
}

void GLBufferObject::ReloadVertexBuffer(int index, int offset, int vertex_count, float* source)
{
	GLBufferInfo info = vb[index];

	if (source == nullptr)
		source = info.vertices;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, info.object_id);

	glBufferSubData(GL_ARRAY_BUFFER, offset, (sizeof(float) * info.vertex_count * info.vertex_size) /*(vertex_count == -1) ? (sizeof(float) * info.vertex_count * info.vertex_size) : (sizeof(float)*vertex_count*info.vertex_size)*/, source);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int GLBufferObject::GetLastVertexBufferIndex()
{
	return vb_count-1;
}

void GLBufferObject::AddIndexBuffer(int mode, int triangles_count)
{
	int new_ind_count = ind_count + 1;
	GLBufferInfo* new_ind = new GLBufferInfo[new_ind_count];

	for (int i = 0; i < ind_count; i++)
	{
		new_ind[i] = ind[i];
	}
	delete[] ind;

	ind = new_ind;
	ind_count = new_ind_count;

	GLBufferInfo info;
	info.indices = new int[triangles_count*3];
	memset(info.indices, 0, sizeof(int) * triangles_count * 3);
	info.vertices = nullptr;
	info.vertex_count = vertex_count;
	info.vertex_size = -1;
	info.triangles_count = triangles_count;

	glGenBuffers(1, &(info.object_id));

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.object_id);
	current_index_buffer_id = ind_count - 1;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * triangles_count * 3, info.indices, mode);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ind[ind_count - 1] = info;
}

GLBufferInfo GLBufferObject::GetIndexBuffer(int index)
{
	return ind[index];
}

void GLBufferObject::SwitchIndexBuffer(int index)
{
	if (index != current_index_buffer_id)
	{
		GLBufferInfo info = ind[index];

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.object_id);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		current_index_buffer_id = index;
	}
}

void GLBufferObject::ReloadIndexBuffer(int index, int offset, int indexes_count)
{
	GLBufferInfo info = ind[index];

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.object_id);

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, (sizeof(int) * info.triangles_count * 3)/*(indexes_count == -1) ? (sizeof(int) * info.triangles_count * 3) : (sizeof(int)*indexes_count)*/, info.indices);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLBufferObject::Bind()
{
	glBindVertexArray(VAO);
}

void GLBufferObject::Unbind()
{
	glBindVertexArray(0);
}

void GLBufferObject::Dispose()
{
	glBindVertexArray(VAO);

	for (int i = 0; i < vb_count; i++)
	{
		glDeleteBuffers(1, &(vb[i].object_id));
		delete[] vb[i].vertices;
		glDisableVertexAttribArray(i);
	}

	delete[] vb;

	for (int i = 0; i < ind_count; i++)
	{
		glDeleteBuffers(1, &(ind[i].object_id));
		delete[] ind[i].indices;
	}

	delete[] ind;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vb_count = 0;
	ind_count = 0;
	vb = nullptr;
	ind = nullptr;
}

int GLBufferObject::GetMaxVertexBufferCount()
{
	return static_cast<int>(max_vertex_buffer_count);
}
