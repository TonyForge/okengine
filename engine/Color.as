package com.engine 
{

	public class Color 
	{
		
		public function Color() 
		{
			
		}
		
		public static function A(argb:uint):Number
		{
			argb = (argb >> 24) & 0x000000ff;
			return (argb as Number) / 255.0;
		}
		
		public static function R(argb:uint):Number
		{
			argb = (argb >> 16) & 0x000000ff;
			return (argb as Number) / 255.0;
		}
		
		public static function G(argb:uint):Number
		{
			argb = (argb >> 8) & 0x000000ff;
			return (argb as Number) / 255.0;
		}
		
		public static function B(argb:uint):Number
		{
			argb = (argb) & 0x000000ff;
			return (argb as Number) / 255.0;
		}
		
		public static function ARGB(a:int, r:int, g:int, b:int):uint
		{
			return ((a << 24) | (r << 16) | (g << 8) | (b));
		}
		
		public static function RGB(r:int, g:int, b:int):uint
		{
			return ((255 << 24) | (r << 16) | (g << 8) | (b));
		}
		
		public static function ExtractRGB(argb:uint):uint
		{
			return argb & 0x00ffffff;
		}
		
		public static function ExtractOpacity(argb:uint):Number
		{
			argb = (argb >> 24) & 0x000000ff;
			return (argb as Number) / 255.0;
		}
	}

}