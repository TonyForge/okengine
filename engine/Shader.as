package com.engine 
{
	import com.adobe.utils.AGALMiniAssembler;
	import com.adobe.utils.OGSL;
	import com.adobe.utils.OGSLRegisterMapExternal;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Program3D;

	public class Shader 
	{
		//private var _stage3d_shader:Program3D = null;
		public var _stage3d_shader:Program3D = null;
		public var _ogsl_registers:OGSLRegisterMapExternal = null;
		private var _ogsl:OGSL = null;
		
		public var blend_source:String = Context3DBlendFactor.SOURCE_ALPHA;
		public var blend_destination:String = Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA;
		
		public var name:String = "";
		public var vertex_shader_source:String = "";
		public var fragment_shader_source:String = "";
		
		public function Shader() 
		{
			
		}
		
		public function Assemble():void
		{
			var vertexShaderAssembler:AGALMiniAssembler = new AGALMiniAssembler(false);
			var fragmentShaderAssembler:AGALMiniAssembler = new AGALMiniAssembler(false);
			
			vertexShaderAssembler.assemble(Context3DProgramType.VERTEX, vertex_shader_source);
			fragmentShaderAssembler.assemble(Context3DProgramType.FRAGMENT, fragment_shader_source);
			
			_stage3d_shader = Application.context3d.createProgram();
			_stage3d_shader.upload(vertexShaderAssembler.agalcode, fragmentShaderAssembler.agalcode);
		}
		
		public function OGSL_Assemble(_ogsl:OGSL):void
		{
			var vertexShaderAssembler:AGALMiniAssembler = new AGALMiniAssembler(false);
			var fragmentShaderAssembler:AGALMiniAssembler = new AGALMiniAssembler(false);
			
			vertexShaderAssembler.assemble(Context3DProgramType.VERTEX, vertex_shader_source, 2);
			fragmentShaderAssembler.assemble(Context3DProgramType.FRAGMENT, fragment_shader_source, 2);
			
			_stage3d_shader = Application.context3d.createProgram();
			_stage3d_shader.upload(vertexShaderAssembler.agalcode, fragmentShaderAssembler.agalcode);
			
			this._ogsl = _ogsl;
			_ogsl_registers = _ogsl.getExternalRegisterMap();
		}
		
		public function GetOGSL():OGSL
		{
			return _ogsl;
		}
		
		public function SetDefaultBlendMode():void
		{
			blend_source = Context3DBlendFactor.ONE;
			blend_destination = Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA;
		}
		
		public function get GetStage3DShader():Program3D
		{
			return _stage3d_shader;
		}
		
		public function _Internal_OGSL_Begin():void
		{
			_ogsl.externalRegisterMapBegin(_ogsl_registers);
		}
		
		public function _Internal_OGSL_End():void
		{
			_ogsl.externalRegisterMapEnd();
		}
	}

}