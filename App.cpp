#include "App.h"
#include "CubeTestVertexShader.h"
#include "CubeTestPixelShader.h"
#include "framework/Cube.h"
#include "framework/CwfException.h"
#include "framework/Graphics.h"
#include "framework/Material.h"
#include "framework/Window.h"
#include "framework/WindowBuilder.h"
#include "framework/WindowClass.h"
#include <algorithm>
#include <d3d11.h>
#include <memory>

void App::doFrame() {
	/*w->gfx().clearBuffer(0.0f,
		std::clamp(static_cast<float>(w->mouse.getX()) / static_cast<float>(w->getClientWidth()), 0.0f, 1.0f),
		std::clamp(static_cast<float>(w->mouse.getY()) / static_cast<float>(w->getClientHeight()), 0.0f, 1.0f));
	w->gfx().drawTestCube(w->kbd.isKeyPressed('A'), w->kbd.isKeyPressed('D'),
		w->kbd.isKeyPressed('W'), w->kbd.isKeyPressed('S'));*/
	Graphics& gfx{ w->gfx() };
	cube.draw(gfx);
	otherCube.draw(gfx);
	gfx.endFrame();
}

LRESULT WndProc(Window* pWindow, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

App::App(HINSTANCE hInstance) : cube{ DXGI_FORMAT_R16_UINT }, otherCube{ cube } {
	WindowClass wc{ hInstance, className };
	wc.registerClass();

	w = std::make_unique<Window>(WindowBuilder{ hInstance, className,
												windowName, WndProc }
		.addWindowStyle(WS_MINIMIZEBOX)
		.setClientSize(1000, 1000)
		.build());
	Graphics::IndexedVertexList<math::XMFLOAT3, uint16_t> mesh{ Cube::mesh<math::XMFLOAT3, uint16_t>() };
	
	cube.setInputLayout(Cube::defaultLayout(), Cube::defaultLayoutSize());
	cube.setTopology(Cube::topology());
	cube.setVertexShader(g_pVertexShader, sizeof(g_pVertexShader));
	cube.setPixelShader(g_pPixelShader, sizeof(g_pPixelShader));
	cube.setRenderTarget(w->gfx().getRenderTargetView(), nullptr);
	cube.setViewport(0.0f, 0.0f, w->getClientWidth(), w->getClientHeight());
	cube.addMesh(mesh.vertices, mesh.indices);
	struct CBuf {
		math::XMMATRIX transformation;
	};
	CBuf constantBuffer{ math::XMMatrixTranspose(math::XMMatrixRotationY(1.0f) * math::XMMatrixTranslation(0, 0, 2.0f)
		* math::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 4.0f)) };
	cube.addConstantBuffer(&constantBuffer, sizeof(constantBuffer), ShaderStage::VERTEX);

	CBuf otherConstantBuffer{ math::XMMatrixTranspose(math::XMMatrixTranslation(-0.5, 0, 3.0f)
		* math::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 4.0f)) };
	otherCube.addMesh(mesh.vertices, mesh.indices);
	otherCube.addConstantBuffer(&otherConstantBuffer, sizeof(otherConstantBuffer), ShaderStage::VERTEX);
}

int App::run() {
	cube.setupPipeline(w->gfx());
	otherCube.setupPipeline(w->gfx());
	w->showWindow();
	std::optional<int> exitCode{};
	while (true) {
		try {
			exitCode = w->processMessagesOnQueue();
			if (exitCode) return *exitCode; // if the exitCode isn't empty, return its value
			doFrame();
		}
		catch (const CwfException& e) {
			w->createExceptionMessageBox(e);
			break;
		}
		catch (const std::exception& e) {
			w->createExceptionMessageBox(e);
			break;
		}
		catch (...) {
			w->createExceptionMessageBox(CWF_EXCEPTION(CwfException::Type::OTHER, L"Unknown exception occurred."));
			break;
		}
	}
	return 2;
}