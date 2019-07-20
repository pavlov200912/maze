#pragma once
#include "AnimatedTexture.h"

class Handler
{
public:

	Handler();
	void Update(float elapsed);
	void UpdateOffset(float delta_y);
	void Play(RECT objectRect);
	void Draw(DirectX::SpriteBatch* batch);
	bool IsIntersect(RECT objectRect);
	std::vector<DirectX::SimpleMath::Vector2> mPositions;
	std::vector<std::unique_ptr<AnimatedTexture>> mTextures;
	DirectX::SimpleMath::Vector2 mOffset;
	DirectX::SimpleMath::Vector2 mCenter;
	void setWindowHeight(float screenHeight);
	void setLevelHeight(float levelHeight);

private:
	bool inBounds(float y) const;
	float mScreenHeight;
	float mLevelHeight;
};

