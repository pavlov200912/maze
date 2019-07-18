#pragma once
#include <stdexcept>
#include <SpriteBatch.h>
#include <vector>

class WallsHandler
{
public:
	enum ORIENTATION
	{
		VERTICAL,
		HORIZONTAL
	};

	struct Wall
	{
		ORIENTATION Orientation;
		float x;
		float y;
		float Depth;
		float Rotation;
		DirectX::XMFLOAT2 Origin;
		DirectX::XMFLOAT2 Scale;
		DirectX::SpriteEffects Effects;

		Wall(ORIENTATION orientation, float x, float y, float depth, float rotation, const DirectX::XMFLOAT2& origin,
		     const DirectX::XMFLOAT2& scale,
		     DirectX::SpriteEffects effects);
	};

	void addWall(float x, float y, int count = 1, ORIENTATION orientation = VERTICAL, float depth = 0,
	             float rotation = 0, DirectX::XMFLOAT2 origin = {0, 0}, DirectX::XMFLOAT2 scale = {0.3f, 0.3f},
	             // TODO: fix magic const
	             DirectX::SpriteEffects effects = DirectX::SpriteEffects_None);

	void Draw(DirectX::SpriteBatch* batch, Wall wall) const;

	void Draw(DirectX::SpriteBatch* batch) const;

	void Load(ID3D11Device* device);

	void Reset();

	void ClearWalls();

	bool IsIntersect(RECT objectRect);

	float getVerticalHeight() const
	{
		return m_verticalWallTexture.TextureHeight * BASIC_SCALE;
	}

	float getVerticalWidth() const
	{
		return m_verticalWallTexture.TextureWidth * BASIC_SCALE;
	}

	void setScale(float floorWidth)
	{
		BASIC_SCALE = floorWidth / (9.f * m_horizontalWallTexture.TextureWidth);
	}

	void Update(float delta_y);

	void setWindowHeight(float screenHeight);

	void setLevelHeight(int count);
	float getLevelHeight() const;

	bool inBounds(int y) const;
private:
	static void my_log(std::string message);

	static bool IsIntersect(const RECT& a, const RECT& b);

	struct WallTexture
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture;
		float TextureWidth;
		float TextureHeight;
	};

	float BASIC_SCALE = 0.3f; // TODO: make this static 
	WallTexture m_horizontalWallTexture;
	WallTexture m_verticalWallTexture;
	std::vector<Wall> m_walls;
	DirectX::XMFLOAT2 m_centerPos = {0, 0}; 
	float m_screenHeight;
	float m_levelHeight;
};

