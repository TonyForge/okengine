package com.engine 
{
	import com.adobe.utils.OGSL;
	import flash.display.BlendMode;
	import flash.display.Stage;
	import flash.display.Stage3D;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DClearMask;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProfile;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DRenderMode;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.Program3D;
	import flash.events.ErrorEvent;
	import flash.events.Event;
	import flash.geom.Matrix3D;
	
	public class FrameBuffer 
	{
		public static const DEPTH_BUFFER_MODE_IGNORE:int = 0;
		public static const DEPTH_BUFFER_MODE_READ_WRITE:int = 1;
		public static const DEPTH_BUFFER_MODE_READ_ONLY:int = 2;
		public static const DEPTH_BUFFER_MODE_WRITE_ONLY:int = 3;
		
		public static const DEPTH_BUFFER_MODE_DEFAULT:int = DEPTH_BUFFER_MODE_IGNORE;
		
		public static const FACE_CULLING_MODE_DISABLED:int = 0;
		public static const FACE_CULLING_MODE_BACK:int = 1;
		public static const FACE_CULLING_MODE_FRONT:int = 2;
		
		public static const FACE_CULLING_MODE_DEFAULT:int = FACE_CULLING_MODE_BACK;
		
		
		private var stage3d:Stage3D = null;
		private var context3d:Context3D = null;
		
		private var buffer_width:int;
		private var buffer_height:int;
		
		private var buffer_width_inv:Number;
		private var buffer_height_inv:Number;
		
		private var buffer2screen_scale_x:Number = 1.0;
		private var buffer2screen_scale_y:Number = 1.0;
		
		private var buffer_enabled:Boolean = false;
		
		private var render_m4x4_Transform : Matrix3D = new Matrix3D();
		private var render_m4x4_Projection : Matrix3D = new Matrix3D();
		private var render_m4x4_Viewport : Matrix3D = new Matrix3D();
		private var render_m4x4_Model : Matrix3D = new Matrix3D();
		
		private var _ogsl:OGSL = null;
		private var _ogsl_shader:Shader = null;
		
		public function FrameBuffer(stage:Stage, width:int, height:int) 
		{
			stage3d = stage.stage3Ds[0] as Stage3D;
			stage3d.addEventListener(Event.CONTEXT3D_CREATE, OnContext3DCreate);
			stage3d.requestContext3D(Context3DRenderMode.AUTO, Context3DProfile.STANDARD);
			
			buffer_width = width;
			buffer_height = height;
			buffer_width_inv = 1.0 / buffer_width;
			buffer_height_inv = 1.0 / buffer_height;
		}
		
		private function OnContext3DCreate(e:Event):void
		{
			stage3d.removeEventListener(Event.CONTEXT3D_CREATE, OnContext3DCreate);
			
			context3d = stage3d.context3D;
			
			var final_width:int = buffer_width * buffer2screen_scale_x;
			var final_height:int = buffer_height * buffer2screen_scale_y;
			
			if (final_width < 32) final_width = 32;
			if (final_height < 32) final_height = 32;
			
			if (final_width > context3d.maxBackBufferWidth) final_width = context3d.maxBackBufferWidth;
			if (final_height > context3d.maxBackBufferHeight) final_height = context3d.maxBackBufferHeight;
			
			context3d.configureBackBuffer(final_width, final_height, Application.antialiasing, true, false, false);
			
			context3d.setStencilActions(Context3DTriangleFace.NONE, Context3DCompareMode.NEVER);
			//context3d.setStencilReferenceValue(
			
			buffer_enabled = true;
		}
		
		public function Resize(width:int, height:int):void
		{
			buffer_width = width;
			buffer_height = height;
			buffer_width_inv = 1.0 / buffer_width;
			buffer_height_inv = 1.0 / buffer_height;
			
			if (context3d != null)
			{
				var final_width:int = buffer_width * buffer2screen_scale_x;
				var final_height:int = buffer_height * buffer2screen_scale_y;
				
				if (final_width < 32) final_width = 32;
				if (final_height < 32) final_height = 32;
				
				if (final_width > context3d.maxBackBufferWidth) final_width = context3d.maxBackBufferWidth;
				if (final_height > context3d.maxBackBufferHeight) final_height = context3d.maxBackBufferHeight;
				
				context3d.configureBackBuffer(final_width, final_height, Application.antialiasing, true, false, false);
			}
		}
		
		public function SetScreenRegion(left:int, top:int, width:int, height:int):void
		{
			buffer2screen_scale_x = width * buffer_width_inv;
			buffer2screen_scale_y = height * buffer_height_inv;
			
			var final_width:int = buffer_width * buffer2screen_scale_x;
			var final_height:int = buffer_height * buffer2screen_scale_y;
			
			if (final_width < 32) final_width = 32;
			if (final_height < 32) final_height = 32;
			
			if (final_width > context3d.maxBackBufferWidth) final_width = context3d.maxBackBufferWidth;
			if (final_height > context3d.maxBackBufferHeight) final_height = context3d.maxBackBufferHeight;
			
			context3d.configureBackBuffer(final_width, final_height, Application.antialiasing, true, false, false);
			stage3d.x = left;
			stage3d.y = top;
		}
		
		public function Clear(color:uint = 0):void
		{
			context3d.clear(Color.R(color), Color.G(color), Color.B(color), Color.A(color), 0, 0, Context3DClearMask.ALL);
		}
		
		public function get MaxBufferWidth():int
		{
			return context3d.maxBackBufferWidth;
		}
		
		public function get MaxBufferHeight():int
		{
			return context3d.maxBackBufferHeight;
		}
		
		public function get StageRegionLeft():int
		{
			return stage3d.x;
		}
		
		public function get StageRegionTop():int
		{
			return stage3d.y;
		}
		
		public function get IsEnabled():Boolean
		{
			return buffer_enabled;
		}
		
		public function GetOGSLShaderInUse():Shader
		{
			return _ogsl_shader;
		}
		
		public function Render_Begin(clear:Boolean, color:uint = 0):void
		{
			if (clear) Clear(color);
		}

		public function Render_End():void
		{
			context3d.present();
		}
		
		public function Render_SetTexture(channel:int, texture:Texture, ogsl_name:String = ""):void
		{
			if (_ogsl)
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setTexture(ogsl_name, texture.Hardware);
				_ogsl_shader._Internal_OGSL_End();
			}
			else
			{
				context3d.setTextureAt(channel, texture.Hardware);
			}
		}
		
		public function Render_SetShader(shader:Shader):void
		{
			var i:int = 0;
			var i_max:int = 0;
			
			if (_ogsl_shader != null)
			{
				i_max = _ogsl_shader._ogsl_registers.vertex_buffers_used;
				
				for (i = 0; i < i_max; i++)
				context3d.setVertexBufferAt(i, null);
				
				i_max = _ogsl_shader._ogsl_registers.texture_samplers_used;
				
				for (i = 0; i < i_max; i++)
				context3d.setTextureAt(i, null);
			}
			
			_ogsl = shader.GetOGSL();
			
			if (_ogsl == null)
			{
				_ogsl_shader = null;
			}
			else
			{
				_ogsl_shader = shader;
			}
			
			
			//clear unused texture channels
			/*for (i = 0; i < 8; i++)
			{
				if (i < shader.used_texture_channels)
				{
					//do nothing
				} else
				{
					context3d.setTextureAt(i, null);
				}
			}*/
			
			context3d.setProgram(shader.GetStage3DShader);
			context3d.setBlendFactors(shader.blend_source, shader.blend_destination);
			
			if (_ogsl == null)
			{
				//do nothing
			} 
			else
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setDefaultConstants();
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetProjectionOrthoOffCenterLH(nearClipDistance:Number = 1, farClipDistance:Number = 1000):void
		{
			var a:Number = 2.0 / buffer_width;
			var b:Number = 2.0 / buffer_height;
			var c:Number = 1.0/(farClipDistance-nearClipDistance);
			var d:Number = -nearClipDistance / (farClipDistance - nearClipDistance);
			

			//was
			/*render_m4x4_Projection = new Matrix3D(Vector.<Number>([
			a, 0, 0, 0,
			0, b, 0, 0,
			0, 0, c, d,
			0, 0, 0, 1
			]));*/
			
			render_m4x4_Projection = new Matrix3D(Vector.<Number>([
			a, 0, 0, 0,
			0, b, 0, 0,
			0, 0, c, 0,
			0, 0, d, 1
			]));
		}
		
		public function Render_SetProjectionIdentity():void
		{
			render_m4x4_Projection.identity();
		}
		
		public function Render_SetViewportTransform(x:Number, y:Number, scale_x:Number, scale_y:Number):void
		{
			render_m4x4_Viewport.identity();
			
			render_m4x4_Viewport.appendScale(scale_x, -scale_y, 1);
			
			if (_current_render_target == null)
			render_m4x4_Viewport.appendTranslation( -buffer_width * 0.5 + x * scale_x, (buffer_height * 0.5 - y * scale_y), 0);
			else
			render_m4x4_Viewport.appendTranslation( -_current_render_target.Width * 0.5 + x * scale_x, (_current_render_target.Height * 0.5 - y * scale_y), 0);
			
			//render_m4x4_Viewport.appendScale(2.0 * buffer_width_inv * scale_x, -2.0 * buffer_height_inv * scale_y, 1); //здесь ошибка? вертикальный скейл должен быть как горизонтальный?
			//render_m4x4_Viewport.appendTranslation( -1.0 + 2*(x*buffer_width_inv)* scale_x, 1.0 - 2*(y*buffer_height_inv)* scale_y , 0);
		}
		
		public function Render_SetModelTransform(transform:MeshTransform):void
		{
			render_m4x4_Model.identity();
			
			render_m4x4_Model.appendScale(transform.scale.x, transform.scale.y, transform.scale.z);
			render_m4x4_Model.append(transform.rotation);
			render_m4x4_Model.appendTranslation(transform.position.x, transform.position.y, transform.position.z);
		}
		
		public function Render_SendVertexShaderTransform(append_viewport:Boolean, append_model:Boolean):void
		{
			render_m4x4_Transform.identity();
			
			if (append_model)
			render_m4x4_Transform.append(render_m4x4_Model);
			
			if (append_viewport)
			render_m4x4_Transform.append(render_m4x4_Viewport);

			if (_current_render_target == null)
			render_m4x4_Transform.append(render_m4x4_Projection);
			else
			render_m4x4_Transform.append(_current_render_target.render_m4x4_Projection);
			
			if (_ogsl)
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromMatrix("OGSL_MATRIX_MVP", render_m4x4_Transform, true);
				_ogsl_shader._Internal_OGSL_End();
			}
			else
			{
				context3d.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX, 0, render_m4x4_Transform, true);
			}
		}
		
		public function Render_SendPreviousVertexShaderTransform():void
		{
			if (_ogsl)
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromMatrix("OGSL_MATRIX_MVP", render_m4x4_Transform, true);
				_ogsl_shader._Internal_OGSL_End();
			}
			else
			{
				context3d.setProgramConstantsFromMatrix(Context3DProgramType.VERTEX, 0, render_m4x4_Transform, true);
			}
		}
		
		public function Render_SetDepthBufferMode(mode:int = 0):void
		{
			if (mode == DEPTH_BUFFER_MODE_IGNORE)
			{
				context3d.setDepthTest(false, Context3DCompareMode.ALWAYS);
			}
			else
			if (mode == DEPTH_BUFFER_MODE_READ_WRITE)
			{
				context3d.setDepthTest(true, Context3DCompareMode.GREATER_EQUAL);
			}
			else
			if (mode == DEPTH_BUFFER_MODE_READ_ONLY)
			{
				context3d.setDepthTest(false, Context3DCompareMode.GREATER_EQUAL);
			}
			else
			if (mode == DEPTH_BUFFER_MODE_WRITE_ONLY)
			{
				context3d.setDepthTest(true, Context3DCompareMode.ALWAYS);
			}
		}
		
		public function Render_SetFaceCullingMode(mode:int = 0):void
		{
			if (mode == FACE_CULLING_MODE_DISABLED)
			{
				context3d.setCulling(Context3DTriangleFace.NONE);
			}
			else
			if (mode == FACE_CULLING_MODE_BACK)
			{
				context3d.setCulling(Context3DTriangleFace.FRONT);
			}
			else
			if (mode == FACE_CULLING_MODE_FRONT)
			{
				context3d.setCulling(Context3DTriangleFace.BACK);
			}
		}
		
		//flash.display.BlendMode.NORMAL
		public function Render_SetBlendMode(blend_mode:String = null):void
		{
			if (blend_mode == null) blend_mode = BlendMode.NORMAL;
			
			switch (blend_mode)
			{
				case BlendMode.NORMAL :
					context3d.setBlendFactors(Context3DBlendFactor.SOURCE_ALPHA, Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA);
				return;
				case BlendMode.ADD :
					context3d.setBlendFactors(Context3DBlendFactor.SOURCE_ALPHA, Context3DBlendFactor.ONE);
				return;
				case BlendMode.MULTIPLY :
					context3d.setBlendFactors(Context3DBlendFactor.DESTINATION_COLOR, Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA);
				return;
			}
		}
		
		public function get Context():Context3D
		{
			return context3d;
		}
		
		
		//OGSL stuff
		
		private var __fs_vec4:Vector.<Number> = new Vector.<Number>(4, true);
		public function Render_SetFragmentShaderUniform_Vec4(name:String, x:Number, y:Number, z:Number, w:Number):void
		{
			if (_ogsl)
			{
				__fs_vec4[3] = w;
				__fs_vec4[2] = z;
				__fs_vec4[1] = y;
				__fs_vec4[0] = x;
				
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setFragmentConstantsFromVector(name, __fs_vec4);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		private var __fs_vec3:Vector.<Number> = new Vector.<Number>(3, true);
		public function Render_SetFragmentShaderUniform_Vec3(name:String, x:Number, y:Number, z:Number):void
		{
			if (_ogsl)
			{
				__fs_vec3[2] = z;
				__fs_vec3[1] = y;
				__fs_vec3[0] = x;
				
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setFragmentConstantsFromVector(name, __fs_vec3);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		private var __fs_vec2:Vector.<Number> = new Vector.<Number>(2, true);
		public function Render_SetFragmentShaderUniform_Vec2(name:String, x:Number, y:Number):void
		{
			if (_ogsl)
			{
				__fs_vec2[1] = y;
				__fs_vec2[0] = x;
				
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setFragmentConstantsFromVector(name, __fs_vec2);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetFragmentShaderUniform_Number(name:String, value:Number):void
		{
			if (_ogsl)
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setFragmentConstantsFromNumber(name, value);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetVertexShaderUniform_Vec4(name:String, x:Number, y:Number, z:Number, w:Number):void
		{
			if (_ogsl)
			{
				__fs_vec4[3] = w;
				__fs_vec4[2] = z;
				__fs_vec4[1] = y;
				__fs_vec4[0] = x;
				
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromVector(name, __fs_vec4);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetVertexShaderUniform_Vec3(name:String, x:Number, y:Number, z:Number):void
		{
			if (_ogsl)
			{
				__fs_vec3[2] = z;
				__fs_vec3[1] = y;
				__fs_vec3[0] = x;
				
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromVector(name, __fs_vec3);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetVertexShaderUniform_Vec2(name:String, x:Number, y:Number):void
		{
			if (_ogsl)
			{
				__fs_vec2[1] = y;
				__fs_vec2[0] = x;
				
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromVector(name, __fs_vec2);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetVertexShaderUniform_Number(name:String, value:Number):void
		{
			if (_ogsl)
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromNumber(name, value);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		public function Render_SetVertexShaderUniform_Matrix(name:String, value:Matrix3D):void
		{
			if (_ogsl)
			{
				_ogsl_shader._Internal_OGSL_Begin();
					_ogsl.setVertexConstantsFromMatrix(name, value);
				_ogsl_shader._Internal_OGSL_End();
			}
		}
		
		private var _current_render_target:RenderTarget = null;
		public function SetRenderTarget(target:RenderTarget = null):void
		{
			_current_render_target = target;
			
			if (target != null)
			context3d.setRenderToTexture(target.Hardware, target._enableDepthAndStencil, target._antiAlias);
			else
			context3d.setRenderToBackBuffer();
		}
	}

}