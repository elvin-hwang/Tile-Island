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
	return Utils::getDist({ motion1.position.x, motion1.position.y }, { motion2.position.x, motion2.position.y });
}

bool Utils::circleIntersectsLine(vec2 center, float radius, vec2 lineStart, vec2 lineEnd)
{
	float a = lineStart.y - lineEnd.y;
	float b = lineEnd.x - lineStart.x;
	float c = lineStart.x * lineEnd.y - lineEnd.x * lineStart.y;

	float dist = (abs(a * center.x + b * center.y + c)) / sqrt(a * a + b * b);
	vec2 perpendicular = getPerpendicularPoint(center, lineStart, lineEnd);
	bool onLineSegment = false;
	if (lineStart.x == lineEnd.x) {
		onLineSegment = perpendicular.y < lineEnd.y && perpendicular.y > lineStart.y;
	}
	else {
		onLineSegment = perpendicular.x < lineEnd.x && perpendicular.x > lineStart.x;
	}
	return radius > dist && onLineSegment;
}

float Utils::getVelocityMagnitude(Motion motion)
{
	return sqrt((float)pow(motion.velocity.x, 2) + (float)pow(motion.velocity.y, 2));
}

float Utils::getDist(vec2 lineStart, vec2 lineEnd)
{
	float x = lineStart.x - lineEnd.x;
	float y = lineStart.y - lineEnd.y;

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
}

vec2 Utils::getPerpendicularPoint(vec2 center, vec2 lineStart, vec2 lineEnd) {
	float k = ((lineEnd.y - lineStart.y) * (center.x - lineStart.x) - (lineEnd.x - lineStart.x) * (center.y - lineStart.y)) / ((lineEnd.y - lineStart.y) * (lineEnd.y - lineStart.y) + (lineEnd.x - lineStart.x) * (lineEnd.x - lineStart.x));
	return vec2{ center.x - k * (lineEnd.y - lineStart.y), center.y + k * (lineEnd.x - lineStart.x) };
}