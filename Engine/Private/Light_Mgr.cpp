#include "..\Public\Light_Mgr.h"
#include "Lights.h"

IMPLEMENT_SINGLETON(CLight_Mgr)

CLight_Mgr::CLight_Mgr()
{
}

const LIGHT_DESC * CLight_Mgr::Get_Light(_uint iIndex)
{
	auto iter = m_Lights.begin();
	for (_uint i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_Light();
}

HRESULT CLight_Mgr::Add_Lights(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHT_DESC & LightDesc)
{
	CLights* pLight = CLights::Create(pDevice, pContext, LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Mgr::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

void CLight_Mgr::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
