#ifndef CWF_DXDEBUGINFOMANAGER_H
#define CWF_DXDEBUGINFOMANAGER_H

#include <dxgidebug.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>

class DXDebugInfoManager { // courtesy of ChiliTomatoNoodle, great guy (explicit language warning tho)
private:
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pInfoQ;
	UINT64 next;
public:
	DXDebugInfoManager();
	~DXDebugInfoManager() = default;
	// no copy init/assign
	DXDebugInfoManager(const DXDebugInfoManager& o) = delete;
	DXDebugInfoManager& operator=(const DXDebugInfoManager& o) = delete;

	void set() noexcept;
	std::vector<std::wstring> getMessages() const; // DO NOT call during stack unraveling
};

#endif