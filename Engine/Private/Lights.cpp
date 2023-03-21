#include "..\Public\Lights.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

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

HRESULT CLights::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::LIGHT_DIRECTIONAL == m_LightDesc.eLightType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::LIGHT_POINT == m_LightDesc.eLightType)
	{
		iPassIndex = 2;

		if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Set_RawValue("g_vLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
	}


	if (FAILED(pShader->Begin(iPassIndex)))
		return E_FAIL;

	return pVIBuffer->Render();
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
