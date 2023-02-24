#include "stdafx.h"

#include "..\Public\Loading.h"
#include "Loader.h"
#include "Logo.h"
#include "GamePlay.h"
#include "GameInstance.h"

CLoading::CLoading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel(pDevice, pContext)
{
}


HRESULT CLoading::Initialize(LEVELID eNextLevel)
{

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

	/* 로딩에 필요한 객체의 갱신 : 로딩바에 증가,, QORUDDP DOSLAPDLTUS */

	if (true == m_pLoader->isFinished() &&
		GetKeyState(VK_RETURN) & 0x8000)
	{

		CGameInstance*			pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CLevel*			pLevel = { nullptr };

		/* Loader를 통해 사용할 레벨의 로딩이 끝나면 
		m_eNextLevelID로 실제 할당하는 작업이여기서 이뤄짐 */
		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLogo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CGamePlay::Create(m_pDevice, m_pContext);
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

	_tchar			szWindowText[MAX_PATH] = TEXT("");
	lstrcpy(szWindowText, TEXT("로딩레벨입니다 : "));
	lstrcat(szWindowText, m_pLoader->Get_LoadingText());
	SetWindowText(g_hWnd, szWindowText);

#endif
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
