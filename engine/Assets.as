package com.engine 
{
	import flash.display.BitmapData;
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class Assets 
	{
		public static var root_folder:String = "";
		
		private var textures:Object = new Object();
		private var textures_length:int = 0;
		
		private var meshes:Object = new Object();
		private var meshes_length:int = 0;
		
		private var bytefiles:Object = new Object();
		private var bytefiles_length:int = 0;
		
		public function Assets() 
		{
			if (instance != null)
			{
				throw new Error("Assets is a singleton, use Assets.instance");
			} else {
				root_folder =  Application.working_folder + Application.project_data_folder + "assets\\";
			}
		}
		
		public function GetFullPathToAsset(path:String):String
		{
			if (path.charAt(0) == '.' && (path.charAt(1) == '/' || path.charAt(1) == '\\'))
			{
				return Application.working_folder+path.substr(2, path.length - 2);
			}
			
			return root_folder + path;
		}
		
		public function GetMesh(path:String, type:String = null):Mesh
		{
			var result:Mesh = meshes[path] as Mesh;
			
			if (result == null)
			{
				var bytes:ByteArray = _LoadFileToByteArray(path);
				if (bytes.length == 0) return null;
				
				bytes.endian = Endian.LITTLE_ENDIAN;
				
				var vertices_count:int = bytes.readInt();
				var indices_count:int = bytes.readInt();
				var uv_count:int = bytes.readInt();
				
				//Read Base Mesh
				var mesh_vertices:Vector.<Number> = new Vector.<Number>(vertices_count*3, true);
				var mesh_indices:Vector.<uint> = new Vector.<uint>(indices_count, true);
				var mesh_uv:Vector.<Number> = new Vector.<Number>(uv_count*2, true);
				
				var i:int = 0;
				var j:int = 0;
				var k:int = 0;
				
				result = new Mesh();
				
				for (i = 0; i < vertices_count*3; i++)
				{
					mesh_vertices[i] = bytes.readFloat();
				}
				
				for (i = 0; i < indices_count; i++)
				{
					mesh_indices[i] = bytes.readInt();
				}
				
				for (i = 0; i < uv_count*2; i++)
				{
					mesh_uv[i] = bytes.readFloat();
				}
				
				result.submesh_vertices[0] = mesh_vertices;
				result.submesh_indices[0] = mesh_indices;
				result.submesh_uv[0] = mesh_uv;
				
				if (type == Mesh.STATIC)
				{
					result.mesh_type = 0;
				}
				
				if (type == Mesh.VERTEX_ANIMATED)
				{
					result.mesh_type = 1;
					
					var animations_count:int = bytes.readInt();
					var animation_name:String = "";
					var animation_length:int = 0;
					
					result.animations_count = animations_count;
					
					for (i = 0; i < animations_count; i++)
					{
						result.submesh_vertices[1 + i] = new Array();
						
						animation_name = bytes.readUTFBytes(bytes.readInt());
						animation_length = bytes.readInt();
						
						result.submesh_vertices[1 + i][0] = animation_name;
						result.submesh_vertices[1 + i][1] = animation_length;
						
						for (j = 0; j < animation_length; j++)
						{
							mesh_vertices = new Vector.<Number>(vertices_count * 3, true);
							
							for (k = 0; k < vertices_count*3; k++)
							{
								mesh_vertices[k] = bytes.readFloat();
							}
							
							result.submesh_vertices[1 + i][2+j] = mesh_vertices;
						}
					}
				}
				
				if (type == Mesh.BONE_ANIMATED)
				{
					result.mesh_type = 2;
				}
				
				bytes.clear();
				
				meshes[path] = result;
				meshes_length++;
			}
			
			return result;
		}
		
		//type Texture.HARDWARE, Texture.SOFTWARE, Texture.MIXED
		public function GetTexture(path:String, type:String = null, force_reload:Boolean = false):Texture
		{
			if (type == null) type = Texture.HARDWARE;
			
			var result:Texture = textures[path] as Texture;
			
			if (result == null || force_reload == true)
			{
				var bytes:ByteArray = _LoadFileToByteArray(path);
				
				var width:int = bytes.readInt();
				var height:int = bytes.readInt();
				bytes.position = 0;
				
				result = new Texture(type, width, height, bytes);
				
				bytes.clear();
				
				textures[path] = result;
				textures_length++;
			}
			
			
			return result;
		}
		
		public function GetFileAsText(path:String):String
		{
			var bytes:ByteArray = _LoadFileToByteArray(path);
			return bytes.toString();
		}
		
		public function GetFileAsByteArray(path:String, force_reload:Boolean = false):ByteArray
		{
			var result:ByteArray = bytefiles[path] as ByteArray;
			
			if (result == null || force_reload == true)
			{
				result = _LoadFileToByteArray(path);
				
				bytefiles[path] = result;
				bytefiles_length++;
			} else {
				result.position = 0;
			}
			
			return result;
		}
		
		private function _LoadFileToByteArray(path:String):ByteArray
		{
			var f:File = null;
			var fs:FileStream = null;
			
			if (path.charAt(0) == '.' && (path.charAt(1) == '/' || path.charAt(1) == '\\'))
			{
				f = new File(Application.working_folder+path.substr(2, path.length - 2));
			}
			else
			{
				f = new File(root_folder + path);
			}
			
			if (!f.exists) return null;
			
			fs = new FileStream();
			fs.open(f, FileMode.READ);
			
			var result:ByteArray = new ByteArray();
			fs.readBytes(result);
			
			fs.close();
			
			return result;
		}
		
		//---------------------------------------------------------
		
		public static var instance:Assets = null;
		
		public static function create():void
		{
			if (instance == null)
			{
				instance = new Assets();
			}
		}
	}

}