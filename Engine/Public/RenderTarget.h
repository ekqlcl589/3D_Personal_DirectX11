#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT Clear();
	HRESULT Set_ShaderResourceView(class CShader* pShader, const char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr }; /* ����Ÿ�ٿ�*/
	ID3D11ShaderResourceView*	m_pSRV = { nullptr }; /* ���̴����޿� */

	_float4						m_vColor;

#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
	_float						m_fX, m_fY, m_fSizeX, m_fSizeY;
#endif // _DEBUG

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vColor);
	virtual void Free() override;
};

END