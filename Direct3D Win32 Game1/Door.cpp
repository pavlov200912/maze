#include "pch.h"
#include "Door.h"

void Door::Update(float elapsed)
{
	if (mPaused || mFrame == mFrameCount - 1)
		return;

	mTotalElapsed += elapsed;

	if (mTotalElapsed > mTimePerFrame)
	{
		++mFrame;
		mFrame = mFrame % mFrameCount;
		mTotalElapsed -= mTimePerFrame;
	}
}

bool Door::IsIntersect(RECT objectRect, const DirectX::XMFLOAT2& screenPos) const {
	RECT doorRect = { 0, 0, 0, 0 };
	doorRect.left = screenPos.x - m_texture_width() / 2;
	doorRect.right = screenPos.x + m_texture_width() / 2;
	doorRect.top = screenPos.y - m_texture_height() / 2;
	doorRect.bottom = screenPos.y + m_texture_height() / 2;
	return (objectRect.left < doorRect.right && objectRect.right > doorRect.left &&
		objectRect.top < doorRect.bottom && objectRect.bottom> doorRect.top);
}
