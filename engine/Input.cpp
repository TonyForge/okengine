#include "Input.h"

ok::Input::Input()
{
	states.resize(sf::Keyboard::KeyCount + 3);
	last_states_keydown.resize(sf::Keyboard::KeyCount + 3);
	states_queue.resize(100);
	states_queue_position = 0;

	mouse_x = 0;
	mouse_y = 0;

	state_down = 1;
	state_pressed = 2;
	state_released = 4;

	last_key_index = static_cast<int>(sf::Keyboard::KeyCount);

	ClearStatesQueue();

	SetCurrentLayer();
}

ok::Input & ok::Input::instance()
{
	static ok::Input s;
	return s;
}

ok::Input & ok::Input::o()
{
	return instance();
}

bool ok::Input::KeyDown(ok::KKey key)
{
	return (states[static_cast<int>(key)] & state_down) ? true : false;
}

bool ok::Input::KeyDown(ok::MKey key)
{
	if (IsPointBlocked(_current_input_layer, MousePX(), MousePY())) return false;

	return (states[last_key_index + static_cast<int>(key)] & state_down) ? true : false;
}

bool ok::Input::KeyDown(int key)
{
	return (states[key] & state_down) ? true : false;
}

bool ok::Input::KeyPressed(ok::KKey key)
{
	return (states[static_cast<int>(key)] & state_pressed) ? true : false;
}

bool ok::Input::KeyPressed(ok::MKey key)
{
	if (IsPointBlocked(_current_input_layer, MousePX(), MousePY())) return false;

	return (states[last_key_index + static_cast<int>(key)] & state_pressed) ? true : false;
}

bool ok::Input::KeyPressed(int key)
{
	return (states[key] & state_pressed) ? true : false;
}

bool ok::Input::KeyReleased(ok::KKey key)
{
	return (states[static_cast<int>(key)] & state_released) ? true : false;
}

bool ok::Input::KeyReleased(ok::MKey key)
{
	if (IsPointBlocked(_current_input_layer, MousePX(), MousePY())) return false;

	return (states[last_key_index + static_cast<int>(key)] & state_released) ? true : false;
}

bool ok::Input::KeyReleased(int key)
{
	return (states[key] & state_released) ? true : false;
}

glm::vec2 ok::Input::MouseXY()
{
	return glm::vec2(mouse_x, mouse_y);
}

float ok::Input::MouseX()
{
	return mouse_x;
}

float ok::Input::MouseY()
{
	return mouse_y;
}

glm::vec2 ok::Input::MousePXPY()
{
	return glm::vec2(mouse_px, mouse_py);
}

float ok::Input::MousePX()
{
	return mouse_px;
}

float ok::Input::MousePY()
{
	return mouse_py;
}

void ok::Input::OnKeyDown(int key)
{
	if (last_states_keydown[key] == false)
	{
		PushStateToQueue(key, state_pressed | state_down);
		last_states_keydown[key] = true;

		if (on_char_listeners.size() > 0)
		{
			int translated_key = OnCharTranslate(key);

			for (auto& listener : on_char_listeners)
			{
				(*listener)(translated_key);
			}
		}
	}
}

void ok::Input::OnKeyUp(int key)
{
	PushStateToQueue(key, state_released);
	last_states_keydown[key] = false;
}

void ok::Input::OnMouseDown(int key)
{
	PushStateToQueue(last_key_index + key, state_pressed | state_down);
}

void ok::Input::OnMouseUp(int key)
{
	PushStateToQueue(last_key_index + key, state_released);
}

void ok::Input::OnMouseMove(float x, float y, float screen_width, float screen_height)
{
	mouse_x = glm::clamp(x, 0.0f, 1.0f);
	mouse_y = glm::clamp(y, 0.0f, 1.0f);
	mouse_px = x * screen_width;
	mouse_py = y * screen_height;
}

void ok::Input::Update()
{
	size_t i;
	int keyCode;
	int keyState;

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

int ok::Input::OnCharTranslate(int key)
{
	if (key >= static_cast<int>(ok::KKey::A) && key <= static_cast<int>(ok::KKey::Z))
	{
		//characters
		if (language == 0) //en
		{
			if (KeyDown(ok::KKey::LShift || KeyDown(ok::KKey::RShift)))
			{
				return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[key - static_cast<int>(ok::KKey::A)];
			}
			else
			{
				return "abcdefghijklmnopqrstuvwxyz"[key - static_cast<int>(ok::KKey::A)];
			}
		}
		else if (language == 1) //ru
		{
			if (KeyDown(ok::KKey::LShift || KeyDown(ok::KKey::RShift)))
			{
				return "ÔÈÑÂÓÀÏĞØÎËÄÜÒÙÇÉÊÛÅÃÌÖ×Íß"[key - static_cast<int>(ok::KKey::A)];
			}
			else
			{
				return "ôèñâóàïğøîëäüòùçéêûåãìö÷íÿ"[key - static_cast<int>(ok::KKey::A)];
			}
		}
	}
	else
	{
		if (key >= static_cast<int>(ok::KKey::Num0) && key <= static_cast<int>(ok::KKey::Num9))
		{
			if (KeyDown(ok::KKey::LShift || KeyDown(ok::KKey::RShift)))
			{
				if (language == 0) //en
					return ")!@#$%^&*("[key - static_cast<int>(ok::KKey::Num0)];
				else if (language == 1) //ru
					return ")!\"¹;%:?*("[key - static_cast<int>(ok::KKey::Num0)];
			}
			else
			{
				return "0123456789"[key - static_cast<int>(ok::KKey::Num0)];
			}
		}
		else
		{
			if (key >= static_cast<int>(ok::KKey::LBracket) && key <= static_cast<int>(ok::KKey::Space))
			{
				if (KeyDown(ok::KKey::LShift || KeyDown(ok::KKey::RShift)))
				{
					if (language == 0) //en
						return "{}:<>\"?|~+_ "[key - static_cast<int>(ok::KKey::LBracket)];
					else if (language == 1) //ru
						return "ÕÚÆÁŞİ,|¨+_ "[key - static_cast<int>(ok::KKey::LBracket)];
				}
				else
				{
					if (language == 0) //en
						 return "[];,.'/\\`=- "[key - static_cast<int>(ok::KKey::LBracket)];
					else if (language == 1) //ru
						return "õúæáşı.\\¸=- "[key - static_cast<int>(ok::KKey::LBracket)];
				}
			}
			else
			{
				//do nothing
			}
		}
	}

	return -1;
}

void ok::Input::AddOnCharListener(std::function<void(int)>* listener)
{
	on_char_listeners.push_back(listener);
}

void ok::Input::RemoveOnCharListener(std::function<void(int)>* listener)
{
	on_char_listeners.erase(std::find(on_char_listeners.begin(), on_char_listeners.end(), listener));
}

void ok::Input::EnableLanguageRu()
{
	language = 1;
}

void ok::Input::EnableLanguageEn()
{
	language = 0;
}

void ok::Input::SetCurrentLayer(int layer)
{
	while (static_cast<int>(_layers.size()) - 1 < layer)
	{
		_layers.push_back(ok::Input::input_layer());
	}

	_current_input_layer = layer;
}

void ok::Input::AddBlockedArea(int layer, ok::Rect2Di area)
{
	int prev_layer = _current_input_layer;
	SetCurrentLayer(layer);

	ok::Input::input_layer& _layer = _layers[_current_input_layer];

	if (_layer.free_areas == 0)
	{
		_layer.blocked_areas.push_back(area);
	}
	else
	{
		_layer.free_areas--;
		_layer.blocked_areas[_layer.free_areas] = area;	
	}

	SetCurrentLayer(prev_layer);
}

void ok::Input::RemoveBlockedArea(int layer, ok::Rect2Di area)
{
	int prev_layer = _current_input_layer;
	SetCurrentLayer(layer);

	ok::Input::input_layer& _layer = _layers[_current_input_layer];

	for (size_t i = _layer.free_areas; i < _layer.blocked_areas.size(); i++)
	{
		if (_layer.blocked_areas[i].GetXYWH() == area.GetXYWH())
		{
			std::swap(_layer.blocked_areas[i], _layer.blocked_areas[_layer.free_areas]);
			_layer.free_areas++;
			break;
		}
	}

	SetCurrentLayer(prev_layer);
}

bool ok::Input::IsPointBlocked(int layer, float px, float py)
{
	int prev_layer = _current_input_layer;
	SetCurrentLayer(layer);

	bool result = false;
	ok::Input::input_layer& _layer = _layers[_current_input_layer];

	for (size_t i = _layer.free_areas; i < _layer.blocked_areas.size(); i++)
	{
		if (_layer.blocked_areas[i].Contains(px, py))
		{
			result = true;
			break;
		}
	}

	SetCurrentLayer(prev_layer);
	
	//bool result = false;
	return result;
}

void ok::Input::PushStateToQueue(int key, int state)
{
	int old_state = states_queue[states_queue_position];

	if (old_state != -1)
	{
		//reset previous state before overwrite
		states[((old_state >> 8) & 0x000000ff)] = 0;
	}

	states_queue[states_queue_position] = (((key << 8) & 0x0000ff00) | state);
	states[key] |= state;

	states_queue_position++;
	if (states_queue_position > states_queue.size())
	{
		states_queue_position = 0;
	}
}

void ok::Input::ClearStatesQueue()
{
	std::fill(states_queue.begin(), states_queue.end(), -1);

	states_queue_position = 0;
}
