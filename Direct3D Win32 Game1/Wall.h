#pragma once
// TODO: Разнести .h и .cpp
#include <stdexcept>
#include <SpriteBatch.h>
#include <vector>
class WallsHandler {
public:
	enum ORIENTATION {
		VERTICAL, 
		HORIZONTAL
	};
	struct Wall {
		ORIENTATION											Orientation;
		float												x;
		float												y;
		float                                               Depth;
		float                                               Rotation;
		DirectX::XMFLOAT2                                   Origin;
		DirectX::XMFLOAT2                                   Scale;
		DirectX::SpriteEffects								Effects;

		Wall(ORIENTATION orientation, float x, float y, float depth, float rotation, const DirectX::XMFLOAT2& origin, const DirectX::XMFLOAT2& scale,
		     DirectX::SpriteEffects effects);
	};

	void addWall(float x, float y, int count = 1, ORIENTATION orientation = VERTICAL, float depth = 0,
		float rotation = 0, DirectX::XMFLOAT2 origin = { 0, 0 }, DirectX::XMFLOAT2 scale = { 0.3f ,  0.3f }, // TODO: fix magic const
		DirectX::SpriteEffects effects = DirectX::SpriteEffects_None) {
		Wall wall(orientation, x, y, depth, rotation, origin, scale, effects);
		for (int i = 0; i < count; i++) {
			wall.x = (wall.Orientation == HORIZONTAL) ?  (x + BASIC_SCALE * i * mHorizontalWallTexture.TextureWidth) : x;
			wall.y = (wall.Orientation == VERTICAL) ? (y + BASIC_SCALE * i * mVerticalWallTexture.TextureHeight) : y;
			mWalls.push_back(wall);
		}
	}

	void Draw(DirectX::SpriteBatch* batch, Wall wall) const
	{
		float width = (wall.Orientation == VERTICAL) ? (mVerticalWallTexture.TextureWidth) : (mHorizontalWallTexture.TextureWidth);
		float heigth = (wall.Orientation == VERTICAL) ? (mVerticalWallTexture.TextureHeight) : (mHorizontalWallTexture.TextureHeight);
		RECT sourceRect = { 0, 0, width, heigth};
		DirectX::XMFLOAT2 temp;
		if (wall.Orientation == VERTICAL)
		{
			temp = { width / 2, width / 2 };
		} 
		else
		{
			temp = { heigth / 2, heigth / 2 };
		}

		batch->Draw(((wall.Orientation == VERTICAL) ? mVerticalWallTexture : mHorizontalWallTexture).Texture.Get(), // TODO: Nonereadable code
			{ wall.x, wall.y }, &sourceRect, DirectX::Colors::White,
			wall.Rotation, temp, wall.Scale, wall.Effects, wall.Depth);
	}

	void Draw(DirectX::SpriteBatch* batch) const {
		for (auto wall : mWalls) {
			Draw(batch, wall);
		}
	}

	void Load(ID3D11Device* device) {
		// Horizontal
		DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, L"sidewall.png",    // TODO: remove magic filename constants 
			nullptr, mHorizontalWallTexture.Texture.ReleaseAndGetAddressOf()));
		if (mHorizontalWallTexture.Texture) {
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			mHorizontalWallTexture.Texture->GetResource(resource.GetAddressOf());

			D3D11_RESOURCE_DIMENSION dim;
			resource->GetType(&dim);

			if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
				throw std::exception("Wall expects a Texture2D");

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
			resource.As(&tex2D);

			D3D11_TEXTURE2D_DESC desc;
			tex2D->GetDesc(&desc);

			mHorizontalWallTexture.TextureWidth = int(desc.Width);
			mHorizontalWallTexture.TextureHeight = int(desc.Height);
		}
		// Vertical 
		DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, L"upwall.png",  // TODO: remove magic filename constants 
			nullptr, mVerticalWallTexture.Texture.ReleaseAndGetAddressOf()));
		if (mVerticalWallTexture.Texture) {
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			mVerticalWallTexture.Texture->GetResource(resource.GetAddressOf());

			D3D11_RESOURCE_DIMENSION dim;
			resource->GetType(&dim);

			if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
				throw std::exception("Wall expects a Texture2D");

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
			resource.As(&tex2D);

			D3D11_TEXTURE2D_DESC desc;
			tex2D->GetDesc(&desc);

			mVerticalWallTexture.TextureWidth = int(desc.Width);
			mVerticalWallTexture.TextureHeight = int(desc.Height);
		} // TODO: refactor copy-pasted code
	}

	void Reset() {
		mHorizontalWallTexture.Texture.Reset();
		mVerticalWallTexture.Texture.Reset();
	}

	void ClearWalls()
	{
		mWalls.clear();
	}

	// TODO: (intersection) How long ? Oprimization? 
	
	bool IsIntersect(RECT objectRect)
	{
		for (const auto& wall : mWalls)
		{
			LONG delta = mVerticalWallTexture.TextureWidth * BASIC_SCALE / 2;
			RECT wallRect = {0, 0, 0, 0};  // TODO: LONG TO FLOAT CONVERSION! FIX!
 
			if (wall.Orientation == VERTICAL)
			{

				wallRect = { (LONG)wall.x - delta, (LONG)wall.y - delta,
				(LONG)(wall.x + mVerticalWallTexture.TextureWidth * BASIC_SCALE - delta),
				(LONG)(wall.y + mVerticalWallTexture.TextureHeight * BASIC_SCALE - delta)};

			} 
			else
			{
				wallRect = { (LONG)wall.x - delta, (LONG)wall.y - delta,
				(LONG)(wall.x + mHorizontalWallTexture.TextureWidth * BASIC_SCALE - delta),
				(LONG)(wall.y + mHorizontalWallTexture.TextureHeight * BASIC_SCALE - delta)};
			}
			if (IsIntersect(wallRect, objectRect))
			{
				return true; // TODO: Так идти вдоль стены не получается, нужно это как-то исправить
			}

		}
		return false;
	}
	float getVerticalHeight()
	{
		return mVerticalWallTexture.TextureHeight * BASIC_SCALE;
	}
	float getVerticalWidth()
	{
		return mVerticalWallTexture.TextureWidth * BASIC_SCALE;
	}

	void setScale(float floorWidth)
	{
		BASIC_SCALE = floorWidth / (9.f * mHorizontalWallTexture.TextureWidth);
	}

private:
	static bool IsIntersect(const RECT &a , const RECT &b)
	{
		return a.left < b.right && a.right > b.left && a.top < b.bottom && a.bottom > b.top; // Y orientation changed
	}
	struct WallTexture {
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture;
		float											 TextureWidth;
		float											 TextureHeight;
	};
	float												BASIC_SCALE = 0.3f;  // TODO: make this static 
	WallTexture												mHorizontalWallTexture;   
	WallTexture												mVerticalWallTexture;
	std::vector<Wall>										mWalls;
	void my_log(std::string message)
	{
#ifndef NDEBUG
		// print message
		util::ServiceLocator::getFileLogger()->print<util::SeverityType::info>(message);
#endif
	}
};
	
inline WallsHandler::Wall::Wall(ORIENTATION orientation, float x, float y, float depth, float rotation,
	const DirectX::XMFLOAT2& origin, const DirectX::XMFLOAT2& scale, DirectX::SpriteEffects effects)
{
	this->x = x;
	this->y = y;
	this->Orientation = orientation;
	this->Rotation = rotation;
	this->Depth = depth;
	this->Effects = effects;
	this->Scale = scale;
	this->Origin = origin;
}
