package com.engine 
{
	import flash.display.BitmapData;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.Texture;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class Texture 
	{
		//texture types
		public static const HARDWARE : String = "hardware"; //texture is stored in video memory only
		public static const SOFTWARE : String = "software"; //texture is stored in main memory only
		public static const MIXED : String = "mixed"; //hardware + software
		public static const DYNAMIC : String = "dynamic"; //software -> hardware
		public static const RENDER_TARGET : String = "render_target"; //
		
		private var _hardware_texture : flash.display3D.textures.Texture = null;
		private var _software_texture : BitmapData = null;
		
		private var texture_type:int = 0; //0 - software, 1 - hardware, 2 - mixed
		private var texture_width:int = 0;
		private var texture_height:int = 0;
		
		private var _dynamic_argb_color:ByteArray = null;
		
		public function Texture(type:String, width:int, height:int, argb_colors:ByteArray) 
		{
			texture_width = width;
			texture_height = height;
			
			if (type == SOFTWARE) 		texture_type = 0; else
			if (type == HARDWARE) 		texture_type = 1; else
			if (type == MIXED) 			texture_type = 2; else
			if (type == DYNAMIC) 		texture_type = 3; else
			if (type == RENDER_TARGET) 	texture_type = 4;
			
			if (texture_type == 0)
			{
				paradiselib.instance.ARGB2BGRA(argb_colors);
				
				_software_texture = new BitmapData(width, height, true, 0);
				_software_texture.lock();
				_software_texture.setPixels(new Rectangle(0, 0, width, height), argb_colors);
				_software_texture.unlock();
			}
			
			if (texture_type == 1)
			{
				/*_software_texture = new BitmapData(width, height, true, 0);
				_software_texture.lock();
				_software_texture.setPixels(new Rectangle(0, 0, width, height), argb_colors);
				_software_texture.unlock();*/
				
				_hardware_texture = Application.context3d.createTexture(width, height, Context3DTextureFormat.BGRA, false, 0);
				_hardware_texture.uploadFromByteArray(argb_colors, 0);
				//_hardware_texture.uploadFromBitmapData(_software_texture, 0);
				
				/*_software_texture.dispose();
				_software_texture = null;*/
			}
			
			if (texture_type == 2)
			{
				_hardware_texture = Application.context3d.createTexture(width, height, Context3DTextureFormat.BGRA, false, 0);
				_hardware_texture.uploadFromByteArray(argb_colors, 0);
				//_hardware_texture.uploadFromBitmapData(_software_texture, 0);
				
				paradiselib.instance.ARGB2BGRA(argb_colors);
				
				_software_texture = new BitmapData(width, height, true, 0);
				_software_texture.lock();
				_software_texture.setPixels(new Rectangle(0, 0, width, height), argb_colors);
				_software_texture.unlock();
			}
			
			if (texture_type == 3)
			{
				_software_texture = new BitmapData(width, height, true, 0);
				_hardware_texture = Application.context3d.createTexture(width, height, Context3DTextureFormat.BGRA, false, 0);
				_dynamic_argb_color = argb_colors;
				
				Upload();
			}
			
			if (texture_type == 4)
			{
				_hardware_texture = Application.context3d.createTexture(width, height, Context3DTextureFormat.BGRA, true, 0);
			}
		}
		
		public function Upload():void
		{
			if (_dynamic_argb_color != null)
			{
				_dynamic_argb_color.position = 0;
				
				_software_texture.lock();
				_software_texture.setPixels(new Rectangle(0, 0, texture_width, texture_height), _dynamic_argb_color);
				_software_texture.unlock();
				
				_hardware_texture.uploadFromBitmapData(_software_texture, 0);
			}
		}
		
		public function get Width():int
		{
			return texture_width;
		}
		
		public function get Height():int
		{
			return texture_height;
		}
		
		public function get Software():BitmapData
		{
			return _software_texture;
		}
		
		public function get Hardware():flash.display3D.textures.Texture
		{
			return _hardware_texture;
		}
	}

}