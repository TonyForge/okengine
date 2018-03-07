package com.engine 
{
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	public class SceneCamera extends SceneNode 
	{
		private var _world_rotation_x:SceneNode = new SceneNode();
		private var _world_rotation_y:SceneNode = new SceneNode();
		private var _world_rotation_z:SceneNode = new SceneNode();
		private var _world_translator:SceneNode = new SceneNode();
		
		public static const ROTATION_ORDER_XYZ:String = "xyz";
		public static const ROTATION_ORDER_XZY:String = "xzy";
		public static const ROTATION_ORDER_YXZ:String = "yxz";
		public static const ROTATION_ORDER_YZX:String = "yzx";
		public static const ROTATION_ORDER_ZXY:String = "zxy";
		public static const ROTATION_ORDER_ZYX:String = "zyx";
		
		private var _viewport_zoom:Number = 1.0;
		
		public var viewport_center_x:Number = 0;
		public var viewport_center_y:Number = 0;
		public var viewport_center_z:Number = 0;
		private var _viewport_x:Number = 0;
		private var _viewport_y:Number = 0;
		private var _viewport_z:Number = 0;
		
		
		public function SceneCamera(rotation_order:String = "xyz") 
		{
			super();
			
			this["_world_rotation_" + rotation_order.charAt(0)].ConnectChild(this["_world_rotation_" + rotation_order.charAt(1)]);
			this["_world_rotation_" + rotation_order.charAt(1)].ConnectChild(this["_world_rotation_" + rotation_order.charAt(2)]);
			super.ConnectChild(this["_world_rotation_" + rotation_order.charAt(0)]);
			
			var tmp_x:SceneNode = this["_world_rotation_" + rotation_order.charAt(0)];
			var tmp_y:SceneNode = this["_world_rotation_" + rotation_order.charAt(1)];
			var tmp_z:SceneNode = this["_world_rotation_" + rotation_order.charAt(2)];
			
			_world_rotation_x = tmp_x;
			_world_rotation_y = tmp_y;
			_world_rotation_z = tmp_z;
			
			_world_rotation_z.ConnectChild(_world_translator);
		}

		public function SetViewportXY(x:Number, y:Number):void
		{
			_viewport_x = x;
			_viewport_y = y;
			
			SetRelativeTranslation(viewport_center_x + _viewport_x, viewport_center_y + _viewport_y, viewport_center_z + _viewport_z);
		}
		
		public function SetViewportZ(z:Number):void
		{
			_viewport_z = z;
			
			SetRelativeTranslation(viewport_center_x + _viewport_x, viewport_center_y + _viewport_y, viewport_center_z + _viewport_z);
		}
		
		public function SetWorldXYZ(x:Number, y:Number, z:Number = 0):void
		{
			_world_translator.SetRelativeTranslation(x / _viewport_zoom, y / _viewport_zoom, z / _viewport_zoom);
		}
		
		public function SetWorldRotation(angle_x:Number, angle_y:Number, angle_z:Number):void
		{
			_world_rotation_z.SetRelativeRotation(0, 0, angle_z);
			_world_rotation_y.SetRelativeRotation(0, angle_y, 0);
			_world_rotation_x.SetRelativeRotation(angle_x, 0, 0);
		}
		
		public function SetViewportZoom(zoom:Number = 1.0):void
		{
			_viewport_zoom = zoom;
			SetRelativeScale(zoom, zoom, zoom);
		}
		
		
		override public function ConnectChild(child:SceneNode, keep_transform:Boolean = true):void
		{
			_world_translator.ConnectChild(child, false);
		}
	}

}