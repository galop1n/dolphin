// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include "VideoBackends/D3D/D3DBase.h"
#include "VideoBackends/D3D/D3DBlob.h"
#include "VideoBackends/D3D/VertexManager.h"
#include "VideoBackends/D3D/VertexShaderCache.h"
#include "VideoCommon/NativeVertexFormat.h"

namespace DX11
{

class D3DVertexFormat : public NativeVertexFormat
{
	D3D11_INPUT_ELEMENT_DESC m_elems[32];
	UINT m_num_elems{};

	D3D::InputLayoutPtr m_layout;

public:

	void Initialize(const PortableVertexDeclaration &_vtx_decl) override;
	void SetupVertexPointers() override;
	bool Equal(NativeVertexFormat const &) const override;
};

NativeVertexFormat* VertexManager::CreateNativeVertexFormat()
{
	return new D3DVertexFormat();
}

static const DXGI_FORMAT d3d_format_lookup[5*4*2] =
{
	// float formats
	DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_SNORM, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R16_SNORM, DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R8G8_UNORM, DXGI_FORMAT_R8G8_SNORM, DXGI_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16_SNORM, DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,

	// integer formats
	DXGI_FORMAT_R8_UINT, DXGI_FORMAT_R8_SINT, DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R16_SINT, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8_SINT, DXGI_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16_SINT, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_SINT, DXGI_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_UNKNOWN,
};

DXGI_FORMAT VarToD3D(VarType t, int size, bool integer)
{
	DXGI_FORMAT retval = d3d_format_lookup[(int)t + 5*(size-1) + 5*4*(int)integer];
	if (retval == DXGI_FORMAT_UNKNOWN)
	{
		PanicAlert("VarToD3D: Invalid type/size combo %i , %i, %i", (int)t, size, (int)integer);
	}
	return retval;
}

void D3DVertexFormat::Initialize(const PortableVertexDeclaration &_vtx_decl)
{
	vertex_stride = _vtx_decl.stride;
	memset(m_elems, 0, sizeof(m_elems));
	const AttributeFormat* format = &_vtx_decl.position;

	if (format->enable)
	{
		m_elems[m_num_elems].SemanticName = "POSITION";
		m_elems[m_num_elems].AlignedByteOffset = format->offset;
		m_elems[m_num_elems].Format = VarToD3D(format->type, format->components, format->integer);
		m_elems[m_num_elems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		++m_num_elems;
	}

	for (int i = 0; i < 3; i++)
	{
		format = &_vtx_decl.normals[i];
		if (format->enable)
		{
			m_elems[m_num_elems].SemanticName = "NORMAL";
			m_elems[m_num_elems].SemanticIndex = i;
			m_elems[m_num_elems].AlignedByteOffset = format->offset;
			m_elems[m_num_elems].Format = VarToD3D(format->type, format->components, format->integer);
			m_elems[m_num_elems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			++m_num_elems;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		format = &_vtx_decl.colors[i];
		if (format->enable)
		{
			m_elems[m_num_elems].SemanticName = "COLOR";
			m_elems[m_num_elems].SemanticIndex = i;
			m_elems[m_num_elems].AlignedByteOffset = format->offset;
			m_elems[m_num_elems].Format = VarToD3D(format->type, format->components, format->integer);
			m_elems[m_num_elems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			++m_num_elems;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		format = &_vtx_decl.texcoords[i];
		if (format->enable)
		{
			m_elems[m_num_elems].SemanticName = "TEXCOORD";
			m_elems[m_num_elems].SemanticIndex = i;
			m_elems[m_num_elems].AlignedByteOffset = format->offset;
			m_elems[m_num_elems].Format = VarToD3D(format->type, format->components, format->integer);
			m_elems[m_num_elems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			++m_num_elems;
		}
	}

	format = &_vtx_decl.posmtx;
	if (format->enable)
	{
		m_elems[m_num_elems].SemanticName = "BLENDINDICES";
		m_elems[m_num_elems].AlignedByteOffset = format->offset;
		m_elems[m_num_elems].Format = VarToD3D(format->type, format->components, format->integer);
		m_elems[m_num_elems].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		++m_num_elems;
	}
}

void D3DVertexFormat::SetupVertexPointers()
{
	if (!m_layout)
	{
		// CreateInputLayout requires a shader input, but it only looks at the
		// signature of the shader, so we don't need to recompute it if the shader
		// changes.
		auto& vs_bytecode = DX11::VertexShaderCache::GetActiveShaderBytecode();

		HRESULT hr = DX11::D3D::device->CreateInputLayout(m_elems, m_num_elems, vs_bytecode.Data(), vs_bytecode.Size(), ToAddr(m_layout) );
		if (FAILED(hr)) PanicAlert("Failed to create input layout, %s %d\n", __FILE__, __LINE__);
			DX11::D3D::SetDebugObjectName((ID3D11DeviceChild*)m_layout.get(), "input layout used to emulate the GX pipeline");
	}
	DX11::D3D::context->IASetInputLayout(m_layout.get());
}

bool D3DVertexFormat::Equal(NativeVertexFormat const& other) const
{
	D3DVertexFormat const & d3dvf = static_cast<D3DVertexFormat const &>( other );
	return d3dvf.m_num_elems == m_num_elems && memcmp( m_elems, d3dvf.m_elems, sizeof( D3D11_INPUT_ELEMENT_DESC )*m_num_elems ) == 0;
}

} // namespace DX11
