#pragma once

#include "Rect2D.h"

namespace ok
{
	enum class InputState
	{

	};

	typedef sf::Keyboard::Key KKey;
	typedef sf::Mouse::Button MKey;

	class Input
	{
	public:
		static ok::Input& instance();
		static ok::Input& o();

		bool KeyDown(ok::KKey key);
		bool KeyDown(ok::MKey key);
		bool KeyDown(int key);

		bool KeyPressed(ok::KKey key);
		bool KeyPressed(ok::MKey key);
		bool KeyPressed(int key);

		bool KeyReleased(ok::KKey key);
		bool KeyReleased(ok::MKey key);
		bool KeyReleased(int key);

		glm::vec2 MouseXY();
		float MouseX();
		float MouseY();

		glm::vec2 MousePXPY();
		float MousePX();
		float MousePY();

		void OnKeyDown(int key);
		void OnKeyUp(int key);
		void OnMouseDown(int key);
		void OnMouseUp(int key);
		void OnMouseMove(float x, float y, float screen_width, float screen_height);
		void Update();

		int OnCharTranslate(int key);
		void AddOnCharListener(std::function<void(int)>* listener);
		void RemoveOnCharListener(std::function<void(int)>* listener);

		void EnableLanguageRu();
		void EnableLanguageEn();

		void SetCurrentLayer(int layer = 0);
		void AddBlockedArea(int layer, ok::Rect2Di area);
		void RemoveBlockedArea(int layer, ok::Rect2Di area);
		bool IsPointBlocked(int layer, float px, float py);
	protected:
	private:
		struct input_layer
		{
			std::vector<ok::Rect2Di> blocked_areas;
			int free_areas = 0;
		};
		std::vector<input_layer> _layers;
		int _current_input_layer = 0;

		int state_down;
		int state_pressed;
		int state_released;
		int last_key_index;

		void PushStateToQueue(int key, int state);
		void ClearStatesQueue();

		//normalized format 0 .. 1
		float mouse_x;
		float mouse_y;
		//pixel screen width height format
		float mouse_px;
		float mouse_py;

		std::vector<int> states;
		std::vector<bool> last_states_keydown;

		std::vector<int> states_queue;
		size_t states_queue_position;

		Input();
		~Input() {}
		ok::Input(ok::Input const&) {}
		ok::Input& operator= (ok::Input const&) {}

		std::vector<std::function<void(int)>*> on_char_listeners;
		int language = 0;
	};
}