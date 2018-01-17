#pragma once

#define GL_BUFFER_OFFSET(i) ((char *)NULL + (i))

class GLBufferInfo
{
public:
	unsigned int object_id;
	float* vertices;
	int* indices;
	int vertex_count;
	int triangles_count;
	int vertex_size;
};

class GLBufferObject
{
public:
	GLBufferObject(int vertex_count);
	~GLBufferObject();
	/*mode: GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
	*/
	void AddVertexBuffer(GLenum mode, int vertex_size, bool allocate = true);
	GLBufferInfo GetVertexBuffer(int index);
	void ReloadVertexBuffer(int index, int offset = 0, int vertex_count = -1, float* source = nullptr);
	int GetLastVertexBufferIndex();
	/*mode: GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
	*/
	void AddIndexBuffer(int mode, int triangles_count);
	GLBufferInfo GetIndexBuffer(int index);
	void SwitchIndexBuffer(int index);
	void ReloadIndexBuffer(int index, int offset = 0, int indexes_count = -1);

	void Bind();
	void Unbind();
	void Dispose();

	int GetMaxVertexBufferCount();
protected:
private:
	GLint max_vertex_buffer_count;
	int current_index_buffer_id;
	int vb_count;
	int ind_count;
	int vertex_count;
	GLBufferInfo* vb;
	GLBufferInfo* ind;
	unsigned int VAO;
};