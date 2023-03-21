#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight_Mgr final :	public CBase
{
	DECLARE_SINGLETON(CLight_Mgr)
private:
	CLight_Mgr();
	virtual ~CLight_Mgr() = default;

public:
	const LIGHT_DESC* Get_Light(_uint iIndex);

public:
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	list<class CLights*> m_Lights;

public:
	virtual void Free()override;
};

END