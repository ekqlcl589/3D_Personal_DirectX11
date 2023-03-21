#pragma once
#include "Base.h"

BEGIN(Engine)

class CLights final :
	public CBase
{
private:
	CLights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLights() = default;

public:
	const LIGHT_DESC* Get_Light() const { return &m_LightDesc; }

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	LIGHT_DESC m_LightDesc;

public:
	static CLights* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	virtual void Free()override;
};

END