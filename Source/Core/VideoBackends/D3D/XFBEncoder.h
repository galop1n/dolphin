// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include "VideoCommon/VideoCommon.h"
#include "VideoBackends/D3D/D3DPtr.h"

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;

namespace DX11
{

class XFBEncoder
{

public:

	XFBEncoder();

	void Init();
	void Shutdown();

	void Encode(u8* dst, u32 width, u32 height, const EFBRectangle& srcRect, float gamma);

private:

	ID3D11Texture2D* m_out;
	ID3D11RenderTargetView* m_outRTV;
	ID3D11Texture2D* m_outStage;
	ID3D11Buffer* m_encodeParams;
	ID3D11Buffer* m_quad;
	D3D::UniquePtr<ID3D11VertexShader> m_vShader;
	ID3D11InputLayout* m_quadLayout;
	D3D::UniquePtr<ID3D11PixelShader> m_pShader;
	ID3D11BlendState* m_xfbEncodeBlendState;
	ID3D11DepthStencilState* m_xfbEncodeDepthState;
	ID3D11RasterizerState* m_xfbEncodeRastState;
	ID3D11SamplerState* m_efbSampler;

};

}
