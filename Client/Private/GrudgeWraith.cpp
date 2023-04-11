#include "stdafx.h"
#include "..\Public\GrudgeWraith.h"
#include "GameInstance.h"
#include "MonsterSickle.h"
#include "TSPlayer.h"
#include "TargetCamera.h"

CGrudgeWraith::CGrudgeWraith(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CGrudgeWraith::CGrudgeWraith(const CGrudgeWraith & rhs)
	: CMonster(rhs)
{
}

HRESULT CGrudgeWraith::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrudgeWraith::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Coll()))
		return	E_FAIL;

	_float3 fPosition = { 5.f, 0.f, 5.f }; // 임시 위치값
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_tInfo._MaxHp = 500.f;
	m_tInfo._Hp = 500.f;

	m_tInfo.CurrAnim = G_Wait;
	m_pModelCom->SetUp_Animation(m_tInfo.CurrAnim);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(195.0f));

	m_eCollisionState = OBJ_BOSS2;
	m_iObjID = 5;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	
	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CGrudgeWraith::Tick(_double TimeDelta)
{
	if (m_bDead)
	{
		return;

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pInstance->Dleate_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster2"))))
			return;

		RELEASE_INSTANCE(CGameInstance);

	}
	else
	{
		__super::Tick(TimeDelta);

		Animation_State(TimeDelta);

		Attack_Go(TimeDelta); // 애니메이션이 진행 되면서 앞으로 가야 하니까 중간이 맞다?

		Animation(m_tInfo.CurrAnim);

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
}

void CGrudgeWraith::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		m_pModelCom->Play_Animation(TimeDelta);
		m_AnimDuration = m_pModelCom->Get_AnimDuration();
		m_AnimTimeAcc = m_pModelCom->Get_AnimTimeAcc();

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
}

HRESULT CGrudgeWraith::Render()
{
	if (!m_bDead)
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

	#ifdef _DEBUG

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Render();

	#endif

		return S_OK;

	}
	return S_OK;
}

void CGrudgeWraith::OnCollision(CGameObject * pObj)
{

	COLLISIONSTATE eType = pObj->Get_ObjType();

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer = nullptr;
	pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_bHit = static_cast<CTSPlayer*>(pPlayer)->Get_Attack();

	RELEASE_INSTANCE(CGameInstance);

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	case Engine::OBJ_WEAPON_SS1:
		break;
	case Engine::OBJ_BOSS1:
		break;
	case Engine::OBJ_WEAPON_KARMA14:
		break;
	case Engine::OBJ_BOSS2:
		break;
	case Engine::OBJ_MONSTER_WEAPONL:
		break;
	case Engine::OBJ_MONSTER_WEAPONR:
		break;
	case Engine::OBJ_MONSTER_BODY:
		break;
	case Engine::OBJ_MONSTER_BALL:
		break;
	case Engine::OBJ_PLAYER_RAGESKILL:
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
	
}

void CGrudgeWraith::EnterCollision(CGameObject * pObj)
{

	COLLISIONSTATE eType = pObj->Get_ObjType();

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer = nullptr;
	pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_bHit = static_cast<CTSPlayer*>(pPlayer)->Get_Attack();

	RELEASE_INSTANCE(CGameInstance);

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	case Engine::OBJ_WEAPON_SS1:
		break;
	case Engine::OBJ_BOSS1:
		break;
	case Engine::OBJ_WEAPON_KARMA14:
	{
		if (true == m_bHit)// && false == m_bGod)
		{
			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pTarget = nullptr;
			pTarget = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

			_bool m_PlayerRSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rSkill;
			_bool m_PlayerFSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().fSkill;
			_bool m_PlayerRageSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rageSkill;

			if (true == m_PlayerRSkill)
				Damage = 30.f;
			else if (true == m_PlayerFSkill)
				Damage = 45.f;
			else if (true == m_PlayerRageSkill)
				Damage = 100.f;
			else
				Damage = 5.f;

			Hit(Damage);
			cout << "칼한테 맞음" << m_tInfo._Hp << endl;

			RELEASE_INSTANCE(CGameInstance)

		}
		break;
	}
	case Engine::OBJ_BOSS2:
		break;
	case Engine::OBJ_MONSTER_WEAPONL:
		break;
	case Engine::OBJ_MONSTER_WEAPONR:
		break;
	case Engine::OBJ_MONSTER_BODY:
		break;
	case Engine::OBJ_MONSTER_BALL:
		break;
	case Engine::OBJ_PLAYER_RAGESKILL:
		if (true == m_bHit)// && false == m_bGod)
		{
			Hit(100.f);
		}
		break;

	case Engine::OBJ_END:
		break;
	default:
		break;
	}
	
}

HRESULT CGrudgeWraith::Add_Coll()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBoneR = m_pModelCom->Get_BonePtr("Bip001-R-Finger01");

	if (nullptr == pBoneR)// || nullptr == pBoneSpine)
		return E_FAIL;

	CMonsterSickle::WEAPONDESC WeaponDesc1 = { pBoneR, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterSickle::WEAPON_MONSTER_SICKLE };
	Safe_AddRef(pBoneR);

	CGameObject* pWeaponR = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon2"), &WeaponDesc1);

	if (nullptr == pWeaponR)// || nullptr == pWeaponSpine)
		return E_FAIL;

	m_vecWeapon[WEAPON_MONSTERR].push_back(pWeaponR);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGrudgeWraith::Add_Particle()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Effect_Wraith_Particle"), TEXT("Wraith_Particle"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CGrudgeWraith::Animation_State(_double TimeDelta)
{
	if (m_tInfo._Hp <= 1.f)
	{
		m_tInfo._Hp = 1.f;
		m_tInfo.CurrAnim = G_DIE;

		if (m_tInfo.PrevAnim == G_DIE && true == m_pModelCom->Get_AnimFinished())
		{
			m_tInfo._Hp = 0.f;
			Set_Dead();
		}
	}

	if (!m_bAttack && m_SkillDelay >= 0)
		--m_SkillDelay;
	else if (false == m_bWlak && !m_bAttack && m_SkillDelay < 0)
		Use_Skill(TimeDelta);

	if (m_tInfo._Hp <= 250.f)
		m_bSkill3 = true; // 이펙트 
	
	Skill01(TimeDelta);
	Skill05(TimeDelta);
	Skill02(TimeDelta);

	Combat_Wait(TimeDelta);

	Run(TimeDelta);

	if (m_tInfo.PrevAnim == G_RTStand_Big_F && m_AnimTimeAcc >= 28.0)
	{
		m_tInfo.CurrAnim = G_Wait;
		m_bAttack = false;
	}
}

void CGrudgeWraith::Animation(WRAITHSTATE eType)
{
	if (m_tInfo.CurrAnim != m_tInfo.PrevAnim)
	{
		switch (eType)
		{
		case Engine::G_Air:
			m_iAnimIndex = 0;
			break;
		case Engine::G_Avoid:
			m_iAnimIndex = 1;
			break;
		case Engine::G_Down_B_B:
			m_iAnimIndex = 2;
			break;
		case Engine::G_Down_F_F:
			m_iAnimIndex = 3;
			break;
		case Engine::G_DIE:
			m_iAnimIndex = 4;
			break;
		case Engine::G_RTDown_B_B:
			m_iAnimIndex = 5;
			break;
		case Engine::G_RTDown_F_F:
			m_iAnimIndex = 6;
			break;
		case Engine::G_RTStand_Air_Big_B:
			m_iAnimIndex = 7;
			break;
		case Engine::G_RTStand_Air_Big_F:
			m_iAnimIndex = 8;
			break;
		case Engine::G_RTStand_Air_Fail:
			m_iAnimIndex = 9;
			break;
		case Engine::G_RTStand_Air_Lending:
			m_iAnimIndex = 10;
			break;
		case Engine::G_RTStand_Big_F:
			m_iAnimIndex = 11;
			break;
		case Engine::G_Run:
			m_iAnimIndex = 12;
			break;
		case Engine::G_Skill01_1:
			m_iAnimIndex = 13;
			break;
		case Engine::G_Skill01_2:
			m_iAnimIndex = 14;
			break;
		case Engine::G_Skill01_3:
			m_iAnimIndex = 15;
			break;
		case Engine::G_Skill02_1:
			m_iAnimIndex = 16;
			break;
		case Engine::G_Skill02_2:
			m_iAnimIndex = 17;
			break;
		case Engine::G_Skill02_3:
			m_iAnimIndex = 18;
			break;
		case Engine::G_Skill03_1:
			m_iAnimIndex = 19;
			break;
		case Engine::G_Skill03_2:
			m_iAnimIndex = 20;
			break;
		case Engine::G_Skill03_3:
			m_iAnimIndex = 21;
			break;
		case Engine::G_Skill04_1:
			m_iAnimIndex = 22;
			break;
		case Engine::G_Skill04_2:
			m_iAnimIndex = 23;
			break;
		case Engine::G_Skill04_3:
			m_iAnimIndex = 24;
			break;
		case Engine::G_Skill05_1:
			m_iAnimIndex = 25;
			break;
		case Engine::G_Skill07_1:
			m_iAnimIndex = 26;
			break;
		case Engine::G_StandUp_B:
			m_iAnimIndex = 27;
			break;
		case Engine::G_StandUp_F:
			m_iAnimIndex = 28;
			break;
		case Engine::G_Stun_Loop:
			m_iAnimIndex = 29;
			break;
		case Engine::G_Wait:
			m_iAnimIndex = 30;
			break;
		case Engine::G_Walk:
			m_iAnimIndex = 31;
			break;
		case Engine::G_END:
			break;
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);
		m_tInfo.PrevAnim = m_tInfo.CurrAnim;
	}
}

void CGrudgeWraith::Use_Skill(_double TimeDelta)
{
	_uint RandSkill = 0;

	RandSkill = rand() % 3;

	switch (RandSkill)
	{
	case 0:
		m_bSkill1 = true;
		break;
	case 1:
		m_bSkill5 = true;
		break;
	case 2:
		m_bSkill2 = true;
		break;
	default:
		break;
	}
	m_SkillDelay = 50.f;
}

void CGrudgeWraith::Use_Skill_Next(_double TimeDelta)
{

	//if (m_Skill1Pair)
	//{
	//	int RandSkill = 0; // use_skill을 통해 부르게 되면 한번 더 스위치 문으로 다음 모션을 부르는 건 안 될 듯
	//
	//	RandSkill = rand() % 2;
	//
	//	switch (RandSkill)
	//	{
	//	case 0:
	//		Skill01_1();
	//		break;
	//
	//	case 1:
	//		Skill01_2();
	//		break;
	//
	//	default:
	//		break;
	//	}
	//
	//}

}

void CGrudgeWraith::Combat_Wait(_double TimeDelta)
{
	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 47.0)
	{
		m_tInfo.CurrAnim = G_Skill01_2;
		m_bSkill1 = false;
		m_Skill1Pair = false;
	}

	if (m_tInfo.PrevAnim == G_Skill01_2 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 36.0)
	{
		m_bAttack = false;
		m_bSkill1 = false;
		m_Skill1Pair = false;
		m_tInfo.CurrAnim = G_Wait;
	}

	if (m_tInfo.PrevAnim == G_Skill01_3 && m_AnimTimeAcc >= 81.0)
	{
		m_bAttack = false;
		m_bSkill1 = false;
		m_Skill1Pair = false;
		m_tInfo.CurrAnim = G_Wait;
	}

	// 이 위치에 스킬 2_2 부터 있었음 
	if (m_tInfo.PrevAnim == G_Skill03_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 27.5)
	{
		m_tInfo.CurrAnim = G_Skill03_2;
	}

	if (m_tInfo.PrevAnim == G_Skill03_2 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 13.5)
	{
		m_tInfo.CurrAnim = G_Skill03_3;
	}

	if (m_tInfo.PrevAnim == G_Skill03_3 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 29.5)
	{
		m_tInfo.CurrAnim = G_Wait;
		m_bAttack = false;
		m_SkillNext = false;
	}

	if (m_tInfo.PrevAnim == G_Skill05_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 46.0)
	{
		m_tInfo.CurrAnim = G_Wait;
		m_bAttack = false;
		m_bGod = false;
	}

}

void CGrudgeWraith::Attack_Go(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) - 3.5)
	{
		m_pTransformCom->Go_Straight(0.3 * TimeDelta);
	}

	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 1.5)
		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 19.5)
	{
		m_pTransformCom->Go_Straight(0.3 * TimeDelta);

	}

	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 31.0)
		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if(m_tInfo.PrevAnim == G_Skill01_3 && m_AnimTimeAcc >= 19.0)
		m_pTransformCom->Go_Straight(0.5 * TimeDelta);

	if (m_tInfo.PrevAnim == G_Skill01_3 && m_AnimTimeAcc >= 40.0)
		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CGrudgeWraith::Run(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (false == m_bAttack)
	{
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 30.f)
		{
			m_bWlak = true;
			m_tInfo.CurrAnim = G_Walk;
			m_pTransformCom->Chase_Tatget(m_vTargetPos, 5.f, TimeDelta);
		}
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 5.f)
		{
			m_bWlak = false;
			m_tInfo.CurrAnim = G_Wait;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
}

void CGrudgeWraith::Hit(const _int & _Damage)
{
	m_tInfo._Hp -= _Damage;
	//m_tInfo.CurrAnim = G_RTStand_Big_F;
	//m_bAttack = false;
}

void CGrudgeWraith::Skill01(_double TimeDelta)
{
	if (m_bSkill1)
	{
		m_tInfo.CurrAnim = G_Skill01_1;
		m_bAttack = true;
		m_SkillNext = false;
		m_Skill1Pair = true;
	}
}

void CGrudgeWraith::Skill01_1()
{
	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 48.0)
	{
		m_tInfo.CurrAnim = G_Skill01_2;
		m_Skill1Pair = false;
	}

}

void CGrudgeWraith::Skill01_2()
{
	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 48.0)
	{
		m_tInfo.CurrAnim = G_Skill01_3;
		m_Skill1Pair = false;
	}

}

void CGrudgeWraith::Skill02(_double TimeDelta)
{
	if (m_bSkill2 && !m_bAttack)
	{
		m_tInfo.CurrAnim = G_Skill02_1;
		m_bAttack = true;
		m_SkillNext = true;
		m_isParticle = true;
		Add_Particle();
	}

	if (m_tInfo.PrevAnim == G_Skill02_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 19.0)
	{
		m_tInfo.CurrAnim = G_Skill02_2;
	}

	if (m_tInfo.PrevAnim == G_Skill02_2 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 14.0)
	{
		m_tInfo.CurrAnim = G_Skill02_3;
	}

	if (m_tInfo.PrevAnim == G_Skill02_3 && m_AnimTimeAcc >= 5.0 && m_AnimTimeAcc <= 6.0)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pCamera = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));

		m_isParticle = false;
		if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Effect_Wraith_Attack"), TEXT("Wraith_Effect"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
			return ;

		RELEASE_INSTANCE(CGameInstance);

		static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.7f, 0.1f);
	}

	if (m_tInfo.PrevAnim == G_Skill02_3 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 33.0)
	{
		m_tInfo.CurrAnim = G_Wait;
		m_bAttack = false;
		m_SkillNext = false;
		m_bSkill2 = false;
	}

}

void CGrudgeWraith::Skill03(_double TimeDelta)
{
	m_tInfo.CurrAnim = G_Skill03_1;
	m_bAttack = true;
	m_SkillNext = true;
}

void CGrudgeWraith::Skill04(_double TimeDelta)
{
	if (m_tInfo.PrevAnim == G_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = G_Skill04_1;
		m_bAttack = true;
	}

	if (m_tInfo.PrevAnim == G_Skill04_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 12)
	{
		m_tInfo.CurrAnim = G_Skill04_2;
	}

	if (m_tInfo.PrevAnim == G_Skill04_2 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 8.5)
	{
		m_tInfo.CurrAnim = G_Skill04_3;
	}

	if (m_tInfo.PrevAnim == G_Skill04_3 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 26)
	{
		m_tInfo.CurrAnim = G_Wait;
		m_bAttack = false;
	}
	// 앞으로 이동 해야 함, 모션 이상함 고쳐야 함
}

void CGrudgeWraith::Skill05(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 fPosition;

	XMStoreFloat3(&fPosition, vPosition);

	if (m_bSkill5 && true == m_pModelCom->Get_AnimFinished()) // 그냥 현재 애니메이션이 종료되면?
	{
		m_tInfo.CurrAnim = G_Skill05_1;
		m_bAttack = true;
		m_bGod = true;
	}

	if (m_tInfo.PrevAnim == G_Skill05_1 && m_AnimTimeAcc >= 42.0 && m_AnimTimeAcc <= 43.0)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pBlade = nullptr;

		if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Monster_Blade"), TEXT("Layer_Monster_Blade_Effect"), &fPosition)))
			return;

		RELEASE_INSTANCE(CGameInstance);

	}
}

void CGrudgeWraith::Skill07(_double TimeDelta)
{
	if (m_tInfo.PrevAnim == G_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = G_Skill07_1;
		m_bAttack = true;
	}

	if (m_tInfo.PrevAnim == G_Skill07_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 53.5)
	{
		m_tInfo.CurrAnim = G_Wait;
		m_bAttack = false;
	}

}

HRESULT CGrudgeWraith::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss2"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider1"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CGrudgeWraith::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CGrudgeWraith * CGrudgeWraith::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGrudgeWraith*		pInstance = new CGrudgeWraith(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGrudgeWraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGrudgeWraith::Clone(void * pArg)
{
	CGrudgeWraith*		pInstance = new CGrudgeWraith(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGrudgeWraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrudgeWraith::Free()
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
