package com.engine 
{
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageDisplayState;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.display3D.Context3D;
	import flash.events.Event;
	import flash.events.NativeWindowBoundsEvent;
	import flash.filesystem.File;
	import flash.geom.Point;
	
	public class Application extends Sprite 
	{
		public static var instance:Application = null;
		public static var context3d:Context3D = null;
		public static var working_folder:String = File.applicationDirectory.nativePath + "\\";
		public static var project_data_folder:String = "";
		
		public static var antialiasing:int = 0; //0 is off, 1 is 2x, 2 is 4x, 3 is 8x, 4 is 16x
		
		public var frame_buffer:FrameBuffer = null;
		public var screen_pivot_x:int = 0;
		public var screen_pivot_y:int = 0;
		public var screen_width:int = 800;
		public var screen_height:int = 600;
		public var screen_scale_x:Number = 1.0;
		public var screen_scale_y:Number = 1.0;
		
		public function Application() 
		{
			super();
			instance = this;
			
			Setup();
			
			if (stage) OnInit();
			else addEventListener(Event.ADDED_TO_STAGE, OnInit);
		}
		
		public function GetFrameBuffer():FrameBuffer
		{
			return frame_buffer;
		}
		
		//Switch to windowed mode
		public function SetWindowed():void
		{
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			
			stage.displayState = StageDisplayState.NORMAL;
			
			stage.stageWidth = screen_width;
			stage.stageHeight = screen_height;
			
			OnResize();
		}
		
		//Switch to fullscreen mode
		public function SetFullscreen():void
		{
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;

			stage.displayState = StageDisplayState.FULL_SCREEN;
			
			stage.stageWidth = stage.nativeWindow.width;
			stage.stageHeight = stage.nativeWindow.height;
			
			OnResize();
		}
		
		//----------------------------------------------------------------------
		
		//Virtual Methods
		public function Run():void
		{
		}
		
		public function Update():void
		{
		}
		
		public function Setup():void
		{
			
		}
		
		//----------------------------------------------------------------------
		
		//Private Methods
		
		private function OnInit(e:Event = null):void 
		{
			removeEventListener(Event.ADDED_TO_STAGE, OnInit);
		
			//critical events
			addEventListener(Event.ENTER_FRAME, OnUpdate);
			stage.nativeWindow.addEventListener(NativeWindowBoundsEvent.RESIZE, OnResize);
			stage.nativeWindow.addEventListener(Event.CLOSING, OnShutdown);
			
			frame_buffer = new FrameBuffer(stage, screen_width, screen_height);
			
			//global settings
			stage.quality = StageQuality.BEST;
			SetWindowed(); //Let it be windowed here! Switch to fullscreen (if needed) at entry point.
			
			//libraries
			paradiselib.create(stage);
		}
		
		private function OnShutdown(e:Event = null):void 
		{
			paradiselib.destroy();
		}
		
		private function OnUpdate(e:Event):void 
		{
			if (initialization_done == true)
			{
				Update();
				
				Input.instance.Update();
			}
			else
			{
				initialization_done = InitializationUpdate();
				
				if (initialization_done == true)
				{
					// entry point
					context3d = frame_buffer.Context;
					
					Noise.create();
					Input.create(stage);
					Assets.create();
					ShaderFactory.create();
					
					Run(); 
				}
			}
		}
		
		private function OnResize(e:NativeWindowBoundsEvent = null):void
		{
			if (paradiselib.instance == null) return;
			
			var actual_window_size:Point = paradiselib.instance.GetWindowRectSize();
			
			if (actual_window_size.x < 32)
			{
				stage.stageWidth = 32;
				actual_window_size.x = 32;
			}
			
			if (actual_window_size.y < 32)
			{
				stage.stageHeight = 32;
				actual_window_size.y = 32;
			}
			
			if (actual_window_size.x > frame_buffer.MaxBufferWidth)
			{
				stage.stageWidth = frame_buffer.MaxBufferWidth;
				actual_window_size.x = frame_buffer.MaxBufferWidth;
			}
			
			if (actual_window_size.y > frame_buffer.MaxBufferHeight)
			{
				stage.stageHeight = frame_buffer.MaxBufferHeight;
				actual_window_size.y = frame_buffer.MaxBufferHeight;
			}
			
			var horizontal_scale:Number = actual_window_size.x / screen_width;
			var vertical_scale:Number = actual_window_size.y / screen_height;
			var proportional_scale:Number = 1.0;
			
			if (horizontal_scale < vertical_scale)
				proportional_scale = horizontal_scale;
			else
				proportional_scale = vertical_scale;
				
			var window_resize_mode:int = 0;	
			
			if (window_resize_mode == 1)
			{
				//dynamic resolution adaptation
			}
			else
			if (window_resize_mode == 0)
			{
				//fixed resolution
				
				frame_buffer.SetScreenRegion((actual_window_size.x / 2) - (screen_width * proportional_scale / 2),
											 (actual_window_size.y / 2) - (screen_height * proportional_scale / 2),
											 screen_width * proportional_scale,
											 screen_height * proportional_scale);
			}
			
			screen_scale_x = proportional_scale;
			screen_scale_y = proportional_scale;
			screen_pivot_x = (actual_window_size.x / 2) - (screen_width * proportional_scale / 2);
			screen_pivot_y = (actual_window_size.y / 2) - (screen_height * proportional_scale / 2);
		}
		
		private function InitializationUpdate():Boolean
		{
			var is_done:Boolean = true;
			
			is_done = is_done && frame_buffer.IsEnabled;
			
			return is_done;
		}
		
		private var initialization_done:Boolean = false;
	}

}