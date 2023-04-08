#include "stdafx.h"
#include "..\Public\TargetCamera.h"
#include "GameInstance.h"
#include "TSPlayer.h"
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

	XMStoreFloat4x4(&m_matWorld, m_Transform->Get_WorldMatrix());

	return S_OK;
}

void CTargetCamera::Tick(_double TimeDelta)
{
	m_fFov = m_CameraDesc.vFov;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	RELEASE_INSTANCE(CGameInstance);
	
	m_Player_F_Skill = static_cast<CTSPlayer*>(pPlayer)->Get_Info().fSkill;

	m_Player_Rage_Skill = static_cast<CTSPlayer*>(pPlayer)->Get_Info().rageSkill;

	if (m_Player_F_Skill)
	{
		m_fmulLook -= TimeDelta * 3.0 ;

		if (m_fmulLook <= 5.5)
			m_fmulLook = 5.5;
	}
	else
	{
		m_fmulLook += TimeDelta * 3.0;

	}

	if (m_fmulLook >= 9.0)
		m_fmulLook = 9.0;

	if (m_Player_Rage_Skill)
	{
		//Player_RageSkill(TimeDelta);
	}
	else
	{
		if (!m_BossOn)
			Target_Renewal(TimeDelta);
		else
			Target_Boss(TimeDelta);
	}

	Key_Input(TimeDelta);

	Camera_Shake(TimeDelta);

	//Player_Skill(TimeDelta);

	__super::Tick(TimeDelta);
}

void CTargetCamera::LateTick(_double TimeDelta)
{
	if (false == m_bFix)
	{
		Mouse_Check(TimeDelta);
		Mouse_Fix();
	}

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

	CTransform* pCamT = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_CameraTransform")));
	
	_vector vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vLook = pCamT->Get_State(CTransform::STATE_LOOK);

	_float3 fTarget;

	XMStoreFloat3(&fTarget, vPosition);
	
	fTarget.y += m_fDis;

	m_CameraDesc.vEye = fTarget;
	m_vLook = XMVector3Normalize(vLook);
	
	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye) - m_vLook * m_fmulLook);

	XMStoreFloat3(&m_CameraDesc.vAt, XMLoadFloat3(&m_CameraDesc.vEye) + m_vLook);

   	_long MouseX = 0;
	_long MouseY = 0;

	if (MouseX = m_pInstance->Get_DIMouseMove(DIMM_X))
	{
		pCamT->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseX * 0.05f);
	}

	if (MouseY = m_pInstance->Get_DIMouseMove(DIMM_Y))
	{
		pCamT->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), TimeDelta * MouseY * 0.02f);
	}
	
	m_Transform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));

	if (fTarget.z >= -42.0 && m_BossCamTimer >= 5.0)
	{
		m_BossOn = true;
		m_RenderName = true;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CTargetCamera::Test(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_CameraTransform")));

	_vector vPosition = m_Transform->Get_State(CTransform::STATE_POSITION);

	_vector vTargetPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	_vector vDir = vTargetPos - vPosition;

	_float3 fTarget;

	XMStoreFloat3(&fTarget, vTargetPos);

	_float3 Pos;

	XMStoreFloat3(&Pos, vPosition);

	XMLoadFloat3(&m_fDistance) = (vLook * -1);
	fTarget.y += 3.f;

	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fTarget));

	_long MouseX = 0;

	_matrix RotateMat = XMMatrixIdentity();

	// 앵글x,y
	// convert to tadian
	// rotatemat을 누적 시켜줘야 함
	
	if (m_fAngleX = m_pInstance->Get_DIMouseMove(DIMM_X))
	{
		pPlayerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * m_fAngleX * 0.1f);
		//RotateMat = XMMatrixRotationY(XMConvertToRadians(m_fAngleX));
		//
		//if (m_fAngleX >= 360.f)
		//	m_fAngleX = 360.f;
	}

	//m_vLook = XMVector3TransformNormal(m_vLook, RotateMat);
	//
	//m_Transform->LookAt(vTargetPos);

	RELEASE_INSTANCE(CGameInstance);
}

void CTargetCamera::Sibal(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
	_vector PlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	PlayerPosition.m128_f32[1] += 2.f;

	_long		MouseMove = 0;

	if (MouseMove = m_pInstance->Get_DIMouseMove(DIMM_X))
	{
		m_fAngleX += (_float)MouseMove * 0.001f;

	}

	if (MouseMove = m_pInstance->Get_DIMouseMove(DIMM_Y))
	{
		m_fAngleY += (_float)MouseMove * 0.001f;
	}

	_vector TargetQuaternion = XMQuaternionRotationRollPitchYaw(m_fAngleX, m_fAngleY, 0.f);
	_matrix RotationMatrix = XMMatrixRotationQuaternion(TargetQuaternion);
	m_fd = min(4.8f, max(3.6f, m_fd * 0.4f));
	RotationMatrix = Go_Straight(RotationMatrix, -m_fd);


	RotationMatrix.r[3] += PlayerPosition;
	RotationMatrix.r[3].m128_f32[3] = 1.f;

	XMStoreFloat4(&m_vTargetQuaternion, TargetQuaternion);
	XMStoreFloat3(&m_vTargetPosition, RotationMatrix.r[3]);
}

void CTargetCamera::Key_Input(_double TimeDelta)
{
	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_N))
		m_fDis += 1.f * TimeDelta;

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_M))
		m_fDis -= 1.f * TimeDelta;

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

void CTargetCamera::Player_RageSkill(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	CTransform* pCamT = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_CameraTransform")));

	_vector vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector vLook = pCamT->Get_State(CTransform::STATE_LOOK) * -1;

	_float3 fTarget;

	XMStoreFloat3(&fTarget, vPosition);

	fTarget.y -= m_fDis;

	m_CameraDesc.vEye = fTarget;
	m_vLook = XMVector3Normalize(vLook);

	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye) -= m_vLook * 2);

	pCamT->Turn(XMLoadFloat3(&m_CameraDesc.vAt), 1.5 * TimeDelta);
	//XMStoreFloat3(&m_CameraDesc.vAt, XMLoadFloat3(&m_CameraDesc.vEye) + m_vLook);

	m_Transform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));

	RELEASE_INSTANCE(CGameInstance);
}

void CTargetCamera::Target_Boss(_double TimeDelta)
{
	// 플레이어의 카메라 트랜스폼이 특정 위치에 도달하면 ? 
	// 혹은 특정 스테이지에 진입하면 바로 발동 시키고 카메라를 보스 앞 까지 이동 시켰다가 
	// 특정 타임 이후 다시 플레이어로 돌아가게 
	if (m_BossOn)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pMonster = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

		CTransform* pMonsterTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));

		RELEASE_INSTANCE(CGameInstance);

		_vector vPosition = pMonsterTransform->Get_State(CTransform::STATE_POSITION);

		_vector vLook = pMonsterTransform->Get_State(CTransform::STATE_LOOK);

		_float3 fPosition;

		XMStoreFloat3(&fPosition, vPosition);

		fPosition.y += m_fDis;

		m_CameraDesc.vEye = fPosition;
		m_vLook = XMVector3Normalize(vLook);

		m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye) + m_vLook * m_fmulLook);
		
		XMStoreFloat3(&m_CameraDesc.vAt, XMLoadFloat3(&m_CameraDesc.vEye) + m_vLook);

		m_Transform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
		//m_BossOn = false;

		m_BossCamTimer -= 1.f * TimeDelta;

		if (m_BossCamTimer <= 0.0)
		{
			m_BossCamTimer = 0.0;
			m_BossOn = false;
		}
	}
}

void CTargetCamera::View_TarGet(_double TimeDelta)
{
	if (!m_bTalkState)
	{
		if (m_fMaxAtX > 0)
		{
			if (m_fMaxAtX > m_CameraDesc.vAt.x)
			{
				XMLoadFloat3(&m_CameraDesc.vAt) += XMLoadFloat3(&_float3{ -m_CameraDesc.vEye.x, 0, 0 });
				XMLoadFloat3(&m_CameraDesc.vEye) += XMLoadFloat3(&_float3{ -m_CameraDesc.vAt.x, 0, 0 });

				m_CameraDesc.vEye.x += 0.1f;
				m_CameraDesc.vAt.x += 0.1f;
			}
			else if (m_fMaxEyeX < m_CameraDesc.vEye.x)
			{
				m_CameraDesc.vAt.x = 0;
				m_CameraDesc.vEye.x = 0;
				m_fMaxEyeX = 0;
				m_fMaxAtX = 0;
			}
			else
			{
				m_fDis = m_fDistanceValue;
			}
		}
		else
		{
			if (m_fMaxAtX < m_CameraDesc.vAt.x)
			{
				XMLoadFloat3(&m_CameraDesc.vAt) -= XMLoadFloat3(&_float3{ -m_CameraDesc.vEye.x, 0, 0 });
				XMLoadFloat3(&m_CameraDesc.vEye) -= XMLoadFloat3(&_float3{ -m_CameraDesc.vAt.x, 0, 0 });

				m_CameraDesc.vEye.x -= 0.1f;
				m_CameraDesc.vAt.x -= 0.1f;
			}
			else if (m_fMaxEyeX < m_CameraDesc.vEye.x)
			{
				m_CameraDesc.vAt.x = 0;
				m_CameraDesc.vEye.x = 0;
				m_fMaxEyeX = 0;
				m_fMaxAtX = 0;
			}
			else
			{
				m_fDis = m_fDistanceValue;
			}
		}
	}
	else if (m_bTalkState)
	{
		m_CameraDesc.vAt = m_vNpcPos;
		//m_vEye.x = m_vNpcPos.x;
		//m_vEye.y = m_vNpcPos.y;
		m_bSome = true;
		if (!m_AdjustX)
		{
			if (m_CameraDesc.vEye.x < m_vNpcPos.x)
			{
				m_CameraDesc.vEye.x += TimeDelta * 7.f;

				if (m_CameraDesc.vEye.x > m_vNpcPos.x)
				{
					m_CameraDesc.vEye.x = m_vNpcPos.x;
					m_AdjustX = true;
				}
			}
			else if (m_CameraDesc.vEye.x > m_vNpcPos.x)
			{
				m_CameraDesc.vEye.x -= TimeDelta * 7.f;

				if (m_CameraDesc.vEye.x < m_vNpcPos.x)
				{
					m_CameraDesc.vEye.x = m_vNpcPos.x;
					m_AdjustX = true;
				}
			}
		}

		if (!m_AdjustY)
		{
			if (m_CameraDesc.vEye.y < m_vNpcPos.y)
			{
				m_CameraDesc.vEye.y += TimeDelta * 7.f;

				if (m_CameraDesc.vEye.y > m_vNpcPos.y)
				{
					m_CameraDesc.vEye.y = m_vNpcPos.y;
					m_AdjustY = true;
				}
			}
			else if (m_CameraDesc.vEye.y > m_vNpcPos.y)
			{
				m_CameraDesc.vEye.y -= TimeDelta * 7.f;

				if (m_CameraDesc.vEye.y < m_vNpcPos.y)
				{
					m_CameraDesc.vEye.y = m_vNpcPos.y;
					m_AdjustY = true;
				}
			}
		}

		if (m_CameraDesc.vEye.z < m_vNpcPos.z - 3.3f)
		{
			m_CameraDesc.vEye.z += TimeDelta * 7.f;
		}

	}
}

void CTargetCamera::Set_CameraPos(_float x, _float z)
{
	m_CameraDesc.vEye.x = x;
	m_CameraDesc.vEye.z = z - 9.f;
}

void CTargetCamera::Camera_Shake(_double TimeDelta)
{
	if (m_fMaxShakeTime <= 0.f)
	{
		m_eCurrentShakeDir = SHAKE_DIRECTION::DIRECTION_END;
		return;
	}

	m_fShakeRatio = m_fCurrentShakeTime / m_fLoopShakeTime;
	m_fShakeRatio = min(1.f, max(0.f, m_fShakeRatio));

	if (m_bReverseShake)
	{
		m_fCurrentShakeTime -= TimeDelta;
		m_CameraDesc.vFov -= XMConvertToRadians(m_fShakePower); //m_fShakePower
	}
	else
	{
		m_fCurrentShakeTime += TimeDelta;
		m_CameraDesc.vFov += XMConvertToRadians(m_fShakePower);

	}

	if (m_fCurrentShakeTime >= m_fLoopShakeTime)
		m_bReverseShake = true;
	else if (m_fCurrentShakeTime <= 0.f)
		m_bReverseShake = false;

	m_fMaxShakeTime -= TimeDelta;
}

void CTargetCamera::Add_Shaking(const SHAKE_DIRECTION& In_eState, const _float& In_fPower, const _float& In_fTime)
{
	m_eCurrentShakeDir = In_eState;
	m_fShakePower = In_fPower;
	m_fMaxShakeTime = In_fTime;
	m_fCurrentShakeTime = m_fLoopShakeTime * 0.5f;
	m_bReverseShake = false;
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

XMMATRIX CTargetCamera::Go_Straight(FXMMATRIX Mat, float fScaler)
{
	XMMATRIX ResultMat = Mat;

	_vector		vPosition = ResultMat.r[3];
	_vector		vLook = ResultMat.r[2];

	vPosition += XMVector3Normalize(vLook) * fScaler;

	ResultMat.r[3] = vPosition;

	return ResultMat;
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
