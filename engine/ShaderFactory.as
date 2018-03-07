package com.engine 
{
	import com.adobe.utils.OGSL;
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	
	public class ShaderFactory 
	{
		private var _ogsl:OGSL = null;
		private var _context3d:Context3D = null;
		private var shaders:Array = new Array();
		
		public function ShaderFactory() 
		{
			if (instance != null)
			{
				throw new Error("ShaderFactory is a singleton, use ShaderFactory.instance");
			} else {
				_context3d = Application.context3d;
				shaders[0] = _MakeDefaultShader();
			}
		}
		
		public function GetDefaultShader():Shader
		{
			return shaders[0] as Shader;
		}
		
		public function GetShader(shader_name:String):Shader
		{
			for each (var shader:Shader in shaders)
			{
				if (shader.name == shader_name)
				{
					return shader;
				}
			}
			
			return null;
		}
		
		public function AddShader(shader:Shader):void
		{
			shaders.push(shader);
		}
		
		private function _MakeDefaultShader():Shader
		{
			var shader:Shader = new Shader();
			shader.name = "default";
			
			shader.vertex_shader_source = 	"dp4 op.x, va0, vc0 	\n"+
											"dp4 op.y, va0, vc1 	\n"+
											"mov op.z, vc2.z    	\n"+
											"mov op.w, vc3.w    	\n"+
											"mov v0.xyz, va1.xyz	\n"+
											"mov v0.w, va0.z";
											
			shader.fragment_shader_source = "tex ft0, v0, fs0 <2d, clamp, linear, nomip>	\n"+
											"mul ft0.a, ft0.a, v0.w							\n"+	
											"mov oc, ft0";
			
											
			shader.Assemble();
			
			return shader;
		}
		
		//---------------------------------------------------------
		
		public static var instance:ShaderFactory = null;
		
		public static function create():void
		{
			if (instance == null)
			{
				instance = new ShaderFactory();
			}
		}
		
		//---------------------------------------------------------
				
		public function OGSL_Enable():void
		{
			if (_ogsl == null) 
			{
				_ogsl = new OGSL();
				_ogsl.setContext3D(_context3d);
			}
		}
		
		public function OGSL_Compile(ogsl_code:String):Shader
		{
			var shader:Shader = null;
			
			if (_ogsl)
			{
				_ogsl.compile(ogsl_code);
				
				if (_ogsl.isCompiled())
				{
					shader = new Shader();
					shader.vertex_shader_source = _ogsl.getVertexAGAL();
					shader.fragment_shader_source = _ogsl.getFragmentAGAL();
					shader.OGSL_Assemble(_ogsl);
				}
				else
				{
					
				}
			}
			
			return shader;
		}
	}
}