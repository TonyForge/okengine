#pragma once

#include "IDOEGame.h"
#include "DOEFloor.h"
#include "Scriptables.h"

namespace DOE
{
	class DOEGame : public DOE::IDOEGame
	{
	public:
		DOEGame();
		void Init();
		void Update(float dt);

		//void Set
		void ConvertTMXToChunk(ok::String& in, ok::String& out);
		void LoadGameFromSlot(int slot_id);

		void PlaceCamera(float x, float y);
		void SetCameraSpeed(float speed);
		void SetCameraTarget(float x, float y);
		ok::Rect2Df GetCameraViewport();
		ok::graphics::TextBatch2D* GetTextBatch();
	protected:
	private:
		std::vector<DOE::DOEFloor> floors;
		ok::GameObject* scene_root;
		ok::graphics::SpriteBatch* def_batch;
		ok::graphics::TextBatch2D* def_text_batch;
		//DOE::DOEFloor floor1;
		float _camera_target_x = 0.f;
		float _camera_target_y = 0.f;
		float _camera_speed = 1.f;
	};
}