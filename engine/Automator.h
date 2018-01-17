#pragma once

#include "Input.h"
#include "Color.h"

namespace ok
{
	enum class AutomatorCurveFunction
	{
		EXTERNAL_FUNCTION = 0,
		EXTERNAL_VARIABLE,
		CONSTANT,
		LINEAR_GROW,
		LINEAR_ARC,
		SIN,
		COS,
		SLOW_HILL,
		FAST_HILL,
		SLOW_PIT,
		FAST_PIT,
		KEY,
		MOUSE_X,
		MOUSE_Y,
		LINEAR_PATH,
		SMOOTH_PATH,
		INPUT_INVERT,
		INPUT_INERTIA,
		BLEND
	};

	enum class AutomatorCurveFunctionParameter
	{
		constant = 0,
		shift_x,
		shift_y,
		scale_y,
		repeat,
		input_key,
		inertia_pre_scale,
		inertia_post_scale,
		blend_mode,
		use_weights,
		custom_parameter
	};

	class AutomatorCurve
	{
	public:
		AutomatorCurve(ok::AutomatorCurveFunction function);

		float dt_cache;

		std::vector<float> outs;
		float out;
		float time;
		float frames;
		float framerate;
		float playback_speed;
		std::vector<void*> custom_parameters;

		operator float();
		operator int();
		operator bool();
		operator glm::vec3();

		void* external_variable;
		std::function<void()> external_function;

		ok::AutomatorCurve& Input(int index);
		ok::AutomatorCurve& Output(int index);

		static void Connect(ok::AutomatorCurve * from, int from_index, ok::AutomatorCurve * to, int to_index, float weight = 1.0f);
		static void Disconnect(ok::AutomatorCurve * from, int from_index, ok::AutomatorCurve * to, int to_index);

		void Update(float dt);
		void AssignExternalController_PlaybackSpeed(ok::AutomatorCurve * controller);
		void AssignExternalController_Time(ok::AutomatorCurve * controller);

		void SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, float value, int custom_parameter_id = -1);
		void SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, int value, int custom_parameter_id = -1);
		void SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, bool value, int custom_parameter_id = -1);
		void SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, std::string& value, int custom_parameter_id = -1);
		void SetFunctionParameter(ok::KKey value);
		void SetFunctionParameter(ok::MKey value);
		void SetFunctionParameter(ok::ColorBlendMode value);

		float GetFunctionParameter_f(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id = -1);
		int GetFunctionParameter_i(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id = -1);
		bool GetFunctionParameter_b(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id = -1);
		std::string GetFunctionParameter_s(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id = -1);

	private:
		ok::AutomatorCurveFunction _function;
		std::vector<ok::AutomatorCurve*> _inputs;
		std::vector<float> _inputs_weights;
		std::vector<ok::AutomatorCurve*> _outputs;

		ok::AutomatorCurve* external_playback_speed_controller;
		ok::AutomatorCurve* external_time_controller;

		float repeat;
		float shift_x;
		float shift_y;
		float scale_y;
		float constant;
		int input_key;
		float inertia_time;
		float inertia_pre_scale;
		float inertia_post_scale;
		bool use_weights;
		ok::ColorBlendMode blend_mode;


		void Function_Constant();
		void Function_Linear_Grow();
		void Function_Linear_Arc();
		void Function_Sin();
		void Function_Cos();
		void Function_Slow_Hill();
		void Function_Fast_Hill();
		void Function_Slow_Pit();
		void Function_Fast_Pit();
		void Function_Key();
		void Function_Mouse_X();
		void Function_Mouse_Y();
		//void Function_Linear_Path();
		//void Function_Smooth_Path();
		void Function_Input_Invert();
		void Function_Input_Inertia();
		void Function_Blend();
	};

	enum class AutomatorChannelType
	{
		Float = 0,
		Int,
		Function
	};

	class AutomatorChannel
	{
	public:
		int* ref_int;
		float* ref_float;
		std::function<void(std::vector<int>& curves_indexes)> ref_function;

		ok::AutomatorChannelType type;
	};

	class AutomatorChannelConnection
	{
	public:
		int channel_index;
		std::vector<int> curves_indexes;
	};

	class Automator
	{
	public:
		void Update(float dt);
		ok::AutomatorCurve& Curve(int index);

		void AssignCurve(int index, ok::AutomatorCurve* curve);

		void AssignChannel(int index, float & reference);
		void AssignChannel(int index, int & reference);
		void AssignChannel(int index, std::function<void(std::vector<int>& curves_indexes)> reference);

		void Connect(int channel_index, std::vector<int>& curves_indexes);
	protected:
	private:
		std::vector<ok::AutomatorCurve*> curves;
		std::vector<ok::AutomatorChannel> channels;
		std::vector<ok::AutomatorChannelConnection*> connections;
	};
}
