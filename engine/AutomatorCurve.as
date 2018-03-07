package com.engine 
{
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class AutomatorCurve 
	{	
		public static const DEFAULT:String = LINEAR_GROW;
		public static const CONSTANT:String = "constant";
		public static const LINEAR_GROW:String = "linear_grow";
		public static const LINEAR_ARC:String = "linear_arc";
		public static const SIN:String = "sin";
		public static const COS:String = "cos";
		public static const SLOW_HILL:String = "slow_hill";
		public static const FAST_HILL:String = "fast_hill";
		public static const SLOW_PIT:String = "slow_pit";
		public static const FAST_PIT:String = "fast_pit";
		public static const KEY:String = "key";
		public static const MOUSE_X:String = "mouse_x";
		public static const MOUSE_Y:String = "mouse_y";
		
		public static const LINEAR_PATH:String = "linear_path";
		public static const SMOOTH_PATH:String = "smooth_path";
		
		public static const INPUT_INVERT:String = "input_invert";
		public static const INPUT_INERTIA:String = "input_inertia";
		
		public var frames:Number = 30.0;
		public var framerate:Number = 30.0;
		public var time:Number = 0;
		public var playback_speed:Number = 1.0;
		public var output:Number = 0;
		
		public var playback_speed_controller:Object = null;
		public var time_controller:Object = null;
		public var input_controller:Object = null;
		public var output_controller:Object = null;
		private var _basic_output_controller:int = -1;
		
		public var parameters:Object = new Object();
		
		private var _cache_dt:Number = 0;
		
		public function AutomatorCurve() 
		{
			
		}
		
		public function AssignBasicController(controller_name:String = "linear_grow"):void
		{
			switch (controller_name)
			{
				case CONSTANT 		: _basic_output_controller = 0;  break;
				case LINEAR_GROW 	: _basic_output_controller = 1;  break;
				case DEFAULT 		: _basic_output_controller = 1;  break;
				case LINEAR_ARC 	: _basic_output_controller = 2;  break;
				case SIN 			: _basic_output_controller = 3;  break;
				case COS 			: _basic_output_controller = 4;  break;
				case SLOW_HILL 		: _basic_output_controller = 5;  break;
				case FAST_HILL 		: _basic_output_controller = 6;  break;
				case SLOW_PIT 		: _basic_output_controller = 7;  break;
				case FAST_PIT 		: _basic_output_controller = 8;  break;
				case KEY 			: _basic_output_controller = 9;  break;
				case MOUSE_X 		: _basic_output_controller = 10; break;
				case MOUSE_Y 		: _basic_output_controller = 11; break;
				case LINEAR_PATH	: _basic_output_controller = 12; break;
				case SMOOTH_PATH	: _basic_output_controller = 13; break;
				case INPUT_INVERT	: _basic_output_controller = 14; break;
				case INPUT_INERTIA	: { _basic_output_controller = 15; SetParameter("inertia_time", 0); } break;
				default				: _basic_output_controller = 1;
			}
		}
		
		public function Update(dt:Number):void
		{	
			_cache_dt = dt;
			
			if (time_controller == null)
			{
				if (playback_speed_controller != null)
				{
					playback_speed = playback_speed_controller.var_owner[playback_speed_controller.var_name];
				}
				
				time += (dt * (framerate / frames))*playback_speed;
			}
			else
			{
				time = time_controller.var_owner[time_controller.var_name];
			}
			
			if (time > 1.0) time = time % 1.0;
			if (time < 0.0) time = 1.0 + (time % 1.0);
			
			if (output_controller == null)
			{
				//basic curve types
				switch (_basic_output_controller)
				{
					case 0  : output = _basic_constant(); 		break; //CONSTANT
					case 1  : output = _basic_linear_grow(); 	break; //LINEAR_GROW, DEFAULT
					case 2  : output = _basic_linear_arc(); 	break; //LINEAR_ARC
					case 3  : output = _basic_sin(); 			break; //SIN
					case 4  : output = _basic_cos(); 			break; //COS
					case 5  : output = _basic_slow_hill(); 		break; //SLOW_HILL
					case 6  : output = _basic_fast_hill(); 		break; //FAST_HILL
					case 7  : output = _basic_slow_pit(); 		break; //SLOW_PIT
					case 8  : output = _basic_fast_pit(); 		break; //FAST_PIT
					case 9  : output = _basic_key(); 			break; //KEY
					case 10 : output = _basic_mouse_x(); 		break; //MOUSE_X
					case 11 : output = _basic_mouse_y(); 		break; //MOUSE_Y
					case 12 : output = _basic_linear_path(); 	break; //LINEAR_PATH
					
					case 14 : output = _basic_input_invert(); 	break; //INPUT_INVERT
					case 15 : output = _basic_input_inertia(); 	break; //INPUT_INERTIA
				}
			}
			else
			{
				if (output_controller.type == 0)
				{
					output = output_controller.var_owner[output_controller.var_name];
				}
				else
				{
					var args_out:Array = ParseArguments(output_controller.args as Array);
					
					var out_value:* = (output_controller.fnc as Function).apply(output_controller.fnc_owner, args_out);
					if (out_value is Boolean)
					{
						output = (out_value == true) ? 1 : 0;
					}
					if (out_value is Number)
					{
						output = out_value as Number;
					}
				}
			}
		}
		
		public function InputToValue():Number
		{
			if (input_controller != null)
			{
				if (input_controller.type == 0)
				{
					return input_controller.var_owner[input_controller.var_name];
				}
				else
				{
					var args_out:Array = ParseArguments(input_controller.args as Array);
					
					var out_value:* = (input_controller.fnc as Function).apply(input_controller.fnc_owner, args_out);
					if (out_value is Boolean)
					{
						return (out_value == true) ? 1 : 0;
					}
					if (out_value is Number)
					{
						return out_value as Number;
					}
				}
			}
			
			return 0;
		}
		
		public function ParseArguments(args:Array):Array
		{
			var args_out:Array = new Array();
			var args_in:Array = args;
			
			var i:int;
			var count:int;
			var arg:Object = null;
			
			count = args_in.length;
			
			for (i = 0; i < count; i++)
			{
				arg = args_in[i] as Object;
				if (arg.type == 0)
				{
					args_out.push(arg.arg_owner[arg.arg_name]);
				}
				else if (arg.type == 1)
				{
					args_out.push((arg.fnc as Function).apply(arg.fnc_owner, ParseArguments(arg.args)));
				}
				else
				{
					args_out.push(arg.value);
				}
			}
			
			return args_out;
		}
		
		public function AssignPlaybackSpeedController(var_owner:*, var_name:String):void
		{
			if (var_owner == null) playback_speed_controller = null;
			else
			playback_speed_controller = { var_owner:(var_owner as Object), var_name:var_name } as Object;
		}
		
		public function AssignTimeController(var_owner:*, var_name:String):void
		{
			if (var_owner == null) time_controller = null;
			else
			time_controller = { var_owner:(var_owner as Object), var_name:var_name } as Object;
		}
		
		public function AssignOutputControllerVariable(var_owner:*, var_name:String):void
		{
			if (var_owner == null) output_controller = null;
			else
			output_controller = { type:0, var_owner:(var_owner as Object), var_name:var_name } as Object;
		}
		
		public function AssignOutputControllerFunction(fnc_owner:*, fnc:Function, fnc_args:Array):void
		{
			if (fnc_owner == null) output_controller = null;
			else
			output_controller = { type:1, fnc_owner:(fnc_owner as Object), fnc:fnc, args:fnc_args} as Object;
		}
		
		public function AssignInputControllerVariable(var_owner:*, var_name:String):void
		{
			if (var_owner == null) input_controller = null;
			else
			input_controller = { type:0, var_owner:(var_owner as Object), var_name:var_name } as Object;
		}
		
		public function AssignInputControllerFunction(fnc_owner:*, fnc:Function, fnc_args:Array):void
		{
			if (fnc_owner == null) input_controller = null;
			else
			input_controller = { type:1, fnc_owner:(fnc_owner as Object), fnc:fnc, args:fnc_args} as Object;
		}
		
		public function LinkInputCurve(input_curve:AutomatorCurve):void
		{
			AssignInputControllerVariable(input_curve, "output");
		}
		
		public function LinkOutputCurve(output_curve:AutomatorCurve):void
		{
			output_curve.AssignInputControllerVariable(this, "output");
		}
		
		//------------------------------------------------
		
		public static function MakeVariableArgument(arg_owner:*, arg_name:String):Object
		{
			return { type:0, arg_owner:(arg_owner as Object), arg_name:arg_name } as Object;
		}
		
		public static function MakeConstantArgument(constant:*):Object
		{
			return { type:2, value:constant } as Object;
		}
		
		public static function MakeFunctionArgument(fnc_owner:*, fnc:Function, fnc_args:Array):Object
		{
			return { type:1, fnc_owner:(fnc_owner as Object), fnc:fnc, args:fnc_args } as Object;
		}
		
		//------------------------------------------------
		private var repeat:Number = 1;
		private var shift_x:Number = 0;
		private var shift_y:Number = 0;
		private var scale_y:Number = 1;
		private var constant:Number = 0;
		
		public function SetParameter(parameter:String, value:Number):void
		{
			parameters["enabled"] = true;
			
			switch (parameter)
			{
				case "repeat" : repeat = value; break;
				case "shift_x" : shift_x = value; break;
				case "shift_y" : shift_y = value; break;
				case "constant" : constant = value; break;
				case "scale_y" : scale_y = value; break;
				default : parameters[parameter] = value;
			}
		}
		
		private function _basic_constant():Number
		{
			return constant;
		}
		
		private function _basic_linear_grow():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (0.0+shift_y)*scale_y;
			max_range = (1.0+shift_y)*scale_y;
			
			y = min_range + (max_range - min_range) * x;
			
			return y;
		}
		
		private function _basic_linear_arc():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (0.0+shift_y)*scale_y;
			max_range = (1.0+shift_y)*scale_y;
			
			if (x < 0.5)
			y = x * 2.0;
			else
			y = 1.0 - (x - 0.5) * 2.0;
			
			y = min_range + (max_range - min_range) * y;
			
			return y;
		}
		
		private function _basic_sin():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (-1.0+shift_y)*scale_y;
			max_range = (1.0+shift_y)*scale_y;
			
			y = Math.sin(2.0 * Math.PI * x);
			y = (y + 1.0) * 0.5;
			
			y = min_range + (max_range - min_range) * y;
			
			return y;
		}
		
		private function _basic_cos():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (-1.0+shift_y)*scale_y;
			max_range = (1.0+shift_y)*scale_y;
			
			y = Math.cos(2.0 * Math.PI * x);
			y = (y + 1.0) * 0.5;
			
			y = min_range + (max_range - min_range) * y;
			
			return y;
		}
		
		private function _basic_slow_hill():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (0.0+shift_y)*scale_y;
			max_range = (1.0+shift_y)*scale_y;
			
			y = Math.sin(-Math.PI*0.5 + 3.0*Math.PI*0.5 * x);
			y = (y + 1.0) * 0.5;
			
			y = min_range + (max_range - min_range) * y;
			
			return y;
		}
		
		private function _basic_fast_hill():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (0.0+shift_y)*scale_y;
			max_range = (1.0+shift_y)*scale_y;
			
			y = Math.sin(Math.PI * x);
			
			y = min_range + (max_range - min_range) * y;
			
			return y;
		}
		
		private function _basic_slow_pit():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (-1.0+shift_y)*scale_y;
			max_range = (0.0+shift_y)*scale_y;
			
			y = Math.sin(-Math.PI*0.5 + 3.0*Math.PI*0.5 * x);
			y = (y + 1.0) * 0.5;
			
			y = min_range + (max_range - min_range) * (1.0-y);
			
			return y;
		}
		
		private function _basic_fast_pit():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			min_range = (-1.0+shift_y)*scale_y;;
			max_range = (0.0+shift_y)*scale_y;;
			
			y = Math.sin(Math.PI * x);

			y = min_range + (max_range - min_range) * (1.0-y);
			
			return y;
		}
		
		private function _basic_key():Number
		{
			var keycode:int = 0;
			if (parameters.enabled == true)
			{
				keycode = parameters.keycode;
			}
			
			return (Input.o.KeyDown(keycode) == true) ? 1 : 0;
		}
		
		private function _basic_mouse_x():Number
		{
			return Input.o.mouse_x;
		}
		
		private function _basic_mouse_y():Number
		{
			return Input.o.mouse_y;
		}
		
		private function _basic_linear_path():Number
		{
			var x:Number = time;
			var y:Number = 0;
			var min_range:Number;
			var max_range:Number;
			
			x *= repeat;
			x += shift_x;
			x = Math.abs(x % 1.0);
			
			var path_points:Vector.<Number> = null;
			var count:int = 0;
			var i:int = 0;
			
			var left_i:int = 0;
			var right_i:int = 0;
			
			if (parameters.enabled == true)
			{
				path_points = parameters.path_points;
				if (path_points != null)
				{
					count = path_points.length;
					
					x = parameters.path_x_min + parameters.path_width * x;
					
					for (i = 0; i < count; i+=2)
					{
						if (path_points[i] <= x) left_i = i;
						if (path_points[i] >= x) { right_i = i; break; }
					}
					
					y = path_points[left_i + 1] + (path_points[right_i + 1] - path_points[left_i + 1]) * ( (x - path_points[left_i]) / (path_points[right_i] - path_points[left_i]) );
				}
			}
			
			return y;
		}
		
		private function _basic_input_invert():Number
		{
			return 1.0 - InputToValue();
		}
		
		private function _basic_input_inertia():Number
		{
			var input_value:Number = InputToValue();
			
			if (parameters.enabled == true)
			{
				if (input_value > 0)
				parameters.inertia_time += _cache_dt*parameters.inertia_scale;
				else
				parameters.inertia_time -= _cache_dt*parameters.inertia_scale;
				
				if (parameters.inertia_time > 1.0) parameters.inertia_time = 1.0;
				if (parameters.inertia_time < 0.0) parameters.inertia_time = 0.0;
				
				return parameters.inertia_time;
			}
			
			return 0;
		}
		
		//----------------------
		
		public static function CreateFromPathFile(path:String, smooth:Boolean = false, flat:Boolean = true):Vector.<AutomatorCurve>
		{
			var file:ByteArray = Assets.instance.GetFileAsByteArray(path);
			file.endian = Endian.LITTLE_ENDIAN;
			
			var i:int = 0;
			var j:int = 0;
			var count:int = 0;
			var points_count:int = 0;
			
			var result:Vector.<AutomatorCurve> = null;
			
			while (file.bytesAvailable > 0)
			{
				file.readUTFBytes(file.readInt()); //read node name (not used yet)
				
				count = file.readInt();
				
				if (flat == true) //interpret path as 2d curve
				{
					result = new Vector.<AutomatorCurve>(count, true);
					
					for (i = 0; i < count; i++)
					{
						result[i] = new AutomatorCurve();
						
						if (smooth)
						result[i].AssignBasicController(SMOOTH_PATH);
						else
						result[i].AssignBasicController(LINEAR_PATH);
						
						result[i].parameters["enabled"] = true;
						result[i].parameters["name"] = file.readUTFBytes(file.readInt()); //read curve name (position_x, spline1, etc...)
						points_count = file.readInt();
						
						result[i].parameters["path_points"] = null;
						
						if (points_count > 0)
						{
							result[i].parameters.path_points = new Vector.<Number>(points_count * 2, true);
							
							for (j = 0; j < points_count; j++)
							{
								result[i].parameters.path_points[j * 2] = file.readFloat();
								result[i].parameters.path_points[j * 2 + 1] = file.readFloat();
								file.readFloat();
							}
							
							result[i].parameters["path_x_min"] = file.readFloat();
							result[i].parameters["path_y_min"] = file.readFloat();
							file.readFloat();
							
							result[i].parameters["path_x_max"] = file.readFloat();
							result[i].parameters["path_y_max"] = file.readFloat();
							file.readFloat();
							
							result[i].parameters["path_width"] = Math.abs(result[i].parameters.path_x_max - result[i].parameters.path_x_min);
							result[i].parameters["path_height"] = Math.abs(result[i].parameters.path_y_max - result[i].parameters.path_y_min);
						}
					}
				}
				else if (flat == false) //interpret path as list of 3d points (x axis is point index, y axis is point coordinate X, Y, or Z)
				{
					result = new Vector.<AutomatorCurve>(count * 3, true);
					
					for (i = 0; i < count; i++)
					{
						result[i * 3] = new AutomatorCurve();
						result[i * 3 + 1] = new AutomatorCurve();
						result[i * 3 + 2] = new AutomatorCurve();
						
						
						if (smooth)
						{
							result[i * 3].AssignBasicController(SMOOTH_PATH);
							result[i * 3 + 1].AssignBasicController(SMOOTH_PATH);
							result[i * 3 + 2].AssignBasicController(SMOOTH_PATH);
						}
						else
						{
							result[i * 3].AssignBasicController(LINEAR_PATH);
							result[i * 3 + 1].AssignBasicController(LINEAR_PATH);
							result[i * 3 + 2].AssignBasicController(LINEAR_PATH);
						}
						
						var channel_name:String = file.readUTFBytes(file.readInt()); //read curve name (position_x, spline1, etc...)
						
						result[i * 3].parameters["enabled"] = true;
						result[i * 3 + 1].parameters["enabled"] = true;
						result[i * 3 + 2].parameters["enabled"] = true;
						
						result[i * 3].parameters["name"] = channel_name + "_x";
						result[i * 3 + 1].parameters["name"] = channel_name + "_y";
						result[i * 3 + 2].parameters["name"] = channel_name + "_z";
						
						points_count = file.readInt();
						
						result[i * 3].parameters["path_points"] = null;
						result[i * 3 + 1].parameters["path_points"] = null;
						result[i * 3 + 2].parameters["path_points"] = null;
						
						if (points_count > 0)
						{
							result[i * 3].parameters.path_points = new Vector.<Number>(points_count * 2, true);
							result[i * 3 + 1].parameters.path_points = new Vector.<Number>(points_count * 2, true);
							result[i * 3 + 2].parameters.path_points = new Vector.<Number>(points_count * 2, true);
							
							for (j = 0; j < points_count; j++)
							{
								result[i * 3].parameters.path_points[j * 2] = j;
								result[i * 3 + 1].parameters.path_points[j * 2] = j;
								result[i * 3 + 2].parameters.path_points[j * 2] = j;
								
								result[i * 3].parameters.path_points[j * 2 + 1] = file.readFloat();
								result[i * 3 + 1].parameters.path_points[j * 2 + 1] = file.readFloat();
								result[i * 3 + 2].parameters.path_points[j * 2 + 1] = file.readFloat();
							}
							
							result[i * 3].parameters["path_x_min"] = 0;
							result[i * 3 + 1].parameters["path_x_min"] = 0;
							result[i * 3 + 2].parameters["path_x_min"] = 0;
							
							result[i * 3].parameters["path_y_min"] = file.readFloat(); //x min
							result[i * 3 + 1].parameters["path_y_min"] = file.readFloat(); //y min
							result[i * 3 + 2].parameters["path_y_min"] = file.readFloat(); //z min
							
							result[i * 3].parameters["path_x_max"] = points_count-1;
							result[i * 3 + 1].parameters["path_x_max"] = points_count-1;
							result[i * 3 + 2].parameters["path_x_max"] = points_count - 1;
							
							result[i * 3].parameters["path_y_max"] = file.readFloat(); //x max
							result[i * 3 + 1].parameters["path_y_max"] = file.readFloat(); //y max
							result[i * 3 + 2].parameters["path_y_max"] = file.readFloat(); //z max
							
							result[i * 3].parameters["path_width"] = Math.abs(result[i * 3].parameters.path_x_max - result[i * 3].parameters.path_x_min);
							result[i * 3 + 1].parameters["path_width"] = Math.abs(result[i * 3].parameters.path_x_max - result[i * 3].parameters.path_x_min);
							result[i * 3 + 2].parameters["path_width"] = Math.abs(result[i * 3].parameters.path_x_max - result[i * 3].parameters.path_x_min);
							
							result[i * 3].parameters["path_height"] = Math.abs(result[i * 3].parameters.path_y_max - result[i * 3].parameters.path_y_min);
							result[i * 3 + 1].parameters["path_height"] = Math.abs(result[i * 3].parameters.path_y_max - result[i * 3].parameters.path_y_min);
							result[i * 3 + 2].parameters["path_height"] = Math.abs(result[i * 3].parameters.path_y_max - result[i * 3].parameters.path_y_min);
						}
					}
				}
			}
			
			return result;
		}
	}

}