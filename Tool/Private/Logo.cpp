#include "stdafx.h"
#include "..\Public\Logo.h"
#include "GameInstance.h"
#include "Loading.h"

CLogo::CLogo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLogo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLogo::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	//_tchar			szWindowText[MAX_PATH] = TEXT("");
	//lstrcpy(szWindowText, TEXT("Tool_Ready"));
	//SetWindowText(g_hWnd, szWindowText);
#endif

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLoading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		Safe_Release(pGameInstance);

	}
}

HRESULT CLogo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"), pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLogo * CLogo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLogo * pInstance = new CLogo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Fail Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLogo::Free()
{
	__super::Free();
}
