#pragma once

#include <exception>
#include <SpriteBatch.h>

#include  "WallsHandler.h"
class ScrollingBackground
{
public:
	ScrollingBackground() :
		m_screenHeight(0),
		m_textureWidth(0),
		m_textureHeight(0),
		m_screenPos(0, 0),
		m_textureSize(0, 0),
		m_origin(0, 0),
		m_levelHeight(0),
		m_zeroY(0)
	{
	}

	void Load(ID3D11ShaderResourceView* texture)
	{
		m_texture = texture;

		if (texture)
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			texture->GetResource(resource.GetAddressOf());

			D3D11_RESOURCE_DIMENSION dim;
			resource->GetType(&dim);

			if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
				throw std::exception("ScrollingBackground expects a Texture2D");

			Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
			resource.As(&tex2D);

			D3D11_TEXTURE2D_DESC desc;
			tex2D->GetDesc(&desc);

			m_textureWidth = desc.Width;
			m_textureHeight = desc.Height;

			m_textureSize.x = 0.f;
			m_textureSize.y = float(desc.Height);

			m_origin.x = desc.Width / 2.f;
			m_origin.y = 0;
		}
	}

	void SetWindow(int screenWidth, int screenHeight, float padding)
	{
		m_screenHeight = screenHeight;

		m_screenPos.x = float(screenWidth) / 2.f;
		m_screenPos.y = float(screenHeight) - m_textureHeight - padding;
		m_textureCount = int(m_screenHeight / m_textureHeight);
	}

	void setLevelHeight(float height)
	{
		m_levelHeight = height;
	}

	void Update(float deltaY)
	{
		if (inBounds(m_zeroY - deltaY)) {
			m_zeroY -= deltaY;
			m_screenPos.y = fmodf(m_screenPos.y, float(m_textureHeight));
		}
	}

	void Draw(DirectX::SpriteBatch* batch) const
	{
		using namespace DirectX;

		SimpleMath::Vector2 screenPos = { m_screenPos};
		SimpleMath::Vector2 origin = { m_origin };

		my_log("count: " + std::to_string(m_textureCount));
		for (int i = -1; i <= m_textureCount; i++)
		{
			SimpleMath::Vector2 temp = screenPos;
			temp.y -= i * m_textureHeight - m_zeroY;
			batch->Draw(m_texture.Get(), temp, nullptr,
				Colors::White, 0.f, origin, g_XMOne, SpriteEffects_None, 0.f);
		}
	}

	
private:
	void my_log(std::string message) const 
	{
#ifndef NDEBUG
		// print message
		util::ServiceLocator::getFileLogger()->print<util::SeverityType::info>(message);
#endif
	}
	bool inBounds(float y) const
	{
		return y > 0 && y < m_levelHeight - m_screenHeight;
	}
	int													m_textureCount;
	int                                                 m_textureWidth;
	int                                                 m_textureHeight;
	DirectX::XMFLOAT2                                   m_screenPos;
	DirectX::XMFLOAT2                                   m_textureSize;
	DirectX::XMFLOAT2                                   m_origin;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_texture;
	float												m_levelHeight;
	float												m_zeroY;
	float                                               m_screenHeight;
};
