package com.engine 
{
	import flash.display.NativeWindow;
	import flash.display.Stage;
	import flash.external.ExtensionContext;
	import flash.geom.Point;
	import flash.utils.ByteArray;
	
	public class paradiselib 
	{
		private var _context:ExtensionContext = null;
		
		public function paradiselib(nativeWindow:NativeWindow) 
		{
			if (instance != null)
			{
				throw new Error("paradise_lib is a singleton, use paradise_lib.instance");
			} else {
				_context = ExtensionContext.createExtensionContext("com.engine.paradiselib", "");
				
				var titleOrig:String = nativeWindow.title;
				var strRand:String = generateRandomString(6);
				
				nativeWindow.title = strRand;
					_context.call("GrabHWND", strRand);
				nativeWindow.title = titleOrig;
			}
		}
		
		public function destructor():void
		{
			_context.dispose();
		}
		
		private function generateRandomString(strlen:Number) : String
		{
			var chars:String = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
			var num_chars:Number = chars.length - 1;
			var randomStr:String = "";

			for (var i:Number = 0; i < strlen; i++){
				randomStr += chars.charAt(Math.floor(Math.random() * num_chars));
			}
			
			return randomStr;
		}
		
		//---------------------------------------------------------
		
		public function GetWindowRectSize():Point
		{
			var size:Array = _context.call("GetWindowRectSize") as Array;
			
			return new Point(size[0], size[1]);
		}
		
		//---------------------------------------------------------
		//SFML

		public function SFMLSoundBufferCreateFromFile(filename:String):uint
		{
			return _context.call("SFMLSoundBufferCreateFromFile", filename) as uint;
		}


		public function SFMLSoundCreate():uint
		{
			return _context.call("SFMLSoundCreate") as uint;
		}

		public function SFMLSoundSetBuffer(sound:uint, buffer:uint):void
		{
			_context.call("SFMLSoundSetBuffer", sound, buffer);
		}

		public function SFMLSoundPlay(sound:uint):void
		{
			_context.call("SFMLSoundPlay", sound);
		}

		public function SFMLSoundPause(sound:uint):void
		{
			_context.call("SFMLSoundPause", sound);
		}

		public function SFMLSoundStop(sound:uint):void
		{
			_context.call("SFMLSoundStop", sound);
		}


		public function SFMLMusicCreate():uint
		{
			return _context.call("SFMLMusicCreate") as uint;
		}

		public function SFMLMusicOpenFromFile(music:uint, filename:String):Boolean
		{
			return _context.call("SFMLMusicOpenFromFile", music, filename) as Boolean;
		}

		public function SFMLMusicPlay(music:uint):void
		{
			_context.call("SFMLMusicPlay", music);
		}

		public function SFMLMusicPause(music:uint):void
		{
			_context.call("SFMLMusicPause", music);
		}

		public function SFMLMusicStop(music:uint):void
		{
			_context.call("SFMLMusicStop", music);
		}

		//---   ---   ---   ---   ---   ---   ---   ---   ---   ---   

		public function SFMLSoundSetLoop(sound:uint, loop:Boolean):void
		{
			_context.call("SFMLSoundSetLoop", sound, loop);
		}

		public function SFMLSoundGetLoop(sound:uint):Boolean
		{
			return _context.call("SFMLSoundGetLoop", sound) as Boolean;
		}

		public function SFMLSoundSetPlayingOffset(sound:uint, percentage:Number):void
		{
			_context.call("SFMLSoundSetPlayingOffset", sound, percentage);
		}

		public function SFMLSoundGetPlayingOffset(sound:uint):Number
		{
			return _context.call("SFMLSoundGetPlayingOffset", sound) as Number;
		}

		public function SFMLSoundGetBuffer(sound:uint):uint
		{
			return _context.call("SFMLSoundGetBuffer", sound) as uint;
		}

		//0 - stopped, 1 - playing, 2 - paused
		public function SFMLSoundGetStatus(sound:uint):uint
		{
			return _context.call("SFMLSoundGetStatus", sound) as uint;
		}

		public function SFMLSoundSetPitch(sound:uint, pitch:Number):void
		{
			_context.call("SFMLSoundSetPitch", sound, pitch);
		}

		public function SFMLSoundGetPitch(sound:uint):Number
		{
			return _context.call("SFMLSoundGetPitch", sound) as Number;
		}

		public function SFMLSoundSetVolume(sound:uint, volume:Number):void
		{
			_context.call("SFMLSoundSetVolume", sound, volume);
		}

		public function SFMLSoundGetVolume(sound:uint):Number
		{
			return _context.call("SFMLSoundGetVolume", sound) as Number;
		}

		public function SFMLSoundSetPosition(sound:uint, x:Number, y:Number, z:Number):void
		{
			_context.call("SFMLSoundSetPosition", sound, x, y, z);
		}

		public function SFMLSoundGetPosition(sound:uint):Array
		{
			return _context.call("SFMLSoundGetPosition", sound) as Array;
		}

		public function SFMLSoundSetRelativeToListener(sound:uint, relative:Boolean):void
		{
			_context.call("SFMLSoundSetRelativeToListener", sound, relative);
		}

		public function SFMLSoundSetMinDistance(sound:uint, minDistance:Number):void
		{
			_context.call("SFMLSoundSetMinDistance", sound, minDistance);
		}

		public function SFMLSoundGetMinDistance(sound:uint):Number
		{
			return _context.call("SFMLSoundGetMinDistance", sound) as Number;
		}

		public function SFMLSoundSetAttenuation(sound:uint, attenuation:Number):void
		{
			_context.call("SFMLSoundSetAttenuation", sound, attenuation);
		}

		public function SFMLSoundGetAttenuation(sound:uint):Number
		{
			return _context.call("SFMLSoundGetAttenuation", sound) as Number;
		}

		public function SFMLSoundIsRelativeToListener(sound:uint):Boolean
		{
			return _context.call("SFMLSoundIsRelativeToListener", sound) as Boolean;
		}

		//---   ---   ---   ---   ---   ---   ---   ---   ---   ---   

		public function SFMLMusicSetLoop(music:uint, loop:Boolean):void
		{
			_context.call("SFMLMusicSetLoop", music, loop);
		}

		public function SFMLMusicGetLoop(music:uint):Boolean
		{
			return _context.call("SFMLMusicGetLoop", music) as Boolean;
		}

		public function SFMLMusicSetPlayingOffset(music:uint, percentage:Number):void
		{
			_context.call("SFMLMusicSetPlayingOffset", music, percentage);
		}

		public function SFMLMusicGetPlayingOffset(music:uint):Number
		{
			return _context.call("SFMLMusicGetPlayingOffset", music) as Number;
		}

		public function SFMLMusicGetDuration(music:uint):Number
		{
			return _context.call("SFMLMusicGetDuration", music) as Number;
		}

		//0 - stopped, 1 - playing, 2 - paused
		public function SFMLMusicGetStatus(music:uint):uint
		{
			return _context.call("SFMLMusicGetStatus", music) as uint;
		}

		public function SFMLMusicSetPitch(music:uint, pitch:Number):void
		{
			_context.call("SFMLMusicSetPitch", music, pitch);
		}

		public function SFMLMusicGetPitch(music:uint):Number
		{
			return _context.call("SFMLMusicGetPitch", music) as Number;
		}

		public function SFMLMusicSetVolume(music:uint, volume:Number):void
		{
			_context.call("SFMLMusicSetVolume", music, volume);
		}

		public function SFMLMusicGetVolume(music:uint):Number
		{
			return _context.call("SFMLMusicGetVolume", music) as Number;
		}

		public function SFMLMusicSetPosition(music:uint, x:Number, y:Number, z:Number):void
		{
			_context.call("SFMLMusicSetPosition", music, x, y, z);
		}

		public function SFMLMusicGetPosition(music:uint):Array
		{
			return _context.call("SFMLMusicGetPosition", music) as Array;
		}

		public function SFMLMusicSetRelativeToListener(music:uint, relative:Boolean):void
		{
			_context.call("SFMLMusicSetRelativeToListener", music, relative);
		}

		public function SFMLMusicSetMinDistance(music:uint, minDistance:Number):void
		{
			_context.call("SFMLMusicSetMinDistance", music, minDistance);
		}

		public function SFMLMusicGetMinDistance(music:uint):Number
		{
			return _context.call("SFMLMusicGetMinDistance", music) as Number;
		}

		public function SFMLMusicSetAttenuation(music:uint, attenuation:Number):void
		{
			_context.call("SFMLMusicSetAttenuation", music, attenuation);
		}

		public function SFMLMusicGetAttenuation(music:uint):Number
		{
			return _context.call("SFMLMusicGetAttenuation", music) as Number;
		}

		public function SFMLMusicIsRelativeToListener(music:uint):Boolean
		{
			return _context.call("SFMLMusicIsRelativeToListener", music) as Boolean;
		}

		//---   ---   ---   ---   ---   ---   ---   ---   ---   ---   

		public function SFMLSetGlobalVolume(volume:Number):void
		{
			_context.call("SFMLSetGlobalVolume", volume);
		}

		public function SFMLGetGlobalVolume():Number
		{
			return _context.call("SFMLGetGlobalVolume") as Number;
		}

		public function SFMLSetGlobalPosition(x:Number, y:Number, z:Number):void
		{
			_context.call("SFMLSetGlobalPosition", x, y, z);
		}

		public function SFMLGetGlobalPosition():Array
		{
			return _context.call("SFMLGetGlobalPosition") as Array;
		}

		public function SFMLSetGlobalDirection(x:Number, y:Number, z:Number):void
		{
			_context.call("SFMLSetGlobalDirection", x, y, z);
		}

		public function SFMLGetGlobalDirection():Array
		{
			return _context.call("SFMLGetGlobalDirection") as Array;
		}

		public function SFMLSetGlobalUpVector(x:Number, y:Number, z:Number):void
		{
			_context.call("SFMLSetGlobalUpVector", x, y, z);
		}

		public function SFMLGetGlobalUpVector():Array
		{
			return _context.call("SFMLGetGlobalUpVector") as Array;
		}

		//---------------------------------------------------------

		public function ARGB2BGRA(pixels:ByteArray):void
		{
			_context.call("ARGB2BGRA", pixels);
		}
		
		public static var instance:paradiselib = null;
		
		public static function create(stage:Stage):void
		{
			if (instance == null)
			{
				instance = new paradiselib(stage.nativeWindow);
			}
		}
		
		public static function destroy():void
		{
			if (instance != null)
			{
				instance.destructor();
				instance = null;
			}
		}
	}

}