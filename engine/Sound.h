#pragma once

#include "AssetsBasic.h"
#include "Pool.h"
#include "Transform.h"

namespace ok
{
	namespace sound
	{
		class Sound;

		class SoundInstance
		{
		public:
		protected:
		private:
			friend class ok::sound::Sound;

			ok::PoolContainer<std::pair<ok::sound::SoundInstance*, ok::sound::Sample>>* _sample_slot;
			ok::PoolContainer<std::pair<ok::sound::SoundInstance*, ok::sound::Stream>>* _stream_slot;

			ok::Pool<ok::sound::SoundInstance>* _parent_pool;
			ok::PoolContainer<ok::sound::SoundInstance>* _parent_pool_container;
		};

		class Sound
		{
		public:
			Sound(ok::sound::SoundAsset* asset, int instances_limit = -1);

			//void SetLoop
			//void SetVolume
			//void SetSpatial
			ok::sound::SoundInstance* Play();

			void Update(float dt);
		protected:
		private:
			friend class ok::Pool<std::pair<ok::sound::SoundInstance*, ok::sound::Sample>>;

			ok::sound::SoundAsset* _asset;
			ok::Pool<ok::sound::SoundInstance> _instances;

			static ok::Pool<std::pair<ok::sound::SoundInstance*, ok::sound::Sample>> _sample_slots;
			static ok::Pool<std::pair<ok::sound::SoundInstance*, ok::sound::Stream>> _stream_slots;
			static ok::sound::SoundInstance* _injection_safety_lock;
		};
	}
}