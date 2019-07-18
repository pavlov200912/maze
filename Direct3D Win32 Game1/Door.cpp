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
	/*wall.y += m_centerPos.y;
	LONG delta = m_verticalWallTexture.TextureWidth * BASIC_SCALE / 2;
	RECT wallRect = { 0, 0, 0, 0 }; // TODO: LONG TO FLOAT CONVERSION! FIX!

	if (wall.Orientation == VERTICAL)
	{
		wallRect = {
			(LONG)wall.x - delta, (LONG)wall.y - delta,
			(LONG)(wall.x + m_verticalWallTexture.TextureWidth * BASIC_SCALE - delta),
			(LONG)(wall.y + m_verticalWallTexture.TextureHeight * BASIC_SCALE - delta)
		};
	}
	else
	{
		wallRect = {
			(LONG)wall.x - delta, (LONG)wall.y - delta,
			(LONG)(wall.x + m_horizontalWallTexture.TextureWidth * BASIC_SCALE - delta),
			(LONG)(wall.y + m_horizontalWallTexture.TextureHeight * BASIC_SCALE - delta)
		};
	}
	wall.y -= m_centerPos.y;
	if (IsIntersect(wallRect, objectRect))
	{
		// (vanya translate) TODO: With this code, you can't walk close to the wall, 
		// you should somehow fix it. 
		return true; // TODO: Так идти вдоль стены не получается, нужно это как-то исправить
	}*/
}
