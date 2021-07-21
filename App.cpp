#include "App.h"
#include "CubeTestVertexShader.h"
#include "CubeTestPixelShader.h"
#include "framework/CubeSkinned.h"
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
	static Graphics& gfx{ w->gfx() };
	static constexpr float dTheta = 0.1f;
	static float angle{};
	bool changed{ false };
	/*w->gfx().clearBuffer(0.0f,
		std::clamp(static_cast<float>(w->mouse.getX()) / static_cast<float>(w->getClientWidth()), 0.0f, 1.0f),
		std::clamp(static_cast<float>(w->mouse.getY()) / static_cast<float>(w->getClientHeight()), 0.0f, 1.0f));
	w->gfx().drawTestCube(w->kbd.isKeyPressed('A'), w->kbd.isKeyPressed('D'),
		w->kbd.isKeyPressed('W'), w->kbd.isKeyPressed('S'));*/
	if (w->kbd.isKeyPressed('A')) {
		angle += dTheta;
		changed = true;
	}
	if (w->kbd.isKeyPressed('D')) {
		angle -= dTheta; 
		changed = true;
	}
	if (changed) {
		cbuf = { math::XMMatrixMultiply(math::XMMatrixRotationY(angle),
			math::XMMatrixMultiply(math::XMMatrixTranslation(0, 0, 2.0f),
			gfx.getProjection())) };
		cube.updateCopyConstantBuffer(0, gfx, &cbuf, sizeof(cbuf));
	}
	gfx.clearBuffer(0, 0, 0);
	cube.draw(gfx);
	//otherCube.draw(gfx);
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

App::App(HINSTANCE hInstance) : cube{ CubeSkinned<Graphics::Float3Tex>::material() }, /*otherCube{cube},*/ cbuf{} {
	WindowClass wc{ hInstance, className };
	wc.registerClass();

	w = std::make_unique<Window>(WindowBuilder{ hInstance, className,
												windowName, WndProc }
		.addWindowStyle(WS_MINIMIZEBOX)
		.setClientSize(1000, 1000)
		.build());
	w->gfx().setProjection(90.0f, 0.5f, 4.0f);

	cbuf = math::XMMatrixTranslation(0, 0, 2.0f) * w->gfx().getProjection();

	CubeSkinned<Graphics::Float3Tex>::addMesh();
	cube.setVertexShader(g_pVertexShader, sizeof(g_pVertexShader));
	cube.setPixelShader(g_pPixelShader, sizeof(g_pPixelShader));
	cube.setRenderTarget(w->gfx().getRenderTargetView(), w->gfx().getZBuffer());
	cube.setViewport(0.0f, 0.0f, w->getClientWidth(), w->getClientHeight());
	cube.addConstantBuffer(&cbuf, sizeof(cbuf), ShaderStage::VERTEX, false);
	cube.setTexture2D(Graphics::Texture2D{ L"bitmap.DDS" });
	//Graphics::TConstBuffer otherConstantBuffer{ math::XMMatrixTranslation(-0.5, 0, 3.0f) * w->gfx().getProjection() };
	//otherCube.addMesh(CubeSkinned<Graphics::Float3>::mesh());
	//otherCube.copyConstantBuffer(&otherConstantBuffer, sizeof(otherConstantBuffer), ShaderStage::VERTEX, true, true);
}

int App::run() {
	cube.setupPipeline(w->gfx());
	// otherCube.setupPipeline(w->gfx());
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