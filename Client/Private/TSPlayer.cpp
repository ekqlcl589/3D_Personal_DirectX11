#include "stdafx.h"
#include "..\Public\TSPlayer.h"

#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Hair.h"
#include "PlayerTop.h"
#include "TwoHandedSword.h"

#include "TargetCamera.h"

CTSPlayer::CTSPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTSPlayer::CTSPlayer(const CTSPlayer & rhs)
	: CGameObject(rhs)
	, m_tInfo(rhs.m_tInfo)
{
}

HRESULT CTSPlayer::Initialize_Prototype()
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

	m_tInfo.CurrAnim = TS_WAIT;
	m_tInfo.PrevAnim = TS_END;
	m_bJump = false;


	return S_OK;
}

HRESULT CTSPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Parts()))
		return E_FAIL;

	if (FAILED(Add_Weapon()))
		return	E_FAIL;

	m_eCollisionState = OBJ_PLAYER;

	m_iObjID = 0;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CTSPlayer::Tick(_double TimeDelta)
{
	m_HitDelay += TimeDelta;
	if (m_HitDelay >= 1.5f)
	{
		m_Hit = false;
		m_HitDelay = 0.0;
	}

	__super::Tick(TimeDelta);

	Key_Input(TimeDelta);

	Dash(TimeDelta);

	Animation(m_tInfo.CurrAnim, TimeDelta);


	for (_uint i = 0; i < WEAPON_END; i++)
	{
		for (auto& pWeapon : m_vecWeapon[i])
		{
			if (nullptr != pWeapon)
				pWeapon->Tick(TimeDelta);
		}
	}

	for (_uint i = 0; i < PART_END; i++)
	{
		for (auto& pPart : m_vecParts[i])
		{
			if (nullptr != pPart)
				pPart->Tick(TimeDelta);
		}
	}

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CTSPlayer::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

	m_AnimDuration = m_pModelCom->Get_AnimDuration();

	m_AnimTimeAcc = m_pModelCom->Get_AnimTimeAcc();

	m_AnimTickPerSecond = m_pModelCom->Get_AnimTick();

	for (_uint i = 0; i < WEAPON_END; i++)
	{
		for (auto& pWeapon : m_vecWeapon[i])
		{
			if (nullptr != pWeapon)
				pWeapon->LateTick(TimeDelta);
		}
	}

	for (_uint i = 0; i < PART_END; i++)
	{
		for (auto& pPart : m_vecParts[i])
		{
			if (nullptr != pPart)
				pPart->LateTick(TimeDelta);
		}
	}

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

		for (_uint i = 0; i < PART_END; i++)
		{
			for (auto& pPart : m_vecParts[i])
			{
				if (nullptr != pPart)
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, pPart);
			}
		}

	}


}

HRESULT CTSPlayer::Render()
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

void CTSPlayer::OnCollision(CGameObject * pObj) // LateTick에서 불림
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_PLAYER:
		break;
	case Engine::OBJ_WEAPON_SS:
		break;
	case Engine::OBJ_WEAPON_SS1:
		break;
	case Engine::OBJ_BOSS1:
		if (!m_Hit)
		{
			Hit(10); // 몬스터 공격력 가져와서 대입 
			m_Hit = true;
			cout << "지웠던 obj_boss 한테 히트" << endl;

		}

		break;
	case Engine::OBJ_WEAPON_KARMA14:
		break;
	case Engine::OBJ_BOSS2:
		break;
	case Engine::OBJ_MONSTER_WEAPONL:
	
		if (!m_Hit)
		{
			Hit(10); // 몬스터 공격력 가져와서 대입 
			m_Hit = true;
			cout << "왼 팔 히트" << endl;
		}
		break;
	
	case Engine::OBJ_MONSTER_WEAPONR:
	{
		if (!m_Hit)
		{
			Hit(10); // 몬스터 공격력 가져와서 대입 
			m_Hit = true;
			cout << "오른 팔 히트" << endl;

		}
		break;
	}
	case Engine::OBJ_MONSTER_BODY:
	
		if (!m_Hit)
		{
			Damage(10); // 몬스터 공격력 가져와서 대입 
			m_Hit = true;
			cout << "몸통 히트" << endl;

		}

		break;
	
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CTSPlayer::Key_Input(_double TimeDelta)
{

	_vector vPos;
	vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_W))
	{
		m_bTest = false;

		m_pTransformCom->Go_Straight(TimeDelta);
		m_tInfo.CurrAnim = TS_RUN_END;
		//m_tInfo.CurrAnimState = ANIM_RUN;

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_W))
	{
		m_bTest = false;

		m_tInfo.CurrAnim = TS_WAIT;
		//m_tInfo.CurrAnimState = ANIM_IDEL;
	}

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_S))
	{
		m_bTest = false;

		m_pTransformCom->Go_Back(TimeDelta);
		m_tInfo.CurrAnim = TS_RUN_END;
		//m_tInfo.CurrAnimState = ANIM_RUN;

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_S))
	{
		m_bTest = false;

		m_tInfo.CurrAnim = TS_WAIT;

		//m_tInfo.CurrAnimState = ANIM_IDEL;
	}

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_LSHIFT))
	{
		m_bDeah = true;

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_LSHIFT))
		m_bDeah = false;

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_A))
	{
		m_bTest = false;

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -1.f);
		//m_tInfo.CurrAnimState = ANIM_RUN_L;
	}
	//else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_LEFT))
	//	m_tInfo.CurrAnimState = ANIM_RUN_END;


	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_D))
	{
		m_bTest = false;

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
		//m_tInfo.CurrAnimState = ANIM_RUN_R;
	}
	//if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_RIGHT))
	//	m_tInfo.CurrAnimState = ANIM_RUN_END;

	//Attack_Combo(TimeDelta);

	if (false == m_bJump)
	{
		if (false == m_bDeah)
		{
			if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB)) // 점프중이 아니고 대시 상태가 아니라면 
				Attack_Combo(TimeDelta);
		}
		else
		{
			if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB))// 점프중이 아니고 대시 상태라면 
				DashAttack(TimeDelta);
		}

		if (m_AttackCheck)
		{
			if (m_ComboCheck2 && CKeyMgr::GetInstance()->Mouse_Pressing(DIMK_RB))
			{
				Attack_Special2(TimeDelta);
			}
			else if (m_ComboCheck2 && CKeyMgr::GetInstance()->Mouse_Up(DIMK_RB))
			{
				m_ComboCheck2 = false;
				Attack_Special2(TimeDelta);

			}

			if (m_ComboCheck && CKeyMgr::GetInstance()->Mouse_Pressing(DIMK_RB))
			{
				Attack_Special(TimeDelta);
			}
			else if (m_ComboCheck && CKeyMgr::GetInstance()->Mouse_Up(DIMK_RB))
			{
				m_ComboCheck = false;
				Attack_Special(TimeDelta);

			}
		}

	}
	else
	{
		if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB)) // 점프중일 때 
		{
			m_JumpAttack = true;
			Jump_Attack(TimeDelta);
		}
	}

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_SPACE))
	{
		m_bJump = true;
		m_bSIbal = true;

	}

	CombatWait();

#pragma region SkillMotion Test 

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_E))
		m_pModelCom->SetUp_Animation(39); // Start_End 나눠져 있음 
	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_R))
		m_pModelCom->SetUp_Animation(43); // 애는 단일로 쓰기에는 스킬이라는 느낌이 안 남
	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_F))
		m_pModelCom->SetUp_Animation(46); // 공중에서 쓰는 스킬인가;

#pragma endregion 


	Jump(TimeDelta);

}

void CTSPlayer::Animation_State(PLAYERANIMSTATE eType, _double TimeDelta)
{
	if (m_tInfo.CurrAnimState != m_tInfo.prevAnimState )
	{
		switch (m_tInfo.CurrAnimState)
		{
		case ANIM_IDEL:
			m_iAnimIndex = 19;
			break;

		case ANIM_RUN:
			m_iAnimIndex = 9;
			break;

		case ANIM_RUN_L:
			m_iAnimIndex = 6;
			break;

		case ANIM_RUN_R:
			m_iAnimIndex = 9;
			break;

		case ANIM_RUN_END:
			m_iAnimIndex = 12;
			break;

		case ANIM_ATTACK:
			m_iAnimIndex = 26;
			break;

		case ANIM_ATTACK_COMBO1:
			m_iAnimIndex = 59;
			break;

		case ANIM_ATTACK_COMBO2:
			m_iAnimIndex = 60;
			break;

		case ANIM_ATTACK_COMBO3:
			m_iAnimIndex = 61;
			break;

		case ANIM_COMBAT_WAIT:
		{
			m_iAnimIndex = 5;
			break;
		}

		case ANIM_JUMP:
			m_iAnimIndex = 4;
			break;
		case ANIM_JUMP_ING:
			break;
		case ANIM_JUMP_LENDING:
			m_iAnimIndex = 2;
			break;
		case ANIM_JUMP_ATTACK1:
			m_iAnimIndex = 22;
			break;
		case ANIM_JUMP_ATTACK2:
			m_iAnimIndex = 23;
			break;
		case ANIM_JUMP_ATTACK3:
			m_iAnimIndex = 24;
			break;

		case ANIM_STUN:
			m_iAnimIndex = 16;
			break;

		case ANIM_SKILL_E:
			m_iAnimIndex = 38;
			break;

		case ANIM_SKILL_F:
			m_iAnimIndex = 42;
			break;

		case ANIM_SKILL_R:
			m_iAnimIndex = 43;
			break;

		case ANIM_END:
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);

		m_tInfo.prevAnimState = m_tInfo.CurrAnimState;
	}

}

void CTSPlayer::Animation(TSPLAYERANIM eType, _double TimeDelta)
{
	if (m_tInfo.CurrAnim != m_tInfo.PrevAnim)
	{
		switch (eType)
		{
		case Engine::TS_JUMP:
			m_iAnimIndex = 4;
			break;
		case Engine::TS_JUMP_DOWN:
			break;
		case Engine::TS_JUMP_LENDING:
			m_iAnimIndex = 2;
			break;
		case Engine::TS_JUMP_UP:
			break;
		case Engine::TS_JUMP_START:
			break;
		case Engine::TS_COMBAT_WAIT:
			m_iAnimIndex = 5;
			break;
		case Engine::TS_DASH:
			m_iAnimIndex = 6;
			break;
		case Engine::TS_DIE:
			break;
		case Engine::TS_RESPWAN:
			break;
		case Engine::TS_RUN_L:
			m_iAnimIndex = 6;
			break;
		case Engine::TS_RUN_R:
			break;
		case Engine::TS_RUN_END:
			m_iAnimIndex = 9;
			break;
		case Engine::TS_STANSUP_ROLLING:
			break;
		case Engine::TS_STANDUP_ATTACK:
			break;
		case Engine::TS_START:
			break;
		case Engine::TS_STURN_LOOP:
			break;
		case Engine::TS_NOENERGY:
			break;
		case Engine::TS_NOENERGY_RUN:
			break;
		case Engine::TS_WAIT:
			m_iAnimIndex = 19;

			break;
		case Engine::TS_WAIT_HABBIT:
			break;
		case Engine::TS_WALK:
			break;
		case Engine::TS_AIR_COMBO01:
			m_iAnimIndex = 22;
			break;
		case Engine::TS_AIR_COMBO02:
			m_iAnimIndex = 23;
			break;
		case Engine::TS_AIR_COMBO03:
			m_iAnimIndex = 24;
			break;
		case Engine::TS_AIR_COMBO04:
			m_iAnimIndex = 25;
			break;
		case Engine::TS_AIR_COMBO04_LENDING:
			m_iAnimIndex = 26;
			break;
		case Engine::TS_AVOID_ATTACK:
			m_iAnimIndex = 27;
			break;
		case Engine::TS_BASIC_COMBO02_END:
			m_iAnimIndex = 28;
			break;
		case Engine::TS_BASIC_COMBO02_LOOP:
			m_iAnimIndex = 29;
			break;
		case Engine::TS_BASIC_COMBO03_START:
			m_iAnimIndex = 30;
			break;
		case Engine::TS_DASHCOMBO:
			m_iAnimIndex = 31;
			break;
		case Engine::TS_SKILL_GROUNDCRASH:
			m_iAnimIndex = 32;
			break;
		case Engine::TS_SKILL_OUTRAGE_END:
			m_iAnimIndex = 38;
			break;
		case Engine::TS_SKILL_OUTRAGE_START:
			m_iAnimIndex = 39;
			break;
		case Engine::TS_SKILL_PRIERCINGRUSH_START_END:
			m_iAnimIndex = 43;
			break;
		case Engine::TS_RAGESKILL_ARMAGEDDONBLADE:
			m_iAnimIndex = 44;
			break;
		case Engine::TS_RAGESKILL_DOUBLESLASH:
			m_iAnimIndex = 45;
			break;
		case Engine::TS_SKILL_ROCKBREAK:
			m_iAnimIndex = 46;
			break;
		case Engine::TS_SPECIALCOMBO_CRASH:
			m_iAnimIndex = 47;
			break;
		case Engine::TS_SPECIALCOMBO_CRASH_READY:
			m_iAnimIndex = 48;
			break;
		case Engine::TS_FRONT_EVASION:
			m_iAnimIndex = 57;
			break;
		case Engine::TS_BACK_EVASION:
			m_iAnimIndex = 58;
			break;
		case Engine::TS_BASIC_COMBO01:
			m_iAnimIndex = 59;
			break;
		case Engine::TS_BASIC_COMBO02:
			m_iAnimIndex = 60;
			break;
		case Engine::TS_BASIC_COMBO03:
			m_iAnimIndex = 61;
			break;
		case Engine::TS_END:
			break;
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);

		m_tInfo.PrevAnim = m_tInfo.CurrAnim;
	}
}

void CTSPlayer::Hit(const _int & _Damage)
{
	m_tInfo._Hp -= _Damage;
	m_tInfo.CurrAnimState = ANIM_STUN;

	if (m_tInfo.prevAnimState == ANIM_STUN && true == m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnimState = ANIM_COMBAT_WAIT;

	if (m_tInfo.prevAnimState == ANIM_COMBAT_WAIT == true == m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnimState = ANIM_IDEL;


	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pMonsterTransform = nullptr;

	pMonsterTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vMonPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vMonPos - vPosition;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);

	_vector vUp = XMVector3Cross(vDir, vRight);

	_float3 vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vDir) * vScale.z);

	vPosition -= XMVector3Normalize(vDir) * 0.1f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	RELEASE_INSTANCE(CGameInstance);

}

void CTSPlayer::Damage(const _int & _Damage)
{
	m_tInfo._Hp -= _Damage;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pMonsterTransform = nullptr;

	pMonsterTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vMonPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vMonPos - vPosition;

	_vector vTest = vPosition - vMonPos;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);

	_vector vUp = XMVector3Cross(vDir, vRight);

	_float3 vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vDir) * vScale.z);

	/*
		m_tInfo.fX = tTargetInfo.fX - (tTargetInfo.fCX + m_tInfo.fCX) * 0.5f;

		m_tInfo.fX += (m_fSpeed + 300.f + fDashTime) * m_Dir;

	*/
	//vPosition = vDir - vPosition; // 겹친만큼 밀어줘야 함 -> 계산을 콜리전 매니저에서? 아님 콜라이더에서? 
	//
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	RELEASE_INSTANCE(CGameInstance);

}

void CTSPlayer::Attack()
{
	m_tInfo.CurrAnimState = ANIM_ATTACK;
	m_bAnimCheck = true;

}

void CTSPlayer::Attack_Combo(_double TimeDelta)
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (false == m_bJump)
	{
	
		m_bTest = true;
		m_AttackCheck = true;
		m_ComboCheck2 = true;
		m_tInfo.CurrAnim = TS_BASIC_COMBO01;
		//m_pTransformCom->Go_Straight(0.05); // 루트 모션이 없어서 움직임 제어 직접 해줘야 함 
	}

	if (m_tInfo.PrevAnim == TS_BASIC_COMBO01 && true != m_pModelCom->Get_AnimFinished())
	{
		m_ComboCheck = true;
		//m_ComboCheck2 = false;
		m_tInfo.CurrAnim = TS_BASIC_COMBO02;
		//m_pTransformCom->Go_Straight(0.05);

	}

	else if (m_tInfo.PrevAnim == TS_BASIC_COMBO02 && true != m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_BASIC_COMBO03;
		//m_pTransformCom->Go_Straight(0.07);
		m_bTest = false;

	}

	if (m_tInfo.PrevAnim == TS_BASIC_COMBO03 && true == m_pModelCom->Get_LerpAnimFinished())
	{
		m_AttackCheck = false;
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
		m_bTest = false;
		m_dLerpTime = 0.f;
	}

	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_COMBAT_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_dLerpTime = 0.f;

		m_bTest = false;
		m_tInfo.CurrAnim = TS_WAIT;
	}

}

void CTSPlayer::Attack_Special(_double TimeDelta)
{
	if (true == m_AttackCheck)
	{
		if (m_tInfo.CurrAnim == TS_BASIC_COMBO02 && true != m_pModelCom->Get_AnimFinished())
		{
			m_tInfo.CurrAnim = TS_SPECIALCOMBO_CRASH_READY;
		}
	}

	if (m_tInfo.CurrAnim == TS_SPECIALCOMBO_CRASH_READY && false == m_ComboCheck)
	{
		m_AttackCheck = false;
		m_tInfo.CurrAnim = TS_SPECIALCOMBO_CRASH;
	}
}

void CTSPlayer::Attack_Special2(_double TimeDelta)
{
	if (true == m_AttackCheck)
	{
		if (m_tInfo.CurrAnim == TS_BASIC_COMBO01 && true != m_pModelCom->Get_AnimFinished())
		{
			m_tInfo.CurrAnim = TS_BASIC_COMBO03_START;
			m_tInfo.CurrAnim = TS_BASIC_COMBO02_LOOP; //

		}

	}

	if (m_tInfo.CurrAnim == TS_BASIC_COMBO02_LOOP && false == m_ComboCheck2)
	{
		m_AttackCheck = false;
		m_tInfo.CurrAnim = TS_BASIC_COMBO02_END;
	}
}

void CTSPlayer::Jump(_double TimeDelta)
{
	m_JumpAttack = false;

	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3	vPos;

	XMStoreFloat3(&vPos, vPosition);

	if (true == m_JumpAttack)
		m_fGravity = 0.f;
	else
		m_fGravity = 0.1f + (TimeDelta * 0.5);

	if (false == m_bCheck)
	{
		if (false == m_bFall)
			vPos.y += m_fGravity;
		else
			vPos.y -= m_fGravity + 0.05f;

	}
	if (true == m_bJump)
	{
		if (true == m_bSIbal)
		{
			m_tInfo.CurrAnim = TS_JUMP; // 여기도 조건 잡아줘서 넘어가게 -> 아니면 계속 이게 들어 옴
			_double sibal = 0.02;
			m_pModelCom->Set_AnimTick(sibal);
		}
		m_bSIbal = false;
		m_pModelCom->Set_AnimTick(m_AnimTickPerSecond);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

		//if (m_tInfo.prevAnimState == ANIM_JUMP && true == m_pModelCom->Get_AnimFinished())
		//{
		//	m_tInfo.CurrAnimState = ANIM_JUMP_ING;
		//}

		if (vPos.y >= 5.f) // 공중에 있고, 공격중이 아니라면 // 여기
		{
			m_bFall = true;
			m_tInfo.CurrAnim = TS_JUMP_LENDING;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos)); // 렌딩 끝나면 아이들 모션 적용 

			if (m_tInfo.PrevAnim == TS_JUMP_LENDING && m_pModelCom->Get_AnimFinished())
				m_tInfo.CurrAnim = TS_WAIT;

		}
		// 여기에 들어오는 조건이 좀 잡기 빡셈 점프는 업데이트를 계속 돌고 있고, 점프어택이 들어 가면 점프 어택이 돌아 가면서 그 사이 값을 어떻게 잡아야 할 지 모르겠음.. 시발;
		if (vPos.y >= 1.5f && m_tInfo.CurrAnim == TS_AIR_COMBO01) // 공중에 있고, 공격중이라면
		{
			m_bCheck = true;

			if (true == m_bLendiongCheck)
				m_fGravity = 0.f;

		}
			if( false == m_bLendiongCheck) // 공격이 끝났다면 여기 
			{
				//m_tInfo.CurrAnimState = ANIM_JUMP_LENDING;

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

				if (m_tInfo.PrevAnim == TS_AIR_COMBO04_LENDING && m_pModelCom->Get_AnimFinished())
					m_tInfo.CurrAnim = TS_COMBAT_WAIT;

			}

		if (vPos.y <= 0.0f) //  이 씨발 y 조절 어케 하는데 
		{
			vPos.y = 0.00f;
			m_tInfo.CurrAnim = TS_WAIT;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
			m_fGravity = 0.f;
			m_bFall = false;
			m_bJump = false;
			m_bCheck = false;
		}
	}

}

void CTSPlayer::Jump_Attack(_double TimeDelta)
{
	m_JumpAttack = true;
	m_bLendiongCheck = true;
	
	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3	vPos;

	XMStoreFloat3(&vPos, vPosition);

	if (true == m_bJump)
	{
		m_tInfo.CurrAnim = TS_AIR_COMBO01;

	}

	if (m_tInfo.PrevAnim == TS_AIR_COMBO01 && true != m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnim = TS_AIR_COMBO03;

	//if (m_tInfo.PrevAnim == TS_AIR_COMBO02 && true != m_pModelCom->Get_AnimFinished())
	//{
	//	m_tInfo.CurrAnim = TS_AIR_COMBO03;
	//}

	if (m_tInfo.PrevAnim == TS_AIR_COMBO03 && true != m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_AIR_COMBO04;
	}

	if (m_tInfo.PrevAnim == TS_AIR_COMBO04 && true == m_pModelCom->Get_LerpAnimFinished())
	{
		m_tInfo.CurrAnim = TS_AIR_COMBO04_LENDING;
		//m_bJump = false;
		m_JumpAttack = false;
		m_bLendiongCheck = false;
		m_bFall = true;
		m_bCheck = false;
	

	}

	if (false == m_JumpAttack && m_tInfo.PrevAnim == TS_AIR_COMBO04_LENDING && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
	}

	

}

void CTSPlayer::Dash(_double TimeDelta)
{
	if (true == m_bDeah)
	{
		m_tInfo.CurrAnimState = ANIM_RUN_L;

		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		vPosition += vLook;

		_float3 fPos;
		XMStoreFloat3(&fPos, vPosition);

		fPos.x += m_fPower;
		fPos.z += m_fPower;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));
		m_bDeah = false;

	}
}

void CTSPlayer::DashAttack(_double TimeDelta)
{
	m_tInfo.CurrAnim = TS_DASHCOMBO;
}

void CTSPlayer::CombatWait()
{
	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_SPECIALCOMBO_CRASH && true == m_pModelCom->Get_AnimFinished()) // CRASH가 두 번 들어 갔다가 끊기는 느낌 
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
	
	}

	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_BASIC_COMBO02_END && true == m_pModelCom->Get_AnimFinished()) // CRASH가 두 번 들어 갔다가 끊기는 느낌 
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;

	}

	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_COMBAT_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_WAIT;
	
	}
}

HRESULT CTSPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);

	if (true == m_bDeah)
		TransformDesc.fSpeed += 5.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Body"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTSPlayer::Add_Parts() 
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBonePtr = m_pModelCom->Get_BonePtr("Neck");
	if (nullptr == pBonePtr)
		return E_FAIL;

	CPlayer::PARTSDESC PartsDesc = { pBonePtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom };
	Safe_AddRef(pBonePtr);

	CGameObject* pHead = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player"), &PartsDesc);

	if (nullptr == pHead)
		return E_FAIL;

	m_vecParts[PART_HEAD].push_back(pHead);

	CBone* pHairBontPtr = m_pModelCom->Get_BonePtr("Neck");
	if (nullptr == pHairBontPtr)
		return E_FAIL;

	CHair::HAIRDESC HairDesc1 = { pHairBontPtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CHair::HAIR_BACK };
	//CHair::HAIRDESC HairDesc2 = { pHairBontPtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CHair::HAIR_FRONT };
	//CHair::HAIRDESC HairDesc3 = { pHairBontPtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CHair::HAIR_SIDE };
	//CHair::HAIRDESC HairDesc4 = { pHairBontPtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CHair::HAIR_TAIL };

	CGameObject* pHair = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hair"), &HairDesc1);

	//CGameObject* pHair_F = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hair"), &HairDesc2);
	//
	//CGameObject* pHair_S = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hair"), &HairDesc3);
	//
	//CGameObject* pHair_T = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hair"), &HairDesc4);

	if (nullptr == pHair)// || nullptr == pHair_F || nullptr == pHair_S || nullptr == pHair_T)
		return E_FAIL;

	m_vecParts[PART_HAIR_B].push_back(pHair);
	//m_vecParts[PART_HAIR_F].push_back(pHair_F);
	//m_vecParts[PART_HAIR_S].push_back(pHair_S);
	//m_vecParts[PART_HAIR_T].push_back(pHair_T);

//	CBone* pBoneTopPtr = m_pModelCom->Get_BonePtr("Spine");
//	if (nullptr == pBoneTopPtr)
//		return E_FAIL;
//	
//	CPlayerTop::TOPDESC TopDesc = { pBoneTopPtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CPlayerTop::CLOTHES_TOP };
//	//CPlayerTop::TOPDESC TopDesc2 = { pBoneTopPtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CPlayerTop::CLOTHES_PANTS };
//	Safe_AddRef(pBoneTopPtr);
//
//	CGameObject* pTop = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Top"), &TopDesc);
//	//CGameObject* pPants = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Top"), &TopDesc2);
//
//	if (nullptr == pTop)// || nullptr == pPants)
//		return E_FAIL;
//
//	m_vecParts[PART_TOP].push_back(pTop);
////	m_vecParts[PART_PANTS].push_back(pPants);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTSPlayer::Add_Weapon()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBonePtr = m_pModelCom->Get_BonePtr("Weapon_Hand_R"); // 전투가 아닐 때 붙이는 뼈 == Weapon_Spine_R

	if (nullptr == pBonePtr)
		return E_FAIL;

	CTwoHandedSword::WEAPONDESC WeaponDesc = { pBonePtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom};
	Safe_AddRef(pBonePtr);

	CGameObject* pWeapon = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_TS"), &WeaponDesc);

	if (nullptr == pWeapon)
		return E_FAIL;

	m_vecWeapon[WEAPON_KARMA14].push_back(pWeapon);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTSPlayer::SetUp_ShaderResources()
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

CTSPlayer * CTSPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTSPlayer*		pInstance = new CTSPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTSPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTSPlayer::Clone(void * pArg)
{
	CTSPlayer*		pInstance = new CTSPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTSPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTSPlayer::Free()
{
	__super::Free();

	for (_uint i = 0; i < WEAPON_END; ++i)
	{
		for (auto& pWeapon : m_vecWeapon[i])
			Safe_Release(pWeapon);

		m_vecWeapon[i].clear();
	}

	for (_uint i = 0; i < PART_END; ++i)
	{
		for (auto& pPart : m_vecParts[i])
			Safe_Release(pPart);

		m_vecParts[i].clear();
	}

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
