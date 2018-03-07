package com.engine 
{
	import flash.display3D.Context3D;
	import flash.geom.Matrix3D;

	public class RenderTarget extends Texture
	{
		private var _context3d:Context3D = null;
		public var _enableDepthAndStencil:Boolean = false;
		public var _antiAlias:int = 0;
		
		public function RenderTarget(width:int, height:int, near_z:Number = 1, far_z:Number = 1000, enableDepthAndStencil:Boolean = false) 
		{
			super(Texture.RENDER_TARGET, width, height, null);
			_context3d = Application.context3d;
			_enableDepthAndStencil = enableDepthAndStencil;
			_antiAlias = Math.max(Application.antialiasing, 0);
			
			_Render_SetProjectionOrthoOffCenterLH(near_z, far_z);
		}
		
		public function SetAntialiasing(antialiasing:int = 0):void
		{
			_antiAlias = antialiasing;
		}
		

		private var _render_m4x4_Projection:Matrix3D = new Matrix3D();
		private function _Render_SetProjectionOrthoOffCenterLH(nearClipDistance:Number = 1, farClipDistance:Number = 1000):void
		{
			var a:Number = 2.0 / Width;
			var b:Number = 2.0 / Height;
			var c:Number = 1.0/(farClipDistance-nearClipDistance);
			var d:Number = -nearClipDistance / (farClipDistance - nearClipDistance);
			
			_render_m4x4_Projection = new Matrix3D(Vector.<Number>([
			a, 0, 0, 0,
			0, b, 0, 0,
			0, 0, c, 0,
			0, 0, d, 1
			]));
		}
		public function get render_m4x4_Projection():Matrix3D
		{
			return _render_m4x4_Projection;
		}
	}

}