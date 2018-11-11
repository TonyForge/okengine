#pragma once

#include "GameObject.h"
#include "UI.h"
#include "Application.h"
#include "Input.h"
#include "TextBatch.h"
#include "Font.h"

namespace ok
{
	class TileMap2D
	{
	public:
	protected:
	private:
	};

	class TileMap2DEditor : public ok::GameObject
	{
	public:
		TileMap2DEditor();
		
		void Open();
		void Close();
		void Update(float dt);
		void OnChar(int key);
	protected:
	private:
		bool _open = false;
		std::function<void(int)> OnCharCallback = nullptr;

		ok::graphics::TextCache text_cache;
		ok::graphics::Font font_default_normal;

		ok::String _command;
	};
}