#include "pch.h"
#include "AnimatedTexture.h"

AnimatedTexture::AnimatedTexture() :
	mPaused(false),
	mFrame(0),
	mFrameCount(0),
	mTextureWidth(0),
	mTextureHeight(0),
	mTimePerFrame(0.f),
	mTotalElapsed(0.f),
	mDepth(0.f),
	mRotation(0.f),
	mOrigin(0.f, 0.f),
	mScale(1.f, 1.f),
	mEffects(DirectX::SpriteEffects_None) 
{ 
}

AnimatedTexture::AnimatedTexture(const DirectX::XMFLOAT2& origin,
	float rotation,
	float scale,
	float depth) :
	mPaused(false),
	mFrame(0),
	mFrameCount(0),
	mTextureWidth(0),
	mTextureHeight(0),
	mTimePerFrame(0.f),
	mTotalElapsed(0.f),
	mDepth(depth),
	mRotation(rotation),
	mOrigin(origin),
	mScale(scale, scale) 
{ 
}

void AnimatedTexture::Load(ID3D11ShaderResourceView* texture, int frameCount, int stateCount, int framesPerSecond)
{
	if (frameCount < 0 || framesPerSecond <= 0)
		throw std::invalid_argument("AnimatedTexture");

	mPaused = false;
	mFrame = 0;
	mFrameCount = frameCount;
	mStateCount = stateCount;
	mState = DOWN;
	mTimePerFrame = 1.f / float(framesPerSecond);
	mTotalElapsed = 0.f;
	mTexture = texture;

	if (texture)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		texture->GetResource(resource.GetAddressOf());

		D3D11_RESOURCE_DIMENSION dim;
		resource->GetType(&dim);

		if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
			throw std::exception("AnimatedTexture expects a Texture2D");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
		resource.As(&tex2D);

		D3D11_TEXTURE2D_DESC desc;
		tex2D->GetDesc(&desc);

		mTextureWidth = int(desc.Width);
		mTextureHeight = int(desc.Height);
	}
}

void AnimatedTexture::Update(float elapsed)
{
	if (mPaused)
		return;

	mTotalElapsed += elapsed;

	if (mTotalElapsed > mTimePerFrame)
	{
		++mFrame;
		mFrame = mFrame % mFrameCount;
		mTotalElapsed -= mTimePerFrame;
	}
}

void AnimatedTexture::Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& screenPos) const
{
	Draw(batch, mFrame, mState, screenPos);
}

float AnimatedTexture::getFrameWidth()
{
	return float(mTextureWidth) / mFrameCount;
}

void AnimatedTexture::Draw(DirectX::SpriteBatch* batch, int frame, int state, const DirectX::XMFLOAT2& screenPos) const
{
	int frameWidth = mTextureWidth / mFrameCount;
	int frameHeigth = mTextureHeight / mStateCount;
	RECT sourceRect;
	sourceRect.left = frameWidth * frame;
	sourceRect.top = frameHeigth * state;
	sourceRect.right = sourceRect.left + frameWidth;
	sourceRect.bottom = sourceRect.top + frameHeigth;
	DirectX::XMFLOAT2 temp;
	temp.x = frameWidth / 2;
	temp.y = frameHeigth / 2;
	batch->Draw(mTexture.Get(), screenPos, &sourceRect, DirectX::Colors::White,
		mRotation, temp, mScale, mEffects, mDepth);
}

void AnimatedTexture::setRotation(float x)
{
	mRotation = x;
}

void AnimatedTexture::isForward(bool forward)
{
	if (!forward) mEffects = DirectX::SpriteEffects_FlipVertically;
	else mEffects = DirectX::SpriteEffects_None;
}

void AnimatedTexture::Reset()
{
	mFrame = 0;
	mTotalElapsed = 0.f;
}

void AnimatedTexture::Stop()
{
	mPaused = true;
	mFrame = 0;
	mTotalElapsed = 0.f;
}

void AnimatedTexture::Play() { mPaused = false; }
void AnimatedTexture::Paused() { mPaused = true; }

bool AnimatedTexture::IsPaused() const { return mPaused; }
bool AnimatedTexture::IsAvailable() const { return true; }
void AnimatedTexture::setState(STATE state)
{
	mState = state;
}

bool AnimatedTexture::IsIntersect(RECT objectRect, const DirectX::XMFLOAT2& screenPos) const {
	return true;
}