#include "stdafx.h"
#include "..\Public\Player_Body.h"

#include "GameInstance.h"
#include "KeyMgr.h"

CPlayer_Body::CPlayer_Body(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer_Body::CPlayer_Body(const CPlayer_Body & rhs)
	: CGameObject(rhs)
	, m_tInfo(rhs.m_tInfo)
{
}

HRESULT CPlayer_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_tInfo, sizeof(PLAYERINFO));
	m_tInfo._Lv = 1;
	m_tInfo._ATK = 100;
	m_tInfo._MaxHp = 100.f;
	m_tInfo._Hp = 100.f;
	m_tInfo._MaxMp = 100.f;
	m_tInfo._Mp = 100;
	m_tInfo.CurrAnimState = ANIM_IDEL;
	m_tInfo.prevAnimState = ANIM_END;

	return S_OK;
}

HRESULT CPlayer_Body::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_animation = m_pModelCom->Get_Animations();


	//m_pModelCom->SetUp_Animation(9);

	return S_OK;
}

void CPlayer_Body::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	m_pModelCom->Play_Animation(TimeDelta);


}

void CPlayer_Body::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	Animation_State(m_tInfo.CurrAnimState, TimeDelta);
	Key_Input(TimeDelta);

	m_AnimDuration = m_pModelCom->Get_AnimDuration();

	m_AnimTimeAcc = m_pModelCom->Get_AnimTimeAcc();


}

HRESULT CPlayer_Body::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/
	
		m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

void CPlayer_Body::Key_Input(_double TimeDelta)
{
	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_UP))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
		m_tInfo.CurrAnimState = ANIM_RUN;
	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_UP))
		m_tInfo.CurrAnimState = ANIM_RUN_END;

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_DOWN))
		m_pTransformCom->Go_Back(TimeDelta);


	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_LEFT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -1.f);
		m_tInfo.CurrAnimState = ANIM_RUN_L;
	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_LEFT))
		m_tInfo.CurrAnimState = ANIM_RUN_END;


	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_RIGHT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
		m_tInfo.CurrAnimState = ANIM_RUN_R;
	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_RIGHT))
		m_tInfo.CurrAnimState = ANIM_RUN_END;


	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_D))
	{
		Attack();
	}
	//else if (CKeyMgr::GetInstance()->Mouse_Up(DIMK_LB))
	//	m_tInfo.CurrAnimState = ANIM_IDEL;

	//else
	//	m_tInfo.CurrAnimState = ANIM_IDEL;

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_Q))
		m_tInfo._Hp -= 10.f;
}

void CPlayer_Body::Animation_State(PLAYERANIMSTATE eType, _double TimeDelta)
{
	if (m_tInfo.CurrAnimState != m_tInfo.prevAnimState )
	{
		switch (m_tInfo.CurrAnimState)
		{
		case ANIM_IDEL:
			m_pModelCom->SetUp_Animation(9);
			break;

		case ANIM_RUN:
			m_pModelCom->SetUp_Animation(3);
			break;

		case ANIM_RUN_L:
			m_pModelCom->SetUp_Animation(4);
			break;

		case ANIM_RUN_R:
			m_pModelCom->SetUp_Animation(5);
			break;

		case ANIM_RUN_END:
			m_pModelCom->SetUp_Animation(6);
			break;

		case ANIM_ATTACK:
			m_pModelCom->SetUp_Animation(11);
			break;

		case ANIM_ATTACK_COMBO:
			Attack_Combo(TimeDelta);

		case ANIM_END:
		default:
			break;
		}
		//���⼭ ���� �ִϸ��̼��� ���� �Ǹ� idel�� �Ѿ�� �ڵ� ¥���� 
		// ���� �ִϸ��̼��� ����ƴٴ� ������ timeacc �� duration�̶� ��ġ �ϸ�?
		m_tInfo.prevAnimState = m_tInfo.CurrAnimState;
	}
	if (m_AnimTimeAcc >= m_AnimDuration)
	{
		m_tInfo.CurrAnimState = ANIM_IDEL;
	}

}

void CPlayer_Body::Hit(const _int & _Damage)
{
}

void CPlayer_Body::Attack()
{
	m_tInfo.CurrAnimState = ANIM_ATTACK;

	//if (m_AnimDuration >= 24.f)
		//m_tInfo.CurrAnimState = ANIM_IDEL;

	m_ComboCheck = true;

}

void CPlayer_Body::Attack_Combo(_double TimeDelta)
{
	if (true == m_ComboCheck)
	{
		if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_RB))
		{
			m_pModelCom->SetUp_Animation(11);
			m_ComboTime = 0.5f * TimeDelta;
			if (m_ComboTime <= 5.f && CKeyMgr::GetInstance()->Mouse_Down(DIMK_RB))
			{
				m_pModelCom->SetUp_Animation(12);
				if (m_ComboTime <= 5.f && CKeyMgr::GetInstance()->Mouse_Down(DIMK_RB))
				{
					m_pModelCom->SetUp_Animation(13);
					m_ComboCheck = false;
				}
			}

		}
	}
}

CTransform * CPlayer_Body::Get_Transform()
{
	return m_pTransformCom;
}

HRESULT CPlayer_Body::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Body"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Body::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pInstance->Get_CamPos(), sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = pInstance->Get_Light(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CPlayer_Body * CPlayer_Body::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Body*		pInstance = new CPlayer_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Body::Clone(void * pArg)
{
	CPlayer_Body*		pInstance = new CPlayer_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Body::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}