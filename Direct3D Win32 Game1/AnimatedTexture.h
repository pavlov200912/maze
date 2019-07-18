//--------------------------------------------------------------------------------------
// File: AnimatedTexture.h
//
// C++ version of the C# example on how to animate a 2D sprite using SpriteBatch
// http://msdn.microsoft.com/en-us/library/bb203866.aspx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#pragma once

#include <stdexcept>
#include <SpriteBatch.h>

class AnimatedTexture
{
public:
	AnimatedTexture(); 
	AnimatedTexture(const DirectX::XMFLOAT2& origin,
	                float rotation,
	                float scale,
	                float depth);
	void Load(ID3D11ShaderResourceView* texture, int frameCount, int stateCount, int framesPerSecond);
	virtual void Update(float elapsed);
	void Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& screenPos) const;
	float getFrameWidth();
	void Draw(DirectX::SpriteBatch* batch, int frame, int state, const DirectX::XMFLOAT2& screenPos) const;
	void setRotation(float x);
	void isForward(bool forward);
	void Reset();
	void Stop();
	void Play();
	void Paused();
	bool IsPaused() const;

	int m_texture_width() const; // TODO: codestyle 
	int m_texture_height() const;

	enum STATE
	{
		DOWN = 0,
		LEFT = 1,
		RIGHT = 2,
		UP = 3
	};

	void setState(STATE state);

protected:
	bool mPaused;
	int mFrame;
	int mFrameCount;
	STATE mState;
	int mStateCount;
	int mTextureWidth;
	int mTextureHeight;
	float mTimePerFrame;
	float mTotalElapsed;
	float mDepth;
	float mRotation;
	DirectX::XMFLOAT2 mOrigin;
	DirectX::XMFLOAT2 mScale;
	DirectX::SpriteEffects mEffects;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
};

inline int AnimatedTexture::m_texture_width() const
{
	return mTextureWidth / mFrameCount;
}

inline int AnimatedTexture::m_texture_height() const
{
	return mTextureHeight / mStateCount;
}
