#pragma once
#include "../ImGui/smooth_api/smooth_api.h"

namespace myimgui {

	inline static ID3D11Device* g_pd3dDevice = nullptr;
	inline static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	inline static IDXGISwapChain* g_pSwapChain = nullptr;
	inline static bool                     g_SwapChainOccluded = false;
	inline static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	inline static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

	inline HWND g_hwnd;

	int CreateImGui();
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}


