#include "App.h"
#include "CubeTestVertexShader.h"
#include "CubeTestPixelShader.h"
#include "framework/CubeSkinned.h"
#include "framework/CwfException.h"
#include "framework/Graphics.h"
#include "framework/Material.h"
#include "framework/Orientation.h"
#include "framework/Window.h"
#include "framework/WindowBuilder.h"
#include "framework/WindowClass.h"
#include <algorithm>
#include <d3d11.h>
#include <memory>

void App::doFrame() {
	static Graphics& gfx{ m_window->gfx() };
	static constexpr float dTheta = 0.1f;
	static constexpr float dThetaMouse = 0.01f;
	// static Orientation o{};

	/*w->gfx().clearBuffer(0.0f,
		std::clamp(static_cast<float>(w->mouse.getX()) / static_cast<float>(w->getClientWidth()), 0.0f, 1.0f),
		std::clamp(static_cast<float>(w->mouse.getY()) / static_cast<float>(w->getClientHeight()), 0.0f, 1.0f));
	w->gfx().drawTestCube(w->kbd.isKeyPressed('A'), w->kbd.isKeyPressed('D'),
		w->kbd.isKeyPressed('W'), w->kbd.isKeyPressed('S'));*/
	
	float dX{ 0.0f };
	float dY{ 0.0f };

	/*if (m_window->kbd.isKeyPressed('A'))
		dY += dTheta;
	if (m_window->kbd.isKeyPressed('D'))
		dY -= dTheta; 
	if (m_window->kbd.isKeyPressed('W'))
		dX += dTheta;
	if (m_window->kbd.isKeyPressed('S'))
		dX -= dTheta;*/

	while (!m_window->mouse.isRawQueueEmpty()) {
		Mouse::PositionDelta dP{ *(m_window->mouse.pollRawQueue()) };
		dY += dP.x * dThetaMouse;
		dX += dP.y * dThetaMouse;
	}

	if (dX != 0.0f || dY != 0.0f) {
		// o.update(dX, dY, 0.0f);
		gfx.camera().updateOrientation(dX, dY, 0.0f);
		m_cbuf = {
			math::XMMatrixMultiply(
				gfx.camera().get(),
				gfx.getProjection()
			)
		};
		m_cube.updateCopyConstantBuffer(0, gfx, &m_cbuf, sizeof(m_cbuf));
	}

	gfx.clearBuffer(0, 0, 0);
	m_cube.draw(gfx);
	// m_otherCube.draw(gfx);
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

App::App(HINSTANCE hInstance) : m_cube{ CubeSkinned<L"bitmap.DDS", Graphics::Float3Tex>::material() }, 
	m_otherCube{ m_cube }, m_cbuf{} {

	WindowClass wc{ hInstance, s_className };
	wc.registerClass();

	m_window = std::make_unique<Window>(WindowBuilder{ hInstance, s_className,
												s_windowName, WndProc }
		.addWindowStyle(WS_MINIMIZEBOX)
		.setClientSize(1000, 1000)
		.build());
	Graphics& gfx{ m_window->gfx() };
	gfx.setProjection(90.0f, 0.5f, 4.0f);
	gfx.camera().setPosition( 0.0f, 0.0f, -2.0f );

	m_cbuf = gfx.camera().get() * gfx.getProjection();

	using TexturedCube = CubeSkinned<L"bitmap.DDS", Graphics::Float3Tex>;
	TexturedCube::addMesh();
	m_cube.setVertexShader(g_pVertexShader, sizeof(g_pVertexShader));
	m_cube.setPixelShader(g_pPixelShader, sizeof(g_pPixelShader));
	m_cube.setRenderTarget(gfx.getRenderTargetView(), gfx.getZBuffer());
	m_cube.setViewport(0.0f, 0.0f, m_window->getClientWidth(), m_window->getClientHeight());
	m_cube.addConstantBuffer(&m_cbuf, sizeof(m_cbuf), ShaderStage::VERTEX, false);
	Graphics::TConstBuffer otherConstantBuffer{ math::XMMatrixTranslation(-0.5, 0, 3.0f) * gfx.getProjection() };
	m_otherCube.addMesh(TexturedCube::mesh());
	m_otherCube.copyConstantBuffer(&otherConstantBuffer, sizeof(otherConstantBuffer), ShaderStage::VERTEX, true, true);
}

int App::run() {
	m_window->mouse.enableRawInput();
	m_cube.setupPipeline(m_window->gfx());
	m_otherCube.setupPipeline(m_window->gfx());
	m_window->showWindow();
	std::optional<int> exitCode{};
	while (true) {
		try {
			exitCode = m_window->processMessagesOnQueue();
			if (exitCode) return *exitCode; // if the exitCode isn't empty, return its value
			doFrame();
		} catch (const CwfException& e) {
			m_window->createExceptionMessageBox(e);
			break;
		} catch (const std::exception& e) {
			m_window->createExceptionMessageBox(e);
			break;
		} catch (...) {
			m_window->createExceptionMessageBox(CWF_EXCEPTION(CwfException::Type::OTHER, L"Unknown exception occurred."));
			break;
		}
	}
	return 2;
}