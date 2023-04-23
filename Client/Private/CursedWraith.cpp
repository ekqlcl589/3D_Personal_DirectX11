#include "stdafx.h"
#include "..\Public\CursedWraith.h"
#include "GameInstance.h"
#include "MonsterWeapon.h"
#include "GrudgeWraith.h"
#include "BlackBall.h"
#include "TSPlayer.h"

CCursedWraith::CCursedWraith(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CCursedWraith::CCursedWraith(const CCursedWraith & rhs)
	: CMonster(rhs)
{
}

HRESULT CCursedWraith::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursedWraith::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//if (FAILED(Add_Coll()))
	//	return	E_FAIL;

	_float3 fPosition = { -1.f, 0.f, -5.f }; // 임시 위치값
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_tInfo._MaxHp = 1000.f;
	m_tInfo._Hp = 1000.f;

	m_tInfo._Hit = false;
	m_tInfo.CurrAnim = CW_Start;
	m_pModelCom->SetUp_Animation(m_tInfo.CurrAnim);

	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(195.0f));

	m_eCollisionState = OBJ_BOSS1;
	m_iObjID = 3;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CCursedWraith::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		if (!m_bjump)
			m_pTransformCom->LookAt(m_vTargetPos);

		m_vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		//m_vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		Animation_State(TimeDelta);

		//Attack_Go(TimeDelta); // 애니메이션이 진행 되면서 앞으로 가야 하니까 중간이 맞다?

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

void CCursedWraith::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		Hit(TimeDelta);

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

HRESULT CCursedWraith::Render()
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

	//if (nullptr != m_pColliderCom)
	//	m_pColliderCom->Render();

#endif

	return S_OK;

}

void CCursedWraith::OnCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer = nullptr;
	pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Player"));
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
		m_bBlow = false;
		break;
	case Engine::OBJ_PLAYER_RAGESKILL:
		m_bBlow = false;
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CCursedWraith::EnterCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer = nullptr;
	pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Player"));
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
		if (m_bHit)
		{
			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			CGameObject* pTarget = nullptr;
			pTarget = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Player"));

			_bool m_PlayerRSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().rSkill;
			_bool m_PlayerFSkill = static_cast<CTSPlayer*>(pTarget)->Get_Info().fSkill;
			_bool m_PlayerRageSkill = static_cast<CTSPlayer*>(pTarget)->Get_RageDamageOn();

			if (true == m_PlayerRSkill)
				Damage = 30.f;
			else if (true == m_PlayerFSkill)
				Damage = 45.f;
			else if (true == m_PlayerRageSkill)
				Damage = 50.f;
			else
				Damage = 5.f;

			m_tInfo._Hp -= Damage;

			m_tInfo._Hit = true;
			//Hit(Damage);
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
		// 그러다가 그 공을 얘가 맞으면 기절상태 시전 그러기 위한 변수 하나 더 필요할 듯 
		if (m_bHit)
		{
			m_tInfo._Hp -= 10.f;
			m_bBlow = true;
		}
		break;
	case Engine::OBJ_END:
		break;
	case Engine::OBJ_PLAYER_RAGESKILL:
		if (m_bHit)
		{
			m_bBlow = true;
			m_tInfo._Hp -= 30.f;
			cout << "필살기 맞음" << endl;
		}
		break;

	default:
		break;
	}
}

void CCursedWraith::Animation_State(_double TimeDelta)
{
	if (m_tInfo._Hp <= 1.f)
	{
		m_tInfo._Hp = 1.f;
		m_tInfo.CurrAnim = CW_DIE;

		if (m_tInfo.PrevAnim == CW_DIE && m_AnimTimeAcc >= 1.0 && m_AnimTimeAcc <= 1.0)
		{
			CSoundMgr::GetInstance()->StopSound(SOUND_CURSEDWRAITH_VOICE);
			CSoundMgr::GetInstance()->SoundPlay(L"BV_Dead.wav", SOUND_CURSEDWRAITH_VOICE, 1.0f);
		}

		if (m_tInfo.PrevAnim == CW_DIE && true == m_pModelCom->Get_AnimFinished())
		{
			m_tInfo._Hp = 0.f;
			Set_Dead();
		}
	}

	if (m_tInfo.PrevAnim == CW_Start && m_AnimTimeAcc >= 148.0)
		m_tInfo.CurrAnim = CW_Wait;

	if (!m_bAttack && m_SkillDelay >= 0)
		--m_SkillDelay;
	else if (false == m_bWlak && false == m_tInfo._Hit && !m_bAttack && m_SkillDelay < 0)
		Use_Skill(TimeDelta);

	Skill01(TimeDelta);
	Skill02(TimeDelta);
	Skill03(TimeDelta);
	SummonsEffectMonster();
	if (m_tInfo._Hp <= 800.f)
	{
		Summons();
	}

	Avoid(TimeDelta); // 스킬을 쓰고 나면 무조건 한 번은 뒤로 도망 

	RTBlow(TimeDelta);

	//CombatWait(TimeDelta);

}

void CCursedWraith::Animation(CURSEDWRAITHSTATE eType)
{
	if (m_tInfo.CurrAnim != m_tInfo.PrevAnim)
	{
		switch (eType)
		{
		case Engine::CW_Air:
			m_iAnimIndex = 0;
			break;
		case Engine::CW_Avoid:
			m_iAnimIndex = 1;
			break;
		case Engine::CW_Run:
			m_iAnimIndex = 2;
			break;
		case Engine::CW_Start:
			m_iAnimIndex = 3;
			break;
		case Engine::CW_Wait:
			m_iAnimIndex = 4;
			break;
		case Engine::CW_Walk:
			m_iAnimIndex = 5;
			break;
		case Engine::CW_Down_F_F:
			break;
		case Engine::CW_DIE:
			m_iAnimIndex = 7;
			break;
		case Engine::CW_RTBLOW_AIR_F:
			m_iAnimIndex = 8;
			break;
		case Engine::CW_RTBLOW_AIR_FALL_F:
			m_iAnimIndex = 9;
			break;
		case Engine::CW_RBBLOW_AIR_LENDING:
			m_iAnimIndex = 10;
			break;
		case Engine::CW_RTBLOW_AIR_R:
			break;
		case Engine::CW_RTBLOW_DOWN_F:
			m_iAnimIndex = 12;
			break;
		case Engine::CW_RTCHASE_BIG_F:
			m_iAnimIndex = 13;
			break;
		case Engine::CW_RTDOWN_AIR_LENDING_F_F:
		case Engine::CW_RTDOWN_F_F:
			break;
		case Engine::CW_RTSTAND_BIG_F:
			m_iAnimIndex = 16;
			break;
		case Engine::CW_SKILL_01:
			m_iAnimIndex = 17;
			break;
		case Engine::CW_SKILL_02:
			m_iAnimIndex = 18;
			break;
		case Engine::CW_SKILL_03:
			m_iAnimIndex = 19;
			break;
		case Engine::CW_SKILL_04:
			m_iAnimIndex = 20;
			break;
		case Engine::CW_SKILL_05:
			m_iAnimIndex = 21;
			break;
		case Engine::CW_SKILL_07:
			m_iAnimIndex = 22;
			break;
		case Engine::CW_SKILL_09_1:
			m_iAnimIndex = 23;
			break;
		case Engine::CW_SKILL_09_2:
			m_iAnimIndex = 24;
			break;
		case Engine::CW_STANDUP_F:
			m_iAnimIndex = 25;
			break;
		case Engine::CW_STUN_LOOP:
			m_iAnimIndex = 26;
			break;
		case Engine::CW_ANIMEND:
			break;
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);
		m_tInfo.PrevAnim = m_tInfo.CurrAnim;
	}
}

void CCursedWraith::Avoid(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_bAvoid)
	{
		m_tInfo.CurrAnim = CW_Avoid;
		m_pTransformCom->Go_Back(0.9 * TimeDelta);

		if (m_tInfo.PrevAnim == CW_Avoid && m_AnimTimeAcc >= (m_AnimDuration / 2) + 2.0)
		{
			m_bAvoid = false;
			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_tInfo.CurrAnim = CW_Wait;
		}
	}
}

void CCursedWraith::Skill01(_double TimeDelta)
{
	if (m_Skill1 && m_tInfo.PrevAnim == CW_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = CW_SKILL_01;
		m_bAttack = true;
	}

	if (m_tInfo.PrevAnim == CW_SKILL_01 && m_AnimTimeAcc >= 35.5 && m_AnimTimeAcc <= 36.0)
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_CURSEDWRAITH_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"VO_MON_Wraith_Shout_01_A.ogg", SOUND_CURSEDWRAITH_EFFECT, 1.0f);

	}

	if (m_tInfo.PrevAnim == CW_SKILL_01 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 9.0 && m_AnimTimeAcc <= (m_AnimDuration / 2) + 10.0)
	{
		for (_uint i = 0; i < 7; i++)
		{
			CBlackBall::BALLDESC BallDesc;
			ZeroMemory(&BallDesc, sizeof(CBlackBall::BALLDESC));
			//m_vPosition == 몬스터 포지션
			// vPosition == 공 위치 
			_vector vPosition = m_vPosition; // 공의 위치는 몬스터의 위치

			_float3 fPos;
			XMStoreFloat3(&fPos, vPosition);

			_matrix matRotate = XMMatrixIdentity();
			_float Angle = XMConvertToRadians(45.f);

			matRotate = XMMatrixRotationY(Angle + i * Angle);

			vPosition = XMVector3TransformNormal(m_vPosition, matRotate); // 공의 위치는 몬스터 위치에 회전 매트릭스를 적용시킨 값
			CGameInstance* Pinstance = GET_INSTANCE(CGameInstance);

			CGameObject* pBall = nullptr;

			BallDesc.vPosition = XMLoadFloat3(&fPos);// vPosition; // 생성시킬 위치는 회전이 적용된 vPosition;
			BallDesc.fOriginPos = fPos;
			BallDesc.vLook = XMLoadFloat3(&fPos) - vPosition;// vPosition; // 방향은 몬스터 위치 - 내(공) 위치 
			BallDesc.eType = CBlackBall::TYPE_8;

			if (FAILED(Pinstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Effect_Ball"), TEXT("Layer_Monster_Effect"), &BallDesc)))
				return;

			RELEASE_INSTANCE(CGameInstance);
		}

	}

	if (m_tInfo.PrevAnim == CW_SKILL_01 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 59.0)
	{
		m_tInfo.CurrAnim = CW_Wait;
		m_bAttack = false;
		m_Skill1 = true;
		m_bAvoid = true;

	}
}

void CCursedWraith::Skill02(_double TimeDelta)
{
	if (m_Skill2 && m_tInfo.PrevAnim == CW_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = CW_SKILL_02;
		m_bAttack = true;
	}

	if (m_tInfo.PrevAnim == CW_SKILL_02 && m_AnimTimeAcc >= 30.5 && m_AnimTimeAcc <= 31.0)
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_CURSEDWRAITH_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"VO_MON_Wraith_Shout_01_B.ogg", SOUND_CURSEDWRAITH_EFFECT, 1.0f);

	}

	if (m_tInfo.PrevAnim == CW_SKILL_02 && m_AnimTimeAcc >= 60.5 && m_AnimTimeAcc <= 61.0)
	{
		for (_uint i = 0; i < 3; i++)
		{
			CBlackBall::BALLDESC BallDesc;
			ZeroMemory(&BallDesc, sizeof(CBlackBall::BALLDESC));

			_vector vPosition = m_vPosition; // 공의 위치는 몬스터의 위치
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float3 fPos;
			
			XMStoreFloat3(&fPos, m_vPosition); // 현재 자신의 포지션

			fPos.x = -3.f + i * 3.f; // 에 w값에 좌우 크기 대입
			//fPos.y = 1.5f;

			BallDesc.vPosition = XMLoadFloat3(&fPos); // 볼 들의 위치 
			XMStoreFloat3(&BallDesc.fOriginPos, m_vPosition); // 볼의 생성 위치 
			BallDesc.vLook = m_vTargetPos - XMLoadFloat3(&fPos); //방향 
			BallDesc.eType = CBlackBall::TYPE_3;

			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Effect_Ball"), TEXT("Layer_Monster_Effect"), &BallDesc)))
				return;

			RELEASE_INSTANCE(CGameInstance);
		}
	}

	if (m_tInfo.PrevAnim == CW_SKILL_02 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 67.0)
	{
		m_tInfo.CurrAnim = CW_Wait;
		m_bAttack = false;
		m_SkillNext = false;
		m_Skill2 = false;
		m_bAvoid = true;
	}

}

void CCursedWraith::Skill03(_double TimeDelta)
{
	if (m_Skill3 && m_tInfo.PrevAnim == CW_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = CW_SKILL_03;
	}

	if (m_tInfo.PrevAnim == CW_SKILL_03 && m_AnimTimeAcc >= 5.0 && m_AnimTimeAcc <= 6.0)
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_CURSEDWRAITH_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"VO_MON_Wraith_Common_01_A.ogg", SOUND_CURSEDWRAITH_EFFECT, 1.0f);

	}

	if (m_tInfo.PrevAnim == CW_SKILL_03 && m_AnimTimeAcc >= 145.0)
	{
		
		CBlackBall::BALLDESC BallDesc;
		ZeroMemory(&BallDesc, sizeof(CBlackBall::BALLDESC));

		BallDesc.vPosition = m_vPosition; // 생성 위치는 몬스터 위 LookAt 으로 생성될 때 플레이어 위치를 향해 날아감 
		BallDesc.eType = CBlackBall::TYPE_DDEBASI;
		
		XMStoreFloat3(&m_fTargetPos, BallDesc.vPosition);

		CGameObject* pMonster = nullptr;
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	
		if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Effect_Ball"), TEXT("Layer_Monster_Effect"), &BallDesc)))
			return ;
		
		RELEASE_INSTANCE(CGameInstance);

		m_tInfo.CurrAnim = CW_Wait;
		m_Skill3 = false;
		m_bAvoid = true;
	}
}

void CCursedWraith::Summons()
{

	if (!m_bSummons && m_tInfo.PrevAnim == CW_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = CW_SKILL_04;
	}

	if (m_tInfo.PrevAnim == CW_SKILL_04 && m_AnimTimeAcc >= 5.0 && m_AnimTimeAcc <= 6.0)
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_CURSEDWRAITH_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"VO_MON_Wraith_Common_01_G-2.ogg", SOUND_CURSEDWRAITH_EFFECT, 1.0f);

	}

	if (m_tInfo.PrevAnim == CW_SKILL_04 && m_AnimTimeAcc >= 145.0)
	{
		
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pMonster = nullptr;

		//pMonster = pInstance->Clone_GameObject_Add_Layer(TEXT("Prototype_GameObject_Monster2"));
		if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Monster2"), TEXT("Layer_Monster_JJol"))))
			return;

		RELEASE_INSTANCE(CGameInstance);

		m_tInfo.CurrAnim = CW_Wait;
		m_bSummons = true;
		m_bAvoid = true;

	}
}

HRESULT CCursedWraith::SummonsEffectMonster()
{
	if(m_SummonsMonster && m_tInfo.PrevAnim == CW_Wait && true == m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnim = CW_SKILL_07;

	if (m_tInfo.PrevAnim == CW_SKILL_07 && m_AnimTimeAcc >= 8.0 && m_AnimTimeAcc <= 9.0)
	{
		CSoundMgr::GetInstance()->StopSound(SOUND_CURSEDWRAITH_EFFECT);
		CSoundMgr::GetInstance()->SoundPlay(L"VO_MON_Wraith_Shout_02_C.ogg", SOUND_CURSEDWRAITH_EFFECT, 1.0f);

	}

	if (m_tInfo.PrevAnim == CW_SKILL_07 && m_AnimTimeAcc >= 98.5 && m_AnimTimeAcc <= 99.0)
	{
		
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pInstance->Add_GameObject(LEVEL_GAMEPLAY2, TEXT("Prototype_GameObject_Monster_Effect_Monster"), TEXT("Effect_Monster"), &m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);
	}
	
	if (m_tInfo.PrevAnim == CW_SKILL_07 && m_AnimTimeAcc >= 138.0)
	{
		m_tInfo.CurrAnim = CW_Wait;
		m_SummonsMonster = false;
	}

	return S_OK;
}

void CCursedWraith::RTBlow(_double TimeDelta)
{
	if (m_bBlow)
	{
		m_tInfo.CurrAnim = CW_RTSTAND_BIG_F;
		m_pTransformCom->Go_Back(0.5 * TimeDelta);
	}
	

	if (m_tInfo.PrevAnim == CW_RTSTAND_BIG_F && true == m_pModelCom->Get_AnimFinished())
	{
		m_bBlow = false;
		m_tInfo.CurrAnim = CW_Wait;
	}
}

void CCursedWraith::Use_Skill(_double TimeDelta)
{
	_uint RandSkill = 0;

	RandSkill = rand() % 4;

	switch (RandSkill)
	{
	case 0:
		m_Skill3 = true;
		break;

	case 1:
		m_Skill2 = true;
		break;

	case 2:
		m_Skill1 = true;
		break;

	case 3:
		m_SummonsMonster = true;
		break;

	default:
		break;
	}
	m_SkillDelay = 50.f;
}

void CCursedWraith::Use_Skill_Next(_double TimeDelta)
{
	if (m_tInfo.PrevAnim == CW_SKILL_02 && m_AnimTimeAcc >= m_AnimDuration / 2)
	{
		m_BallCreate = false;
		//BallCreate(7);
	}

	if (m_tInfo.PrevAnim == CW_SKILL_02 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 67.0)
	{
		m_tInfo.CurrAnim = CW_Wait;
		m_bAttack = false;
		m_SkillNext = false;
	}
}

void CCursedWraith::Hit(_double TimeDelta)
{
	if (m_tInfo._Hit)
	{

		m_pTransformCom->Go_Back(TimeDelta * 1.0);
		m_tInfo._Hit = false;
	}
		//m_tInfo.CurrAnim = CW_RTSTAND_BIG_F;

	if (m_tInfo.PrevAnim == CW_RTSTAND_BIG_F && true == m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnim = CW_Wait;
}

void CCursedWraith::CombatWait(_double TimeDelta)
{
	if (m_tInfo.PrevAnim == CW_RTBLOW_AIR_F && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = CW_RTBLOW_AIR_FALL_F;
	}

	if (m_tInfo.PrevAnim == CW_RTBLOW_AIR_FALL_F && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = CW_RTBLOW_DOWN_F;
	}

}

HRESULT CCursedWraith::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Model_Boss3"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursedWraith::SetUp_ShaderResources()
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

HRESULT CCursedWraith::Add_Coll()
{
	CGameInstance* pInstnace = GET_INSTANCE(CGameInstance);

	CBone* pBone = m_pModelCom->Get_BonePtr("Bip001");

	if (nullptr == pBone)
		return E_FAIL;

	CMonsterWeapon::WEAPONDESC WeaponDesc = { pBone, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_BODY, CMonsterWeapon::OWNER_WRAITH2 };
	Safe_AddRef(pBone);

	CGameObject* pSpine = pInstnace->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc);

	m_vecWeapon[WEAPON_MONSTERBODY].push_back(pSpine);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CCursedWraith * CCursedWraith::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCursedWraith*		pInstance = new CCursedWraith(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCursedWraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCursedWraith::Clone(void * pArg)
{
	CCursedWraith*		pInstance = new CCursedWraith(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCursedWraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCursedWraith::Free()
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
