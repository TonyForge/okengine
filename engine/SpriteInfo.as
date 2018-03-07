package com.engine 
{
	public class SpriteInfo 
	{
		private var uv_left:Number = 0;
		private var uv_top:Number = 0;
		private var uv_right:Number = 1.0;
		private var uv_bottom:Number = 1.0;
		
		private var hotspot_x:Number = 0;
		private var hotspot_y:Number = 0;
		
		private var _width:int = 1.0;
		private var _height:int = 1.0;
		private var _opacity:Number = 1.0;
		
		public function SpriteInfo() 
		{
		}
		
		public function SetHotspot(x:Number, y:Number):void
		{
			hotspot_x = x;
			hotspot_y = y;
		}
		
		public function SetHotspotToCenter():void
		{
			hotspot_x = _width * 0.5;
			hotspot_y = _height * 0.5;
		}
		
		public function SetTextureRegion(left:int, top:int, width:int, height:int, texture:Texture, pixel_snap:Boolean = false):void
		{
			uv_left = left / texture.Width;
			uv_top= top / texture.Height;
			uv_right = uv_left + width / texture.Width;
			uv_bottom = uv_top + height / texture.Height;
			
			if (pixel_snap == true)
			{
				left = Math.round(uv_left * texture.Width);
				top = Math.round(uv_top * texture.Height);
				width = Math.round(uv_right * texture.Width) - left;
				height = Math.round(uv_bottom * texture.Height) - top;
				
				uv_left = left / texture.Width;
				uv_top= top / texture.Height;
				uv_right = uv_left + width / texture.Width;
				uv_bottom = uv_top + height / texture.Height;
			}
			
			_width = width;
			_height = height;
		}
		
		public function SetSize(width:int, height:int):void
		{
			_width = width;
			_height = height;
		}
		
		public function SetOpacity(opacity:Number = 1.0):void
		{
			_opacity = opacity;
		}
		
		public function get Width():int
		{
			return _width;
		}
		
		public function get Height():int
		{
			return _height;
		}
		
		public function get HotspotX():Number
		{
			return hotspot_x;
		}
		
		public function get HotspotY():Number
		{
			return hotspot_y;
		}
		
		public function get Opacity():Number
		{
			return _opacity;
		}
		
		public function get UV_L():Number
		{
			return uv_left;
		}
		
		public function get UV_R():Number
		{
			return uv_right;
		}
		
		public function get UV_T():Number
		{
			return uv_top;
		}
		
		public function get UV_B():Number
		{
			return uv_bottom;
		}
	}

}