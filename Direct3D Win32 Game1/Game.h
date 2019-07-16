//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include "AnimatedTexture.h"
#include "ScrollingBackground.h"
#include "Wall.h"
#include "LevelParser.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:
	void my_log(std::string message)
	{
#ifndef NDEBUG
		// print message
		util::ServiceLocator::getFileLogger()->print<util::SeverityType::info>(message);
#endif
	}
    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

	
	/*// Cat.png
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_catTexture;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_floorOrigin;
	std::unique_ptr<DirectX::CommonStates> m_states;*/
	
	// grass.jpg
	std::unique_ptr<DirectX::SpriteBatch>            m_spriteBatch;
	RECT											 m_fullscreenRect;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;

	// Walls 
	std::unique_ptr<WallsHandler>					 m_walls;

	// LevelParser
	std::unique_ptr<LevelParser>					 m_levelParser;

	// spaceship (animated)
	std::unique_ptr<AnimatedTexture>				 m_object;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shipTexture;
	DirectX::SimpleMath::Vector2					 m_objectPos;

	// starfield 
	/*std::unique_ptr<ScrollingBackground> m_stars;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_starsTexture;*/
	
	// wooden_floor 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexture;
	DirectX::SimpleMath::Vector2					 m_floorPos;
	DirectX::SimpleMath::Vector2					 m_floorOrigin;

	// keyboard and mouse 
	std::unique_ptr<DirectX::Mouse>					 m_mouse;
	std::unique_ptr<DirectX::Keyboard>				 m_keyboard;
};
