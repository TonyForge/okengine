#include "KriptaTileMap3D.h"

ok::Kripta::KriptaTileMap3D::KriptaTileMap3D() : _renderer_request(this)
{

}

void ok::Kripta::KriptaTileMap3D::Resize(int x_size, int y_size)
{
	_map.resize(x_size * y_size);

	for (auto& tile : _map)
	{
		tile.first = -1;
		tile.second = false;
	}

	_map_size_x = x_size;
	_map_size_y = y_size;
}

void ok::Kripta::KriptaTileMap3D::RegisterTile(int id, ok::graphics::Mesh * mesh, glm::vec2 uv_shift)
{
	_tiles.resize(id + 1);
	ok::Kripta::KriptaTile3D& dest = _tiles[id];
	dest.mesh = mesh;
	dest.uv_shift = uv_shift;
}

void ok::Kripta::KriptaTileMap3D::Rebuild(int x, int y, int radius, ok::FOVMap & fov_map)
{
	_nothing_to_render = true;

	_tiles_to_render.resize(0);
	int total_vert_count = 0;
	int total_tris_count = 0;

	int left = glm::max(x - radius, 0);
	int top = glm::max(y - radius, 0);
	int right = glm::min(x + radius, _map_size_x);
	int bottom = glm::min(y + radius, _map_size_y);

	for (int _y = top; _y < bottom; _y++)
	{
		for (int _x = left; _x < right; _x++)
		{
			auto& tile = _map[_x + _y * _map_size_x];
			tile.second = fov_map.GetFOV(_x, _y);

			if (tile.second == true)
			{
				ok::Kripta::KriptaTile3D& _tile_3d = _tiles[tile.first];
				_tile_3d._x = _x;
				_tile_3d._y = _y;
				_tiles_to_render.push_back(_tile_3d);

				if (_tile_3d.mesh != nullptr)
				{
					total_vert_count += _tile_3d.mesh->GetVertexCount();
					total_tris_count += _tile_3d.mesh->GetTrianglesCount();
				}
			}
		}
	}

	if (total_vert_count != 0 && total_tris_count != 0) _nothing_to_render = false; else return;

	if (_merged_mesh == nullptr)
	{
		_merged_mesh = new ok::graphics::Mesh(total_vert_count, total_tris_count, 3);
		_merged_mesh->EnableNormals();
	}
	else
	{
		if (_merged_mesh->GetVertexCount() != total_vert_count ||
			_merged_mesh->GetTrianglesCount() != total_tris_count)
		{
			delete _merged_mesh;
			_merged_mesh = new ok::graphics::Mesh(total_vert_count, total_tris_count, 3);
			_merged_mesh->EnableNormals();
		}
	}

	float* vertices = _merged_mesh->GetVertices().vertices;
	int* indices = _merged_mesh->GetIndices().indices;
	float* uvs = _merged_mesh->GetUV(0).vertices;
	ok::Kripta::KriptaTileMap3D::duo_float* tile_xy = (ok::Kripta::KriptaTileMap3D::duo_float*)(_merged_mesh->GetUV(1).vertices);
	ok::Kripta::KriptaTileMap3D::duo_float* tile_uv_xy = (ok::Kripta::KriptaTileMap3D::duo_float*)(_merged_mesh->GetUV(2).vertices);
	float* normals = _merged_mesh->GetNormals().vertices;

	for (auto&& _tile_to_render : _tiles_to_render)
	{
		memcpy(vertices, _tile_to_render.mesh->GetVertices().vertices, sizeof(float) * 3 * _tile_to_render.mesh->GetVertexCount());
		vertices += 3 * _tile_to_render.mesh->GetVertexCount();

		memcpy(indices, _tile_to_render.mesh->GetIndices().indices, sizeof(int) * 3 * _tile_to_render.mesh->GetTrianglesCount());
		indices += 3 * _tile_to_render.mesh->GetTrianglesCount();

		memcpy(uvs, _tile_to_render.mesh->GetUV().vertices, sizeof(float) * 2 * _tile_to_render.mesh->GetVertexCount());
		uvs += 2 * _tile_to_render.mesh->GetVertexCount();

		ok::Kripta::KriptaTileMap3D::duo_float df;
		df.lpart = static_cast<float>(_tile_to_render._x);
		df.rpart = static_cast<float>(_tile_to_render._y);

		memcpy(tile_xy, &df, sizeof(ok::Kripta::KriptaTileMap3D::duo_float) * _tile_to_render.mesh->GetVertexCount());
		tile_xy += _tile_to_render.mesh->GetVertexCount();

		df.lpart = _tile_to_render.uv_shift.x;
		df.rpart = _tile_to_render.uv_shift.y;

		memcpy(tile_uv_xy, &df, sizeof(ok::Kripta::KriptaTileMap3D::duo_float) * _tile_to_render.mesh->GetVertexCount());
		tile_uv_xy += _tile_to_render.mesh->GetVertexCount();

		memcpy(normals, _tile_to_render.mesh->GetNormals().vertices, sizeof(float) * 3 * _tile_to_render.mesh->GetVertexCount());
		normals += 3 * _tile_to_render.mesh->GetVertexCount();
	}

	_merged_mesh->UpdateVertices();
	_merged_mesh->UpdateIndices();
	_merged_mesh->UpdateUV();
	_merged_mesh->UpdateNormals();
}

void ok::Kripta::KriptaTileMap3D::Render()
{
	if (_nothing_to_render == true) return;
	if (_merged_mesh == nullptr) return;

	_material->Bind(this);

	_merged_mesh->Bind();

	glDrawElements(GL_TRIANGLES, _merged_mesh->GetTrianglesCount() * 3, GL_UNSIGNED_INT, 0);

	_merged_mesh->Unbind();
}

void ok::Kripta::KriptaTileMap3D::SetMaterial(ok::graphics::Material * material)
{
	_material = material;
}

void ok::Kripta::KriptaTileMap3D::SetMaterial(ok::String material_asset)
{
	SetMaterial(ok::AssetsBasic::instance().GetMaterial(material_asset));
}

ok::graphics::Material * ok::Kripta::KriptaTileMap3D::GetMaterial()
{
	return _material;
}

void ok::Kripta::KriptaTileMap3D::Update(float dt)
{
	if (RenderingDisabled == true) return;

	if (gameObject().IsVisible())
		ok::graphics::LayeredRenderer::instance().Render(_renderer_request);
}

void ok::Kripta::KriptaTileMap3D::SetStage(ok::graphics::LayeredRendererStage stage)
{
	_renderer_request.stage = stage;
}

ok::graphics::LayeredRendererStage ok::Kripta::KriptaTileMap3D::GetStage()
{
	return _renderer_request.stage;
}

void ok::Kripta::KriptaTileMap3D::SetLayer(int layer)
{
	_renderer_request.layer = layer;
}

int ok::Kripta::KriptaTileMap3D::GetLayer()
{
	return _renderer_request.layer;
}

glm::mat4 ok::Kripta::KriptaTileMap3D::DispatchAliasMat4(ok::graphics::ShaderAliasReference alias_type)
{
	switch (alias_type)
	{
	case ok::graphics::ShaderAliasReference::ModelViewProjectionMatrix:
	{
		return ok::graphics::Camera::GetCurrent()->GetVPMatrix() * transform().GetAbsoluteTransformMatrix();
	}
	break;
	case ok::graphics::ShaderAliasReference::ViewProjectionMatrix:
	{
		return ok::graphics::Camera::GetCurrent()->GetVPMatrix();
	}
	break;
	case ok::graphics::ShaderAliasReference::ModelMatrix:
	{
		return transform().GetAbsoluteTransformMatrix();
	}
	break;
	}

	unresolved_alias = true;
	return glm::mat4(1.f);
}
