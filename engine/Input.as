package com.engine 
{
	import flash.display.Stage;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	
	public class Input 
	{	
		//normalized format 0 .. 1
		public var mouse_x:Number = 0;
		public var mouse_y:Number = 0; 
		
		public var states:Vector.<int> = new Vector.<int>(128 + 3, true); //128 keyboard + 3 mouse
		private var last_states_keydown:Vector.<Boolean> = new Vector.<Boolean>(128 + 3, true);
		
		private var states_queue:Vector.<int> = new Vector.<int>(100, true);
		private var states_queue_position:int = 0;
		
		public static const state_down:int = 1; 	//00000001
		public static const state_pressed:int = 2; 	//00000010
		public static const state_released:int = 4; //00000100
		
		public function Input(stage:Stage) 
		{
			if (instance != null)
			{
				throw new Error("Input is a singleton, use Input.instance");
			} else {
				stage.addEventListener(KeyboardEvent.KEY_DOWN, OnKeyDown);
				stage.addEventListener(KeyboardEvent.KEY_UP, OnKeyUp);
				
				stage.addEventListener(MouseEvent.MOUSE_DOWN, OnLeftMouseDown);
				stage.addEventListener(MouseEvent.MOUSE_UP, OnLeftMouseUp);
				stage.addEventListener(MouseEvent.RIGHT_MOUSE_DOWN, OnRightMouseDown);
				stage.addEventListener(MouseEvent.RIGHT_MOUSE_UP, OnRightMouseUp);
				stage.addEventListener(MouseEvent.MIDDLE_MOUSE_DOWN, OnMiddleMouseDown);
				stage.addEventListener(MouseEvent.MIDDLE_MOUSE_UP, OnMiddleMouseUp);
				
				stage.addEventListener(MouseEvent.MOUSE_MOVE, OnMouseMove);
				
				ClearStatesQueue();
			}
		}
		
		public function Update():void
		{
			var i:int;
			var keyCode:int;
			var keyState:int;
			
			for (i = 0; i < states_queue_position; i++)
			{
				keyState = states_queue[i];
				keyCode = ((keyState >> 8) & 0x000000ff);
				keyState = (keyState & 0x000000ff);
				
				if (keyState & state_released)
				{
					states[keyCode] = 0;
				}
				
				if (keyState & state_pressed)
				{
					states[keyCode] = (states[keyCode] & ~state_pressed) | state_down;
				}
				
				states_queue[i] = -1;
			}
			
			states_queue_position = 0;
		}
		
		public function KeyDown(keyCode:int):Boolean
		{
			return (states[keyCode] & state_down) ? true : false;
		}
		
		public function KeyPressed(keyCode:int):Boolean
		{
			return (states[keyCode] & state_pressed) ? true : false;
		}
		
		public function KeyReleased(keyCode:int):Boolean
		{
			return (states[keyCode] & state_released) ? true : false;
		}
		
		private function OnKeyDown(e:KeyboardEvent):void
		{
			if (e.charCode == 0)
			{
				if (e.keyCode < 128)
				{
					if (last_states_keydown[e.keyCode] == false)
					{
						PushStateToQueue(e.keyCode, state_pressed | state_down);
						last_states_keydown[e.keyCode] = true;
					}
				}
			}
			else
			{
				if (e.charCode < 128)
				{
					if (last_states_keydown[e.charCode] == false)
					{
						PushStateToQueue(e.charCode, state_pressed | state_down);
						last_states_keydown[e.charCode] = true;
					}
				}
			}
		}
		
		private function OnKeyUp(e:KeyboardEvent):void
		{
			if (e.charCode == 0)
			{
				if (e.keyCode < 128)
				{
					PushStateToQueue(e.keyCode, state_released);
					last_states_keydown[e.keyCode] = false;
				}
			}
			else
			{
				if (e.charCode < 128)
				{
					PushStateToQueue(e.charCode, state_released);
					last_states_keydown[e.charCode] = false;
				}
			}
		}
		
		private function OnLeftMouseDown(e:MouseEvent = null):void
		{
			PushStateToQueue(127+1, state_pressed | state_down);
		}
		
		private function OnLeftMouseUp(e:MouseEvent = null):void
		{
			PushStateToQueue(127+1, state_released);
		}
		
		private function OnRightMouseDown(e:MouseEvent = null):void
		{
			PushStateToQueue(127+2, state_pressed | state_down);
		}
		
		private function OnRightMouseUp(e:MouseEvent = null):void
		{
			PushStateToQueue(127+2, state_released);
		}
		
		private function OnMiddleMouseDown(e:MouseEvent = null):void
		{
			PushStateToQueue(127+3, state_pressed | state_down);
		}
		
		private function OnMiddleMouseUp(e:MouseEvent = null):void
		{
			PushStateToQueue(127+3, state_released);
		}
		
		private function OnMouseMove(e:MouseEvent):void
		{
			mouse_x = (e.stageX-Application.instance.screen_pivot_x) / (Application.instance.screen_width * Application.instance.screen_scale_x);
			mouse_y = (e.stageY-Application.instance.screen_pivot_y) / (Application.instance.screen_height * Application.instance.screen_scale_y);
		}
		
		private function PushStateToQueue(keyCode:int, state:int):void
		{
			var old_state:int = states_queue[states_queue_position];
			
			if (old_state != -1)
			{
				//reset previous state before overwrite
				states[((old_state >> 8) & 0x000000ff)] = 0;
			}
			
			states_queue[states_queue_position] = (((keyCode << 8) & 0x0000ff00) | state);
			states[keyCode] |= state;
			
			states_queue_position++;
			if (states_queue_position > states_queue.length)
			{
				states_queue_position = 0;
			}
		}
		
		private function ClearStatesQueue():void
		{
			var i:int;
			
			for (i = 0; i < states_queue.length; i++)
			{
				states_queue[i] = -1;
			}
			
			states_queue_position = 0;
		}
		
		//---------------------------------------------------------
		
		public static var instance:Input = null;
		public static var o:Input = null;
		
		public static function create(stage:Stage):void
		{
			if (instance == null)
			{
				instance = new Input(stage);
				o = instance;
			}
		}
		
		/*		
		public static const KEY_SHIFT:int = 16;
		public static const KEY_CTRL:int = 17;
		public static const KEY_ALT:int = 18;
		
		public static const KEY_MOUSE_LEFT:int = 128;
		public static const KEY_MOUSE_RIGHT:int = 129;
		public static const KEY_MOUSE_MIDDLE:int = 130;
		
		public static const KEY_BACKSPACE:int = 8;
		public static const KEY_TAB:int = 9;
		public static const KEY_ENTER:int = 13;
		public static const KEY_CAPSLOCK:int = 20;
		public static const KEY_ESCAPE:int = 27;
		public static const KEY_SPACEBAR:int = 32;
		public static const KEY_PAGE_UP:int = 33;
		public static const KEY_PAGE_DOWN:int = 34;
		public static const KEY_END:int = 35;
		public static const KEY_HOME:int = 36;
		public static const KEY_LEFT_ARROW:int = 37;
		public static const KEY_UP_ARROW:int = 38;
		public static const KEY_RIGHT_ARROW:int = 39;
		public static const KEY_DOWN_ARROW:int = 40;
		public static const KEY_INSERT:int = 45;
		public static const KEY_DELETE:int = 46;
		public static const KEY_NUMLOCK:int = 144;
		public static const KEY_SCRLK:int = 145;
		public static const KEY_PAUSE_BREAK:int = 19;
		public static const KEY_A:int = 65;
		public static const KEY_B:int = 66;
		public static const KEY_C:int = 67;
		public static const KEY_D:int = 68;
		public static const KEY_E:int = 69;
		public static const KEY_F:int = 70;
		public static const KEY_G:int = 71;
		public static const KEY_H:int = 72;
		public static const KEY_I:int = 73;
		public static const KEY_J:int = 74;
		public static const KEY_K:int = 75;
		public static const KEY_L:int = 76;
		public static const KEY_M:int = 77;
		public static const KEY_N:int = 78;
		public static const KEY_O:int = 79;
		public static const KEY_P:int = 80;
		public static const KEY_Q:int = 81;
		public static const KEY_R:int = 82;
		public static const KEY_S:int = 83;
		public static const KEY_T:int = 84;
		public static const KEY_U:int = 85;
		public static const KEY_V:int = 86;
		public static const KEY_W:int = 87;
		public static const KEY_X:int = 88;
		public static const KEY_Y:int = 89;
		public static const KEY_Z:int = 90;
		public static const KEY_a:int = 97;
		public static const KEY_b:int = 98;
		public static const KEY_c:int = 99;
		public static const KEY_d:int = 100;
		public static const KEY_e:int = 101;
		public static const KEY_f:int = 102;
		public static const KEY_g:int = 103;
		public static const KEY_h:int = 104;
		public static const KEY_i:int = 105;
		public static const KEY_j:int = 106;
		public static const KEY_k:int = 107;
		public static const KEY_l:int = 108;
		public static const KEY_m:int = 109;
		public static const KEY_n:int = 110;
		public static const KEY_o:int = 111;
		public static const KEY_p:int = 112;
		public static const KEY_q:int = 113;
		public static const KEY_r:int = 114;
		public static const KEY_s:int = 115;
		public static const KEY_t:int = 116;
		public static const KEY_u:int = 117;
		public static const KEY_v:int = 118;
		public static const KEY_w:int = 119;
		public static const KEY_x:int = 120;
		public static const KEY_y:int = 121;
		public static const KEY_z:int = 122;
		public static const KEY_0:int = 48;
		public static const KEY_1:int = 49;
		public static const KEY_2:int = 50;
		public static const KEY_3:int = 51;
		public static const KEY_4:int = 52;
		public static const KEY_5:int = 53;
		public static const KEY_6:int = 54;
		public static const KEY_7:int = 55;
		public static const KEY_8:int = 56;
		public static const KEY_9:int = 57;
		public static const KEY_SEMICOLON:int = 186;
		public static const KEY_COLON:int = 186;
		public static const KEY_PLUS:int = 187;
		public static const KEY_EQUALS:int = 187;
		public static const KEY_MINUS:int = 189;
		public static const KEY_UNDERSCORE:int = 189;
		public static const KEY_SLASH:int = 191;
		public static const KEY_QUESTION:int = 191;
		public static const KEY_BACKTICK:int = 192;
		public static const KEY_TILDE:int = 192;
		public static const KEY_BRACKET_OPEN:int = 219;
		public static const KEY_VERTICAL_BAR:int = 220;
		public static const KEY_BACKSLASH:int = 220;
		public static const KEY_BRACKET_CLOSE:int = 221;
		public static const KEY_QUOTATION:int = 222;
		public static const KEY_APOSTROPHE:int = 222;
		public static const KEY_COMMA:int = 188;
		public static const KEY_PERIOD:int = 190;
		public static const KEY_NUMPAD_0:int = 96;
		public static const KEY_NUMPAD_1:int = 97;
		public static const KEY_NUMPAD_2:int = 98;
		public static const KEY_NUMPAD_3:int = 99;
		public static const KEY_NUMPAD_4:int = 100;
		public static const KEY_NUMPAD_5:int = 101;
		public static const KEY_NUMPAD_6:int = 102;
		public static const KEY_NUMPAD_7:int = 103;
		public static const KEY_NUMPAD_8:int = 104;
		public static const KEY_NUMPAD_9:int = 105;
		public static const KEY_NUMPAD_MULTIPLY:int = 106;
		public static const KEY_NUMPAD_PLUS:int = 107;
		public static const KEY_NUMPAD_ENTER:int = 13;
		public static const KEY_NUMPAD_MINUS:int = 109;
		public static const KEY_NUMPAD_DECIMAL:int = 110;
		public static const KEY_NUMPAD_DIVIDE:int = 111;
		public static const KEY_F1:int = 112;
		public static const KEY_F2:int = 113;
		public static const KEY_F3:int = 114;
		public static const KEY_F4:int = 115;
		public static const KEY_F5:int = 116;
		public static const KEY_F6:int = 117;
		public static const KEY_F7:int = 118;
		public static const KEY_F8:int = 119;
		public static const KEY_F9:int = 120;
		public static const KEY_F10:int = 121;
		public static const KEY_F11:int = 122;
		public static const KEY_F12:int = 123;
		public static const KEY_F13:int = 124;
		public static const KEY_F14:int = 125;
		public static const KEY_F15:int = 126;
		*/ 
	}

}