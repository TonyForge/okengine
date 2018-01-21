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
			static glm::vec3 GetRoomCoordinates(ok::Transform& listener, ok::Transform& sound_source, float listener_radius);
			void Listen(sf::Sound& sound, glm::vec3 room_coordinates);
			void Listen(sf::Music& sound, glm::vec3 room_coordinates);
		};

		class Man
		{
		public:
			static ok::sound::Man& instance();
			static ok::sound::Man& o();

			//loaders
			ok::sound::SoundID Load(const ok::String& asset, ok::sound::SoundType type);
			void Free(ok::sound::SoundID resource);

			void Play();
			void Stop();
			void Pause();
			void SetVolume();
			float GetVolume();
		protected:
		private:
			Man(){
				sf::Listener::setPosition(0.f, 0.f, 0.f);
				sf::Listener::setUpVector(0.f, 1.f, 0.f);
				sf::Listener::setDirection(0.f, 0.f, 1.f);
				sf::Listener::setGlobalVolume(100.f);
			}
			~Man() {}
			ok::sound::Man(ok::sound::Man const&) {}
			ok::sound::Man& operator= (ok::sound::Man const&) {}

			ok::sound::SoundRoom _room;
		};
	}
}