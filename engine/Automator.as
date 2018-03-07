package com.engine 
{

	public class Automator 
	{
		public var channels:Vector.<Object> = null;
		public var curves:Vector.<AutomatorCurve> = null;
		
		public function Automator(channels_limit:int = 1, curves_limit:int = 1) 
		{
			channels = new Vector.<Object>(channels_limit, true);
			curves = new Vector.<AutomatorCurve>(curves_limit, true);
		}
		
		public function AssignCurve(index:int, curve:AutomatorCurve):void
		{
			curves[index] = curve;
		}
		
		public function AssignVariableChannel(index:int, var_owner:*, var_name:String):void
		{
			if (var_owner == null) channels[index] = null;
			else
			channels[index] = { type:0, var_owner:(var_owner as Object), var_name:var_name, curves:new Vector.<AutomatorCurve>() } as Object;
		}
		
		public function AssignFunctionChannel(index:int, fnc_owner:*, fnc:Function):void
		{
			if (fnc_owner == null) channels[index] = null;
			else
			channels[index] = { type:1, fnc_owner:(fnc_owner as Object), fnc:fnc, curves:new Vector.<AutomatorCurve>() } as Object;
		}
		
		public function Connect(channel_index:int, curves_indices:Array):void
		{
			var channel:Object = channels[channel_index];
			var curve:AutomatorCurve = null;
			var i:int = 0;
			
			for (i = 0; i < curves_indices.length; i++)
			{
				curve = curves[curves_indices[i]];
				channel.curves.push(curve);
			}
		}
		
		public function Update(dt:Number):void
		{
			var i:int = 0;
			var count:int = 0;
			
			var curve_i:int = 0;
			var curve_count:int = 0;
			
			var channel:Object = null;
			var curve:AutomatorCurve = null;
			
			var fnc:Function = null;
			
			count = curves.length;
			for (i = 0; i < count; i++)
			{
				curves[i].Update(dt);
			}
			
			count = channels.length;
			for (i = 0; i < count; i++)
			{
				channel = channels[i];
				
				if (channel != null)
				{
					if (channel.type == 0)
					{
						curve_i = 0;
						curve_count = channel.curves.length;
						
						if (curve_count > 0)
						{
							curve = channel.curves[curve_i] as AutomatorCurve;
							
							channel.var_owner[channel.var_name] = curve.output;
						}
					}
					else
					{
						curve_i = 0;
						curve_count = channel.curves.length;
						var curve_outputs:Array = new Array();
						
						for (curve_i = 0; curve_i < curve_count; curve_i++)
						{
							curve = channel.curves[curve_i] as AutomatorCurve;
							curve_outputs.push(curve.output);
						}
						
						fnc = channel.fnc as Function;
						
						if (curve_count == 0 && fnc.length != 0)
						{
							//skip
						}
						else
						if (curve_count == 0 && fnc.length == 0)
						{
							fnc.apply(channel.fnc_owner, null);
						}
						else
						{
							fnc.apply(channel.fnc_owner, curve_outputs);
						}
						//if ((channel.fnc as Function).length == curve_count)
						//(channel.fnc as Function).apply(channel.fnc_owner, curve_outputs);
					}
				}
			}
		}
	}

}