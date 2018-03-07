package com.engine 
{

	public class Noise 
	{
		
		private var grad3:Vector.<Grad> = new <Grad>[new Grad(1,1,0),new Grad(-1,1,0),new Grad(1,-1,0),new Grad(-1,-1,0),
													 new Grad(1,0,1),new Grad(-1,0,1),new Grad(1,0,-1),new Grad(-1,0,-1),
												     new Grad(0,1,1),new Grad(0,-1,1),new Grad(0,1,-1),new Grad(0,-1,-1)];
		
		private var p:Vector.<int> = new <int>[151, 160, 137, 91, 90, 15,
		131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
		190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
		88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,  
		77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,  
		102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,  
		135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,  
		5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,  
		223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,  
		129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,  
		251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,  
		49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,  
		138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180];

		private var perm:Vector.<int> = new Vector.<int>(512, true);
		private var gradP:Vector.<Grad> = new Vector.<Grad>(512, true);
		
		private var F2:Number = 0.5 * (Math.sqrt(3.0) - 1.0);
		private var G2:Number = (3.0 - Math.sqrt(3.0)) / 6.0;
		private var F3:Number = 1.0 / 3.0;
		private var G3:Number = 1.0 / 6.0;
		
		public function Noise() 
		{
			if (instance != null)
			{
				throw new Error("Noise is a singleton, use Assets.instance");
			} else {
				Seed(0);
			}
		}
		
		public function Seed(_seed:Number):void
		{
			var seed:int = 0;
			
			if(_seed > 0 && _seed < 1) {
				// Scale the seed out
				seed = Math.floor(_seed*65536);
			} else {
				seed = Math.floor(_seed);
			}

			if(seed < 256) {
				seed |= seed << 8;
			}

			var v:int = 0;
			
			for(var i:int = 0; i < 256; i++) {
				
				if (i & 1) {
					v = p[i] ^ (seed & 255);
					} else {
					v = p[i] ^ ((seed>>8) & 255);
				}

				perm[i] = perm[i + 256] = v;
				gradP[i] = gradP[i + 256] = grad3[v % 12];
			}
		}
		
		public function Simplex2D(x:Number, y:Number):Number
		{
			var n0:Number, n1:Number, n2:Number;

			var s:Number = (x + y) * F2;
			var i:int = Math.floor(x + s);
			var j:int = Math.floor(y + s);
			var t:Number = (i + j) * G2;
			var x0:Number = x - i + t;
			var y0:Number = y - j + t;

			var i1:int, j1:int;
			if(x0>y0) {
				i1 = 1;
				j1 = 0;
			} else {
				i1 = 0;
				j1 = 1;
			}

			var x1:Number = x0 - i1 + G2;
			var y1:Number = y0 - j1 + G2;
			var x2:Number = x0 - 1 + 2 * G2;
			var y2:Number = y0 - 1 + 2 * G2;

			i &= 255;
			j &= 255;
			
			var gi0:Grad = gradP[i + perm[j]];
			var gi1:Grad = gradP[i + i1 + perm[j + j1]];
			var gi2:Grad = gradP[i + 1 + perm[j + 1]];

			
			t = 0.5 - x0 * x0 - y0 * y0;
			
			if(t<0) {
				n0 = 0;
			} else {
				t *= t;
				n0 = t * t * gi0.dot2(x0, y0);
			}
			
			t = 0.5 - x1 * x1 - y1 * y1;
			
			if(t<0) {
				n1 = 0;
			} else {
				t *= t;
				n1 = t * t * gi1.dot2(x1, y1);
			}
			
			t = 0.5 - x2 * x2 - y2 * y2;
			
			if(t<0) {
				n2 = 0;
			} else {
				t *= t;
				n2 = t * t * gi2.dot2(x2, y2);
			}

			return 70 * (n0 + n1 + n2);
		}
		
		public function Simplex3D(xin:Number, yin:Number, zin:Number):Number
		{
			var n0:Number, n1:Number, n2:Number, n3:Number;

			var s:Number = (xin+yin+zin)*F3;
			var i:int = Math.floor(xin+s);
			var j:int = Math.floor(yin+s);
			var k:int = Math.floor(zin+s);

			var t:Number = (i+j+k)*G3;
			var x0:Number = xin-i+t;
			var y0:Number = yin-j+t;
			var z0:Number = zin-k+t;

			var i1:int, j1:int, k1:int;
			var i2:int, j2:int, k2:int;
			
			if(x0 >= y0) {
			  if(y0 >= z0)	  { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; }
			  else if(x0 >= z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; }
			  else			  { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; }
			} else {
			  if(y0 < z0)	  { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; }
			  else if(x0 < z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; }
			  else			 { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; }
			}

			var x1:Number = x0 - i1 + G3;
			var y1:Number = y0 - j1 + G3;
			var z1:Number = z0 - k1 + G3;

			var x2:Number = x0 - i2 + 2 * G3;
			var y2:Number = y0 - j2 + 2 * G3;
			var z2:Number = z0 - k2 + 2 * G3;

			var x3:Number = x0 - 1 + 3 * G3;
			var y3:Number = y0 - 1 + 3 * G3;
			var z3:Number = z0 - 1 + 3 * G3;


			i &= 255;
			j &= 255;
			k &= 255;
			
			var gi0:Grad = gradP[i+   perm[j+   perm[k   ]]];
			var gi1:Grad = gradP[i+i1+perm[j+j1+perm[k+k1]]];
			var gi2:Grad = gradP[i+i2+perm[j+j2+perm[k+k2]]];
			var gi3:Grad = gradP[i+ 1+perm[j+ 1+perm[k+ 1]]];

			
			t = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
			
			if(t<0) {
			  n0 = 0;
			} else {
			  t *= t;
			  n0 = t * t * gi0.dot3(x0, y0, z0);
			}
			
			t = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
			
			if(t<0) {
			  n1 = 0;
			} else {
			  t *= t;
			  n1 = t * t * gi1.dot3(x1, y1, z1);
			}
			
			t = 0.6 - x2 * x2 - y2 * y2 - z2 * z2;
			
			if(t<0) {
			  n2 = 0;
			} else {
			  t *= t;
			  n2 = t * t * gi2.dot3(x2, y2, z2);
			}
			
			t = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;
			
			if(t<0) {
			  n3 = 0;
			} else {
			  t *= t;
			  n3 = t * t * gi3.dot3(x3, y3, z3);
			}

			return 32 * (n0 + n1 + n2 + n3);
		}
		
		private function fade(t:Number):Number
		{
			return t*t*t*(t*(t*6.0-15.0)+10.0);
		}

		private function lerp(a:Number, b:Number, t:Number):Number {
			return (1.0-t)*a + t*b;
		}
		
		public function Perlin2D(x:Number, y:Number):Number
		{
			var X:int = Math.floor(x), Y:int = Math.floor(y);
			x = x - X; y = y - Y;
			X = X & 255; Y = Y & 255;

			var n00:Number = gradP[X+perm[Y]].dot2(x, y);
			var n01:Number = gradP[X+perm[Y+1]].dot2(x, y-1);
			var n10:Number = gradP[X+1+perm[Y]].dot2(x-1, y);
			var n11:Number = gradP[X+1+perm[Y+1]].dot2(x-1, y-1);

			var u:Number = fade(x);

			return lerp( lerp(n00, n10, u),
						 lerp(n01, n11, u),
						 fade(y));
		}

		public function Perlin3D(x:Number, y:Number, z:Number):Number
		{
			var X:int = Math.floor(x), Y:int = Math.floor(y), Z:int = Math.floor(z);
			x = x - X; y = y - Y; z = z - Z;
			X = X & 255; Y = Y & 255; Z = Z & 255;

			var n000:Number = gradP[X+  perm[Y+  perm[Z  ]]].dot3(x, y, z);
			var n001:Number = gradP[X+  perm[Y+  perm[Z+1]]].dot3(x, y, z-1);
			var n010:Number = gradP[X+  perm[Y+1+perm[Z  ]]].dot3(x, y-1, z);
			var n011:Number = gradP[X+  perm[Y+1+perm[Z+1]]].dot3(x, y-1, z-1);
			var n100:Number = gradP[X+1+perm[Y+  perm[Z  ]]].dot3(x-1, y, z);
			var n101:Number = gradP[X+1+perm[Y+  perm[Z+1]]].dot3(x-1, y, z-1);
			var n110:Number = gradP[X+1+perm[Y+1+perm[Z  ]]].dot3(x-1, y-1, z);
			var n111:Number = gradP[X+1+perm[Y+1+perm[Z+1]]].dot3(x-1, y-1, z-1);

			var u:Number = fade(x);
			var v:Number = fade(y);
			var w:Number = fade(z);

			return lerp(
				lerp(
				  lerp(n000, n100, u),
				  lerp(n001, n101, u), w),
				lerp(
				  lerp(n010, n110, u),
				  lerp(n011, n111, u), w),
				v);
		}
		
		//---------------------------------------------------------
		
		public static var instance:Noise = null;
		
		public static function create():void
		{
			if (instance == null)
			{
				instance = new Noise();
			}
		}
	}
}

	
class Grad
{
	public var x:Number, y:Number, z:Number;
	
	public function Grad(x:Number = 0, y:Number = 0, z:Number = 0)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public function dot2(x:Number, y:Number):Number
	{
		return this.x * x + this.y * y;
	}
	
	public function dot3(x:Number, y:Number, z:Number):Number
	{
		return this.x * x + this.y * y + this.z * z;
	}
}