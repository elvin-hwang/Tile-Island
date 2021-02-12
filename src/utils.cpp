// Header


#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "utils.hpp"

ECS::Entity& Utils::getActivePlayerBlobule()
{
	for (ECS::Entity& blobule : ECS::registry<Blobule>.entities)
	{
		if (ECS::registry<Blobule>.get(blobule).active_player)
		{
			return blobule;
		}
	}
	throw "no active player set";
}

float Utils::euclideanDist(Motion motion1, Motion motion2)
{
	float x = motion1.position.x - motion2.position.x;
	float y = motion1.position.y - motion2.position.y;

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
}
