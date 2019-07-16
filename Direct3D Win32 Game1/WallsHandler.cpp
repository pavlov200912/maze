#include "pch.h"
#include "WallsHandler.h"

void WallsHandler::addWall(float x, float y, int count, ORIENTATION orientation, float depth, float rotation,
	DirectX::XMFLOAT2 origin, DirectX::XMFLOAT2 scale, DirectX::SpriteEffects effects)
{
	Wall wall(orientation, x, y, depth, rotation, origin, scale, effects);
	for (int i = 0; i < count; i++)
	{
		wall.x = (wall.Orientation == HORIZONTAL) ? (x + BASIC_SCALE * i * m_horizontalWallTexture.TextureWidth) : x;
		wall.y = (wall.Orientation == VERTICAL) ? (y + BASIC_SCALE * i * m_verticalWallTexture.TextureHeight) : y;
		m_walls.push_back(wall);
	}
}

void WallsHandler::Draw(DirectX::SpriteBatch* batch, Wall wall) const
{
	float width = (wall.Orientation == VERTICAL)
		? (m_verticalWallTexture.TextureWidth)
		: (m_horizontalWallTexture.TextureWidth);
	float heigth = (wall.Orientation == VERTICAL)
		? (m_verticalWallTexture.TextureHeight)
		: (m_horizontalWallTexture.TextureHeight);
	RECT sourceRect = { 0, 0, width, heigth };
	DirectX::XMFLOAT2 temp;
	if (wall.Orientation == VERTICAL)
	{
		temp = { width / 2, width / 2 };
	}
	else
	{
		temp = { heigth / 2, heigth / 2 };
	}
	batch->Draw(((wall.Orientation == VERTICAL) ? m_verticalWallTexture : m_horizontalWallTexture).Texture.Get(),
		// TODO: unreadable code
		{ wall.x + m_centerPos.x, wall.y + m_centerPos.y}, &sourceRect, DirectX::Colors::White,
		wall.Rotation, temp, wall.Scale, wall.Effects, wall.Depth);
}

void WallsHandler::Draw(DirectX::SpriteBatch* batch) const
{
	for (auto wall : m_walls)
	{
		Draw(batch, wall);
	}
}


void WallsHandler::Update(float delta_y)
{
	m_centerPos.y -= delta_y;
}

void WallsHandler::Load(ID3D11Device* device)
{
	// Horizontal
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, L"sidewall.png",
	                                                    // TODO: remove magic filename constants 
	                                                    nullptr,
	                                                    m_horizontalWallTexture.Texture.ReleaseAndGetAddressOf()));
	if (m_horizontalWallTexture.Texture)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_horizontalWallTexture.Texture->GetResource(resource.GetAddressOf());

		D3D11_RESOURCE_DIMENSION dim;
		resource->GetType(&dim);

		if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
			throw std::exception("Wall expects a Texture2D");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
		resource.As(&tex2D);

		D3D11_TEXTURE2D_DESC desc;
		tex2D->GetDesc(&desc);

		m_horizontalWallTexture.TextureWidth = int(desc.Width);
		m_horizontalWallTexture.TextureHeight = int(desc.Height);
	}
	// Vertical 
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(device, L"upwall.png", // TODO: remove magic filename constants 
	                                                    nullptr,
	                                                    m_verticalWallTexture.Texture.ReleaseAndGetAddressOf()));
	if (m_verticalWallTexture.Texture)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		m_verticalWallTexture.Texture->GetResource(resource.GetAddressOf());

		D3D11_RESOURCE_DIMENSION dim;
		resource->GetType(&dim);

		if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
			throw std::exception("Wall expects a Texture2D");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
		resource.As(&tex2D);

		D3D11_TEXTURE2D_DESC desc;
		tex2D->GetDesc(&desc);

		m_verticalWallTexture.TextureWidth = int(desc.Width);
		m_verticalWallTexture.TextureHeight = int(desc.Height);
	} // TODO: refactor copy-pasted code
}

void WallsHandler::Reset()
{
	m_horizontalWallTexture.Texture.Reset();
	m_verticalWallTexture.Texture.Reset();
}

void WallsHandler::ClearWalls()
{
	m_walls.clear();
}

bool WallsHandler::IsIntersect(RECT objectRect)
{
	for (auto& wall : m_walls)
	{
		// TODO: make this in diff. function
		wall.y += m_centerPos.y;
		LONG delta = m_verticalWallTexture.TextureWidth * BASIC_SCALE / 2;
		RECT wallRect = {0, 0, 0, 0}; // TODO: LONG TO FLOAT CONVERSION! FIX!

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
		}
		
	}
	return false;
}

bool WallsHandler::IsIntersect(const RECT& a, const RECT& b)
{
	return a.left < b.right && a.right > b.left && a.top < b.bottom && a.bottom > b.top; // Y orientation changed
}

void WallsHandler::my_log(std::string message)
{
#ifndef NDEBUG
	// print message
	util::ServiceLocator::getFileLogger()->print<util::SeverityType::info>(message);
#endif
}


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

