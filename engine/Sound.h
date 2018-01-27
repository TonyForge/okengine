#pragma once

#include "AssetsBasic.h"
#include "Pool.h"
#include "Transform.h"

namespace ok
{
	namespace sound
	{
		class Sound;
		class SoundInstance;
		class SoundRoom;

		class SampleWrapper
		{
		public:
			SampleWrapper(ok::sound::SoundInstance* _instance, ok::sound::Sample* _sample);
			SampleWrapper();
			
			ok::sound::SampleWrapper& operator =(const ok::sound::SampleWrapper& right);
			ok::sound::Sample* sample;
			ok::sound::SoundInstance* instance;
		private:
			friend class ok::Pool<ok::sound::SampleWrapper>;
			~SampleWrapper();
		};

		class StreamWrapper
		{
		public:
			StreamWrapper(ok::sound::SoundInstance* _instance, ok::sound::Stream* _stream);
			StreamWrapper();
			ok::sound::StreamWrapper& operator =(const ok::sound::StreamWrapper& right);
			ok::sound::Stream* stream;
			ok::sound::SoundInstance* instance;
		private:
			friend class ok::Pool<ok::sound::StreamWrapper>;
			~StreamWrapper();
		};

		class SoundInstance
		{
		public:
			SoundInstance();

			bool IsStopped();
			bool IsLooped();

			void _Inject();
		protected:
		private:
			friend class ok::sound::Sound;
			friend class ok::sound::SoundRoom;

			std::shared_ptr<ok::PoolContainer<ok::sound::SampleWrapper>> _sample_slot;
			std::shared_ptr<ok::PoolContainer<ok::sound::StreamWrapper>> _stream_slot;

			ok::Pool<ok::sound::SoundInstance>* _parent_pool;
			std::shared_ptr<ok::PoolContainer<ok::sound::SoundInstance>> _parent_pool_container;
			static ok::sound::SoundInstance* _injection_safety_lock;

			bool _mute_enabled;
			float _volume;

			bool _spatial_enabled;
			ok::Transform* _spatial_controller_transform;
			glm::vec3* _spatial_controller_position;
			float* _spatial_controller_radius;
		};

		class SoundRoom
		{
		public:
			static ok::sound::SoundRoom& instance();
			static ok::sound::SoundRoom& o();

			void LinkListener(ok::Transform* listener_transform, float* listener_radius);
			void Listen(ok::sound::SoundInstance& sound_instance);
		protected:
		private:
			SoundRoom()
			{
				sf::Listener::setPosition(0.f, 0.f, 0.f);
				sf::Listener::setUpVector(0.f, 1.f, 0.f);
				sf::Listener::setDirection(0.f, 0.f, 1.f);
				sf::Listener::setGlobalVolume(100.f);

				_listener_transform = nullptr;
			}
			~SoundRoom(){}
			ok::sound::SoundRoom(ok::sound::SoundRoom const&) {}
			ok::sound::SoundRoom& operator= (ok::sound::SoundRoom const&) {}

			ok::Transform* _listener_transform;
			float* _listener_radius;
		};

		class Sound
		{
		public:
			Sound(ok::sound::SoundAsset* asset, int instances_limit = -1);

			void SetupBegin(ok::sound::SoundInstance* instance);
			void SetupEnd();

			ok::sound::SoundInstance* Play();

			void Pause();
			void Resume();
			void Stop();
			void EnableLoop();
			void DisableLoop();
			void EnableMute();
			void DisableMute();
			void SetVolume(float volume);
			void LinkToSpatialController(ok::Transform* transform, float* radius);
			void LinkToSpatialController(glm::vec3* position, float* radius);
			void UnlinkFromSpatialController();
			void EnableSpatial();
			void DisableSpatial();

			void Update(float dt);
		protected:
		private:
			bool _setup_paused;
			bool _setup_stopped;
			bool _setup_looped;
			bool _setup_muted;
			float _setup_volume;

			ok::sound::SoundInstance* _setup_instance;

			ok::sound::SoundAsset* _asset;
			ok::Pool<ok::sound::SoundInstance> _instances;

			static ok::Pool<ok::sound::SampleWrapper> _sample_slots;
			static ok::Pool<ok::sound::StreamWrapper> _stream_slots;
		};
	}
}