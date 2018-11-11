#include "Mesh.h"

ok::graphics::Mesh::Mesh(int vertex_count, int triangles_count, int texcoords_channels_count) : buffer(vertex_count)
{
	_morph_normals_enabled = false;
	_morph_enabled = false;
	_hardware_morph_enabled = false;
	_software_morph_enabled = false;
	_skin_enabled = false;
	_hardware_skin_enabled = false;
	_software_skin_enabled = false;
	_morph_dirty = true;
	_triangles_sort_enabled = false;
	_normals_enabled = false;

	buffer.AddVertexBuffer(GL_STATIC_DRAW, 3); //vertices

	for (int i = 0; i < texcoords_channels_count; i++)
	buffer.AddVertexBuffer(GL_STATIC_DRAW, 2); //texcoords

	buffer.AddIndexBuffer(GL_STATIC_DRAW, triangles_count);

	_texcoords_channels_count = texcoords_channels_count;
	_triangles_count = triangles_count;
	_vertex_count = vertex_count;

	_morph_targets_vb_index = -1;
	_normals_vb_index = -1;
	_morph_targets_count = 0;

	_skin_vb_index = -1;
}

void ok::graphics::Mesh::EnableNormals()
{
	if (false == _normals_enabled)
	{
		buffer.AddVertexBuffer(GL_STATIC_DRAW, 3);
		_normals_vb_index = buffer.GetLastVertexBufferIndex();
	}	

	_normals_enabled = true;
}

void ok::graphics::Mesh::EnableTrianglesSort()
{
	
	if (false == _triangles_sort_enabled)
	{
		GLBufferInfo info = buffer.GetIndexBuffer(0);

		buffer.AddIndexBuffer(GL_STATIC_DRAW, info.triangles_count);
		buffer.AddIndexBuffer(GL_STATIC_DRAW, info.triangles_count);
		buffer.AddIndexBuffer(GL_STATIC_DRAW, info.triangles_count);
		buffer.AddIndexBuffer(GL_STATIC_DRAW, info.triangles_count);
		buffer.AddIndexBuffer(GL_STATIC_DRAW, info.triangles_count);
		buffer.SwitchIndexBuffer(0);
	}

	_triangles_sort_enabled = true;
}

void ok::graphics::Mesh::SortTriangles()
{
	if (false == _triangles_sort_enabled) return;

	GLBufferInfo indices = buffer.GetIndexBuffer(0);
	GLBufferInfo vertices = buffer.GetVertexBuffer(0);

	std::vector<ok::graphics::MeshTriangle> triangles;
	triangles.resize(indices.triangles_count);

	MeshTriangle triangle;
	float vertex1[3];
	float vertex2[3];
	float vertex3[3];

	for (int i = 0; i < indices.triangles_count; i++)
	{
		memcpy(triangle.vertex_indexes, &(indices.indices[i * 3]), sizeof(int) * 3);

		memcpy(vertex1, &(vertices.vertices[triangle.vertex_indexes[0] * 3]), sizeof(float) * 3);
		memcpy(vertex2, &(vertices.vertices[triangle.vertex_indexes[1] * 3]), sizeof(float) * 3);
		memcpy(vertex3, &(vertices.vertices[triangle.vertex_indexes[2] * 3]), sizeof(float) * 3);

		triangle.centroid_position = glm::vec3(
			(vertex1[0] + vertex2[0] + vertex3[0]) / 3.f,
			(vertex1[1] + vertex2[1] + vertex3[1]) / 3.f,
			(vertex1[2] + vertex2[2] + vertex3[2]) / 3.f);
		triangles[i] = triangle;
	}

	_SortTrianglesAlongAxis(glm::vec3(1.f, 0.f, 0.f), triangles, buffer.GetIndexBuffer(0));
	_SortTrianglesAlongAxis(glm::vec3(0.f, 1.f, 0.f), triangles, buffer.GetIndexBuffer(1));
	_SortTrianglesAlongAxis(glm::vec3(0.f, 0.f, 1.f), triangles, buffer.GetIndexBuffer(2));
	_SortTrianglesAlongAxis(glm::vec3(-1.f, 0.f, 0.f), triangles, buffer.GetIndexBuffer(3));
	_SortTrianglesAlongAxis(glm::vec3(0.f, -1.f, 0.f), triangles, buffer.GetIndexBuffer(4));
	_SortTrianglesAlongAxis(glm::vec3(0.f, 0.f, -1.f), triangles, buffer.GetIndexBuffer(5));

}

void ok::graphics::Mesh::CalculateNormals(ok::graphics::NormalsCalculationMethod method)
{
	EnableNormals();


}

GLBufferInfo ok::graphics::Mesh::GetVertices()
{
	return buffer.GetVertexBuffer(0);
}

void ok::graphics::Mesh::UpdateVertices()
{
	buffer.ReloadVertexBuffer(0);
}

GLBufferInfo ok::graphics::Mesh::GetUV(int channel)
{
	return buffer.GetVertexBuffer(1 + channel);
}

void ok::graphics::Mesh::UpdateUV(int channel)
{
	if (channel <= -1)
	{
		for (int i = 0; i < _texcoords_channels_count; i++)
			buffer.ReloadVertexBuffer(1 + i);
	}
	else
	{
		buffer.ReloadVertexBuffer(1 + channel);
	}
}

GLBufferInfo ok::graphics::Mesh::GetNormals()
{
	return buffer.GetVertexBuffer(_normals_vb_index);
}

void ok::graphics::Mesh::UpdateNormals()
{
	if (_normals_enabled)
	buffer.ReloadVertexBuffer(_normals_vb_index);
}

GLBufferInfo ok::graphics::Mesh::GetIndices(int index)
{
	buffer.SwitchIndexBuffer(index);
	return buffer.GetIndexBuffer(index);
}

void ok::graphics::Mesh::UpdateIndices()
{
	buffer.ReloadIndexBuffer(0);

	if (_triangles_sort_enabled)
	{
		buffer.ReloadIndexBuffer(1);
		buffer.ReloadIndexBuffer(2);
		buffer.ReloadIndexBuffer(3);
		buffer.ReloadIndexBuffer(4);
		buffer.ReloadIndexBuffer(5);
	}
}

void ok::graphics::Mesh::SwitchIndices(int index)
{
	buffer.SwitchIndexBuffer(index);
}

int ok::graphics::Mesh::GetTrianglesCount()
{
	return _triangles_count;
}

int ok::graphics::Mesh::GetVertexCount()
{
	return _vertex_count;
}

void ok::graphics::Mesh::EnableSkinDeformer(int bones_count, bool hardware)
{
	if (_skin_enabled) return;

	_skin_enabled = true;

	_skin_bones.resize(bones_count);
	for (auto& bone : _skin_bones)
	{
		bone = glm::mat4(1.f);
	}

	if (hardware)
	{
		//enable hardware skin
		_hardware_skin_enabled = true;
		_software_skin_enabled = false;

		_skin_vb_index = buffer.GetLastVertexBufferIndex() + 1;

		buffer.AddVertexBuffer(GL_STATIC_DRAW, 4); //weights
		buffer.AddVertexBuffer(GL_STATIC_DRAW, 4); //indexes

		_skin_hardware_indexes.resize(_vertex_count * 4);
	}
	else
	{
		//enable software skin
		_hardware_skin_enabled = false;
		_software_skin_enabled = true;

		_skin_software_weights.resize(_vertex_count * 4);
		_skin_software_indexes.resize(_vertex_count * 4);

		std::fill(_skin_software_weights.begin(), _skin_software_weights.end(), 0.0f);
		std::fill(_skin_software_indexes.begin(), _skin_software_indexes.end(), -1);

		_skin_software_buffer_positions.resize(_vertex_count * 3);

		if (_normals_enabled)
		_skin_software_buffer_normals.resize(_vertex_count * 3);
	}
}

void ok::graphics::Mesh::ApplyDeformers()
{
	if (_morph_enabled && !_software_skin_enabled)
	_ApplyMorphDeformer();

	if (_skin_enabled)
	_ApplySkinDeformer();
}

void ok::graphics::Mesh::EnableMorphDeformer(int targets_count, ok::graphics::MorphingTargetType targets_type, bool hardware)
{
	if (_morph_enabled) return;

	_morph_enabled = true;

	_morph_normals_enabled = (targets_type == ok::graphics::MorphingTargetType::Positions_Normals);

	_morph_targets_count = targets_count;

	_morph_blends.resize(targets_count);
	std::fill(_morph_blends.begin(), _morph_blends.end(), 0.f);

	_morph_positions.resize(targets_count);
	for (auto& layer : _morph_positions)
	{
		layer = nullptr;
	}

	if (_morph_normals_enabled)
	{
		_morph_normals.resize(targets_count);
		for (auto& layer : _morph_normals)
		{
			layer = nullptr;
		}
	}


	int target_size = 3;
	if (_morph_normals_enabled)
		target_size += 3;

	target_size *= targets_count;

	if (hardware)
	{
		//enable hardware morphing
		_hardware_morph_enabled = true;
		_software_morph_enabled = false;

		_morph_targets_vb_index = buffer.GetLastVertexBufferIndex() + 1;

		for (int i = 0; i < targets_count; i++)
		{
			buffer.AddVertexBuffer(GL_DYNAMIC_DRAW, 3, false);
		}

		if (_morph_normals_enabled)
		{
			for (int i = 0; i < targets_count; i++)
			{
				buffer.AddVertexBuffer(GL_DYNAMIC_DRAW, 3, false);
			}
		}
	}
	else
	{
		//enable software morphing
		_hardware_morph_enabled = false;
		_software_morph_enabled = true;

		_morph_software_buffer_positions.resize(_vertex_count * 3);
		std::fill(_morph_software_buffer_positions.begin(), _morph_software_buffer_positions.end(), 0.f);

		if (_morph_normals_enabled)
		{
			_morph_software_buffer_normals.resize(_vertex_count * 3);
			std::fill(_morph_software_buffer_normals.begin(), _morph_software_buffer_normals.end(), 0.f);
		}	
	}

	_morph_dirty = true;
}

void ok::graphics::Mesh::AssignMorphTarget(
	int target_index,
	float blend,
	float* positions,
	float* normals)
{
	if (_morph_blends[target_index] != blend)
	{
		_morph_blends[target_index] = blend;

		if (_software_morph_enabled) _morph_dirty = true;
	}

	float*& _positions = _morph_positions[target_index];
	float*& _normals = _morph_normals[target_index];

	if (_positions != positions)
	{
		if (_hardware_morph_enabled)
		{
			if (_positions != positions)
				buffer.ReloadVertexBuffer(_morph_targets_vb_index + target_index, 0, -1, positions);
		}

		_positions = positions;
		_morph_dirty = true;
	}

	if (_normals != normals)
	{
		if (_hardware_morph_enabled)
		{
			if (_normals != normals)
				buffer.ReloadVertexBuffer(_morph_targets_vb_index + _morph_targets_count + target_index, 0, -1, normals);
		}

		_normals = normals;
		_morph_dirty = true;
	}
}

void ok::graphics::Mesh::AssignMorphTarget(int target_index, float left_to_right_blend, float blend, std::pair<float*, float*> left, std::pair<float*, float*> right)
{
	float left_blend = (1.0f - left_to_right_blend) * blend;
	float right_blend = left_to_right_blend * blend;

	if (_morph_blends[target_index] != left_blend)
	{
		_morph_blends[target_index] = left_blend;

		if (_software_morph_enabled) _morph_dirty = true;
	}

	if (_morph_blends[target_index+1] != right_blend)
	{
		_morph_blends[target_index+1] = right_blend;

		if (_software_morph_enabled) _morph_dirty = true;
	}

	float*& _positions_left = _morph_positions[target_index];
	float*& _positions_right = _morph_positions[target_index+1];

	float*& _normals_left = _morph_normals[target_index];
	float*& _normals_right = _morph_normals[target_index + 1];

	if (_positions_left != left.first)
	{
		if (_hardware_morph_enabled)
		{
			buffer.ReloadVertexBuffer(_morph_targets_vb_index + target_index, 0, -1, left.first);
		}

		_positions_left = left.first;
		_morph_dirty = true;
	}

	if (_positions_right != right.first)
	{
		if (_hardware_morph_enabled)
		{
			buffer.ReloadVertexBuffer(_morph_targets_vb_index + target_index + 1, 0, -1, right.first);
		}

		_positions_right = right.first;
		_morph_dirty = true;
	}

	if (_normals_left != left.second)
	{
		if (_hardware_morph_enabled)
		{
			buffer.ReloadVertexBuffer(_morph_targets_vb_index + _morph_targets_count + target_index, 0, -1, left.second);
		}

		_normals_left = left.second;
		_morph_dirty = true;
	}

	if (_normals_right != right.second)
	{
		if (_hardware_morph_enabled)
		{
			buffer.ReloadVertexBuffer(_morph_targets_vb_index + _morph_targets_count + target_index + 1, 0, -1, right.second);
		}

		_normals_right = right.second;
		_morph_dirty = true;
	}
}

void ok::graphics::Mesh::UploadSkinWeights(float * weights)
{
	if (_hardware_skin_enabled)
	{
		memcpy(buffer.GetVertexBuffer(_skin_vb_index).vertices, weights, sizeof(float) * _vertex_count * 4);
		buffer.ReloadVertexBuffer(_skin_vb_index);
	}
	else if (_software_skin_enabled)
	{
		_skin_software_weights.assign(weights, weights + _vertex_count * 4);
	}
}

void ok::graphics::Mesh::UploadSkinIndexes(int * indexes)
{
	if (_hardware_skin_enabled)
	{
		float* dest = buffer.GetVertexBuffer(_skin_vb_index + 1).vertices;

		for (int i = 0; i < _vertex_count * 4; i++)
		{
			dest[i] = static_cast<float>(indexes[i]);
		}

		buffer.ReloadVertexBuffer(_skin_vb_index + 1);
	}
	else if (_software_skin_enabled)
	{
		_skin_software_indexes.assign(indexes, indexes + _vertex_count * 4);
	}
}

void ok::graphics::Mesh::AssignSkinBones(float * bones, int* mapping_table, bool hwd_indexes_force_remap)
{
	int i = 0;
	for (auto& bone : _skin_bones)
	{
		if (mapping_table[i] == -1)
		{
			bone = glm::mat4(0.f);
		}
		else
		{
			memcpy(glm::value_ptr(bone), &bones[mapping_table[i] * 16], sizeof(float) * 16);
		}
		
		i++;
	}
}

std::vector<glm::mat4>& ok::graphics::Mesh::GetSkinBones()
{
	return _skin_bones;
}

float& ok::graphics::Mesh::GetMorphBlends(int target_index)
{
	return _morph_blends[target_index];
}

void ok::graphics::Mesh::_ApplyMorphDeformer()
{
	if (_morph_dirty)
	{
		if (_software_morph_enabled)
		{
			//initialize from original mesh
			float* ptr = buffer.GetVertexBuffer(0).vertices;
			std::copy(ptr, ptr + _vertex_count * 3, _morph_software_buffer_positions.begin());

			if (_morph_normals_enabled)
			{
				ptr = buffer.GetVertexBuffer(_normals_vb_index).vertices;
				std::copy(ptr, ptr + _vertex_count * 3, _morph_software_buffer_normals.begin());
			}
				
			//add delta meshes
			size_t i = 0;
			int j = 0;

			float blend;

			j = 0;
			for (auto&& delta : _morph_positions)
			{
				blend = _morph_blends[j];

				if (delta != nullptr)
				{
					i = 0;
					for (auto& position : _morph_software_buffer_positions)
					{
						position += delta[i]*blend;
						i++;
					}
				}

				j++;
			}

			j = 0;
			if (_morph_normals_enabled)
			{
				for (auto&& delta : _morph_normals)
				{
					blend = _morph_blends[j];

					if (delta != nullptr)
					{
						i = 0;
						for (auto& normal : _morph_software_buffer_normals)
						{
							normal += delta[i] * blend;
							i++;
						}
					}

					j++;
				}

				//normalize normals
				i = 0;
				glm::vec3 nrm;
				for (i = 0; i < _morph_software_buffer_normals.size(); i+=3)
				{
					nrm = glm::normalize(glm::vec3(
						_morph_software_buffer_normals[i],
						_morph_software_buffer_normals[i + 1],
						_morph_software_buffer_normals[i + 2]
					));
					_morph_software_buffer_normals[i] = nrm.x;
					_morph_software_buffer_normals[i + 1] = nrm.y;
					_morph_software_buffer_normals[i + 2] = nrm.z;
				}
			}

			if (!_software_skin_enabled)
			{
				//upload to gpu
				buffer.ReloadVertexBuffer(0, 0, -1, _morph_software_buffer_positions.data());

				if (_morph_normals_enabled)
				{
					buffer.ReloadVertexBuffer(_normals_vb_index, 0, -1, _morph_software_buffer_normals.data());
				}
			}
		}

		_morph_dirty = false;
	}
}

void ok::graphics::Mesh::_ApplySkinDeformer()
{
	if (_software_skin_enabled)
	{
		float* ptr;

		if (_software_morph_enabled)
		{
			if (_morph_dirty)
				_ApplyMorphDeformer();

			ptr = _morph_software_buffer_positions.data();
			std::copy(ptr, ptr + _vertex_count * 3, _skin_software_buffer_positions.begin());

			if (_normals_enabled)
			{
				ptr = _morph_software_buffer_normals.data();
				std::copy(ptr, ptr + _vertex_count * 3, _skin_software_buffer_normals.begin());
			}
		}
		else
		{
			ptr = buffer.GetVertexBuffer(0).vertices;
			std::copy(ptr, ptr + _vertex_count * 3, _skin_software_buffer_positions.begin());

			if (_normals_enabled)
			{
				ptr = buffer.GetVertexBuffer(_normals_vb_index).vertices;
				std::copy(ptr, ptr + _vertex_count * 3, _skin_software_buffer_normals.begin());
			}
		}


		int* indexes;
		float* weights;

		if (_normals_enabled == false)
		{
			glm::vec4 skinned_position(0.f, 0.f, 0.f, 1.f);
			float* skinned_position_ptr = glm::value_ptr(skinned_position);

			glm::mat4 skin_matrix = glm::mat4(0.f);
			bool skin_matrix_initiated;

			glm::vec4 weights_vector;
			float weights_sum;

			for (int i = 0; i < _vertex_count; i++)
			{
				indexes = &_skin_software_indexes[i * 4];
				weights = glm::value_ptr(weights_vector);

				memcpy(weights, &_skin_software_weights[i * 4], sizeof(float) * 4);
				memcpy(skinned_position_ptr, &_skin_software_buffer_positions[i * 3], sizeof(float) * 3);

				if (indexes[0] == -1 || _skin_bones[indexes[0]][3][3] == 0.f) weights_vector.x = 0.f;
				if (indexes[1] == -1 || _skin_bones[indexes[1]][3][3] == 0.f) weights_vector.y = 0.f;
				if (indexes[2] == -1 || _skin_bones[indexes[2]][3][3] == 0.f) weights_vector.z = 0.f;
				if (indexes[3] == -1 || _skin_bones[indexes[3]][3][3] == 0.f) weights_vector.w = 0.f;

				weights_sum = weights_vector.x + weights_vector.y + weights_vector.z + weights_vector.w;

				if (weights_sum > 0.f)
				{
					weights_vector.x /= weights_sum;
					weights_vector.y /= weights_sum;
					weights_vector.z /= weights_sum;
					weights_vector.w /= weights_sum;

					skin_matrix_initiated = false;

					for (int bone_index = 0; bone_index < 4; bone_index++)
					{
						if (weights_vector[bone_index] != 0.f)
						{
							if (skin_matrix_initiated)
								skin_matrix += _skin_bones[indexes[bone_index]] * weights[bone_index];
							else
							{
								skin_matrix = _skin_bones[indexes[bone_index]] * weights[bone_index];
								skin_matrix_initiated = true;
							}
						}
					}

					skinned_position = skin_matrix * skinned_position;

					memcpy(&_skin_software_buffer_positions[i * 3], skinned_position_ptr, sizeof(float) * 3);
				}

			}
		}
		else
		{
			glm::vec4 skinned_position(0.f, 0.f, 0.f, 1.f);
			float* skinned_position_ptr = glm::value_ptr(skinned_position);

			glm::vec4 skinned_normal(0.f, 0.f, 0.f, 0.f);
			float* skinned_normal_ptr = glm::value_ptr(skinned_normal);

			glm::mat4 skin_matrix = glm::mat4(0.f);
			bool skin_matrix_initiated;

			glm::vec4 weights_vector;
			float weights_sum;

			glm::vec3 x_axis, y_axis, z_axis;

			for (int i = 0; i < _vertex_count; i++)
			{
				indexes = &_skin_software_indexes[i * 4];
				weights = glm::value_ptr(weights_vector);

				memcpy(weights, &_skin_software_weights[i * 4], sizeof(float) * 4);
				memcpy(skinned_position_ptr, &_skin_software_buffer_positions[i * 3], sizeof(float) * 3);
				memcpy(skinned_normal_ptr, &_skin_software_buffer_normals[i * 3], sizeof(float) * 3);

				if (indexes[0] == -1 || _skin_bones[indexes[0]][3][3] == 0.f) weights_vector.x = 0.f;
				if (indexes[1] == -1 || _skin_bones[indexes[1]][3][3] == 0.f) weights_vector.y = 0.f;
				if (indexes[2] == -1 || _skin_bones[indexes[2]][3][3] == 0.f) weights_vector.z = 0.f;
				if (indexes[3] == -1 || _skin_bones[indexes[3]][3][3] == 0.f) weights_vector.w = 0.f;

				weights_sum = weights_vector.x + weights_vector.y + weights_vector.z + weights_vector.w;

				if (weights_sum > 0.f)
				{
					weights_vector.x /= weights_sum;
					weights_vector.y /= weights_sum;
					weights_vector.z /= weights_sum;
					weights_vector.w /= weights_sum;

					skin_matrix_initiated = false;

					for (int bone_index = 0; bone_index < 4; bone_index++)
					{
						if (weights_vector[bone_index] != 0.f)
						{
							if (skin_matrix_initiated)
								skin_matrix += _skin_bones[indexes[bone_index]] * weights[bone_index];
							else
							{
								skin_matrix = _skin_bones[indexes[bone_index]] * weights[bone_index];
								skin_matrix_initiated = true;
							}
						}
					}

					skinned_position = skin_matrix * skinned_position;
					
					x_axis = glm::vec3(skin_matrix[0]);
					y_axis = glm::vec3(skin_matrix[1]);
					z_axis = glm::vec3(skin_matrix[2]);

					skinned_normal = glm::normalize(skin_matrix * (skinned_normal * glm::vec4(1.f / glm::dot(x_axis, x_axis), 1.f / glm::dot(y_axis, y_axis), 1.f / glm::dot(z_axis, z_axis), 0.f))); //w = 0.f because there is no translation in normals vectors

					memcpy(&_skin_software_buffer_positions[i * 3], skinned_position_ptr, sizeof(float) * 3);
					memcpy(&_skin_software_buffer_normals[i * 3], skinned_normal_ptr, sizeof(float) * 3);
				}
				
			}
		}

		//upload to gpu
		buffer.ReloadVertexBuffer(0, 0, -1, _skin_software_buffer_positions.data());

		if (_normals_enabled)
		{
			buffer.ReloadVertexBuffer(_normals_vb_index, 0, -1, _skin_software_buffer_normals.data());
		}
	}
}


void ok::graphics::Mesh::Bind()
{
	buffer.Bind();
}

void ok::graphics::Mesh::Unbind()
{
	buffer.Unbind();
}

void ok::graphics::Mesh::_SortTrianglesAlongAxis(glm::vec3 axis, std::vector<ok::graphics::MeshTriangle> & in_triangles, GLBufferInfo & out_indices)
{
	std::sort(in_triangles.begin(),
			  in_triangles.end(),
			  [axis](const ok::graphics::MeshTriangle& left, const ok::graphics::MeshTriangle& right) {
				return (left.centroid_position.x*axis.x+ left.centroid_position.y*axis.y+left.centroid_position.z*axis.z) < (right.centroid_position.x*axis.x + right.centroid_position.y*axis.y + right.centroid_position.z*axis.z);
			  });

	for (size_t i = 0; i < in_triangles.size(); i++)
	{
		memcpy(&(out_indices.indices[i * 3]), in_triangles[i].vertex_indexes, sizeof(int) * 3);
	}
}
