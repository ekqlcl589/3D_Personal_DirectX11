#include "stdafx.h"
#include "..\Public\Ancient_StonGolem.h"
#include "GameInstance.h"
#include "KeyMgr.h"

CAncient_StonGolem::CAncient_StonGolem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CAncient_StonGolem::CAncient_StonGolem(const CAncient_StonGolem & rhs)
	: CMonster(rhs)
{
}

HRESULT CAncient_StonGolem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CAncient_StonGolem::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eCollisionState = OBJ_BOSS1;

	//ZeroMemory(&m_MonsterState, sizeof(MONSTERSTATE));
	//
	//memcpy(&m_MonsterState, pArg, sizeof m_MonsterState);

	_float3 fPosition = { 10.f, 1.f, 10.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_eType._MaxHp = 100.f;
	m_eType._Hp = 100.f;

	m_f = m_pModelCom->Get_AnimTick();

	m_CurrAnim = S_WAIT;
	m_pModelCom->SetUp_Animation(m_CurrAnim);

	return S_OK;
}

void CAncient_StonGolem::Tick(_double TimeDelta)
{
	if (false == m_bStart)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		pInstance->Add_Collider(m_eCollisionState, 2, this);

		RELEASE_INSTANCE(CGameInstance);

		m_bStart = true;

	}
	__super::Tick(TimeDelta);

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_J))
		m_eType._Hp -= 10.f; 

	if (true == m_bAttack)
		m_CurrAnim != 5;

	Set_State(TimeDelta);

	Set_AnimationState(m_CurrAnim);

	//if (true == m_bPlayerChecck)
	//{
	//	Set_Anim();
	//
	//}
	cout << m_eType._Hp << endl;
}

void CAncient_StonGolem::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	Collision_ToPlayer();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CAncient_StonGolem::Render()
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

void CAncient_StonGolem::OnCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		m_eType._Hp -= 10.f;
		break;
	case Engine::OBJ_WEAPON_SS:
		m_eType._Hp -= 10.f;
		break;
	case Engine::OBJ_WEAPON_KARMA14:
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}


void CAncient_StonGolem::Set_AnimationState(STONGOLEMANIMSTATE eType)
{
	if (m_CurrAnim != m_PrevAnim)
	{
		switch (m_CurrAnim)
		{
		case Engine::S_ALL:
			break;
		case Engine::S_START:
			m_iAnimIndex = 1;
			break;
		case Engine::S_RESPAN: //  w 어디갔누 ㅋㅋ
			m_iAnimIndex = 2;
			break;
		case Engine::S_RUN:
			m_iAnimIndex = 3;
			break;
		case Engine::S_START2:
			m_iAnimIndex = 4;
			break;
		case Engine::S_WAIT:
			m_iAnimIndex = 5;
			break;
		case Engine::S_SKILL01:
			m_iAnimIndex = 6;
			break;
		case Engine::S_SKILL02:
			m_iAnimIndex = 7;
			break;
		case Engine::S_SKILL03_1:
			m_iAnimIndex = 8;
			break;
		case Engine::S_SKILL03_2:
			m_iAnimIndex = 9;
			break;
		case Engine::S_SKILL03_3:
			m_iAnimIndex = 10;
			break;
		case Engine::S_SKILL04_1:
			m_iAnimIndex = 11;
			break;
		case Engine::S_SKILL04_2:
			m_iAnimIndex = 12;
			break;
		case Engine::S_SKILL04_3:
			m_iAnimIndex = 13;
			break;
		case Engine::S_SKILL05_1:
			m_iAnimIndex = 14;
			break;
		case Engine::S_SKILL05_2:
			m_iAnimIndex = 15;
			break;
		case Engine::S_SKILL05_3:
			m_iAnimIndex = 16;
			break;
		case Engine::S_SKILL07:
			m_iAnimIndex = 17;
			break;
		case Engine::S_SKILL08:
			m_iAnimIndex = 18;
			break;
		case Engine::S_SKILL09:
			m_iAnimIndex = 19;
			break;
		case Engine::S_SKILL10_1:
			m_iAnimIndex = 20;
			break;
		case Engine::S_SKILL10_2:
			m_iAnimIndex = 21;
			break;
		case Engine::S_SKILL10_3:
			m_iAnimIndex = 22;
			break;
		case Engine::S_ANIMEND:
			break;
		default:
			break;
		
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);

		m_PrevAnim = m_CurrAnim;
	}

}

void CAncient_StonGolem::Set_State(_double TimeDelta)
{
	if (m_eType._Hp <= 0)
	{
		m_CurrAnim = S_SKILL10_1; // 원래는 스킬 모션인데 죽는게 따로 없어서 이걸로 대체 

		if (m_PrevAnim == S_SKILL01 && true == m_pModelCom->Get_AnimFinished())
			m_CurrAnim = S_SKILL02;

		if (m_PrevAnim == S_SKILL02 && true == m_pModelCom->Get_AnimFinished())
		{
			m_bDead = true;
			return;
		}
	}

	if (true == m_bPlayerChecck) //  플레이어와 특정 거리 이하로 들어와서 조우 하면
	{
		//m_pTransformCom->Turn(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION), TimeDelta);
		//m_pTransformCom->Chase(m_vTargetPos, TimeDelta, 3.f);
		m_bPlayerChecck = false;
		m_CurrAnim = S_START;
	}

	
	Set_Skill04(TimeDelta); // 조우 후 공격 패턴 1

	Set_Skill05(TimeDelta); // 패턴 1 이후 패턴 2
	//애니메이션이 모두 끝났다면 조건 추가 
}

void CAncient_StonGolem::Set_Skill04(_double TimeDelta)
{
	if (m_PrevAnim == S_START && true == m_pModelCom->Get_AnimFinished())
	{
		m_bCheck = true;
		m_bAttack = true;
		m_CurrAnim = S_SKILL04_1;
	}

	else if (m_PrevAnim == S_SKILL04_1 && true == m_pModelCom->Get_AnimFinished())
	{
		m_pModelCom->Set_AnimTick(m_f * (_double)5.f);
		cout << m_CurrAnim << endl;
		m_CurrAnim = S_SKILL04_2;
		// 애니메이션 끝나면 지형이 부서지는 이팩트 생성
	}
	//cout << m_CurrAnim << endl;

	else if (m_PrevAnim == S_SKILL04_2 && true == m_pModelCom->Get_AnimFinished())
	{
		// 이팩트 FadeIn,Out 후  삭제처리 
		cout << m_CurrAnim << endl;

		m_CurrAnim = S_SKILL04_3;
		m_bAttack = false;
	}

	//cout << m_CurrAnim << endl;
	if (false == m_bAttack && m_PrevAnim == S_SKILL04_3 && true == m_pModelCom->Get_AnimFinished())
		m_CurrAnim = S_WAIT;

}

void CAncient_StonGolem::Set_Skill05(_double TimeDelta)
{
	if (m_PrevAnim == S_WAIT && m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true;
		m_CurrAnim = S_SKILL05_1;
	}

	if (m_PrevAnim == S_SKILL05_1 && true == m_pModelCom->Get_AnimFinished())
		m_CurrAnim = S_SKILL05_2;

	if (m_PrevAnim == S_SKILL05_2 && true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = S_SKILL05_3;
		m_bAttack = false;
	}

	if (false == m_bAttack && m_PrevAnim == S_SKILL05_3 && true == m_pModelCom->Get_AnimFinished())
		m_CurrAnim = S_WAIT;

	
}

HRESULT CAncient_StonGolem::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss0"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAncient_StonGolem::SetUp_ShaderResources()
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

CAncient_StonGolem * CAncient_StonGolem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAncient_StonGolem*		pInstance = new CAncient_StonGolem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAncient_StonGolem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAncient_StonGolem::Clone(void * pArg)
{
	CAncient_StonGolem*		pInstance = new CAncient_StonGolem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAncient_StonGolem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAncient_StonGolem::Free()
{
	__super::Free();
}
