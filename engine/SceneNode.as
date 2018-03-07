package com.engine 
{
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class SceneNode 
	{
		public static const FILE_TYPE_XYZ : String = "xyz";
		
		public var children:Vector.<SceneNode> = new Vector.<SceneNode>();
		
		private var _parent:SceneNode = null;
		private var _index_inside_parent:int = -1;
		
		public var absolute_transform:MeshTransform = new MeshTransform();
		public var relative_transform:MeshTransform = new MeshTransform();
		
		public var name:String = "unnamed";
		
		private var _static_mesh_renderer:StaticMeshRenderer = null;
		private var _animated_mesh_renderer:AnimatedMeshRenderer = null;
		private var _shader:Shader = null;
		
		private var _textures:Vector.<Texture> = null;
		
		
		public function SceneNode() 
		{
			
		}
		
		public function Render():void
		{
			var fb:FrameBuffer = Application.instance.frame_buffer;
			var i:int = 0;
			var count:int = 0;
			
			if (_static_mesh_renderer != null || _animated_mesh_renderer != null)
			{
				fb.Render_SetShader(_shader);

				fb.Render_SetViewportTransform(0, 0, 1.0, 1.0);
				fb.Render_SetModelTransform(absolute_transform);
				fb.Render_SendVertexShaderTransform(true, true);
				
				count = _textures.length;
				
				for (i = 0; i < count; i++)
				{
					fb.Render_SetTexture(i, _textures[i], "Tex"+i.toString());
				}
				
				fb.Render_SetFaceCullingMode(FrameBuffer.FACE_CULLING_MODE_DISABLED);
				fb.Render_SetDepthBufferMode(FrameBuffer.DEPTH_BUFFER_MODE_READ_WRITE);
				
				if (_static_mesh_renderer)
				_static_mesh_renderer.Render();
				
				if (_animated_mesh_renderer)
				{
					//_animated_mesh_renderer.Render(animation, progress);
				}
				
				fb.Render_SetFaceCullingMode(FrameBuffer.FACE_CULLING_MODE_DEFAULT);
				fb.Render_SetDepthBufferMode(FrameBuffer.DEPTH_BUFFER_MODE_DEFAULT);
			}
			
			count = children.length;
			
			for (i = 0; i < count; i++)
			{
				children[i].Render();
			}
		}
		
		public function SetMesh(mesh:Mesh, shader:Shader, textures:Vector.<Texture>, mesh_type:String):void
		{
			if (mesh_type == Mesh.STATIC)
			{
				if (_static_mesh_renderer) _static_mesh_renderer.Dispose();
				_static_mesh_renderer = new StaticMeshRenderer(mesh.submesh_vertices[0], mesh.submesh_uv[0], mesh.submesh_indices[0], shader);
			}
			
			if (mesh_type == Mesh.VERTEX_ANIMATED)
			{
				if (_animated_mesh_renderer) _animated_mesh_renderer.Dispose();
				_animated_mesh_renderer = new AnimatedMeshRenderer(mesh, shader);
			}
			
			_textures = textures;
			_shader = shader;
		}
		
		public function LoadFromFile(path:String, type:String = FILE_TYPE_XYZ):void
		{
			var file:ByteArray = Assets.instance.GetFileAsByteArray(path);
			file.endian = Endian.LITTLE_ENDIAN;

			
			if (type == FILE_TYPE_XYZ)
			_LoadFromFile_XYZ(file);
		}
		
		private function _LoadFromFile_XYZ(file:ByteArray):void
		{
			var count:int = file.readInt();
			var i:int = 0;
			
			name = "root";
			
			var child:SceneNode;
			
			for (i = 0; i < count; i++)
			{
				child = new SceneNode();
				
				child.name = file.readUTFBytes(file.readInt());
				child.SetRelativeTranslation(file.readFloat(), file.readFloat(), file.readFloat(), false);
				child.AttachToParent(this);
			}
			
			UpdateAbsoluteTransform();
		}
		
		public function CallRecursive(fnc:Function, args:Array):void
		{
			fnc.apply(this, args);
			
			var i:int = 0;
			var count:int = children.length;
			
			for (i = 0; i < count; i++)
			{
				fnc.apply(children[i], args);
				children[i].CallRecursive(fnc, args);
			}
		}
		
		public function SetRelativeTranslation(x:Number, y:Number, z:Number, update_absolute_transform:Boolean = true):void
		{
			relative_transform.position.x = x;
			relative_transform.position.y = y;
			relative_transform.position.z = z;
			
			if (update_absolute_transform)
			UpdateAbsoluteTransform();
		}
		
		public function SetRelativeRotation(x:Number, y:Number, z:Number, update_absolute_transform:Boolean = true):void
		{
			relative_transform.SetEulerAngles(x, y, z);
			
			if (update_absolute_transform)
			UpdateAbsoluteTransform();
		}
		
		public function SetRelativeScale(x:Number, y:Number, z:Number, update_absolute_transform:Boolean = true):void
		{
			relative_transform.scale.x = x;
			relative_transform.scale.y = y;
			relative_transform.scale.z = z;
			
			if (update_absolute_transform)
			UpdateAbsoluteTransform();
		}
		
		public function SetRelativeTRS(translate_x:Number, translate_y:Number, translate_z:Number,
									   angle_x:Number, 	   angle_y:Number, 	   angle_z:Number,
									   scale_x:Number, 	   scale_y:Number, 	   scale_z:Number):void
		{
			relative_transform.position.x = translate_x;
			relative_transform.position.y = translate_y;
			relative_transform.position.z = translate_z;
			relative_transform.SetEulerAngles(angle_x, angle_y, angle_z);
			relative_transform.scale.x = scale_x;
			relative_transform.scale.y = scale_y;
			relative_transform.scale.z = scale_z;
			
			UpdateAbsoluteTransform();
		}
		
		public function UpdateAbsoluteTransform():void
		{
			if (_parent)
			{
				absolute_transform.SetToCombination_Hierarchical(_parent.absolute_transform, relative_transform, true);
			}
			else
			{
				absolute_transform.copyFrom(relative_transform);//.SetToCombination(absolute_transform, relative_transform, true);
			}
			
			var i:int = 0;
			var count:int = children.length;
			
			for (i = 0; i < count; i++)
			{
				children[i].UpdateAbsoluteTransform();
			}
		}
		
		public function ConnectChild(child:SceneNode, keep_transform:Boolean = true):void
		{
			DisconnectChild(child, keep_transform);
			child.AttachToParent(this);
			UpdateAbsoluteTransform();
		}
		
		public function DisconnectChild(child:SceneNode, keep_transform:Boolean = true):void
		{
			child.DetachFromParent();
			
			if (keep_transform)
			{
				relative_transform.copyFrom(absolute_transform);
			}
			else
			{
				absolute_transform.copyFrom(relative_transform);
			}
			
			UpdateAbsoluteTransform();
		}
		
		public function AttachToParent(parent:SceneNode):void
		{
			_parent = parent;
			_parent.children.push(this);
			_index_inside_parent = _parent.children.length-1;
		}
		
		public function DetachFromParent():void
		{
			if (_parent)
			{
				_parent.children = _parent.children.splice(_index_inside_parent, 1);
				_parent = null;
				_index_inside_parent = -1;
			}
		}
	}

}