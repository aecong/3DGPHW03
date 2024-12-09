//-----------------------------------------------------------------------------
// File: Shader.h
//-----------------------------------------------------------------------------

#pragma once

#include "Object.h"
#include "Camera.h"

#define BULLETS					30

class CScene;

class CShader
{
public:
	CShader();
	virtual ~CShader();

	CPlayer* player = NULL;

	CGameObject						**m_ppObjects = 0;
	int								m_nObjects = 0;

private:
	int									m_nReferences = 0;


protected:
	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;
	ID3DBlob* m_pd3dGeometryShaderBlob = NULL;

	int									m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;

public:
	void SetPlayer(CPlayer* p) { player = p; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState=0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState=0);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState=0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState=0);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOuputState(int nPipelineState=0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(int nPipelineState) { return(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE); }
	virtual UINT GetNumRenderTargets(int nPipelineState) { return(1); }
	virtual DXGI_FORMAT GetRTVFormat(int nPipelineState, int nRenderTarget) { return(DXGI_FORMAT_R8G8B8A8_UNORM); }
	virtual DXGI_FORMAT GetDSVFormat(int nPipelineState) { return(DXGI_FORMAT_D24_UNORM_S8_UINT); }

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState=0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(int nPipelineState = 0);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariable() { }	

	virtual void ReleaseShaderVariables() { }
	virtual void ReleaseUploadBuffers() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CStandardShader : public CShader
{
public:
	CStandardShader();
	virtual ~CStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	
	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CObjectsShader : public CStandardShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
	
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState=0);
	
	int GetNumberOfObjects() { return(m_nObjects); }

protected:
	//ID3D12Resource* m_pd3dcbGameObjects = NULL;
	//CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState = 0);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);
};

class CBOShader : public CTexturedShader
{
public:
	CBOShader();
	virtual ~CBOShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL) {};
	virtual void AnimateObjects(float fDeltaTime);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	int GetNumberOfObjects() { return(m_nObjects); }

protected:
	CGameObject** m_ppObjects = 0;
	int								m_nObjects = 0;

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
};

class CBillboardObjectsShader : public CBOShader
{
public:
	CBillboardObjectsShader();
	virtual ~CBillboardObjectsShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState = 0);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

	//virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	//virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void ReleaseUploadBuffers();

#ifdef _WITH_BATCH_MATERIAL
	CMaterial* m_ppGrassMaterials[2] = { NULL, NULL };
	CMaterial* m_ppFlowerMaterials[2] = { NULL, NULL };
#endif

	CRawFormatImage* m_pRawFormatImage = NULL;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CBillboardShader : public CTexturedShader
{
public:
	CBillboardShader();
	virtual ~CBillboardShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState = 0);
};

class CParticleShader : public CBillboardShader
{
public:
	CParticleShader();
	virtual ~CParticleShader();

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(int nPipelineState);
	virtual UINT GetNumRenderTargets(int nPipelineState);
	virtual DXGI_FORMAT GetRTVFormat(int nPipelineState, int nRenderTarget);
	virtual DXGI_FORMAT GetDSVFormat(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOuputState(int nPipelineState);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);

	virtual void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTextureToScreenShader : public CShader
{
public:
	CTextureToScreenShader(int nMeshes);
	virtual ~CTextureToScreenShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

public:
	int								m_nMeshes = 0;
	CMesh							**m_ppMeshes = NULL;

	CTexture						*m_pTexture = NULL;

	void SetMesh(UINT nIndex, CMesh* pMesh);
	void SetTexture(CTexture* pTexture);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTerrainShader : public CShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState = 0);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
