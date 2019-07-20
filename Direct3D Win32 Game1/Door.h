#pragma once
#include "AnimatedTexture.h"
 
class Door : public AnimatedTexture {
public:
	void Update(float elapsed) override;
	bool IsIntersect(RECT objectRect, const DirectX::XMFLOAT2& screenPos) const override;
	bool IsAvailable() const override;
};