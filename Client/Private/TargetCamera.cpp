#include "stdafx.h"
#include "..\Public\TargetCamera.h"
#include "GameInstance.h"
#include "KeyMgr.h"


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
	Target_Renewal(TimeDelta);

	Key_Input(TimeDelta);
	//if (m_pInstance->Get_DIKeyState(DIK_TAB))
	//{
	//	if (m_bCheck)
	//		return;
	//
	//	m_bCheck = true;
	//
	//	if (m_bFix)
	//		m_bFix = false;
	//	else
	//		m_bFix = true;
	//}
	//else
	//	m_bCheck = false;
	//
	//if (false == m_bFix)
	//	return;

	//if (false == m_bFix)
	//{
	//  Mouse_Fix();
	//  Mouse_Check(TimeDelta);
	//}

	__super::Tick(TimeDelta);
}

void CTargetCamera::LateTick(_double TimeDelta)
{

	__super::LateTick(TimeDelta);
}

HRESULT CTargetCamera::Render()
{
	return S_OK;
}

void CTargetCamera::Target_Renewal(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector fPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float3 fTarget;

	XMStoreFloat3(&fTarget, fPosition);
	
	fTarget.y += m_fDis;
	fTarget.z -= 6.f;

	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fTarget));

	_long Mouse = 0;

	if (Mouse = m_pInstance->Get_DIMouseMove(DIMM_X))
	{
		pPlayerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * Mouse * 0.1f);
		//m_Transform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * Mouse * 0.1f);
	}

	if (Mouse = m_pInstance->Get_DIMouseMove(DIMM_Y))
	{
		//pPlayerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * Mouse * 0.1f);
		m_Transform->Turn(m_Transform->Get_State(CTransform::STATE_RIGHT), TimeDelta * Mouse * 0.1f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CTargetCamera::Key_Input(_double TimeDelta)
{
	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_N))
		m_fDis += 1.f * TimeDelta;

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_M))
		m_fDis -= 1.f * TimeDelta;
}

void CTargetCamera::Set_CameraPos(_float x, _float z)
{
	m_CameraDesc.vEye.x = x;
	m_CameraDesc.vEye.z = z - 9.f;
}

void CTargetCamera::Mouse_Check(_double TimeDelta)
{
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
