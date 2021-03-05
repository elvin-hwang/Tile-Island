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
	return Utils::getDist(motion1.position.x, motion1.position.y, motion2.position.x, motion2.position.y);
}

bool Utils::circleIntersectsLine(vec2 center, float radius, float x1, float y1, float x2, float y2)
{
	float a = y1 - y2;
	float b = x2 - x1;
	float c = x1 * y2 - x2 * y1;

	float dist = (abs(a * center.x + b * center.y + c)) / sqrt(a * a + b * b);
	float distFromEdgeOne = Utils::getDist(center.x, center.y, x1, y1);
	float distFromEdgeTwo = Utils::getDist(center.x, center.y, x2, y2);

	return radius > dist && (distFromEdgeOne < radius || distFromEdgeTwo < radius);
}

float Utils::getVelocityMagnitude(Motion motion)
{
	return sqrt((float)pow(motion.velocity.x, 2) + (float)pow(motion.velocity.y, 2));
}

float Utils::getDist(float x1, float y1, float x2, float y2)
{
	float x = x1 - x2;
	float y = y1 - y2;

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
}