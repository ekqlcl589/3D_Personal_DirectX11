#include "stdafx.h"
#include "..\Public\DynamicCamera.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Player_Body.h"

CDynamicCamera::CDynamicCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCamera(pDevice, pContext)
{
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera & rhs)
	:CCamera(rhs)
	, m_pInstance(CGameInstance::GetInstance())
{
}

HRESULT CDynamicCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDynamicCamera::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDynamicCamera::Tick(_double TimeDelta)
{
	//Target_Renewal();
	Key_Input(TimeDelta);
	__super::Tick(TimeDelta);

}

void CDynamicCamera::LateTick(_double TimeDelta)
{
	if (false == m_bFix)
	{
		Mouse_Check(TimeDelta);
		Mouse_Fix();
	}

	__super::LateTick(TimeDelta);
}

HRESULT CDynamicCamera::Render()
{
	return S_OK;
}

void CDynamicCamera::Key_Input(_double TimeDelta)
{
	if (m_pInstance->Get_DIKeyState(DIK_W))
		m_Transform->Go_Straight(TimeDelta);

	if (m_pInstance->Get_DIKeyState(DIK_S))
		m_Transform->Go_Back(TimeDelta);

	if (m_pInstance->Get_DIKeyState(DIK_A))
		m_Transform->Go_Left(TimeDelta);

	if (m_pInstance->Get_DIKeyState(DIK_D))
		m_Transform->Go_Right(TimeDelta);

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

}

void CDynamicCamera::Target_Renewal()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Find_Prototype(TEXT("Prototype_GameObject_Player"))->Get_Component(TEXT("Prototype_Component_Transform")));
	//원본이 아니라 사본을 찾는걸 만들어서 적용해야 함

	_vector vLook;
	pPlayerTransform->Set_State(CTransform::STATE_LOOK, vLook);

	 XMStoreFloat3(&m_CameraDesc.vEye, vLook);
	 XMVector3Normalize(XMLoadFloat3(&m_CameraDesc.vEye));
	 m_CameraDesc.vEye.y = 1.f;
	 //_float3 f = {1.f, 1.f, 1.f};
	 //m_CameraDesc.vEye *  f; // 이건 나중에 휠로 조절할 수 있게 변수로 받아야 함

	 _vector vRight;
	 memcpy(&vRight, &pPlayerTransform->Get_WorldMatrix().r[0], sizeof(_vector));

	 //_matrix matRot;
	 //XMMatrixRotationAxis()

	 _float3 fPosition;
	 XMStoreFloat3(&fPosition, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	 m_CameraDesc.vEye.x += fPosition.x;
	 m_CameraDesc.vEye.y += fPosition.y;
	 m_CameraDesc.vEye.z += fPosition.z;
	 XMStoreFloat3(&m_CameraDesc.vAt, pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	 RELEASE_INSTANCE(CGameInstance);
}

void CDynamicCamera::Mouse_Check(_double TimeDelta)
{
	_long Mouse = 0;

	if (Mouse = m_pInstance->Get_DIMouseMove(DIMM_X))
		m_Transform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * Mouse * 0.1f);

	if (Mouse = m_pInstance->Get_DIMouseMove(DIMM_Y))
		m_Transform->Turn(m_Transform->Get_State(CTransform::STATE_RIGHT), TimeDelta * Mouse * 0.1f);
}

void CDynamicCamera::Mouse_Fix()
{
	POINT pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

CDynamicCamera * CDynamicCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDynamicCamera * pInstance = new CDynamicCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("DynamicCamera Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDynamicCamera::Clone(void * pArg)
{
	CDynamicCamera * pInstance = new CDynamicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("DynamicCamera Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamicCamera::Free()
{
	RELEASE_INSTANCE(CGameInstance);
	__super::Free();

}
