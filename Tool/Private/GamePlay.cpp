#include "stdafx.h"

#include "..\Public\GamePlay.h"
#include "GameInstance.h"
#include "DynamicCamera.h"
#include "ImGui_Tool.h"

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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Terrain"), pLayerTag)))
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
