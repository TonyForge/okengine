package com.engine 
{
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBufferUsage;
	import flash.display3D.Context3DVertexBufferFormat;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.VertexBuffer3D;

	public class TileBatch 
	{
		private var tiles_in_xy:int = 0;
		
		private var _attr_vertices:Vector.<Number> = null;
		private var _attr_indices:Vector.<uint> = null;
		private var _attr_tiles:Vector.<Number> = null;
		
		private var _vertexBuffer:VertexBuffer3D = null;
		private var _indexBuffer:IndexBuffer3D = null;
		private var _tilesBuffer:VertexBuffer3D = null;

		private var _context3d:Context3D = null;
		private var _tilesBuffer_dirty:Boolean = false;
		
		public static var _default_shader:Shader = null;
		private var _shader:Shader = null;
		
		public function TileBatch(tiles_in_x:int, tiles_in_y:int, tile_size:int)
		{
			_context3d = Application.context3d;
			
			tiles_in_xy = tiles_in_x * tiles_in_y;
			
			_attr_vertices = new Vector.<Number>(tiles_in_xy * 4*2, true);
			_attr_indices = new Vector.<uint>(tiles_in_xy * 6, true);
			_attr_tiles = new Vector.<Number>(tiles_in_xy * 4*2, true);
			
			_vertexBuffer = _context3d.createVertexBuffer(tiles_in_xy * 4, 2, Context3DBufferUsage.STATIC_DRAW);
			_indexBuffer = _context3d.createIndexBuffer(tiles_in_xy * 6, Context3DBufferUsage.STATIC_DRAW);
			_tilesBuffer = _context3d.createVertexBuffer(tiles_in_xy * 4, 2, Context3DBufferUsage.DYNAMIC_DRAW);
			
			var i:int = 0;
			var ix6:int = 0;
			var ix4x2:int = 0;
			var ix4:int = 0;
			
			var tx:int = 0;
			var ty:int = 0;
			
			for (i = 0; i < tiles_in_xy; i++)
			{
				ix6 = i * 6;
				ix4x2 = i * 4*2;
				ix4 = i * 4;
				
				_attr_indices[ix6	 ] = ix4;
				_attr_indices[ix6 + 1] = ix4 + 1;
				_attr_indices[ix6 + 2] = ix4 + 2;
				_attr_indices[ix6 + 3] = ix4;
				_attr_indices[ix6 + 4] = ix4 + 2;
				_attr_indices[ix6 + 5] = ix4 + 3;
				
				
				_attr_vertices[ix4x2	] = (tx+0)*tile_size;
				_attr_vertices[ix4x2 + 1] = (ty+0)*tile_size;
				
				_attr_vertices[ix4x2 + 2] = (tx+1)*tile_size;
				_attr_vertices[ix4x2 + 3] = (ty+0)*tile_size;
				
				_attr_vertices[ix4x2 + 4] = (tx+1)*tile_size;
				_attr_vertices[ix4x2 + 5] = (ty+1)*tile_size;
				
				_attr_vertices[ix4x2 + 6] = (tx+0)*tile_size;
				_attr_vertices[ix4x2 + 7] = (ty+1)*tile_size;
				
				tx++;
				if (tx >= tiles_in_x)
				{
					tx = 0;
					ty++;
				}
				
				_attr_tiles[ix4x2] = 0;
				_attr_tiles[ix4x2 + 1] = 0;
				_attr_tiles[ix4x2 + 2] = 1;
				_attr_tiles[ix4x2 + 3] = 0;
				
				_attr_tiles[ix4x2 + 4] = 1;
				_attr_tiles[ix4x2 + 5] = 1;
				_attr_tiles[ix4x2 + 6] = 0;
				_attr_tiles[ix4x2 + 7] = 1;
			}
			
			_indexBuffer.uploadFromVector(_attr_indices, 0, tiles_in_xy * 6); //upload full buffer necessary! maybe some sort of initialization of memory at gpu
			_vertexBuffer.uploadFromVector(_attr_vertices, 0, tiles_in_xy * 4); //upload full buffer necessary! maybe some sort of initialization of memory at gpu
			_tilesBuffer.uploadFromVector(_attr_tiles, 0, tiles_in_xy * 4);  //upload full buffer necessary! maybe some sort of initialization of memory at gpu
			
			
			if (_default_shader == null)
			{
				_default_shader = ShaderFactory.instance.OGSL_Compile(Assets.instance.GetFileAsText("./system/tile_batch.ogsl"));
				_default_shader.name = "__tile_batch_default_shader";
			}
			
			_shader = _default_shader;
		}
		
		public function Render():void
		{
			var context3d:Context3D = _context3d;
			
			var fb:FrameBuffer = Application.instance.frame_buffer;
			
			fb.Render_SetShader(_shader);

			fb.Render_SetViewportTransform(0, 0, 1.0, 1.0);
			fb.Render_SendVertexShaderTransform(true, false);
			
			if (_tilesBuffer_dirty)
			{
				_tilesBuffer.uploadFromVector(_attr_tiles, 0, tiles_in_xy * 4);
				_tilesBuffer_dirty = false;
			}
			
			if (_shader)
			{
				_shader._Internal_OGSL_Begin();
					_shader.GetOGSL().setVertexBuffer("POSITION", _vertexBuffer, 0, Context3DVertexBufferFormat.FLOAT_2);
					_shader.GetOGSL().setVertexBuffer("TILE", _tilesBuffer, 0, Context3DVertexBufferFormat.FLOAT_2);
				_shader._Internal_OGSL_End();
			}
			
			fb.Render_SetVertexShaderUniform_Vec2("tex_size", 512, 512);
			
			fb.Render_SetFaceCullingMode(FrameBuffer.FACE_CULLING_MODE_DISABLED);
			fb.Render_SetDepthBufferMode(FrameBuffer.DEPTH_BUFFER_MODE_IGNORE);
			
			context3d.drawTriangles(_indexBuffer, 0, tiles_in_xy * 2);
			
			fb.Render_SetFaceCullingMode(FrameBuffer.FACE_CULLING_MODE_DEFAULT);
			fb.Render_SetDepthBufferMode(FrameBuffer.DEPTH_BUFFER_MODE_DEFAULT);
		}
	}

}