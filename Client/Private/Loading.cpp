#include "stdafx.h"

#include "..\Public\Loading.h"
#include "Loader.h"
#include "Logo.h"
#include "GamePlay.h"
#include "GamePlay2.h"
#include "GameInstance.h"

CLoading::CLoading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel(pDevice, pContext)
{
}


HRESULT CLoading::Initialize(LEVELID eNextLevel)
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_LoadingImg"))))
		return E_FAIL;


	m_eNextLevelID = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);

	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLoading::Tick(_double TimeDelta)
{
	if (nullptr == m_pLoader)
		return;

	/* �ε��� �ʿ��� ��ü�� ���� : �ε��ٿ� ����,, QORUDDP DOSLAPDLTUS */

	if (true == m_pLoader->isFinished() &&
		GetKeyState(VK_RETURN) & 0x8000)
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_BGM);

		CGameInstance*			pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CLevel*			pLevel = { nullptr };

		/* Loader�� ���� ����� ������ �ε��� ������ 
		m_eNextLevelID�� ���� �Ҵ��ϴ� �۾��̿��⼭ �̷��� */
		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLogo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CGamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY2:
			pLevel = CGamePlay2::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
			return;

		Safe_Release(pGameInstance);

		return;
	}

#ifdef _DEBUG

	//_tchar			szWindowText[MAX_PATH] = TEXT("");
	//lstrcpy(szWindowText, TEXT("�ε������Դϴ� : "));
	//lstrcat(szWindowText, m_pLoader->Get_LoadingText());
	//SetWindowText(g_hWnd, szWindowText);

#endif
}


HRESULT CLoading::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOADING, TEXT("Prototype_GameObject_Loading"), pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoading * CLoading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel)
{
	CLoading * pInstance = new CLoading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Create Fail Loading");
		Safe_Release(pInstance);
	}
		return pInstance;
}

void CLoading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
