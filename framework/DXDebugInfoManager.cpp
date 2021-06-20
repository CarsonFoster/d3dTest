#include "CwfException.h"
#include "DXDebugInfoManager.h"
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>

DXDebugInfoManager::DXDebugInfoManager() : pInfoQ{}, next{ 0 } {
	// code credit due to Chuck Walbourn: https://walbourn.github.io/dxgi-debug-device/
	using LPDXGIGETDEBUGINTERFACE = HRESULT (CALLBACK*)(REFIID, void**);

	HMODULE dxgiDebug = LoadLibraryExW(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!dxgiDebug) throw CWF_LAST_EXCEPTION();

	auto dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
		reinterpret_cast<void*>(GetProcAddress(dxgiDebug, "DXGIGetDebugInterface")));
	if (!dxgiGetDebugInterface) throw CWF_LAST_EXCEPTION();

	HRESULT hr = dxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pInfoQ);
	if (FAILED(hr)) throw CWF_DX_EXCEPTION_NOGFX(hr);
}

void DXDebugInfoManager::set() noexcept {
	// no messages are removed, so the queue is essentially just a vector
	next = pInfoQ->GetNumStoredMessages(DXGI_DEBUG_ALL); // current length will be the index of the next message to get
}

std::vector<std::wstring> DXDebugInfoManager::getMessages() const {
	std::vector<std::wstring> messages{};
	const UINT64 currentSize = pInfoQ->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < currentSize; i++) { // for each message in range
		SIZE_T messageLength{}; // in bytes
		HRESULT hr = pInfoQ->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &messageLength); // must first get message length
		if (FAILED(hr)) throw CWF_DX_EXCEPTION_NOGFX(hr);

		// memory trickery courtesy of Chili
		auto bytes = std::make_unique<std::byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		hr = pInfoQ->GetMessageW(DXGI_DEBUG_ALL, i, pMessage, &messageLength); // actually extract message
		if (FAILED(hr)) throw CWF_DX_EXCEPTION_NOGFX(hr);

		// necessary because pDescription is a const char*
		std::wostringstream builder{};
		builder << pMessage->pDescription;

		messages.push_back(builder.str()); // moves string
	}
	return messages; // should be moved
}