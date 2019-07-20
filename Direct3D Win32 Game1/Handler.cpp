#include "pch.h"
#include "Handler.h"

Handler::Handler()
{
	mOffset = { 0, 0 };
	mCenter = { 0, 0 };
}

void Handler::Update(float elapsed) {
	for (int i = 0; i < mTextures.size(); ++i)
		mTextures[i]->Update(elapsed);
}

void Handler::UpdateOffset(float delta_y) {
	if (inBounds(mOffset.y - delta_y)) {
		mOffset.y -= delta_y;
	}
}

bool Handler::inBounds(float y) const {
	return y > 0 && y < mLevelHeight - mScreenHeight;
}

void Handler::Play(RECT objectRect) {
	for (int i = 0; i < mTextures.size(); i++)
		if (mTextures[i]->IsIntersect(objectRect, { mCenter.x + mOffset.x - mPositions[i].x, mCenter.y + mOffset.y - mPositions[i].y }))
			mTextures[i]->Play();
}

bool Handler::IsIntersect(RECT objectRect) {
	for (int i = 0; i < mTextures.size(); ++i) {
		if (!mTextures[i]->IsAvailable() && mTextures[i]->IsIntersect(objectRect, { mCenter.x + mOffset.x - mPositions[i].x, mCenter.y + mOffset.y - mPositions[i].y }))
			return true;
	}
	return false;
}

void Handler::Draw(DirectX::SpriteBatch* batch) {
	for (int i = 0; i < mTextures.size(); i++)
		mTextures[i]->Draw(batch, { mCenter.x + mOffset.x - mPositions[i].x, mCenter.y + mOffset.y - mPositions[i].y });
}

void Handler::setWindowHeight(float windowHeight) {
	mScreenHeight = windowHeight;
}

void Handler::setLevelHeight(float levelHeight) {
	mLevelHeight = levelHeight;
}