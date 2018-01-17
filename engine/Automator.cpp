#include "Automator.h"

ok::AutomatorCurve::AutomatorCurve(ok::AutomatorCurveFunction function)
{
	_function = function;
	out = 0.f;
	time = 0.f;
	frames = 30.f;
	framerate = 30.f;
	playback_speed = 1.f;

	external_playback_speed_controller = nullptr;
	external_time_controller = nullptr;

	dt_cache = 0.f;

	repeat = 1.f;
	shift_x = 0.f;
	shift_y = 0.f;
	scale_y = 1.f;
	constant = 0.f;

	input_key = 0;
	inertia_time = 0.f;
	inertia_pre_scale = 1.f;
	inertia_post_scale = 1.f;
	use_weights = false;
	blend_mode = ok::ColorBlendMode::Normal;
}

ok::AutomatorCurve::operator float()
{
	if (_function == ok::AutomatorCurveFunction::EXTERNAL_FUNCTION)
	{
		external_function();
		return *((float*)external_variable);
	}
	else if (_function == ok::AutomatorCurveFunction::EXTERNAL_VARIABLE)
	{
		return *((float*)external_variable);
	}

	return out;
}

ok::AutomatorCurve::operator int()
{
	if (_function == ok::AutomatorCurveFunction::EXTERNAL_FUNCTION)
	{
		external_function();
		return *((int*)external_variable);
	}
	else if (_function == ok::AutomatorCurveFunction::EXTERNAL_VARIABLE)
	{
		return *((int*)external_variable);
	}

	return (int)out;
}

ok::AutomatorCurve::operator bool()
{
	if (_function == ok::AutomatorCurveFunction::EXTERNAL_FUNCTION)
	{
		external_function();
		return *((bool*)external_variable);
	}
	else if (_function == ok::AutomatorCurveFunction::EXTERNAL_VARIABLE)
	{
		return *((bool*)external_variable);
	}

	return (out != 0.f);
}

ok::AutomatorCurve::operator glm::vec3()
{
	if (_function == ok::AutomatorCurveFunction::EXTERNAL_FUNCTION)
	{
		external_function();
		return *((glm::vec3*)external_variable);
	}
	else if (_function == ok::AutomatorCurveFunction::EXTERNAL_VARIABLE)
	{
		return *((glm::vec3*)external_variable);
	}

	return glm::vec3(out, 0.f, 0.f);
}

ok::AutomatorCurve & ok::AutomatorCurve::Input(int index)
{
	return *_inputs[index];
}

ok::AutomatorCurve & ok::AutomatorCurve::Output(int index)
{
	return *_outputs[index];
}

void ok::AutomatorCurve::Connect(ok::AutomatorCurve * from, int from_index, ok::AutomatorCurve * to, int to_index, float weight)
{
	to->_inputs.resize((to_index + 1));
	if (to->use_weights)
	to->_inputs_weights.resize((to_index + 1));
	from->_outputs.resize((from_index + 1));

	to->_inputs[to_index] = from;
	if (to->use_weights)
	to->_inputs_weights[to_index] = weight;
	from->_outputs[from_index] = to;
}

void ok::AutomatorCurve::Disconnect(ok::AutomatorCurve * from, int from_index, ok::AutomatorCurve * to, int to_index)
{
	to->_inputs.resize((to_index + 1));
	from->_outputs.resize((from_index + 1));

	to->_inputs[to_index] = nullptr;
	from->_outputs[from_index] = nullptr;
}

void ok::AutomatorCurve::Update(float dt)
{
	dt_cache = dt;

	if (external_time_controller == nullptr)
	{
		if (external_playback_speed_controller != nullptr)
		{
			playback_speed = (float)*external_playback_speed_controller;
		}

		time += (dt * (framerate / frames))*playback_speed;
	}
	else
	{
		time = (float)*external_time_controller;
	}
	

	if (time > 1.f)  std::modf(time, &time);
	if (time < 0.f) { std::modf(time, &time); time += 1.f; }

	switch (_function)
	{
		case ok::AutomatorCurveFunction::EXTERNAL_FUNCTION :
		{
			external_function();
		}
		break;

		case ok::AutomatorCurveFunction::EXTERNAL_VARIABLE:
		{
			//do nothing
		}
		break;
	}
}

void ok::AutomatorCurve::AssignExternalController_PlaybackSpeed(ok::AutomatorCurve * controller)
{
	external_playback_speed_controller = controller;
}

void ok::AutomatorCurve::AssignExternalController_Time(ok::AutomatorCurve * controller)
{
	external_time_controller = controller;
}

void ok::AutomatorCurve::SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, float value, int custom_parameter_id)
{

	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::constant :
		{
			constant = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::repeat:
		{
			repeat = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_x:
		{
			shift_x = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_y:
		{
			shift_y = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::scale_y:
		{
			scale_y = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::inertia_pre_scale:
		{
			inertia_pre_scale = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::inertia_post_scale:
		{
			inertia_post_scale = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				custom_parameters.resize(custom_parameter_id + 1);
				custom_parameters[custom_parameter_id] = new float(value);
			}
			else *(float*)custom_parameters[custom_parameter_id] = value;
		}
		break;
	}
}

void ok::AutomatorCurve::SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, int value, int custom_parameter_id)
{
	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				custom_parameters.resize(custom_parameter_id + 1);
				custom_parameters[custom_parameter_id] = new int(value);
			}
			else *(int*)custom_parameters[custom_parameter_id] = value;
		}
		break;
		default:
		{
			SetFunctionParameter(id, (float)value, custom_parameter_id);
		}
		break;
	}
}

void ok::AutomatorCurve::SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, bool value, int custom_parameter_id)
{
	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				custom_parameters.resize(custom_parameter_id + 1);
				custom_parameters[custom_parameter_id] = new bool(value);
			}
			else *(bool*)custom_parameters[custom_parameter_id] = value;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::use_weights:
		{
			use_weights = value;
		}
		break;
		default:
		{
			SetFunctionParameter(id, (float)value, custom_parameter_id);
		}
		break;
	}
}

void ok::AutomatorCurve::SetFunctionParameter(ok::AutomatorCurveFunctionParameter id, std::string & value, int custom_parameter_id)
{
	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				custom_parameters.resize(custom_parameter_id + 1);
				custom_parameters[custom_parameter_id] = new std::string(value);
			}
			else *(std::string*)custom_parameters[custom_parameter_id] = value;
		}
		break;
		default:
		{
			SetFunctionParameter(id, std::stof(value), custom_parameter_id);
		}
		break;
	}
}

void ok::AutomatorCurve::SetFunctionParameter(ok::KKey value)
{
	input_key = static_cast<int>(value);
}

void ok::AutomatorCurve::SetFunctionParameter(ok::MKey value)
{
	input_key = static_cast<int>(ok::KKey::KeyCount) + static_cast<int>(value);
}

void ok::AutomatorCurve::SetFunctionParameter(ok::ColorBlendMode value)
{
	blend_mode = value;
}

float ok::AutomatorCurve::GetFunctionParameter_f(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id)
{
	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::constant:
		{
			return constant;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::repeat:
		{
			return repeat;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_x:
		{
			return shift_x;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_y:
		{
			return shift_y;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::scale_y:
		{
			return scale_y;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::inertia_pre_scale:
		{
			return inertia_pre_scale;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::inertia_post_scale:
		{
			return inertia_post_scale;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				return 0.0f;
			}
			else return *(float*)custom_parameters[custom_parameter_id];
		}
		break;
	}

	return 0.0f;
}

int ok::AutomatorCurve::GetFunctionParameter_i(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id)
{
	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::constant:
		{
			return (int)constant;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::repeat:
		{
			return (int)repeat;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_x:
		{
			return (int)shift_x;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_y:
		{
			return (int)shift_y;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::scale_y:
		{
			return (int)scale_y;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::input_key:
		{
			return input_key;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				return 0;
			}
			else return *(int*)custom_parameters[custom_parameter_id];
		}
		break;
	}

	return 0;
}

bool ok::AutomatorCurve::GetFunctionParameter_b(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id)
{
	switch (id)
	{
		case ok::AutomatorCurveFunctionParameter::constant:
		{
			return constant != 0.0f;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::repeat:
		{
			return repeat != 0.0f;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_x:
		{
			return shift_x != 0.0f;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::shift_y:
		{
			return shift_y != 0.0f;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::scale_y:
		{
			return scale_y != 0.0f;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::use_weights:
		{
			return use_weights;
		}
		break;
		case ok::AutomatorCurveFunctionParameter::custom_parameter:
		{
			if ((int)custom_parameters.size() < custom_parameter_id + 1)
			{
				return false;
			}
			else return *(bool*)custom_parameters[custom_parameter_id];
		}
		break;
	}

	return 0;
}

std::string ok::AutomatorCurve::GetFunctionParameter_s(ok::AutomatorCurveFunctionParameter id, int custom_parameter_id)
{
	switch (id)
	{
	case ok::AutomatorCurveFunctionParameter::constant:
	{
		return std::to_string(constant);
	}
	break;
	case ok::AutomatorCurveFunctionParameter::repeat:
	{
		return std::to_string(repeat);
	}
	break;
	case ok::AutomatorCurveFunctionParameter::shift_x:
	{
		return std::to_string(shift_x);
	}
	break;
	case ok::AutomatorCurveFunctionParameter::shift_y:
	{
		return std::to_string(shift_y);
	}
	break;
	case ok::AutomatorCurveFunctionParameter::scale_y:
	{
		return std::to_string(scale_y);
	}
	break;
	case ok::AutomatorCurveFunctionParameter::custom_parameter:
	{
		if ((int)custom_parameters.size() < custom_parameter_id + 1)
		{
			return "";
		}
		else return *(std::string*)custom_parameters[custom_parameter_id];
	}
	break;
	}

	return "";
}

void ok::Automator::Update(float dt)
{
	//update all curves
	for (auto&& curve : curves)
	{
		curve->Update(dt);
	}

	//update all connections
	for (auto&& connection : connections)
	{
		AutomatorChannel& channel = channels[connection->channel_index];

		if (channel.type == ok::AutomatorChannelType::Float)
		{
			*channel.ref_float = (float)*curves[connection->curves_indexes[0]];
		}
		else if (channel.type == ok::AutomatorChannelType::Function)
		{
			channel.ref_function(connection->curves_indexes);
		}
		else if (channel.type == ok::AutomatorChannelType::Int)
		{
			*channel.ref_int = (int)*curves[connection->curves_indexes[0]];
		}

	}
}

ok::AutomatorCurve & ok::Automator::Curve(int index)
{
	return *curves[index];
}

void ok::Automator::AssignCurve(int index, ok::AutomatorCurve * curve)
{
	curves.resize(index + 1);
	curves[index] = curve;
}

void ok::Automator::AssignChannel(int index, float & reference)
{
	channels.resize(index + 1);

	ok::AutomatorChannel& channel = channels[index];
	channel.ref_float = &reference;
	channel.type = ok::AutomatorChannelType::Float;
}

void ok::Automator::AssignChannel(int index, int & reference)
{
	channels.resize(index + 1);

	ok::AutomatorChannel& channel = channels[index];
	channel.ref_int = &reference;
	channel.type = ok::AutomatorChannelType::Int;
}

void ok::Automator::AssignChannel(int index, std::function<void(std::vector<int>& curves_indexes)> reference)
{
	channels.resize(index + 1);

	ok::AutomatorChannel& channel = channels[index];
	channel.ref_function = reference;
	channel.type = ok::AutomatorChannelType::Function;
}

void ok::Automator::Connect(int channel_index, std::vector<int>& curves_indexes)
{
	ok::AutomatorChannelConnection* connection = new ok::AutomatorChannelConnection();
	connection->channel_index = channel_index;
	connection->curves_indexes = curves_indexes;
	connections.push_back(connection);
}

void ok::AutomatorCurve::Function_Constant()
{
	out = constant;
}

void ok::AutomatorCurve::Function_Linear_Grow()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (0.0f + shift_y)*scale_y;
	max_range = (1.0f + shift_y)*scale_y;

	y = min_range + (max_range - min_range) * x;

	out = y;
}

void ok::AutomatorCurve::Function_Linear_Arc()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (0.0f + shift_y)*scale_y;
	max_range = (1.0f + shift_y)*scale_y;

	if (x < 0.5f)
		y = x * 2.0f;
	else
		y = 1.0f - (x - 0.5f) * 2.0f;

	y = min_range + (max_range - min_range) * y;

	out = y;
}

void ok::AutomatorCurve::Function_Sin()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (-1.0f + shift_y)*scale_y;
	max_range = (1.0f + shift_y)*scale_y;

	y = glm::sin(2.0f * glm::pi<float>() * x);
	y = (y + 1.0f) * 0.5f;

	y = min_range + (max_range - min_range) * y;

	out = y;
}

void ok::AutomatorCurve::Function_Cos()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (-1.0f + shift_y)*scale_y;
	max_range = (1.0f + shift_y)*scale_y;

	y = glm::cos(2.0f * glm::pi<float>() * x);
	y = (y + 1.0f) * 0.5f;

	y = min_range + (max_range - min_range) * y;

	out = y;
}

void ok::AutomatorCurve::Function_Slow_Hill()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (0.0f + shift_y)*scale_y;
	max_range = (1.0f + shift_y)*scale_y;

	float half_pi = glm::half_pi<float>();
	y = glm::sin(-half_pi + 3.0f*half_pi * x);
	y = (y + 1.0f) * 0.5f;

	y = min_range + (max_range - min_range) * y;

	out = y;
}

void ok::AutomatorCurve::Function_Fast_Hill()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (0.0f + shift_y)*scale_y;
	max_range = (1.0f + shift_y)*scale_y;

	y = glm::sin(glm::pi<float>() * x);

	y = min_range + (max_range - min_range) * y;

	out = y;
}

void ok::AutomatorCurve::Function_Slow_Pit()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (-1.0f + shift_y)*scale_y;
	max_range = (0.0f + shift_y)*scale_y;

	float half_pi = glm::half_pi<float>();
	y = glm::sin(-half_pi + 3.0f*half_pi * x);
	y = (y + 1.0f) * 0.5f;

	y = min_range + (max_range - min_range) * (1.0f - y);

	out = y;
}

void ok::AutomatorCurve::Function_Fast_Pit()
{
	float x = time;
	float y = 0.f;
	float min_range;
	float max_range;

	x *= repeat;
	x += shift_x;
	x = glm::abs(glm::fract(x));

	min_range = (-1.0f + shift_y)*scale_y;;
	max_range = (0.0f + shift_y)*scale_y;;

	y = glm::sin(glm::pi<float>() * x);

	y = min_range + (max_range - min_range) * (1.0f - y);

	out = y;
}

void ok::AutomatorCurve::Function_Key()
{
	out = (ok::Input::instance().KeyDown(input_key) == true) ? 1.f : 0.f;
}

void ok::AutomatorCurve::Function_Mouse_X()
{
	out = ok::Input::instance().MouseX();
}

void ok::AutomatorCurve::Function_Mouse_Y()
{
	out = ok::Input::instance().MouseY();
}

void ok::AutomatorCurve::Function_Input_Invert()
{
	if (_inputs.size() == 1)
	{
		out = 1.0f - (float)*_inputs[0];
	}
	else
	{
		outs.resize(_inputs.size());

		for (size_t i = 0; i < _inputs.size(); i++)
		{
			outs[i] = 1.0f - (float)*_inputs[i];
		}
	}
}

void ok::AutomatorCurve::Function_Input_Inertia()
{
	float input_value;

	if (_inputs.size() == 1)
	{
		input_value = (float)*_inputs[0];

		if (input_value > 0)
			out += dt_cache*inertia_pre_scale;
		else
			out -= dt_cache*inertia_post_scale;

		if (out > 1.0f) out = 1.0f;
		if (out < 0.0f) out = 0.0f;
	}
	else
	{
		outs.resize(_inputs.size());

		for (size_t i = 0; i < _inputs.size(); i++)
		{
			input_value = (float)*_inputs[i];
			float& out_i = outs[i];

			if (input_value > 0)
				out_i += dt_cache*inertia_pre_scale;
			else
				out_i -= dt_cache*inertia_post_scale;

			if (out_i > 1.0f) out_i = 1.0f;
			if (out_i < 0.0f) out_i = 0.0f;
		}
	}
}

void ok::AutomatorCurve::Function_Blend()
{
	std::vector<ok::AutomatorCurve*>& inputs = _inputs;
	std::vector<float>& blend_alpha = _inputs_weights;
	int i = 0;
	int count = inputs.size();

	float result = 0.0f;
	float alpha = 1.0f;

	if (false == use_weights)
	{
		result = (float)*inputs[0];

		switch (blend_mode)
		{
		case ok::ColorBlendMode::Normal:
		{
			result = (float)*inputs[inputs.size() - 1];
		}
		break;
		case ok::ColorBlendMode::Additive:
		{
			for (i = 1; i < count; i++)
				result += (float)*inputs[i];
		}
		break;
		case ok::ColorBlendMode::Subtract:
		{
			for (i = 1; i < count; i++)
				result -= (float)*inputs[i];
		}
		break;
		case ok::ColorBlendMode::Multiply:
		{
			for (i = 1; i < count; i++)
				result *= (float)*inputs[i];
		}
		break;
		case ok::ColorBlendMode::Divide:
		{
			for (i = 1; i < count; i++)
				result /= (float)*inputs[i];
		}
		break;
		case ok::ColorBlendMode::DivideSafe:
		{
			for (i = 1; i < count; i++)
			{
				if ((float)*inputs[i] != 0.0)
					result /= (float)*inputs[i];
			}
		}
		break;
		case ok::ColorBlendMode::Average:
		{
			for (i = 1; i < count; i++)
				result += (float)*inputs[i];
			result /= count;
		}
		break;
		case ok::ColorBlendMode::Max:
		{
			for (i = 1; i < count; i++)
			{
				result = (result > (float)*inputs[i]) ? result : (float)*inputs[i];
			}
		}
		break;
		case ok::ColorBlendMode::Min:
		{
			for (i = 1; i < count; i++)
			{
				result = (result < (float)*inputs[i]) ? result : (float)*inputs[i];
			}
		}
		break;
		}
	}
	else if (use_weights)
	{
		alpha = blend_alpha[0];
		result = (float)*inputs[0] * alpha;


		switch (blend_mode)
		{
		case ok::ColorBlendMode::Normal:
		{
			for (i = 1; i < count; i++)
			{
				alpha = blend_alpha[i];
				result = (1.0f - alpha) * result + alpha * (float)*inputs[i];
			}
		}
		break;
		case ok::ColorBlendMode::Additive:
		{
			for (i = 1; i < count; i++)
				result += (float)*inputs[i] * blend_alpha[i];
		}
		break;
		case ok::ColorBlendMode::Subtract:
		{
			for (i = 1; i < count; i++)
				result -= (float)*inputs[i] * blend_alpha[i];
		}
		break;
		case ok::ColorBlendMode::Multiply:
		{
			for (i = 1; i < count; i++)
				result *= (float)*inputs[i] * blend_alpha[i];
		}
		break;
		case ok::ColorBlendMode::Divide:
		{
			for (i = 1; i < count; i++)
				result /= (float)*inputs[i] * blend_alpha[i];
		}
		break;
		case ok::ColorBlendMode::DivideSafe:
		{
			for (i = 1; i < count; i++)
			{
				alpha = blend_alpha[i];
				if (((float)*inputs[i] * alpha) != 0.0)
					result /= (float)*inputs[i] * alpha;
			}
		}
		break;
		case ok::ColorBlendMode::Average:
		{
			for (i = 1; i < count; i++)
				result += (float)*inputs[i] * blend_alpha[i];
			result /= count;
		}
		break;
		case ok::ColorBlendMode::Max:
		{
			for (i = 1; i < count; i++)
			{
				alpha = blend_alpha[i];
				result = (result >(float)*inputs[i] * alpha) ? result : ((float)*inputs[i] * alpha);
			}
		}
		break;
		case ok::ColorBlendMode::Min:
		{
			for (i = 1; i < count; i++)
			{
				alpha = blend_alpha[i];
				result = (result < ((float)*inputs[i] * alpha)) ? result : ((float)*inputs[i] * alpha);
			}
		}
		break;
		}
	}

	out = result;
}
