#include "stdafx.h"
#include "..\Public\Ancient_StonGolem.h"
#include "MonsterWeapon.h"
#include "GameInstance.h"
#include "TargetCamera.h"
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

	_float3 fPosition = { 10.f, 0.f, 20.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_eType._MaxHp = 1000.f;
	m_eType._Hp = 100.f;

	m_f = m_pModelCom->Get_AnimTick();

	m_CurrAnim = S_RESPAN;
	m_pModelCom->SetUp_Animation(m_CurrAnim); 

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(195.0f));

	return S_OK;
}

void CAncient_StonGolem::Tick(_double TimeDelta)
{
	if (false == m_bDead)
	{
		__super::Tick(TimeDelta);

		Set_State(TimeDelta);

		Attack_Go(TimeDelta);

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
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

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
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	
	case Engine::OBJ_WEAPON_KARMA14:
		// 몬스터 자체의 콜리전을 없애서 맞을 일 없음 -> monsterweapon 클래스에서 히트 처리 
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}

}

void CAncient_StonGolem::EnterCollision(CGameObject * pObj)
{
}


HRESULT CAncient_StonGolem::Add_Coll()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBoneL = m_pModelCom->Get_BonePtr("Bip001-L-Finger21");
	CBone* pBoneR = m_pModelCom->Get_BonePtr("Bip001-R-Finger21");
	CBone* pBoneSpine = m_pModelCom->Get_BonePtr("Bip001-Spine1");

	if (nullptr == pBoneL || nullptr == pBoneR || nullptr == pBoneSpine)
		return E_FAIL;

	CMonsterWeapon::WEAPONDESC WeaponDesc = { pBoneL, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_L, CMonsterWeapon::OWNER_GOLEM };
	CMonsterWeapon::WEAPONDESC WeaponDesc1 = { pBoneR, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_R, CMonsterWeapon::OWNER_GOLEM };
	CMonsterWeapon::WEAPONDESC WeaponDesc2 = { pBoneSpine, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_BODY, CMonsterWeapon::OWNER_GOLEM };
	Safe_AddRef(pBoneL);
	Safe_AddRef(pBoneR);
	Safe_AddRef(pBoneSpine);

	CGameObject* pWeapon = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc);
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
		switch (eType)
		{
		case Engine::S_STANDUP_F:
			m_iAnimIndex = 22;
			break;
		case Engine::DOWN_F:
			m_iAnimIndex = 1;
			break;
		case Engine::RTDOWN_F:
			m_iAnimIndex = 4;
			break;
		case Engine::RTSTAND_END:
			m_iAnimIndex = 5;
			break;
		case Engine::RTSTAND_LOOP:
			m_iAnimIndex = 6;
			break;
		case Engine::RTSTAND_START:
			m_iAnimIndex = 7;
			break;
		case Engine::S_START:
			m_iAnimIndex = 23;
			break;
		case Engine::S_RESPAN: //  w 어디갔누 ㅋㅋ
			m_iAnimIndex = 3;
			break;
		case Engine::S_RUN:
			m_iAnimIndex = 8;
			break;
		case Engine::S_WAIT:
			m_iAnimIndex = 25;
			break;
		case Engine::S_SKILL01:
			m_iAnimIndex = 9;
			break;
		case Engine::S_SKILL02:
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
		case Engine::S_SKILL09:
			m_iAnimIndex = 18;
			break;
		case Engine::S_SKILL10_1: // 죽는 모션ㅇ로 사용
			m_iAnimIndex = 2;
			break;
		case Engine::S_SKILL10_2:
			m_iAnimIndex = 20;
			break;
		case Engine::S_SKILL10_3:
			m_iAnimIndex = 21;
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

void CAncient_StonGolem::Use_Skill(_double TimeDelta)
{
	int RandSkill = 0;
	
	RandSkill = rand() % 3;

	switch (RandSkill)
	{
	case 0:
		m_Skill1 = true;
		break;
	case 1:
		m_Skill2 = true;
		break;
	case 2:
		m_Skill7 = true;
		break;

	default:
		break;
	}
	m_SkillDelay = 80.f;
}

void CAncient_StonGolem::Combat_Wait(_double TimeDelta)
{
	if (m_PrevAnim == S_SKILL01 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 67.0) // 그리고 1번 애님 끝나면 바로 2번 실행 
	{
		m_CurrAnim = S_WAIT;
		m_bAttack = false;
	}

	if (m_PrevAnim == S_SKILL02 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 91.0) // 그리고 1번 애님 끝나면 바로 2번 실행 
	{
		m_CurrAnim = S_WAIT;
		m_bAttack = false;
	}

	if (m_PrevAnim == S_SKILL07 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 78.0) // 그리고 1번 애님 끝나면 바로 2번 실행 
	{
		m_CurrAnim = S_WAIT;
		m_bAttack = false;
	}

}

_uint CAncient_StonGolem::Set_State(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pCamera = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
	RELEASE_INSTANCE(CGameInstance);

	if (m_eType._Hp <= 1.f)
	{
		m_eType._Hp = 1.f;
		m_CurrAnim = S_SKILL10_1; // 원래는 스킬 모션인데 죽는게 따로 없어서 이걸로 대체 

		if (m_PrevAnim == S_SKILL10_1 && true == m_pModelCom->Get_AnimFinished())// m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 45.0)
		{
			m_eType._Hp = 0.f;
			m_bDead = true;
			//OnDead();
			return OBJ_DEAD;
		}
	}


	if (false == m_bSkill4)
	{
		Set_Skill04(TimeDelta); // 조우 후 공격 패턴 1
	}

	if (m_PrevAnim == S_SKILL04_2 && true != m_pModelCom->Get_AnimFinished())
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 5.0)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
			m_pTransformCom->Chase_Tatget(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION), 5.f, TimeDelta * 4.0);
			//vPos += m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	
			if (m_pModelCom->Get_AnimTimeAcc() >= 90.0)
			{
				vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 시발 이거 또 이대로 계속 고정 되는 건 아니겠지?
			}

			if (m_pModelCom->Get_AnimTimeAcc() >= 93.0 && m_pModelCom->Get_AnimTimeAcc() <= 95.0)
			{
				static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.7f, 0.4f);

			}
	
		}
	
	}

	if (m_PrevAnim == S_SKILL05_2 && true != m_pModelCom->Get_AnimFinished())
	{
		m_eType._Hp += TimeDelta * 50.f;
		cout << m_eType._Hp << endl;

		if (m_eType._Hp >= m_eType._MaxHp)
		{
			m_eType._Hp = m_eType._MaxHp;
			m_CurrAnim = S_SKILL05_3;
		}

		if (true == m_bDown)
			RT_Down();

	}

	if (m_PrevAnim == S_SKILL05_3 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 69.0)
	{
		m_CurrAnim = S_WAIT;
	}

	if(m_eType._Hp <= 250.f && !m_bSkill5)
		Set_Skill05(TimeDelta); // 체력이 25% 이하로 떨어지면 몸을 웅크리면서 체력 회복 패턴 사용 
	//else
	//{
		if (!m_bAttack && m_SkillDelay >= 0)
			--m_SkillDelay;
		else if (false == m_bRun && true == m_bSkill4 && !m_bAttack && m_SkillDelay < 0)
			Use_Skill(TimeDelta);
	//}

	Set_Skill01(TimeDelta);
	Set_Skill02(TimeDelta);
	Set_Skill07(TimeDelta);

	RT_Down();

	Run(TimeDelta);

	Combat_Wait(TimeDelta);

	return OBJ_NOEVENT;
}

void CAncient_StonGolem::Attack_Go(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pCamera = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
	RELEASE_INSTANCE(CGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 fPos;
	XMStoreFloat3(&fPos, vPosition);

	if (true == m_bAttack && m_PrevAnim == S_SKILL01)
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= 60.0 && m_pModelCom->Get_AnimTimeAcc() <= 65.0)
		{
			m_pTransformCom->Go_Straight(0.1 * TimeDelta);
			static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.5f, 0.1f);

		}
		if (m_pModelCom->Get_AnimTimeAcc() >= 87.0 && m_pModelCom->Get_AnimTimeAcc() <= 100.0)
		{
			m_pTransformCom->Go_Straight(0.05 * TimeDelta);
			static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.5f, 0.1f);
		}
	}

	if (true == m_bAttack && m_PrevAnim == S_SKILL07)
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= 10.0 && m_pModelCom->Get_AnimTimeAcc() <= 11.0)
		{
			static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.3f, 0.1f);
		}

		if (m_pModelCom->Get_AnimTimeAcc() >= 19.0 && m_pModelCom->Get_AnimTimeAcc() <= 20.0)
		{
			static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.3f, 0.1f);

		}
		if (m_pModelCom->Get_AnimTimeAcc() >= 35.0 && m_pModelCom->Get_AnimTimeAcc() <= 41.0)
		{
			m_pTransformCom->Go_Back(TimeDelta * 0.1);
		}

		if (m_pModelCom->Get_AnimTimeAcc() >= 68.0 && m_pModelCom->Get_AnimTimeAcc() <= 69.0)
		{
			static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.5f, 0.1f);
		}

		if (m_pModelCom->Get_AnimTimeAcc() >= 80.0 && m_pModelCom->Get_AnimTimeAcc() <= 82.0)
		{
			m_pTransformCom->Go_Straight(TimeDelta * 0.1);
		}

		if (m_pModelCom->Get_AnimTimeAcc() >= 107.0 && m_pModelCom->Get_AnimTimeAcc() <= 109.0)
		{
			static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.5f, 0.1f);
			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}
	}

	if (m_PrevAnim == S_SKILL09 && m_AnimTimeAcc >= 48.0 && m_AnimTimeAcc <= 49.0)
		static_cast<CTargetCamera*>(pCamera)->Add_Shaking(SHAKE_DIRECTION::RIGHT, 0.5f, 0.1f);


}

void CAncient_StonGolem::Run(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 자기 위치에 고정 
	if (true == m_bCheck && false == m_bAttack) // 무조건 4번 애니메이션 이후
	{
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 30.f) // 타겟과의 거리가 10 보다 적으면 
		{
			m_bRun = true;
			m_pTransformCom->Chase_Tatget(m_vTargetPos, 5.f, TimeDelta);
			m_CurrAnim = S_RUN; // 여기
		}
	}
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 5.f)
		{
			m_bRun = false;
			m_CurrAnim = S_WAIT; 
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); // 자기 위치에 고정 

		}

	Set_Skill09(TimeDelta);

}

void CAncient_StonGolem::Set_Skill01(_double TimeDelta)
{
	if (m_Skill1 && m_PrevAnim == S_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true;
		m_CurrAnim = S_SKILL01;
	}
}

void CAncient_StonGolem::Set_Skill02(_double TimeDelta)
{
	if (m_Skill2 && m_PrevAnim == S_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true;
		m_CurrAnim = S_SKILL02;
	}

}

void CAncient_StonGolem::Set_Skill04(_double TimeDelta)
{
	if (m_CurrAnim == S_RESPAN && true == m_pModelCom->Get_AnimFinished())
	{
		m_bCheck = true;
		m_bRespwan = true;
		m_bAttack = true;
		m_bPlayerChecck = false;
		m_CurrAnim = S_SKILL04_1;
	}

	if (m_CurrAnim == S_SKILL04_1 &&  m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 10.0)//true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = S_SKILL04_2;

		if (m_pModelCom->Get_AnimTimeAcc() == (m_pModelCom->Get_AnimDuration() / 2))
			m_bjump = true;
		// 애니메이션 끝나면 지형이 부서지는 이팩트 생성
	}

	if (m_CurrAnim == S_SKILL04_2 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 50.0)
	{
		m_CurrAnim = S_SKILL04_3;
	}
	
	if (m_PrevAnim == S_SKILL04_3 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 23.0)
	{
		m_bSkill4 = true;
		m_bAttack = false; // false로 한 번 돌려서 거리가 멀어졌다면 따라 오게 

		m_CurrAnim = S_WAIT;
	}

}

void CAncient_StonGolem::Set_Skill05(_double TimeDelta)
{
	if (m_PrevAnim == S_WAIT && true != m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true;

		m_CurrAnim = S_SKILL05_1;
	}

	if (m_PrevAnim == S_SKILL05_1 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 59.0)
	{
		m_CurrAnim = S_SKILL05_2;
	}

	if (m_PrevAnim == S_SKILL05_2 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 928.0)
	{
		m_CurrAnim = S_SKILL05_3;
		m_bAttack = false;
	}

	if (m_PrevAnim == S_SKILL05_3 && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 69.0)
	{
		m_bSkill5 = true;
		m_CurrAnim = S_WAIT;
	}
	
}

void CAncient_StonGolem::Set_Skill07(_double TimeDelta)
{
	if (m_Skill7 && m_PrevAnim == S_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_bAttack = true; // 여기서 다시 true를 줘서 거리가 멀어도 한 번 트루가 됐다면 나를 계속 따라오지 못하게 
		m_CurrAnim = S_SKILL07;
	}

}

void CAncient_StonGolem::Set_Skill09(_double TimeDelta)
{

	if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 4.f)
	{
		if (false == m_bAttack &&  m_PrevAnim == S_WAIT && true == m_pModelCom->Get_AnimFinished())
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

void CAncient_StonGolem::Down()
{
	// RT_DOWN 함수로 통합 
}

void CAncient_StonGolem::RT_Down() // 플레이어 쪽에서 불러준다? 지랄노.
{
	if(true == m_bDown)
		m_CurrAnim = RTDOWN_F;
	
	if (m_PrevAnim == RTDOWN_F && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 29.0)
	{
		m_CurrAnim = DOWN_F;
		m_bAttack = false;
	}
	

	if (m_PrevAnim == DOWN_F && true == m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 5.0)
	{
		m_CurrAnim = S_STANDUP_F;
	}

	if (m_PrevAnim == S_STANDUP_F && m_pModelCom->Get_AnimTimeAcc() >= (m_pModelCom->Get_AnimDuration() / 2) + 43.0)
		m_CurrAnim = S_WAIT;
}

void CAncient_StonGolem::Stand()
{
	float fHp = m_eType._Hp / m_eType._MaxHp;
	static int tmp = 0;
	if ((int(fHp * 10) == 8 || int(fHp * 10) == 6 || int(fHp * 10) == 2)
		&& tmp != int(fHp * 10))
	{
		tmp = int(fHp * 10);
		m_CurrAnim = RTSTAND_START;
	}

	if (m_PrevAnim == RTSTAND_START && true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = RTSTAND_LOOP;

	}

	if (m_PrevAnim == RTSTAND_LOOP && true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = RTSTAND_END;
	}

	if (m_PrevAnim == RTSTAND_END && true == m_pModelCom->Get_AnimFinished())
	{
		m_CurrAnim = S_WAIT;
	}

}

void CAncient_StonGolem::Dying()
{
	// 사실상 스킬 10번 애니메이션으로 돌려서 안 씀
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
