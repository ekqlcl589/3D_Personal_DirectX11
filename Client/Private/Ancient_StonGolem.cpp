#include "stdafx.h"
#include "..\Public\Ancient_StonGolem.h"
#include "MonsterWeapon.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Effect.h"
#include "Bone.h"

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

	if (FAILED(Add_Coll())) 
		return	E_FAIL;


	m_eCollisionState = OBJ_BOSS1;

	m_iObjID = 3;
	//ZeroMemory(&m_MonsterState, sizeof(MONSTERSTATE));
	//
	//memcpy(&m_MonsterState, pArg, sizeof m_MonsterState);

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	_float3 fPosition = { 10.f, 0.f, 20.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_eType._MaxHp = 1000.f;
	m_eType._Hp = 1000.f;

	m_f = m_pModelCom->Get_AnimTick();

	m_CurrAnim = S_WAIT;
	m_pModelCom->SetUp_Animation(m_CurrAnim);

	return S_OK;
}

void CAncient_StonGolem::Tick(_double TimeDelta)
{
	m_CoolTime += TimeDelta;
	if (m_CoolTime > 1.5f)
	{
			m_isColl = false;
			m_CoolTime = 0.0;
	}

	if (false == m_bDead)
	{
		if (m_eType._Hp <= 0.f)
			m_eType._Hp = 1.f;

		__super::Tick(TimeDelta);

		if (true == m_bTest)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 fPos;

			XMStoreFloat3(&fPos, vPos);

			fPos.x = Lerp(fPos.x, fPos.x - 0.01f, m_dLerpTime);
			fPos.y = Lerp(fPos.y, fPos.y - 0.f, m_dLerpTime);
			fPos.z = Lerp(fPos.z, fPos.z - 0.01f, m_dLerpTime);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));

		}

		Set_State(TimeDelta);

		Set_AnimationState(m_CurrAnim);

		for (_uint i = 0; i < WEAPON_END; i++)
		{
			for (auto& pWeapon : m_vecWeapon[i])
			{
				if (nullptr != pWeapon)
					pWeapon->Tick(TimeDelta);
			}
		}

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	}
	else
		return;
}

void CAncient_StonGolem::LateTick(_double TimeDelta)
{
	if (false == m_bDead)
	{
		__super::LateTick(TimeDelta);

		m_pModelCom->Play_Animation(TimeDelta);
		m_AnimDuration = m_pModelCom->Get_AnimDuration();
		m_AnimTimeAcc = m_pModelCom->Get_AnimTimeAcc();

		//Set_Time();

#ifdef _DEBUG

		//Collision_ToPlayer();

#endif
		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

			for (_uint i = 0; i < WEAPON_END; i++)
			{
				for (auto& pWeapon : m_vecWeapon[i])
				{
					if (nullptr != pWeapon)
						m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, pWeapon);
				}
			}

		}

		for (_uint i = 0; i < WEAPON_END; i++)
		{
			for (auto& pWeapon : m_vecWeapon[i])
			{
				if (nullptr != pWeapon)
					pWeapon->LateTick(TimeDelta);
			}
		}

	}
	else
		return;
}

HRESULT CAncient_StonGolem::Render()
{
	if (true == m_bDead)
		return OBJ_DEAD;

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

#ifdef _DEBUG

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

#endif

	return S_OK;
}

void CAncient_StonGolem::OnCollision(CGameObject * pObj)
{
	//pColl = &m_isColl;

	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	
	case Engine::OBJ_WEAPON_KARMA14:
		if (!m_isColl)
		{
			m_eType._Hp -= 50.f;
			cout << "칼 한테 맞아서 몬스터 체력 : " << m_eType._Hp << endl;
			m_isColl = true;
		}
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}

}


HRESULT CAncient_StonGolem::Add_Coll()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBoneL = m_pModelCom->Get_BonePtr("Bip001-L-Finger21");
	CBone* pBoneR = m_pModelCom->Get_BonePtr("Bip001-R-Finger21");
	CBone* pBoneSpine = m_pModelCom->Get_BonePtr("Bip001-Spine1");

	if (nullptr == pBoneL || nullptr == pBoneR || nullptr == pBoneSpine)
		return E_FAIL;

	CMonsterWeapon::WEAPONDESC WeaponDesc = { pBoneL, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_L };
	CMonsterWeapon::WEAPONDESC WeaponDesc1 = { pBoneR, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_R };
	CMonsterWeapon::WEAPONDESC WeaponDesc2 = { pBoneSpine, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_BODY };
	Safe_AddRef(pBoneL);
	Safe_AddRef(pBoneR);
	Safe_AddRef(pBoneSpine);

	CGameObject* pWeapon = pInstance->Clone_GameObject_Add_Layer(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc);
	CGameObject* pWeaponR = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc1);
	CGameObject* pWeaponSpine = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc2);

	if (nullptr == pWeapon || nullptr == pWeaponR || nullptr == pWeaponSpine)
		return E_FAIL;

	m_vecWeapon[WEAPON_MONSTERL].push_back(pWeapon);
	m_vecWeapon[WEAPON_MONSTERR].push_back(pWeaponR);
	m_vecWeapon[WEAPON_MONSTERBODY].push_back(pWeaponSpine);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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
		cout << "현재 애님 인덱스 :" << m_CurrAnim << endl;
		m_PrevAnim = m_CurrAnim;
	}

}

_uint CAncient_StonGolem::Set_State(_double TimeDelta)
{
	if (m_eType._Hp <= 1.f)
	{
		m_CurrAnim = S_SKILL10_1; // 원래는 스킬 모션인데 죽는게 따로 없어서 이걸로 대체 

		if (m_CurrAnim == S_SKILL10_1 && true == m_pModelCom->Get_AnimFinished())
		{
			m_eType._Hp = 0.f;
			m_bDead = true;
			return OBJ_DEAD;
		}
		if (m_PrevAnim == S_SKILL01 && true == m_pModelCom->Get_AnimFinished() && m_pModelCom->Get_LerpAnimFinished())
		{
			m_CurrAnim = S_SKILL02;
		}

		else if (m_PrevAnim == S_SKILL02 && true == m_pModelCom->Get_AnimFinished())
		{
			OnDead();
		}
	}


	if (true == m_bPlayerChecck) //  플레이어와 특정 거리 이하로 들어와서 조우 하면 지금은 가까운데 나중에 맵 찍으면 더 멀리서 이 기능 활성화 하고 카메라 몬스터로 이동
	{
		m_pTransformCom->Chase_Tatget(m_vTargetPos, 3.f, TimeDelta);
		m_CurrAnim = S_RUN;
	}
	else if (false == m_bPlayerChecck && false == m_bRespwan)
	{
		m_CurrAnim = S_RESPAN;
		_double TickPerSecond = 45.0;
		m_pModelCom->Set_AnimTick(TickPerSecond);
	}

	Run(TimeDelta);

	if (false == m_bSkill4)
	{
		Set_Skill04(TimeDelta); // 조우 후 공격 패턴 1
	}

	if (m_PrevAnim == S_SKILL04_2 && true != m_pModelCom->Get_AnimFinished())
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 5.0)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			m_pTransformCom->Chase_Tatget(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION), 1.f, TimeDelta);
			//vPos += m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			if (m_pModelCom->Get_AnimTimeAcc() >= 90.0)
			{
				vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 시발 이거 또 이대로 계속 고정 되는 건 아니겠지?
			}

		}

	}


	Set_Skill07(TimeDelta);
	
	// 란듐 ㄴㄴ 이러면 공격이 끝나지 않아서 나를 따라오지 않음 

	if (m_eType._Hp <= 500.f)
		Set_Skill01(TimeDelta); // 체력이 반 이하로 떨어지면 
	
	if(m_eType._Hp <= 250.f)
		Set_Skill05(TimeDelta); // 체력이 25% 이하로 떨어지면 몸을 웅크리면서 체력 회복 패턴 사용 
	////애니메이션이 모두 끝났다면 조건 추가 

	return OBJ_NOEVENT;
}

void CAncient_StonGolem::Run(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 자기 위치에 고정 
	// 5보다 멀 면 battack false -> 플레이어를 따라오게
	// 5 보다 가까우면 battack true -> 공격, 공격 중 플레이어가 멀어져도 일단 공격 모션은 끝 까지 
	// Chase to Target 후 첫 공격 패턴 4_1 이 후 사용 되는 함수 
	if (true == m_bCheck && false == m_bAttack) // 이게 밑에서 발동하는 란듐 공격 7, 9 때문에 조건이 안 먹을 가능성이 높음 
	{
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 10.f) // 타겟과의 거리가 10 보다 적으면 
		{
			m_pTransformCom->Chase_Tatget(m_vTargetPos, 5.f, TimeDelta);
			m_CurrAnim = S_RUN;
		}
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 5.f)
		{
			m_CurrAnim = S_WAIT; // 보간이 안 되고 넘어가는 느낌 
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); // 자기 위치에 고정 

		}
	}

	Set_Skill09(TimeDelta);

}

void CAncient_StonGolem::Set_Skill01(_double TimeDelta)
{
	if (m_PrevAnim == S_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true;
		m_CurrAnim = S_SKILL01;
	}

	else if (m_PrevAnim == S_SKILL01 && true == m_pModelCom->Get_LerpAnimFinished())
	{
		m_CurrAnim = S_SKILL02;
		m_bAttack = false;
	}

	if (false == m_bAttack && m_PrevAnim == S_SKILL02 && true == m_pModelCom->Get_LerpAnimFinished())
		m_CurrAnim = S_WAIT;
}

void CAncient_StonGolem::Set_Skill02(_double TimeDelta)
{
}

void CAncient_StonGolem::Set_Skill04(_double TimeDelta)
{
	if (m_PrevAnim == S_RESPAN && true == m_pModelCom->Get_AnimFinished())
	{
		m_bCheck = true;
		m_bRespwan = true;
		m_bAttack = true;
		m_bPlayerChecck = false;
		m_CurrAnim = S_SKILL04_1;
	}

	if (m_PrevAnim == S_SKILL04_1 && true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = S_SKILL04_2;

		if (m_pModelCom->Get_AnimTimeAcc() == (m_pModelCom->Get_AnimDuration() / 2))
			m_bjump = true;
		// 애니메이션 끝나면 지형이 부서지는 이팩트 생성
	}

	if (m_PrevAnim == S_SKILL04_2 && true == m_pModelCom->Get_AnimFinished())
	{
		// 이팩트 FadeIn,Out 후  삭제처리 
		m_AnimDuration;  
		m_AnimTimeAcc; // 이게 duration의 절반이 되면 플레이어를 향해 움직여야 하고 
		//한 80퍼 쯤 진행되면 움직임 멈추고 그자리에 찍혀 있어야 함 
		m_CurrAnim = S_SKILL04_3;
	}
	
	if (m_PrevAnim == S_SKILL04_3 && true == m_pModelCom->Get_AnimFinished())
	{
		m_bSkill4 = true;
		m_bAttack = false; // false로 한 번 돌려서 거리가 멀어졌다면 따라 오게 

		m_CurrAnim = S_WAIT;
	}

}

void CAncient_StonGolem::Set_Skill05(_double TimeDelta)
{
	if (m_PrevAnim == S_WAIT && m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true;

		m_CurrAnim = S_SKILL05_1;
	}

	else if (m_PrevAnim == S_SKILL05_1 && true == m_pModelCom->Get_AnimFinished())
		m_CurrAnim = S_SKILL05_2;

	else if (m_PrevAnim == S_SKILL05_2 && true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = S_SKILL05_3;
		m_bAttack = false;
	}

	if (false == m_bAttack && m_PrevAnim == S_SKILL05_3 && true == m_pModelCom->Get_AnimFinished())
		m_CurrAnim = S_WAIT;

	
}

void CAncient_StonGolem::Set_Skill07(_double TimeDelta)
{
	if (false == m_bAttack && m_PrevAnim == S_WAIT && m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true; // 여기서 다시 true를 줘서 거리가 멀어도 한 번 트루가 됐다면 나를 계속 따라오지 못하게 
		//m_bTest = true; // 차라리 7번이 앞으로 조금 이동 하는 
		m_CurrAnim = S_SKILL07;

	}

	if (m_PrevAnim == S_SKILL07 && true == m_pModelCom->Get_AnimFinished())
	{
		m_dLerpTime = 0.f;
		m_bTest = false;
		m_bAttack = false; // 다시 false;
		m_CurrAnim = S_WAIT;
	}
}

void CAncient_StonGolem::Set_Skill09(_double TimeDelta)
{

	if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 4.f)
	{
		if (false == m_bAttack &&  m_PrevAnim == S_WAIT && m_pModelCom->Get_AnimFinished())
		{
			m_bAttack = true;
			//m_bTest = true;
			m_CurrAnim = S_SKILL09;
		}

	}

	if (m_PrevAnim == S_SKILL09 && true == m_pModelCom->Get_AnimFinished())
	{
		m_dLerpTime = 0.f;
		m_bTest = false;
		m_bAttack = false;
		m_CurrAnim = S_WAIT;
	}

}

HRESULT CAncient_StonGolem::Add_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect"), TEXT("Layer_Effect"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CAncient_StonGolem::Set_Time()
{
	if (m_fTime <= m_AnimDuration)
		m_fTime -= m_AnimTimeAcc;

	if (m_fTime <= 2.0)
	{
		m_bAttackTime = true;
		m_fTime = m_AnimDuration;
	}
}

HRESULT CAncient_StonGolem::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 3.f;
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

	Safe_Release(m_pColliderCom);

	for (_uint i = 0; i < WEAPON_END; ++i)
	{
		for (auto& pWeapon : m_vecWeapon[i])
			Safe_Release(pWeapon);

		m_vecWeapon[i].clear();
	}

}
