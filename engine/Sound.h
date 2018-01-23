#pragma once

#include "AssetsBasic.h"
#include "Pool.h"
#include "Transform.h"

namespace ok
{
	namespace sound
	{
		class Man;

		enum class SoundType
		{
			Sound,
			Music
		};

		class SoundID
		{
		public:
			ok::sound::SoundID& operator =(const ok::sound::SoundID& right);
			bool operator==(const ok::sound::SoundID& right);
		private:
			friend class ok::sound::Man;
			int id;
			ok::sound::SoundType type;
		};

		class SoundRoom
		{
		public:
			//sound room is 2x2x2 cube with listener in it's center 0,0,0 looking at 0,0,1 direction with up vector 0,1,0
			static glm::vec3 GetRoomCoordinates(ok::Transform& listener, glm::vec3 sound_source_world_position, float listener_radius);
			static glm::vec3 GetRoomCoordinates(ok::Transform& listener, ok::Transform& sound_source, float listener_radius);
			static float GetSoundSourceRoomRadius(float listener_radius, float sound_source_radius);

			//spatial
			void Listen(sf::Sound& sound, glm::vec3 room_coordinates, float sound_source_room_radius);
			void Listen(sf::Music& music, glm::vec3 room_coordinates, float sound_source_room_radius);
			//global
			void Listen(sf::Sound& sound);
			void Listen(sf::Music& music);
		};

		class SoundChannel;

		class SFSoundWrapper : public sf::Sound
		{
		public:
			SFSoundWrapper();
			void LinktTo(ok::Transform* transform);
			void LinktTo(glm::vec3* position);
			void Unlink();
		private:
			friend class ok::sound::Man;
			friend class ok::sound::SoundChannel;

			ok::Transform* _transform;
			glm::vec3* _position;
			void* _owner;
		};

		class SFMusicWrapper : public sf::Music
		{
		public:
			SFMusicWrapper();
			void LinktTo(ok::Transform* transform);
			void LinktTo(glm::vec3* position);
			void Unlink();
		private:
			friend class ok::sound::Man;
			friend class ok::sound::SoundChannel;

			ok::Transform* _transform;
			glm::vec3* _position;
			void* _owner;
		};

		class SoundChannel
		{
		public:
			SoundChannel();
			bool IsValid();

			ok::sound::SoundChannel& operator =(const ok::sound::SoundChannel& right) = delete;
		private:
			friend class ok::sound::Man;

			void _Play();
			void _Pause();
			void _Stop();

			ok::sound::SFSoundWrapper* _sound_ptr;
			ok::sound::SFMusicWrapper* _music_ptr;
			bool _auto_release;
		};

		class Man
		{
		public:
			static ok::sound::Man& instance();
			static ok::sound::Man& o();

			//loaders
			ok::sound::SoundID Load(const ok::String& asset, ok::sound::SoundType type);
			void Free(ok::sound::SoundID resource);

			ok::sound::SoundChannel* Play(ok::sound::SoundID resource_id, bool auto_release = true);
			void Release(ok::sound::SoundChannel* channel);

			void Play(ok::sound::SoundChannel* channel);
			void Pause(ok::sound::SoundChannel* channel);
			void Stop(ok::sound::SoundChannel* channel);

			void Play();
			void Stop();
			void Pause();
			void SetVolume();
			float GetVolume();

			void Update(float dt);

		protected:
		private:
			Man() :
				_sounds(
					256-16,
					nullptr,//allocator
					[](ok::sound::SFSoundWrapper& _sound)//injector
					{
						_sound.stop();
						_sound.resetBuffer();
						_sound._owner = nullptr;
					},
					nullptr,//ejector
					[](ok::sound::SFSoundWrapper& _sound, float dt)//updater
					{
						if (_sound.getStatus() == sf::Sound::Status::Stopped)
						{
							return false;
						}

						return true;
					}
				),
				_musics(
					16,
					nullptr,//allocator
					[](ok::sound::SFMusicWrapper& _music)//injector
					{
						_music.stop();
						_music._owner = nullptr;
					},
					nullptr,//ejector
					[](ok::sound::SFMusicWrapper& _music, float dt)//updater
					{
						if (_music.getStatus() == sf::Sound::Status::Stopped)
						{
							return false;
						}
						return true;
					}
				),
				_channels(
					-1,
					nullptr,//allocator
					[](ok::sound::SoundChannel& _channel)//injector
					{
						_channel._Stop();
					},
					nullptr,//ejector
					[](ok::sound::SoundChannel& _channel, float dt)//updater
					{
						if (_channel._auto_release && (_channel.IsValid() == false))
						{
							return false;
						}

						return true;
					}
				)
			{
				sf::Listener::setPosition(0.f, 0.f, 0.f);
				sf::Listener::setUpVector(0.f, 1.f, 0.f);
				sf::Listener::setDirection(0.f, 0.f, 1.f);
				sf::Listener::setGlobalVolume(100.f);
			}
			~Man() {}
			ok::sound::Man(ok::sound::Man const&) {}
			ok::sound::Man& operator= (ok::sound::Man const&) {}

			ok::sound::SoundRoom _room;

			ok::Pool<ok::sound::SoundChannel> _channels;
			ok::Pool<ok::sound::SFSoundWrapper> _sounds;
			ok::Pool<ok::sound::SFMusicWrapper> _musics;
		};
	}
}