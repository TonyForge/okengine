package com.engine 
{
	public class Mesh 
	{
		//mesh types
		public static const STATIC : String = "static"; //single non animated mesh
		public static const VERTEX_ANIMATED : String = "vam"; //vertex animated mesh (morphing)
		public static const BONE_ANIMATED : String = "bam"; //skin+skeleton animated mesh (skeletal)
		
		/*public static const VBO_MODE_SINGLE : String = "single";
		public static const VBO_MODE_MULTI : String = "multi";*/
		
		public var submesh_vertices:Array = new Array();
		public var submesh_indices:Array = new Array();
		public var submesh_uv:Array = new Array();
		
		public var mesh_type:int = 0; //0 - static, 1 - vertex animated, 2 - bone animated
		
		public var animations_count:int = 0;
		
		public function Mesh() 
		{
		}
	}

}