#pragma once

#include "..\..\Pool.h"
#include "SmoothPath.h"

namespace Starpom
{
	class Action
	{
	public:
		int tick;
		int cost;

		Action();

		float total_progress;
		float tick_progress;

		virtual void OnTick();
		virtual void OnTickProgress();
		virtual void OnBegin();
		virtual void OnEnd();

		virtual void Begin();
		virtual void End();

		virtual void Tick();
		void UpdateProgress(float _tick_progress);
		//Action* action_to_wait;
	};

	//Progress updates only at tick
	class DiscreteAction : public Starpom::Action
	{

	};

	//Progress updates as frequent as possible
	class ContiniousAction : public Starpom::Action
	{

	};

	class ActionOwner
	{
	public:
		std::list<Starpom::DiscreteAction*> _actions_discrete;
		std::list<Starpom::ContiniousAction*> _actions_continious;

		bool Update(float tick_progress);
	};

	class Actions
	{
	public:
		Actions();

		void Update(float dt);

		std::shared_ptr<ok::PoolContainer<Starpom::ActionOwner>> EjectOwner();
		void InjectOwner(std::shared_ptr<ok::PoolContainer<Starpom::ActionOwner>> owner);

		static Starpom::Actions& o();
		static void SetInstance(Starpom::Actions* instance);
	//private:
		ok::Pool<Starpom::ActionOwner>* _owners;

		void Tick();
		bool _pause;
		float _sec;
		static const unsigned int _ticks_per_sec;
		static const float _sec_per_tick;
		static Starpom::Actions* _instance;
	};

	//------------------------------------------------------------------------------------------------------
	/*class Action_FlyInSS : public  Starpom::ContiniousAction
	{
	public:
		Starpom::SmoothPath path;
		glm::vec3 position;
		glm::vec3 direction;

		float 
	};*/
}