#pragma once

/* 렌더타겟들을 모아놓는다. */
/* 동시에 장치에 바인딩 되어야할 타겟들을 모아놓는다. */
#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

	HRESULT Begin(ID3D11DeviceContext * pContext, const _tchar * pMRTTag);	
	HRESULT End(ID3D11DeviceContext* pContext);

public:
	HRESULT Set_ShaderResourceView(class CShader* pShader, const _tchar* pTargetTag, const char* pContantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const _tchar* pMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	unordered_map<const _tchar*, class CRenderTarget*>			m_RTVs;
	unordered_map<const _tchar*, list<class CRenderTarget*>>	m_MRTs;

private:
	ID3D11RenderTargetView*				m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView*				m_pDepthStencilView = { nullptr };


private:
	class CRenderTarget* Find_Target(const _tchar* pTargetTag);
	class list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END