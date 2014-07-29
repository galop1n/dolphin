// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include <map>

#include "VideoBackends/D3D/D3DBase.h"
#include "VideoBackends/D3D/D3DBlob.h"

#include "VideoCommon/VertexShaderGen.h"

namespace DX11 {

class VertexShaderCache
{
public:
	static void Init();
	static void Clear();
	static void Shutdown();
	static bool SetShader(u32 components); // TODO: Should be renamed to LoadShader

	static ID3D11VertexShader* GetActiveShader() { return last_entry->shader.get(); }
	static D3DBlob const& GetActiveShaderBytecode() { return last_entry->bytecode; }
	static ID3D11Buffer* &GetConstantBuffer();

	static ID3D11VertexShader* GetSimpleVertexShader();
	static ID3D11VertexShader* GetClearVertexShader();
	static ID3D11InputLayout* GetSimpleInputLayout();
	static ID3D11InputLayout* GetClearInputLayout();

	static bool VertexShaderCache::InsertByteCode(const VertexShaderUid &uid, D3DBlob&& bcodeblob);

private:
	struct VSCacheEntry
	{
		D3D::UniquePtr<ID3D11VertexShader> shader;
		D3DBlob bytecode; // needed to initialize the input layout

		std::string code;

		VSCacheEntry() : shader(nullptr), bytecode(nullptr) {}
		void SetByteCode(D3DBlob&& blob)
		{
			bytecode = std::move(blob);
		}
		void Destroy()
		{
			shader.reset();
			bytecode = nullptr;
		}
	};
	typedef std::map<VertexShaderUid, VSCacheEntry> VSCache;

	static VSCache vshaders;
	static const VSCacheEntry* last_entry;
	static VertexShaderUid last_uid;

	static UidChecker<VertexShaderUid,VertexShaderCode> vertex_uid_checker;
};

}  // namespace DX11
