package com.engine 
{
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBufferUsage;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.VertexBuffer3D;
	
	public class StaticMeshRenderer 
	{
		
		private var _vertices:VertexBuffer3D = null;
		private var _uvs:VertexBuffer3D = null;
		private var _indices:IndexBuffer3D = null;
		
		private var _context3d:Context3D = null;
		
		private var _customShader:Shader = null;
		
		public function StaticMeshRenderer(vertices:Vector.<Number>, uvs:Vector.<Number>, indices:Vector.<uint>, customShader:Shader = null) 
		{
			_context3d = Application.context3d;
			
			_vertices = _context3d.createVertexBuffer(vertices.length / 3, 3, Context3DBufferUsage.STATIC_DRAW);
			_uvs = _context3d.createVertexBuffer(uvs.length / 2, 2, Context3DBufferUsage.STATIC_DRAW);
			_indices = _context3d.createIndexBuffer(indices.length, Context3DBufferUsage.STATIC_DRAW);
			
			_vertices.uploadFromVector(vertices, 0, vertices.length / 3);
			_uvs.uploadFromVector(uvs, 0, uvs.length / 2);
			_indices.uploadFromVector(indices, 0, indices.length);
			
			_customShader = customShader;
		}
		
		public function Render():void
		{
			var ogsl_shader:Shader = _customShader;
			
			if (ogsl_shader == null) ogsl_shader = Application.instance.frame_buffer.GetOGSLShaderInUse();
			
			if (ogsl_shader)
			{
				ogsl_shader._Internal_OGSL_Begin();
				
					ogsl_shader.GetOGSL().setVertexBuffer("POSITION0", _vertices, 0, Context3DVertexBufferFormat.FLOAT_3);
					ogsl_shader.GetOGSL().setVertexBuffer("TEXCOORD0", _uvs, 0, Context3DVertexBufferFormat.FLOAT_2);

				ogsl_shader._Internal_OGSL_End();
				
				_context3d.drawTriangles(_indices);
			}
		}
		
		public function Dispose():void
		{
			_vertices.dispose();
			_uvs.dispose();
			_indices.dispose();
		}
	}

}