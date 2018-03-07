package com.engine 
{
	import flash.geom.Matrix;

	public class Quad 
	{
		public var verts:Vector.<Number> = new Vector.<Number>(8, true);
		private static var transform_matrix:Matrix = new Matrix();
		
		public function Quad() 
		{
			
		}
		
		public function Reset(width:Number, height:Number):void
		{
			verts[0] = 0;
			verts[1] = 0;
			
			verts[2] = width;
			verts[3] = 0;
			
			verts[4] = width;
			verts[5] = height;
			
			verts[6] = 0;
			verts[7] = height;
		}
		
		public function Transform(hotspot_x:Number, hotspot_y:Number, translate_x:Number, translate_y:Number, angle_deg:Number, scale_x:Number, scale_y:Number):void
		{

			transform_matrix.identity();
			transform_matrix.translate( -hotspot_x, -hotspot_y);
			transform_matrix.scale(scale_x, scale_y);
			transform_matrix.rotate((angle_deg / 180.0) * Math.PI);
			transform_matrix.translate(translate_x, translate_y);
			
			var x:Number;
			var y:Number;
			
			x = verts[0];
			y = verts[1];
			verts[0] = transform_matrix.a * x + transform_matrix.c * y + transform_matrix.tx;
			verts[1] = transform_matrix.b * x + transform_matrix.d * y + transform_matrix.ty;
			
			x = verts[2];
			y = verts[3];
			verts[2] = transform_matrix.a * x + transform_matrix.c * y + transform_matrix.tx;
			verts[3] = transform_matrix.b * x + transform_matrix.d * y + transform_matrix.ty;
			
			x = verts[4];
			y = verts[5];
			verts[4] = transform_matrix.a * x + transform_matrix.c * y + transform_matrix.tx;
			verts[5] = transform_matrix.b * x + transform_matrix.d * y + transform_matrix.ty;
			
			x = verts[6];
			y = verts[7];
			verts[6] = transform_matrix.a * x + transform_matrix.c * y + transform_matrix.tx;
			verts[7] = transform_matrix.b * x + transform_matrix.d * y + transform_matrix.ty;
		}
	}

}