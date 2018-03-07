package com.engine 
{
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBufferUsage;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.VertexBuffer3D;

	/*
	 * Спрайты могут иметь любые, даже не целые, координаты, быть под любым углом и использовать масштабирование.
	 * Не требуют точной отрисовки.
	 * Постоянно меняются, поэтому весь батч перестраивается и отправляется на GPU в каждом фрейме.
	 * */
	
	public class SpriteBatch 
	{
		private var _vertices:Vector.<Number> = null;
		private var _indices:Vector.<uint> = null;
		private var _uvs:Vector.<Number> = null;
		
		
		private var _vertexBuffer:VertexBuffer3D = null;
		private var _indexBuffer:IndexBuffer3D = null;
		private var _uvBuffer:VertexBuffer3D = null;
		
		private var batch_size:int;
		private var batch_current_index:int;
		
		private static var quad:Quad = new Quad();
		private static const quad_vertex_data_length:int = 12; //4 * 3
		private static const quad_index_data_length:int = 6; //2 * 3
		private static const quad_uv_data_length:int = 8; //4 * 2
		
		private var _context3d:Context3D = null;
		
		public function SpriteBatch(size:int = 1000) 
		{
			_context3d = Application.context3d;
			
			batch_size = size;
			_vertices = new Vector.<Number>(batch_size * quad_vertex_data_length, true); //4 vertices per sprite, 3 values per vertex
			_indices = new Vector.<uint>(batch_size * quad_index_data_length, true); //2 triangles per sprite, 3 indices per triangle
			_uvs = new Vector.<Number>(batch_size * quad_uv_data_length, true); //4 uvs per sprite, 2 values per uv
			
			_vertexBuffer = _context3d.createVertexBuffer(_vertices.length / 3, 3, Context3DBufferUsage.DYNAMIC_DRAW);
			_indexBuffer = _context3d.createIndexBuffer(_indices.length, Context3DBufferUsage.STATIC_DRAW);
			_uvBuffer = _context3d.createVertexBuffer(_uvs.length / 2, 2, Context3DBufferUsage.DYNAMIC_DRAW);

			//indexes won't change
			_BuildIndexes();
			_indexBuffer.uploadFromVector(_indices, 0, _indices.length); //upload full buffer necessary! maybe some sort of initialization of memory at gpu
			_vertexBuffer.uploadFromVector(_vertices, 0, _vertices.length / 3); //upload full buffer necessary! maybe some sort of initialization of memory at gpu
			_uvBuffer.uploadFromVector(_uvs, 0, _uvs.length / 2);  //upload full buffer necessary! maybe some sort of initialization of memory at gpu
		}
		
		public function Begin():void
		{
			batch_current_index = 0;
		}
		
		public function End():void
		{
			var context3d:Context3D = _context3d;
			
			if (batch_current_index == 0) return;
			
			_vertexBuffer.uploadFromVector(_vertices, 0, batch_current_index * 4);
			_uvBuffer.uploadFromVector(_uvs, 0, batch_current_index * 4);
			
			var ogsl_shader:Shader = Application.instance.frame_buffer.GetOGSLShaderInUse();
			
			if (ogsl_shader)
			{
				ogsl_shader._Internal_OGSL_Begin();
					ogsl_shader.GetOGSL().setVertexBuffer("POSITION", _vertexBuffer, 0, Context3DVertexBufferFormat.FLOAT_3);
					ogsl_shader.GetOGSL().setVertexBuffer("TEXCOORD0", _uvBuffer, 0, Context3DVertexBufferFormat.FLOAT_2);
				ogsl_shader._Internal_OGSL_End();
			}
			else
			{
				context3d.setVertexBufferAt(0, _vertexBuffer, 0, Context3DVertexBufferFormat.FLOAT_3);
				context3d.setVertexBufferAt(1, _uvBuffer, 0, Context3DVertexBufferFormat.FLOAT_2);
			}
			
			context3d.drawTriangles(_indexBuffer, 0, batch_current_index * 2);
		}
		
		public function DrawSprite(x:Number, y:Number, spr:SpriteInfo, angle_deg:Number = 0.0, scale:Number = 1.0):void
		{
			var qd:Quad = quad;
			
			qd.Reset(spr.Width, spr.Height);
			qd.Transform(spr.HotspotX, spr.HotspotY, x, y, angle_deg, scale, scale);
			
			_PushQuad(spr);
			_Next();
		}
		
		//----------------------------------------------------------------------
		
		//Private Methods
		
		private function _Next():void
		{
			batch_current_index++;
			if (batch_current_index >= batch_size)
			{
				End();
				Begin();
			}
		}
		
		private function _PushQuad(spr:SpriteInfo):void
		{
			var qd:Quad = quad;
			var offset:int = batch_current_index * quad_vertex_data_length;
			var qd_verts:Vector.<Number> = qd.verts;
			var alpha:Number = spr.Opacity;
			
			//VERTEX POSITION
			
				//left top corner vertex
				_vertices[offset] = qd_verts[0];
				_vertices[offset + 1] = qd_verts[1];
				_vertices[offset + 2] = alpha;
				
				//right top corner vertex
				_vertices[offset + 3] = qd_verts[2];
				_vertices[offset + 4] = qd_verts[3];
				_vertices[offset + 5] = alpha;
				
				//right bottom corner vertex
				_vertices[offset + 6] = qd_verts[4];
				_vertices[offset + 7] = qd_verts[5];
				_vertices[offset + 8] = alpha;
				
				//left bottom corner vertex
				_vertices[offset + 9] = qd_verts[6];
				_vertices[offset + 10] = qd_verts[7];
				_vertices[offset + 11] = alpha;
			
			//VERTEX UV
			
			offset = batch_current_index * quad_uv_data_length;
			
				//left top corner vertex
				_uvs[offset] = spr.UV_L;
				_uvs[offset + 1] = spr.UV_T;
				
				//right top corner vertex
				_uvs[offset + 2] = spr.UV_R;
				_uvs[offset + 3] = spr.UV_T;
				
				//right bottom corner vertex
				_uvs[offset + 4] = spr.UV_R;
				_uvs[offset + 5] = spr.UV_B;
				
				//left bottom corner vertex
				_uvs[offset + 6] = spr.UV_L;
				_uvs[offset + 7] = spr.UV_B;
		}
		
		private function _BuildIndexes():void
		{
			var offset_vertex:uint;
			var offset_index:int;
			var i:int;
			
			for (i = 0; i < batch_size; i++)
			{
				offset_vertex = i * 4;
				offset_index = i * quad_index_data_length;
				
				//first triangle
				_indices[offset_index] = offset_vertex;
				_indices[offset_index + 1] = offset_vertex + 1;
				_indices[offset_index + 2] = offset_vertex + 2;
				
				//second triangle
				_indices[offset_index + 3] = offset_vertex;
				_indices[offset_index + 4] = offset_vertex + 2;
				_indices[offset_index + 5] = offset_vertex + 3;
			}
		}
	}

}