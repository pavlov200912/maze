#pragma once
#include <stdexcept>
#include <SpriteBatch.h>
#include <vector>
class Wall {
public:
	enum ORIENTATION {
		VERTICAL, 
		HORIZONTAL
	};
	struct BLOCK {
		float x;
		float y;
		ORIENTATION orientation;
	};
	void Draw(DirectX::SpriteBatch* batch, 
		const DirectX::XMFLOAT2& screenPos) const
	{

		RECT sourceRect = { 0, 0, mTextureWidth, mTextureHeight };
		DirectX::XMFLOAT2 temp = { mTextureWidth / 2.f , mTextureHeight / 2.f };
		batch->Draw(mTexture.Get(), screenPos, &sourceRect, DirectX::Colors::White,
			mRotation, temp, mScale, mEffects, mDepth);
	}

	void Draw(DirectX::SpriteBatch* batch) const {
		for (auto wallPos : mWallsCoordinates) {
			Draw(batch, wallPos);
		}
	}

	Wall() :
		mTextureWidth(0),
		mTextureHeight(0),
		mRotation(0.f),
		mScale(0.3f, 0.3f),  // Default scaling, magic constant
		mDepth(0.f),
		mOrigin(0.f, 0.f),
		mEffects(DirectX::SpriteEffects_None)
	{
		mWallsCoordinates = { {150, 150} };
	}

	Wall(
		std::vector<DirectX::XMFLOAT2> &&ref,
		const DirectX::XMFLOAT2& origin = { 0, 0 },
		float rotation = { 0 },
		float scale = { 0.3f },
		float depth = { 0.f }
		) :
		mTextureWidth(0),
		mTextureHeight(0),
		mRotation(rotation),
		mScale(scale, scale),
		mDepth(depth),
		mOrigin(origin),
		mWallsCoordinates(ref) // Is move constructor calling? 
	{
	}

	void addWall(float x, float y, int count = 1, ORIENTATION orientation = VERTICAL) {
		for (int i = 0; i < count; i++) {
			mWallsCoordinates.push_back({ x + i * (orientation == HORIZONTAL) * mTextureWidth,
				y + i * (orientation == VERTICAL) * mTextureHeight }); 
		}
	}

	void Load(ID3D11ShaderResourceView* texture)	{
		mTexture = texture;

		if (texture)
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			texture->GetResource(resource.GetAddressOf());

			D3D11_RESOURCE_DIMENSION dim;
			resource->GetType(&dim);

			if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
				throw std::exception("Wall expects a Texture2D");

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
			resource.As(&tex2D);

			D3D11_TEXTURE2D_DESC desc;
			tex2D->GetDesc(&desc);

			mTextureWidth = int(desc.Width);
			mTextureHeight = int(desc.Height);
		}
	}

private:
	int                                                 mTextureWidth;
	int                                                 mTextureHeight;
	float                                               mDepth;
	float                                               mRotation;
	DirectX::XMFLOAT2                                   mOrigin;
	DirectX::XMFLOAT2                                   mScale;
	DirectX::SpriteEffects								mEffects;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    mTexture;
	std::vector<DirectX::XMFLOAT2>					    mWallsCoordinates;

};