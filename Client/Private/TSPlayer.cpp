#include "stdafx.h"
#include "..\Public\TSPlayer.h"

#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Hair.h"
#include "PlayerTop.h"
#include "TwoHandedSword.h"
#include "Ancient_StonGolem.h"

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

	m_tInfo.m_ESkill = 10.f;
	m_tInfo.m_RSkill = 15.f;
	m_tInfo.m_FSkill = 20.f; // �� �ù� ������ ��� ������ m_ �ٿ��� ������
	m_tInfo.m_RageSkill = 25.f;

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
	if (false == m_Eskill)
	{
		m_tInfo.m_ESkill += TimeDelta;
		if (m_tInfo.m_ESkill >= 10.f)
			m_tInfo.m_ESkill = 10.f;
	}
	if (false == m_RsKill)
	{
		m_tInfo.m_RSkill += TimeDelta;
		if (m_tInfo.m_RSkill >= 15.f)
			m_tInfo.m_RSkill = 15.f;
	}
	if (false == m_FsKill)
	{
		m_tInfo.m_FSkill += TimeDelta;
		if (m_tInfo.m_FSkill >= 20.f)
			m_tInfo.m_FSkill = 20.f;
	}
	if (false == m_RagesKill)
	{
		m_tInfo.m_RageSkill += TimeDelta;
		if (m_tInfo.m_RageSkill >= 25.f)
			m_tInfo.m_RageSkill = 25.f;
	}

	__super::Tick(TimeDelta);

	Key_Input(TimeDelta);

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

void CTSPlayer::OnCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	//pColl = &m_Hit;
	//pColl = &m_Hit;
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

		Hit(10); // ���� ���ݷ� �����ͼ� ����  
		m_Hit = true;
		cout << "�� �� ��Ʈ" << endl;
		break;

	case Engine::OBJ_MONSTER_WEAPONR:
	{
		Hit(10); // ���� ���ݷ� �����ͼ� ���� 
		m_Hit = true;
		cout << "���� �� ��Ʈ" << endl;

		break;
	}
	case Engine::OBJ_MONSTER_BODY:
		//m_tInfo.CurrAnim = TS_STURN_LOOP;

		//if (!m_Hit)
		//{
		//	Damage(10); // ���� ���ݷ� �����ͼ� ���� 
		//	m_Hit = true;
		//	m_NoStraight = true;
		//	cout << "���� ��Ʈ" << endl;
		//}
		//pColl = false;
	//}
	//else
	//	m_NoStraight = false;

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
		m_Dir = FRONT;

		//if(!m_NoStraight)
			m_pTransformCom->Go_Straight(TimeDelta);
		m_tInfo.CurrAnim = TS_RUN_END; // �̰� �׳� RUN�ε� enum���� �� �־ �̰ɷ� �� ����

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_W))
	{
		m_bTest = false;

		m_tInfo.CurrAnim = TS_WAIT;
	}

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_S))
	{
		m_bTest = false;
		m_Dir = BACK;

		m_pTransformCom->Go_Back(TimeDelta);
		m_tInfo.CurrAnim = TS_RUN_END;

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_S))
	{
		m_bTest = false;

		m_tInfo.CurrAnim = TS_WAIT;

	}

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_LSHIFT))
	{
		m_tInfo.CurrAnim = TS_DASH;

		m_bDeah = true;
	}

	//if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_Z))
	//{
	//	m_tInfo.CurrAnim = TS_FRONT_EVASION;
	//	m_Evasion = true;
	//}

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
			if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB)) // �������� �ƴϰ� ��� ���°� �ƴ϶�� 
				Attack_Combo(TimeDelta);
		}
		else
		{
			if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB))// �������� �ƴϰ� ��� ���¶�� 
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
		if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB)) // �������� �� 
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

#pragma region SkillMotion Test 

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_E))
	{
		if(m_tInfo.m_ESkill >= 10.f)
			m_Eskill = true; // Start_End ������ ���� 
	}

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_R))
	{
		if (m_tInfo.m_RSkill >= 15.f)
			m_RsKill = true; // Start_End ������ ���� 

	}

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_F))
	{
		if (m_tInfo.m_FSkill >= 20.f)
			m_FsKill = true; // Start_End ������ ���� 

	}

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_Q))
	{
		if (m_tInfo.m_RageSkill >= 25.f)
			m_RagesKill = true; // Start_End ������ ���� 

	}

#pragma endregion 
	Evasion(TimeDelta);

	Attack_Go(TimeDelta);

	CombatWait();

	Dash(TimeDelta);

	Jump(TimeDelta);

	E_Skill(TimeDelta);
	R_Skill(TimeDelta);
	F_Skill(TimeDelta);
	Rage_Skill(TimeDelta);
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
			m_iAnimIndex = 16;
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
	m_tInfo.CurrAnim = TS_STURN_LOOP;
	cout << m_tInfo.CurrAnim << endl;
	//if (m_tInfo.PrevAnim == TS_STURN_LOOP && true == m_pModelCom->Get_AnimFinished())
	//{
	//	cout <<"���� : " << m_tInfo.CurrAnim << endl;
	//	m_tInfo.CurrAnim = TS_COMBAT_WAIT;
	//}
	//
	//if (m_tInfo.PrevAnim == TS_COMBAT_WAIT == true == m_pModelCom->Get_AnimFinished())
	//	m_tInfo.CurrAnim = TS_WAIT;


	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	//
	//CTransform* pMonsterTransform = nullptr;
	//
	//pMonsterTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Transform")));
	//
	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//
	//_vector vMonPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);
	//
	//_vector vDir = vMonPos - vPosition;
	//
	//_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
	//
	//_vector vUp = XMVector3Cross(vDir, vRight);
	//
	//_float3 vScale = m_pTransformCom->Get_Scale();
	//
	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vDir) * vScale.z);
	//
	//// �浹 �� �з� ���� �� 
	//vPosition -= XMVector3Normalize(vDir) * 0.1f;
	//
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	//RELEASE_INSTANCE(CGameInstance);

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

	// ���� ��ġ�� �ʰ� �ؾ� ��
	RELEASE_INSTANCE(CGameInstance);

}

void CTSPlayer::Attack()
{
	m_tInfo.CurrAnimState = ANIM_ATTACK;
	m_bAnimCheck = true;

}

void CTSPlayer::Attack_Combo(_double TimeDelta)
{
	if (false == m_bJump)
	{

		m_bTest = true;
		m_AttackCheck = true;
		m_ComboCheck2 = true;
		m_tInfo.CurrAnim = TS_BASIC_COMBO01;
	}

	if (m_tInfo.PrevAnim == TS_BASIC_COMBO01 && true != m_pModelCom->Get_AnimFinished())
	{
		m_ComboCheck = true;
		m_tInfo.CurrAnim = TS_BASIC_COMBO02;

	}

	else if (m_tInfo.PrevAnim == TS_BASIC_COMBO02 && true != m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_BASIC_COMBO03;
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

		m_DownAttack = true;

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

void CTSPlayer::Attack_Go(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 fPos;

	if (true == m_AttackCheck &&  m_tInfo.CurrAnim == TS_BASIC_COMBO01)
	{
		m_pTransformCom->Go_Straight(TimeDelta * 0.09);

		if (m_pModelCom->Get_AnimTimeAcc() >= 7.0)
		{
			if(!m_NoStraight)
				vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}

	}

	if (true == m_AttackCheck &&  m_tInfo.CurrAnim == TS_BASIC_COMBO02)
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= 9.0)
		{
			m_pTransformCom->Go_Straight(TimeDelta * 0.09);

			if (m_pModelCom->Get_AnimTimeAcc() >= 15.0)
			{
				if (!m_NoStraight)
					vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			}
		}
	}


	if (true == m_AttackCheck &&  m_tInfo.CurrAnim == TS_BASIC_COMBO03)
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= 10.0)
		{
			m_pTransformCom->Go_Straight(TimeDelta * 0.13);

			if (m_pModelCom->Get_AnimTimeAcc() >= 23.0)
			{
				if (!m_NoStraight)
					vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			}
		}
	}

	if (m_tInfo.CurrAnim == TS_BASIC_COMBO02_END)
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= 8.0)
		{
			m_pTransformCom->Go_Straight(TimeDelta * 0.13);

			if (m_pModelCom->Get_AnimTimeAcc() >= 13.0)
			{
				if (!m_NoStraight)
					vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			}
		}
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
		m_fGravity = 0.2f + (TimeDelta * 0.5);

	if (false == m_bCheck)
	{
		if (false == m_bFall)
			vPos.y += m_fGravity;
		else
			vPos.y -= m_fGravity + 0.1f;

	}
	if (true == m_bJump)
	{
		if (true == m_bSIbal)
		{
			m_tInfo.CurrAnim = TS_JUMP; // ���⵵ ���� ����༭ �Ѿ�� -> �ƴϸ� ��� �̰� ��� ��
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

		if (vPos.y >= 5.f) // ���߿� �ְ�, �������� �ƴ϶�� // ����
		{
			m_bFall = true;
			m_tInfo.CurrAnim = TS_JUMP_LENDING;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos)); // ���� ������ ���̵� ��� ���� 

			if (m_tInfo.PrevAnim == TS_JUMP_LENDING && m_pModelCom->Get_AnimFinished())
				m_tInfo.CurrAnim = TS_WAIT;

		}
		// ���⿡ ������ ������ �� ��� ���� ������ ������Ʈ�� ��� ���� �ְ�, ���������� ��� ���� ���� ������ ���� ���鼭 �� ���� ���� ��� ��ƾ� �� �� �𸣰���.. �ù�;
		if (vPos.y >= 1.5f && m_tInfo.CurrAnim == TS_AIR_COMBO01) // ���߿� �ְ�, �������̶��
		{
			m_bCheck = true;

			if (true == m_bLendiongCheck)
				m_fGravity = 0.f;

		}
			if( false == m_bLendiongCheck) // ������ �����ٸ� ���� 
			{
				//m_tInfo.CurrAnimState = ANIM_JUMP_LENDING;

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

				if (m_tInfo.PrevAnim == TS_AIR_COMBO04_LENDING && m_pModelCom->Get_AnimFinished())
					m_tInfo.CurrAnim = TS_COMBAT_WAIT;

			}

		if (vPos.y <= 0.0f) //  �� ���� y ���� ���� �ϴµ� 
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

	if (m_tInfo.PrevAnim == TS_AIR_COMBO03 && true != m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_AIR_COMBO04;
		m_pModelCom->Set_AnimTick(30.0);
	}

	if (m_tInfo.PrevAnim == TS_AIR_COMBO04 && true == m_pModelCom->Get_LerpAnimFinished())
	{
		m_tInfo.CurrAnim = TS_AIR_COMBO04_LENDING;
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
	//if (true == m_bDeah)
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float3 fPos;
	if(true == m_bDeah && m_tInfo.PrevAnim == TS_DASH && true != m_pModelCom->Get_AnimFinished())
	{
		if(m_pModelCom->Get_AnimTimeAcc() >= 4.0)
		{
			vPosition += vLook;

			XMStoreFloat3(&fPos, vPosition);

			fPos.x += m_fPower * TimeDelta;
			fPos.z += m_fPower * TimeDelta;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));

			if (m_pModelCom->Get_AnimTimeAcc() >= 6.0)
			{
				m_bDeah = false;
				m_tInfo.CurrAnim = TS_WAIT;
				vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			}
		}
	}
}

void CTSPlayer::DashAttack(_double TimeDelta)
{
	m_tInfo.CurrAnim = TS_DASHCOMBO;

}

void CTSPlayer::CombatWait()
{
	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_SPECIALCOMBO_CRASH && true == m_pModelCom->Get_AnimFinished()) // CRASH�� �� �� ��� ���ٰ� ����� ���� 
	{
		m_DownAttack = false;
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
	
	}

	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_BASIC_COMBO02_END && true == m_pModelCom->Get_AnimFinished()) // CRASH�� �� �� ��� ���ٰ� ����� ���� 
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;

	}

	if (m_tInfo.PrevAnim == TS_DASHCOMBO && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
		m_bDeah = false;
	}

	if (m_tInfo.PrevAnim == TS_FRONT_EVASION && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
		m_Evasion = false;
	}

	if (m_tInfo.PrevAnim == TS_STURN_LOOP && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
	}

	if (false == m_Eskill && m_tInfo.PrevAnim == TS_SKILL_OUTRAGE_END && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_COMBAT_WAIT;
		m_tInfo.m_ESkill = 0.f;

	}

	//if (m_tInfo.PrevAnim == TS_STURN_LOOP && true == m_pModelCom->Get_AnimFinished())
	//	m_tInfo.CurrAnim = TS_COMBAT_WAIT;// ���Ⱑ �����ΰ� 

	if (false == m_AttackCheck && m_tInfo.PrevAnim == TS_COMBAT_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = TS_WAIT;
	
	}
}

void CTSPlayer::Evasion(_double TimeDelta)
{
	//�� �ù� ��� ���� duration�� ���� tick�� �ʹ� ������ Ÿ�̹� ��� �� ����;
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float3 fPos;

	if (m_Evasion && m_tInfo.PrevAnim == TS_FRONT_EVASION && true != m_pModelCom->Get_AnimFinished())
	{
		if (m_pModelCom->Get_AnimTimeAcc() >= 1.0)
		{
			vPosition += vLook;

			XMStoreFloat3(&fPos, vPosition);

			fPos.x += 0.001f * TimeDelta;
			fPos.z += 0.001f * TimeDelta;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));

			if (m_pModelCom->Get_AnimTimeAcc() >= 3.0)
			{
				m_Evasion = false;
				m_tInfo.CurrAnim = TS_COMBAT_WAIT;
				vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			}
		}
	}
	//else if (m_Evasion && m_Dir == BACK)
	//{
	//	vPosition += vLook;
	//
	//	XMStoreFloat3(&fPos, vPosition);
	//
	//	fPos.x -= 0.005 * TimeDelta;
	//	fPos.z -= 0.005 * TimeDelta;
	//
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));
	//
	//}
}

void CTSPlayer::E_Skill(_double TimeDelta)
{
	// Ű ��ǲ���� ����? ���� �̰� �׻� ���鼭 keyinput������ bool�� true�� �ٲ��ִ� ������ 
	if (m_Eskill && m_tInfo.m_ESkill >= 10.f)
	{
		m_tInfo.CurrAnim = TS_SKILL_OUTRAGE_START;

		if (m_tInfo.PrevAnim == TS_SKILL_OUTRAGE_START && true == m_pModelCom->Get_AnimFinished())
		{
			m_tInfo.CurrAnim = TS_SKILL_OUTRAGE_END;
			m_Eskill = false;

		}
	}
}

void CTSPlayer::R_Skill(_double TimeDelta)
{
	if (m_RsKill && m_tInfo.m_RSkill >= 15.f)
	{
		m_tInfo.CurrAnim = TS_SKILL_ROCKBREAK;

		if (m_tInfo.PrevAnim == TS_SKILL_ROCKBREAK && true == m_pModelCom->Get_AnimFinished())
		{
			m_tInfo.CurrAnim = TS_COMBAT_WAIT;
			m_RsKill = false;
			m_tInfo.m_RSkill = 0.f;

		}
	}
}

void CTSPlayer::F_Skill(_double TimeDelta)
{
	if (m_FsKill && m_tInfo.m_FSkill >= 20.f)
	{
		m_tInfo.CurrAnim = TS_RAGESKILL_ARMAGEDDONBLADE;

		if (m_tInfo.PrevAnim == TS_RAGESKILL_ARMAGEDDONBLADE && true == m_pModelCom->Get_AnimFinished())
		{
			m_tInfo.CurrAnim = TS_COMBAT_WAIT;
			m_FsKill = false;
			m_tInfo.m_FSkill = 0.f;

		}
	}
}

void CTSPlayer::Rage_Skill(_double TimeDelta)
{
	if (m_RagesKill && m_tInfo.m_RageSkill >= 25.f)
	{
		m_tInfo.CurrAnim = TS_RAGESKILL_DOUBLESLASH;

		if (m_tInfo.PrevAnim == TS_RAGESKILL_DOUBLESLASH && true == m_pModelCom->Get_AnimFinished())
		{
			m_tInfo.CurrAnim = TS_COMBAT_WAIT;
			m_RagesKill = false;
			m_tInfo.m_RageSkill = 0.f;

		}
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

	CBone* pBonePtr = m_pModelCom->Get_BonePtr("Weapon_Hand_R"); // ������ �ƴ� �� ���̴� �� == Weapon_Spine_R

	if (nullptr == pBonePtr)
		return E_FAIL;

	CTwoHandedSword::WEAPONDESC WeaponDesc = { pBonePtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CTwoHandedSword::WEAPON_TS, OBJ_WEAPON_KARMA14};
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