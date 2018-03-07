package com.engine 
{
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBufferUsage;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.VertexBuffer3D;

	public class AnimatedMeshRenderer 
	{
		private var _mesh:Mesh = null;
		
		private var vertices_count:int = 0;
		
		private var _vertices:Array = new Array();
		private var _uvs:VertexBuffer3D = null;
		private var _indices:IndexBuffer3D = null;
		
		private var _context3d:Context3D = null;
		
		private var _customShader:Shader = null;
		
		public function AnimatedMeshRenderer(mesh:Mesh, customShader:Shader = null) 
		{
			_context3d = Application.context3d;
			
			_mesh = mesh;

			vertices_count = (mesh.submesh_vertices[0] as Vector.<Number>).length / 3;
			
			_vertices[0] = _context3d.createVertexBuffer(vertices_count, 3, Context3DBufferUsage.DYNAMIC_DRAW);
			_vertices[1] = _context3d.createVertexBuffer(vertices_count, 3, Context3DBufferUsage.DYNAMIC_DRAW);
			_uvs = _context3d.createVertexBuffer((mesh.submesh_uv[0] as Vector.<Number>).length / 2, 2, Context3DBufferUsage.STATIC_DRAW);
			_indices = _context3d.createIndexBuffer((mesh.submesh_indices[0] as Vector.<uint>).length, Context3DBufferUsage.STATIC_DRAW);
			
			(_vertices[0] as VertexBuffer3D).uploadFromVector(mesh.submesh_vertices[0], 0, vertices_count);
			(_vertices[1] as VertexBuffer3D).uploadFromVector(mesh.submesh_vertices[0], 0, vertices_count);
			_uvs.uploadFromVector(mesh.submesh_uv[0], 0, (mesh.submesh_uv[0] as Vector.<Number>).length / 2);
			_indices.uploadFromVector(mesh.submesh_indices[0], 0, (mesh.submesh_indices[0] as Vector.<uint>).length);
			
			_customShader = customShader;
		}
		
		public function Render(_animation:String, progress:Number):void
		{
			var animation:Array = _get_animation(_animation);
			
			if (animation == null) throw new Error("AnimatedMeshRenderer: trying to play undefined animation!");
			
			var length:int = animation[1];
			
			var frame:Number;
			var frame_blend:Number;
			
			var left_frame:int;
			var right_frame:int;
			
			if (length <= 0) throw new Error("AnimatedMeshRenderer: trying to play animation with zero length!");
			else if (length == 1)
			{
				frame = 0;
				frame_blend = 0;
				left_frame = 0;
				right_frame = 0;
			}
			else
			{
				length--;
				
				frame = (length as Number) * progress;
				frame_blend = frame % 1;
				
				left_frame = Math.floor(frame);
				right_frame = left_frame + 1;
				
				if (left_frame > length) left_frame = left_frame % length;
				if (right_frame > length) right_frame = right_frame % length;
			}
			
			(_vertices[0] as VertexBuffer3D).uploadFromVector(animation[2 + left_frame], 0, vertices_count);
			(_vertices[1] as VertexBuffer3D).uploadFromVector(animation[2 + right_frame], 0, vertices_count);
			
			Application.instance.frame_buffer.Render_SetVertexShaderUniform_Number("BLEND0", frame_blend);
			
			var ogsl_shader:Shader = _customShader;
			
			if (ogsl_shader == null) ogsl_shader = Application.instance.frame_buffer.GetOGSLShaderInUse();
			
			if (ogsl_shader)
			{
				ogsl_shader._Internal_OGSL_Begin();
				
					ogsl_shader.GetOGSL().setVertexBuffer("POSITION0", _vertices[0], 0, Context3DVertexBufferFormat.FLOAT_3);
					ogsl_shader.GetOGSL().setVertexBuffer("POSITION1", _vertices[1], 0, Context3DVertexBufferFormat.FLOAT_3);
					
					ogsl_shader.GetOGSL().setVertexBuffer("TEXCOORD0", _uvs, 0, Context3DVertexBufferFormat.FLOAT_2);

				ogsl_shader._Internal_OGSL_End();
				
				_context3d.drawTriangles(_indices);
			}
		}
		
		public function RenderExt(animation_list:Array, time_list:Array):void
		{
			var blend_layers_count:int = animation_list.length;
			var i:int;
			var animation:Array = null;
			
			var length:int;
			var frame:Number;
			var frame_blend:Number;
			
			var left_frame:int;
			var right_frame:int;
			
			var ogsl_shader:Shader = Application.instance.frame_buffer.GetOGSLShaderInUse();
			
			if (ogsl_shader)
			{
				ogsl_shader._Internal_OGSL_Begin();
				
				ogsl_shader.GetOGSL().setVertexBuffer("TEXCOORD0", _uvs, 0, Context3DVertexBufferFormat.FLOAT_2);
				ogsl_shader.GetOGSL().setVertexBuffer("POSITION0", _vertices[0], 0, Context3DVertexBufferFormat.FLOAT_3);
				
				for (i = 0; i < blend_layers_count; i++)
				{
					if (_vertices[i * 2 + 1] == null)
					{
						_vertices[i * 2 + 1] = _context3d.createVertexBuffer(vertices_count, 3, Context3DBufferUsage.DYNAMIC_DRAW);
					}
					
					if (_vertices[i * 2 + 2] == null)
					{
						_vertices[i * 2 + 2] = _context3d.createVertexBuffer(vertices_count, 3, Context3DBufferUsage.DYNAMIC_DRAW);
					}
					
					animation = _get_animation(animation_list[i]);
					
					if (animation == null) throw new Error("AnimatedMeshRenderer: trying to play undefined animation!");
					
					length = animation[1];
					
					if (length <= 0) throw new Error("AnimatedMeshRenderer: trying to play animation with zero length!");
					else if (length == 1)
					{
						frame = 0;
						frame_blend = 0;
						left_frame = 0;
						right_frame = 0;
					}
					else
					{
						length--;
						
						frame = (length as Number) * (time_list[i] as Number);
						frame_blend = frame % 1;
						
						left_frame = Math.floor(frame);
						right_frame = left_frame + 1;
						if (left_frame > length) left_frame = left_frame % length;
						if (right_frame > length) right_frame = right_frame % length;
					}
					
					(_vertices[i * 2 + 1] as VertexBuffer3D).uploadFromVector(animation[2 + left_frame], 0, vertices_count);
					(_vertices[i * 2 + 2] as VertexBuffer3D).uploadFromVector(animation[2 + right_frame], 0, vertices_count);
					
					Application.instance.frame_buffer.Render_SetVertexShaderUniform_Number("BLEND" + i.toString(), frame_blend);
					
					ogsl_shader.GetOGSL().setVertexBuffer("POSITION"+((i*2+1) as int).toString(), _vertices[i * 2 + 1], 0, Context3DVertexBufferFormat.FLOAT_3);
					ogsl_shader.GetOGSL().setVertexBuffer("POSITION"+((i*2+2) as int).toString(), _vertices[i * 2 + 2], 0, Context3DVertexBufferFormat.FLOAT_3);
				}
				
				ogsl_shader._Internal_OGSL_End();
				
				_context3d.drawTriangles(_indices);
			}
		}
		
		private function _get_animation(animation_name:String):Array
		{
			var i:int;
			var animation:Array = null;
			
			for (i = 1; i <= _mesh.animations_count; i++)
			{
				animation = _mesh.submesh_vertices[i];
				if (animation[0] == animation_name)
				{
					return animation;
				}
			}
			
			return null;
		}
		
		public function Dispose():void
		{
			var i:int = 0;
			for (i = 0; i < _vertices.length; i++)
			{
				_vertices[i].dispose();
				_vertices[i] = null;
			}
			
			_uvs.dispose();
			_indices.dispose();
		}
	}

}