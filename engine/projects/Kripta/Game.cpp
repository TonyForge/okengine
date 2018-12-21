#include "Game.h"

Kripta::Game::Game()
{
	_settings_file_name = "kripta.settings.xml";
}

void Kripta::Game::Init()
{
	DisableFeature(ok::ApplicationFeature::AutoClearStencil);

	camera = new ok::graphics::Camera(ok::graphics::CameraCoordinateSystem::Screen);
	camera->SetProjectionOrtho(1024, 768, 100.f, 1.f);

	camera->BeginTransform();
	camera->SetPosition(glm::vec3(0.f, 0.f, 50.0f));
	camera->EndTransform(true);

	sprite_batch = new ok::graphics::SpriteBatch();
	line_batch = new ok::graphics::LineBatch();

}

void Kripta::Game::Update(float dt)
{
	ok::graphics::Camera::PushCamera(camera);

	ok::graphics::Texture* tex = ok::Assets::instance().GetTexture("tiles.png");
	ok::graphics::SpriteInfo info;
	//info.

	sprite_batch->BatchBegin(10.f);
	//sprite_batch->Blit()
	sprite_batch->Draw(tex, glm::vec2(0.f, 0.f), glm::vec2(512.f,512.f),false,glm::vec2(0.f,0.f));
	sprite_batch->BatchEnd();


	line_batch->BatchBegin();
	line_batch->LineAB(glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.f, 100.f, 0.f));
	line_batch->BatchEnd();

	ok::graphics::Camera::PopCamera();
}

ok::graphics::RenderTarget * Kripta::Game::GetScreenBuffer()
{
	return fixed_resolution_framebuffer;
}
