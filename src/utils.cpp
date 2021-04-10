// Header


#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "utils.hpp"
#include "tile.hpp"
#include "egg.hpp"
#include <render_components.hpp>

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
void Utils::moveCamera(float xOffset, float yOffset) {
	// Move all Blobules
	for (auto& blob : ECS::registry<Blobule>.entities)
	{
		ECS::registry<Motion>.get(blob).position += vec2({ xOffset, yOffset });
		ECS::registry<Blobule>.get(blob).origin += vec2({ xOffset, yOffset });
	}
	// Move all tiles
	for (auto& tile : ECS::registry<Tile>.entities)
	{
		auto& tileComponent = ECS::registry<Tile>.get(tile);
		ECS::registry<Motion>.get(tile).position += vec2({ xOffset, yOffset });
		ECS::registry<Motion>.get(tileComponent.splatEntity).position += vec2({ xOffset, yOffset });
	}
	// Move all eggs
	for (auto& egg : ECS::registry<Egg>.entities)
	{
		ECS::registry<Motion>.get(egg).position += vec2({ xOffset, yOffset });
	}

	for (auto& debuglines : ECS::registry<DebugComponent>.entities)
	{
		ECS::registry<Motion>.get(debuglines).position += vec2({ xOffset, yOffset });
	}
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

bool Utils::circleTouchesCorner(vec2 center, float radius, vec2 boxCenter, float halfWidth)
{
	halfWidth -= 10;
	float bottomLeft = getDist(center, {boxCenter.x - halfWidth, boxCenter.y - halfWidth});
	float topLeft = getDist(center, { boxCenter.x - halfWidth, boxCenter.y + halfWidth });
	float topRight = getDist(center, { boxCenter.x + halfWidth, boxCenter.y + halfWidth });
	float bottomRight = getDist(center, { boxCenter.x + halfWidth, boxCenter.y - halfWidth });

	return bottomLeft < radius || topLeft < radius || topRight < radius || bottomRight < radius;
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