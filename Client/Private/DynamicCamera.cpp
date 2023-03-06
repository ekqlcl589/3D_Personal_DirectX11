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
	if (m_pInstance->Get_DIKeyState(DIK_UP))
		m_Transform->Go_Straight(TimeDelta);

	if (m_pInstance->Get_DIKeyState(DIK_DOWN))
		m_Transform->Go_Back(TimeDelta);

	if (m_pInstance->Get_DIKeyState(DIK_LEFT))
		m_Transform->Go_Left(TimeDelta);

	if (m_pInstance->Get_DIKeyState(DIK_RIGHT))
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
