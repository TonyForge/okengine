#include "Action.h"

const unsigned int Starpom::Actions::_ticks_per_sec = 30;
const float Starpom::Actions::_sec_per_tick = 1.0f / static_cast<float>(_ticks_per_sec);
Starpom::Actions* Starpom::Actions::_instance = nullptr;

Starpom::Actions::Actions()
{
	_sec = 0;
	_pause = true;
	_owners = new ok::Pool<Starpom::ActionOwner>(-1, nullptr, nullptr, nullptr,
	[](Starpom::ActionOwner& owner, float tick_progress)
	{
		return owner.Update(tick_progress);
	});
}

void Starpom::Actions::Update(float dt)
{
	if (_pause)
	{
		_sec = 0;
	}
	else
	{
		_sec += dt;

		float _tick_progress = _sec / _sec_per_tick;

		_owners->Update(glm::clamp(_tick_progress, 0.f, 1.f));

		if (_sec >= _sec_per_tick)
		{
			_sec = 0;
			_owners->Update(1.f);
		}
	}
}

std::shared_ptr<ok::PoolContainer<Starpom::ActionOwner>> Starpom::Actions::EjectOwner()
{
	return _owners->Eject();
}

void Starpom::Actions::InjectOwner(std::shared_ptr<ok::PoolContainer<Starpom::ActionOwner>> owner)
{
	_owners->Inject(owner);
}

Starpom::Actions& Starpom::Actions::o()
{
	return *_instance;
}

void Starpom::Actions::SetInstance(Starpom::Actions * instance)
{
	_instance = instance;
}

void Starpom::Actions::Tick()
{
}

Starpom::Action::Action()
{
	tick = 0;
	cost = 1;
	total_progress = 0;
	tick_progress = 0;
}

void Starpom::Action::OnTick()
{
}

void Starpom::Action::OnTickProgress()
{
}

void Starpom::Action::OnBegin()
{
}

void Starpom::Action::OnEnd()
{
}

void Starpom::Action::Begin()
{
	OnBegin();
}

void Starpom::Action::End()
{
	OnEnd();
}

void Starpom::Action::Tick()
{
	tick++;
	if (tick == cost)
	{
		End();
	}
}

void Starpom::Action::UpdateProgress(float _tick_progress)
{
	tick_progress = _tick_progress;
	total_progress = glm::clamp((static_cast<float>(tick) + _tick_progress) / static_cast<float>(cost), 0.f, 1.f);

	OnTickProgress();

	if (tick_progress == 1.f)
	{
		Tick();
	}
}

bool Starpom::ActionOwner::Update(float tick_progress)
{
	for (auto& _action : _actions_continious)
	{
		_action->UpdateProgress(tick_progress);
	}

	if (tick_progress == 1.f)
	{
		for (auto& _action : _actions_continious)
		{
			_action->UpdateProgress(tick_progress);
		}
	}

	return true; //if false then owner will be deleted
}
