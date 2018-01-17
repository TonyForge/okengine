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
	mouse_x = glm::clamp(x,0.0f,1.0f);
	mouse_y = glm::clamp(y, 0.0f, 1.0f);
	mouse_px = x * screen_width;
	mouse_py = y * screen_width;
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
