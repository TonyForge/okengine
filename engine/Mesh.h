#pragma once

#include "GLBufferObject.h"

namespace ok
{
	namespace graphics
	{

		enum class MorphingTargetType
		{
			Positions,
			Positions_Normals
		};

		enum class NormalsCalculationMethod
		{
			Faceted = 0,
			Smooth
		};

		class MeshTriangle
		{
		public:
			int vertex_indexes[3];
			glm::vec3 centroid_position;
		};

		class Mesh
		{
		public:
			Mesh(int vertex_count, int triangles_count, int texcoords_channels_count = 1);
			void EnableNormals();
			void EnableTrianglesSort();
			void SortTriangles();
			void CalculateNormals(ok::graphics::NormalsCalculationMethod method = ok::graphics::NormalsCalculationMethod::Smooth);
			GLBufferInfo GetVertices();
			void UpdateVertices();
			GLBufferInfo GetUV(int channel = 0);
			void UpdateUV(int channel = -1);
			GLBufferInfo GetNormals();
			void UpdateNormals();
			GLBufferInfo GetIndices(int index = 0);
			void UpdateIndices();
			void SwitchIndices(int index);
			int GetTrianglesCount();
			int GetVertexCount();

			void EnableMorphDeformer(int targets_count, ok::graphics::MorphingTargetType targets_type, bool hardware);
			void EnableSkinDeformer(int bones_count, bool hardware);
			void ApplyDeformers();

			void AssignMorphTarget(
				int target_index,
				float blend,
				float* positions = nullptr,
				float* normals = nullptr
			);

			void AssignMorphTarget(
				int target_index,
				float left_to_right_blend,
				float blend,
				std::pair<float*, float*> left,
				std::pair<float*, float*> right
			);

			void UploadSkinWeights(float* weights);
			void UploadSkinIndexes(int* indexes);
			void AssignSkinBones(float* bones, int* mapping_table, bool hwd_indexes_force_remap = false);
			std::vector<glm::mat4>& GetSkinBones();

			float& GetMorphBlends(int target_index);

			void Bind();
			void Unbind();
		protected:
		private:
			int _texcoords_channels_count;

			bool _morph_normals_enabled;

			int _skin_vb_index;
			int _morph_targets_vb_index;
			int _normals_vb_index;

			int _vertex_count;
			int _triangles_count;
			int _morph_targets_count;

			bool _triangles_sort_enabled, _normals_enabled;
			bool _morph_enabled, _hardware_morph_enabled, _software_morph_enabled;
			bool _skin_enabled, _hardware_skin_enabled, _software_skin_enabled;
			bool _morph_dirty;

			GLBufferObject buffer;
			void _SortTrianglesAlongAxis(glm::vec3 axis, std::vector<ok::graphics::MeshTriangle> & in_triangles, GLBufferInfo & out_indices);
			void _ApplyMorphDeformer();
			void _ApplySkinDeformer();

			std::vector<float*> _morph_positions;
			std::vector<float*> _morph_normals;
			std::vector<float> _morph_blends;

			std::vector<float> _morph_software_buffer_positions;
			std::vector<float> _morph_software_buffer_normals;

			std::vector<glm::mat4> _skin_bones;

			std::vector<float> _skin_software_buffer_positions;
			std::vector<float> _skin_software_buffer_normals;

			std::vector<float> _skin_software_weights;
			std::vector<int> _skin_software_indexes;
			std::vector<float> _skin_hardware_indexes;
		};
	}
}