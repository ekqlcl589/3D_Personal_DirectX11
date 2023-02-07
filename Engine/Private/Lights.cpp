#include "..\Public\Lights.h"

CLights::CLights(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLights::Initialize(const LIGHT_DESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

CLights * CLights::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHT_DESC & LightDesc)
{
	CLights*		pInstance = new CLights(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Cloned : CLights");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLights::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
