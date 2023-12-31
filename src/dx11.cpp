#if WINDOWS_NATIVE
#include "headers/dx11.hpp"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gimi::dx11 {
	ID3D11Device *g_pd3dDevice = nullptr;
	ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
	IDXGISwapChain *g_pSwapChain = nullptr;
	UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
	ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;

	void NewFrame(bool &done) {
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			return;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	bool CreateDeviceD3D(HWND hWnd) {
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
		HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
													featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
													&g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
		if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
			res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
												featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
												&g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
		if (res != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}

	void CleanupDeviceD3D() {
		CleanupRenderTarget();
		if (g_pSwapChain) {
			g_pSwapChain->Release();
			g_pSwapChain = nullptr;
		}
		if (g_pd3dDeviceContext) {
			g_pd3dDeviceContext->Release();
			g_pd3dDeviceContext = nullptr;
		}
		if (g_pd3dDevice) {
			g_pd3dDevice->Release();
			g_pd3dDevice = nullptr;
		}
	}

	void CreateRenderTarget() {
		ID3D11Texture2D *pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}

	void CleanupRenderTarget() {
		if (g_mainRenderTargetView) {
			g_mainRenderTargetView->Release();
			g_mainRenderTargetView = nullptr;
		}
	}

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg) {
			case WM_SIZE:
				if (wParam == SIZE_MINIMIZED)
					return 0;
				g_ResizeWidth = (UINT) LOWORD(lParam); // Queue resize
				g_ResizeHeight = (UINT) HIWORD(lParam);
				return 0;
			case WM_SYSCOMMAND:
				if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
					return 0;
				break;
			case WM_DESTROY:
				::PostQuitMessage(0);
				return 0;
			case WM_DPICHANGED:
				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports) {
					//const int dpi = HIWORD(wParam);
					//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
					const RECT *suggested_rect = (RECT *) lParam;
					::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top,
								   suggested_rect->right - suggested_rect->left,
								   suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
				}
				break;
		}
		return ::DefWindowProcW(hWnd, msg, wParam, lParam);
	}
}

#endif