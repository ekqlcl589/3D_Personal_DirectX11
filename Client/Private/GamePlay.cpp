#include "stdafx.h"

#include "..\Public\GamePlay.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "Cube.h"
#include "Monster.h"
#include "TestTile.h"

CGamePlay::CGamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CGamePlay::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//6LoadMeshTile(L"../Data/Tile/Tile.dat");

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	
	//LoadMonster(L"../Data/Monster.dat");


	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	//LoadData(L"../Data/Cube/Cube.dat");

	return S_OK;
}

void CGamePlay::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	_tchar			szWindowText[MAX_PATH] = TEXT("");
	lstrcpy(szWindowText, TEXT("스테이지"));
	SetWindowText(g_hWnd, szWindowText);
#endif
}

HRESULT CGamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), pLayerTag)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTile"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SkyBox"), pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	CCamera::CAMERADESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	CameraDesc.vEye = _float3(0.f, 10.f, -9.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	
	CameraDesc.vFov = XMConvertToRadians(60.f);
	CameraDesc.vAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.vNear = 0.2f;
	CameraDesc.vFar = 300.f;
	
	CameraDesc.Transform_Desc.fSpeed = 10.f;
	CameraDesc.Transform_Desc.fRotation = XMConvertToRadians(90.f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera"), pLayerTag, &CameraDesc)))
		return E_FAIL;
	
	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CGamePlay::Ready_Light()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHT_DESC LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);//대각선 아래로 
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGamePlay::Ready_UI(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Mouse"), pLayerTag)))
	//	return E_FAIL;

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HPBar"), pLayerTag)))
		return E_FAIL;
	
	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MPBar"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Skill_E"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Skill_R"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Skill_F"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RageSkill"), pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player_Body"), pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster"), pLayerTag)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CGamePlay::LoadData(_tchar * szFilePath)
{
	HANDLE        hFile = CreateFile(szFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Cube Load Fail");
		return;
	}

	DWORD        dwByte = 0;
	CCube::CUBESTATE        DataFile;

	while (true)
	{
		ReadFile(hFile, &DataFile, sizeof(CCube::CUBESTATE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Cube"), TEXT("Layer_Cube"), &DataFile);

	}
	RELEASE_INSTANCE(CGameInstance);

	CloseHandle(hFile);
}

void CGamePlay::LoadMonster(_tchar * szFilePath)
{
	HANDLE        hFile = CreateFile(szFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Monster Load Fail");
		return;
	}

	DWORD        dwByte = 0;
	CMonster::MONSTERSTATE        DataFile;

	while (true)
	{
		ReadFile(hFile, &DataFile, sizeof(CMonster::MONSTERSTATE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster"), TEXT("Layer_Player"), &DataFile);

	}
	RELEASE_INSTANCE(CGameInstance);

	CloseHandle(hFile);
}

void CGamePlay::LoadMeshTile(_tchar * szFilePath)
{
	HANDLE        hFile = CreateFile(szFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Tile Load Fail");
		return;
	}

	DWORD        dwByte = 0;
	CTestTile::TILESTATE        DataFile;

	while (true)
	{
		ReadFile(hFile, &DataFile, sizeof(CTestTile::TILESTATE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTile"), TEXT("Layer_Tile"), &DataFile);

	}
	RELEASE_INSTANCE(CGameInstance);

	CloseHandle(hFile);
}

CGamePlay * CGamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGamePlay * pInstance = new CGamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Fail GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGamePlay::Free()
{
	__super::Free();
}
