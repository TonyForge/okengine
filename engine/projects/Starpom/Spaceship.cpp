#include "Spaceship.h"

Starpom::Spaceship::Spaceship()
{
	ship_id_type = Starpom::Ship_ID_Type::spaceship;
}

void Starpom::Spaceship::Update(float dt)
{
	Starpom::Ship::Update(dt);


}
