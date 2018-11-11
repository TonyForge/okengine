#include "TileMap2D.h"

ok::TileMap2DEditor::TileMap2DEditor()
{
	ok::TileMap2DEditor* this_ptr = this;
	OnCharCallback = [this_ptr](int key) { this_ptr->OnChar(key); };

	ok::Input::o().AddOnCharListener(&OnCharCallback);
}

void ok::TileMap2DEditor::Open()
{
	_open = true;
}

void ok::TileMap2DEditor::Close()
{
	_open = false;
}

void ok::TileMap2DEditor::Update(float dt)
{
	if (ok::Input::o().KeyPressed(ok::KKey::Return))
	{
		if (_command == "tilemap2deditor")
		{
			if (true == _open)
				Close();
			else
				Open();
		}

		_command.clear();
	}

	if (true == _open)
	{
		ok::ui::BeginUI(ok::Application::Properties.screen_width, ok::Application::Properties.screen_height);

		ok::ui::EndUI();
	}
}

void ok::TileMap2DEditor::OnChar(int key)
{
	if (key != -1)
	{
		_command += static_cast<char>(key);
	}
}
