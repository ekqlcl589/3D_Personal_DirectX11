#include "stdafx.h"
#include "..\Public\TargetCamera.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Player_Body.h"


CTargetCamera::CTargetCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CTargetCamera::CTargetCamera(const CTargetCamera & rhs)
	: CCamera(rhs)
	, m_pInstance(CGameInstance::GetInstance())
{
}

HRESULT CTargetCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTargetCamera::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTargetCamera::Tick(_double TimeDelta)
{

	Target_Renewal();

	if (m_pInstance->Get_DIKeyState(DIK_TAB))
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}
	else
		m_bCheck = false;

	if (false == m_bFix)
		return;

	__super::Tick(TimeDelta);
}

void CTargetCamera::LateTick(_double TimeDelta)
{
	//if (false == m_bFix)
	//{
	//	Mouse_Check(TimeDelta);
	//	Mouse_Fix();
	//}

	__super::LateTick(TimeDelta);
}

HRESULT CTargetCamera::Render()
{
	return S_OK;
}

void CTargetCamera::Target_Renewal()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector fPosition;
	fPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	XMStoreFloat3(&m_CameraDesc.vEye, fPosition);// -XMLoadFloat3(&m_CameraDesc.vEye));
	// 
	m_CameraDesc.vAt = m_CameraDesc.vEye;
	m_CameraDesc.vAt.y + 10.f;
	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	m_Transform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
	m_Transform->Set_TransformDesc(m_CameraDesc.Transform_Desc);

	RELEASE_INSTANCE(CGameInstance);
}

void CTargetCamera::Mouse_Check(_double TimeDelta)
{
	_long Mouse = 0;

	if (Mouse = m_pInstance->Get_DIMouseMove(DIMM_X))
		m_Transform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * Mouse * 0.1f);

	if (Mouse = m_pInstance->Get_DIMouseMove(DIMM_Y))
		m_Transform->Turn(m_Transform->Get_State(CTransform::STATE_RIGHT), TimeDelta * Mouse * 0.1f);
}

void CTargetCamera::Mouse_Fix()
{
	POINT pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CTargetCamera * CTargetCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTargetCamera * pInstance = new CTargetCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CTargetCamera Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTargetCamera::Clone(void * pArg)
{
	CTargetCamera * pInstance = new CTargetCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTargetCamera Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTargetCamera::Free()
{
	RELEASE_INSTANCE(CGameInstance);
	__super::Free();
}
