//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;


using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
	m_window(nullptr),
	m_outputWidth(800),
	m_outputHeight(600),
	m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_window = window;
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_keyboard = std::make_unique<Keyboard>();

	CreateDevice();

	CreateResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:

	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);

	// level parser
	m_levelParser->ReadLevelFile("level.txt");
}

// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.

	m_object->Update(elapsedTime);
	//m_stars->Update((elapsedTime * 10)); // some function of star_speed(time) 
	// TODO Rewrite this code, nice that it work but... 
	auto kb = m_keyboard->GetState();
	if (kb.Escape)
	{
		ExitGame();
	}
	Vector2 move = {0, 0};
	float boost = 1.f;
	if (kb.LeftShift)
	{
		boost = 3.f;
	}
	float rotateAngle = 0.f;

	if (kb.W)
	{
		move.y -= 1.f * boost;
		m_object->setState(AnimatedTexture::UP);
	}
	if (kb.S)
	{
		move.y += 1.f * boost;
		m_object->setState(AnimatedTexture::DOWN);
	}
	if (kb.A)
	{
		move.x -= 1.f * boost;
		m_object->setState(AnimatedTexture::LEFT);
	}
	if (kb.D)
	{
		move.x += 1.f * boost;
		m_object->setState(AnimatedTexture::RIGHT);
	} /*
	if (kb.W) {
		if (kb.A) {
			rotateAngle = -XM_PI / 4;
		}
		if (kb.D) {
			rotateAngle = XM_PI / 4;
		}
	}
	else if (kb.S) {
		if (kb.A) {
			rotateAngle = XM_PI + XM_PI / 4;
		}
		if (kb.D) {
			rotateAngle = XM_PI - XM_PI / 4;
		}
	}*/
	m_object->setRotation(rotateAngle);

	m_objectPos += move;
	m_walls->Update(move.y);

	RECT objectRect = {
		m_objectPos.x - m_object->m_texture_width() / 2,
		m_objectPos.y - m_object->m_texture_height() / 2,
		m_objectPos.x + m_object->m_texture_width() / 2,
		m_objectPos.y + m_object->m_texture_height() / 2
	};


	if (m_walls->IsIntersect(objectRect)) // O(walls)
	{
		m_objectPos -= move; // TODO: fix this, cancel only move.x || move.y
		m_walls->Update(-move.y);
	}
	

	/*
	if (!m_stars->InBackground(m_objectPos, m_object->getFrameWidth())) {
		m_objectPos.x -= move.x;
	}

	auto mouse = m_mouse->GetState();
	if (mouse.leftButton) {
		m_screenPos.x = mouse.x;
		m_screenPos.y = mouse.y; 
	}*/
}

// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	// TODO: Add your rendering code here.
	// Cat.png
	float time = float(m_timer.GetTotalSeconds());
	m_levelParser->AddWallsToDraw();
	m_spriteBatch->Begin();
	m_spriteBatch->Draw(m_background.Get(), m_fullscreenRect);
	m_spriteBatch->Draw(m_floorTexture.Get(), m_floorPos, nullptr, Colors::White,
	                    0.f, m_floorOrigin);
	m_walls->Draw(m_spriteBatch.get());

	//m_stars->Draw(m_spriteBatch.get());

	//m_spriteBatch->Draw(m_catTexture.Get(), m_screenPos, nullptr, Colors::White, cosf(time) * 4.f, m_floorOrigin, sinf(time) + 2.f); 
	m_object->Draw(m_spriteBatch.get(), m_objectPos);


	m_spriteBatch->End();
	Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	// Clear the views.
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Set the viewport.
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
	m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(1, 0);

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		OnDeviceLost();
	}
	else
	{
		DX::ThrowIfFailed(hr);
	}
}

// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	CreateResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels [] =
	{
		// TODO: Modify for supported Direct3D feature levels
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	DX::ThrowIfFailed(D3D11CreateDevice(
		nullptr, // specify nullptr to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(), // returns the Direct3D device created
		&m_featureLevel, // returns feature level of device created
		context.ReleaseAndGetAddressOf() // returns the device immediate context
	));

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide [] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	DX::ThrowIfFailed(device.As(&m_d3dDevice));
	DX::ThrowIfFailed(context.As(&m_d3dContext));

	// TODO: Initialize device dependent objects here (independent of window size).

	// Cat.png

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
	//m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());


	/*ComPtr<ID3D11Resource> resource; 
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"cat.dds",  // WIC - Windows Imaging Component 
			resource.GetAddressOf(), 
			m_catTexture.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> cat;
	DX::ThrowIfFailed(resource.As(&cat));
	CD3D11_TEXTURE2D_DESC catDesc; 
	cat->GetDesc(&catDesc);

	m_floorOrigin.x = float(catDesc.Width / 2);
	m_floorOrigin.y = float(catDesc.Height / 2);*/

	// background grass.jpg
	DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"grass.png", nullptr,
	                                           m_background.ReleaseAndGetAddressOf()));

	// spaceship (animated) 
	DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"turtles.png",
	                                           nullptr, m_shipTexture.ReleaseAndGetAddressOf()));

	// walls 
	// TODO: (vanya translate) Think about throwing a device in outer class, why samples avoid this? 
	// TODO: ѕодумать, а корректно ли передавать device в сторонние классы, не просто же так этого не делают

	m_walls = std::make_unique<WallsHandler>();
	m_walls->Load(m_d3dDevice.Get());

	//m_walls->addWall(150, 150, 2, WallsHandler::HORIZONTAL);
	//m_walls->addWall(150, 150, 3, WallsHandler::VERTICAL); 

	// object
	m_object = std::make_unique<AnimatedTexture>();
	m_object->Load(m_shipTexture.Get(), 3, 4, 8);

	// wooden_floor
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"wooden-floor.jpg",
	                                           resource.GetAddressOf(), m_floorTexture.ReleaseAndGetAddressOf()));
	ComPtr<ID3D11Texture2D> floor;
	DX::ThrowIfFailed(resource.As(&floor));
	CD3D11_TEXTURE2D_DESC floorDesc;
	floor->GetDesc(&floorDesc);


	m_floorOrigin.x = floorDesc.Width / 2.f;
	m_floorOrigin.y = floorDesc.Height / 2.f;

	m_walls->setScale(floorDesc.Width);

	/*// stars
	DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), L"starfield.png", nullptr,
		m_starsTexture.ReleaseAndGetAddressOf())); 
	m_stars = std::make_unique<ScrollingBackground>();
	m_stars->Load(m_starsTexture.Get());*/
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews [] = {nullptr};
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_d3dContext->Flush();

	UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
	UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT backBufferCount = 2;

	// If the swap chain already exists, resize it, otherwise create one.
	if (m_swapChain)
	{
		HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat,
		                                        0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_swapChain.ReleaseAndGetAddressOf()
		));

		// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
		DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	// Create a view interface on the rendertarget to use on bind.
	DX::ThrowIfFailed(
		m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1,
	                                       D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc,
	                                                      m_depthStencilView.ReleaseAndGetAddressOf()));

	// TODO: Initialize windows-size dependent objects here.

	/*// Cat.png
	m_screenPos.x = backBufferWidth / 2.f;
	m_screenPos.y = backBufferHeight / 2.f;*/
	// grass.png 
	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = backBufferWidth;
	m_fullscreenRect.bottom = backBufferHeight;
	//spaceship 
	m_objectPos.x = float(backBufferWidth / 2);
	m_objectPos.y = float((backBufferHeight / 2) + (backBufferHeight / 4));
	// stars
	//m_stars->SetWindow(backBufferWidth, backBufferHeight);

	// wooden_floor
	m_floorPos.x = float(backBufferWidth / 2);
	m_floorPos.y = float(backBufferHeight) - m_floorOrigin.y;

	// level parser
	DirectX::SimpleMath::Vector2 zero = {backBufferWidth / 2.f - m_floorOrigin.x, (float)backBufferHeight};
	if (m_levelParser)
	{
		m_levelParser->Update(zero);
	}
	else
	{
		m_levelParser = std::make_unique<LevelParser>(zero, m_walls.get());
	}
}


void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.

	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();
	//m_catTexture.Reset();
	m_spriteBatch.reset();

	m_walls->Reset(); // TODO: This is unreadable
	m_walls.reset();

	/*m_states.reset();
	m_background.Reset();
	m_object.reset();
	m_shipTexture.Reset();
	m_stars.reset();
	m_starsTexture.Reset();*/


	CreateDevice();

	CreateResources();
}
