#include "stdafx.h"
#include "..\Public\Player_Body.h"

#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Hair.h"
#include "PlayerTop.h"
#include "SwordShield.h"

#include "TargetCamera.h"

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

	m_bJump = false;


	return S_OK;
}

HRESULT CPlayer_Body::Initialize(void * pArg)
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

	return S_OK;
}

void CPlayer_Body::Tick(_double TimeDelta)
{
	if (false == m_bStart)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

		pInstance->Add_Collider(m_eCollisionState, 0, this);

		RELEASE_INSTANCE(CGameInstance);

		m_bStart = true;
	}

	__super::Tick(TimeDelta);

	Key_Input(TimeDelta);
	CombatWait();
	Animation_State(m_tInfo.CurrAnimState, TimeDelta);

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

void CPlayer_Body::LateTick(_double TimeDelta)
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

#ifdef _DEBUG

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

#endif

	return S_OK;
}

void CPlayer_Body::OnCollision(CGameObject * pObj)
{
	COLLISIONSTATE eType = pObj->Get_ObjType();

	switch (eType)
	{
	case Engine::OBJ_WEAPON_KARMA14:
		break;
	case Engine::OBJ_BOSS1:
		m_tInfo._Hp -= 10.f;
		break;
	case Engine::OBJ_BOSS2:
		break;
	case Engine::OBJ_END:
		break;
	default:
		break;
	}
}

void CPlayer_Body::Key_Input(_double TimeDelta)
{

	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_UP))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
		m_tInfo.CurrAnimState = ANIM_RUN;

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_UP))
	{
		m_tInfo.CurrAnimState = ANIM_IDEL;
	}

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_DOWN))
	{
		m_pTransformCom->Go_Back(TimeDelta);
		m_tInfo.CurrAnimState = ANIM_RUN;

	}

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_NUMPAD0))
	{
		Dash(TimeDelta);
		m_pTransformCom->Go_Straight(TimeDelta);

	}
	else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_NUMPAD0))
		m_bDeah = false;

	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_LEFT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -1.f);
		//m_tInfo.CurrAnimState = ANIM_RUN_L;
	}
	//else if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_LEFT))
	//	m_tInfo.CurrAnimState = ANIM_RUN_END;


	if (CKeyMgr::GetInstance()->Key_Pressing(DIKEYBOARD_RIGHT))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
		//m_tInfo.CurrAnimState = ANIM_RUN_R;
	}
	//if (CKeyMgr::GetInstance()->Key_Up(DIKEYBOARD_RIGHT))
	//	m_tInfo.CurrAnimState = ANIM_RUN_END;

	//Attack_Combo(TimeDelta);

	if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_LB))
	{
		Attack_Combo(TimeDelta);

		//m_pModelCom->SetUp_Animation(27);
	}

	if (CKeyMgr::GetInstance()->Mouse_Down(DIMK_RB))
	{
		Jump_Attack(TimeDelta);

		//m_pModelCom->SetUp_Animation(27);
	}

	//else if (CKeyMgr::GetInstance()->Mouse_Up(DIMK_LB))
	//	m_tInfo.CurrAnimState = ANIM_COMBAT_WAIT;

	//else
	//	m_tInfo.CurrAnimState = ANIM_IDEL;

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_Q))
		m_tInfo._Hp -= 10.f;

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_SPACE))
		Jump(TimeDelta);

#pragma region SkillMotion Test 

	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_E))
		m_pModelCom->SetUp_Animation(38);
	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_R))
		m_pModelCom->SetUp_Animation(42);
	if (CKeyMgr::GetInstance()->Key_Down(DIKEYBOARD_F))
		m_pModelCom->SetUp_Animation(43);

#pragma endregion 
}

void CPlayer_Body::Animation_State(PLAYERANIMSTATE eType, _double TimeDelta)
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
			m_iAnimIndex = 26;
			break;

		case ANIM_ATTACK_COMBO2:
			m_iAnimIndex = 27;
			break;

		case ANIM_ATTACK_COMBO3:
			m_iAnimIndex = 28;
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

		case ANIM_END:
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);

		m_tInfo.prevAnimState = m_tInfo.CurrAnimState;
	}

}

void CPlayer_Body::Hit(const _int & _Damage)
{
	m_tInfo._Hp -= _Damage;
	m_pModelCom->SetUp_Animation(16);

	
}

void CPlayer_Body::Attack()
{
	m_tInfo.CurrAnimState = ANIM_ATTACK;
	m_bAnimCheck = true;

}

void CPlayer_Body::Attack_Combo(_double TimeDelta)
{
	//if (false == m_bJump)
	//{
	//
	//}
	m_AttackCheck = true;

	m_tInfo.CurrAnimState = ANIM_ATTACK;

	if (m_tInfo.prevAnimState == ANIM_ATTACK && true != m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnimState = ANIM_ATTACK_COMBO2;

	else if (m_tInfo.prevAnimState == ANIM_ATTACK_COMBO2 && true != m_pModelCom->Get_AnimFinished())
		m_tInfo.CurrAnimState = ANIM_ATTACK_COMBO3;

		//if (m_tInfo.prevAnimState == ANIM_ATTACK_COMBO3 && true == m_pModelCom->Get_AnimFinished())
		//{
		//	m_tInfo.CurrAnimState = ANIM_COMBAT_WAIT;
		//	m_AttackCheck = false;
		//
		//}
		//
		//if (false == m_AttackCheck && m_tInfo.prevAnimState == ANIM_COMBAT_WAIT && true == m_pModelCom->Get_AnimFinished())
		//{
		//	m_tInfo.CurrAnimState = ANIM_IDEL;
		//}
}

void CPlayer_Body::Jump(_double TimeDelta)
{
	m_tInfo.CurrAnimState = ANIM_JUMP;
	m_pModelCom->Set_AnimTick(14.f);
	m_bJump = true;
	m_JumpAttack = false;

	m_pTransformCom->Jump(TimeDelta, &m_bJump);

	if (m_tInfo.prevAnimState == ANIM_JUMP && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnimState = ANIM_JUMP_ING;
		m_bJump = false;
	}

}

void CPlayer_Body::Jump_Attack(_double TimeDelta)
{
	if (true == m_bJump && false == m_JumpAttack)
	{
		m_fGravity = 0.f;
		//m_JumpAttack = true;
		m_tInfo.CurrAnimState = ANIM_JUMP_ATTACK1;

		if (m_tInfo.prevAnimState == ANIM_JUMP_ATTACK1 && true != m_pModelCom->Get_AnimFinished())
			m_tInfo.CurrAnimState = ANIM_JUMP_ATTACK2;

		else if (m_tInfo.prevAnimState == ANIM_JUMP_ATTACK2 && true != m_pModelCom->Get_AnimFinished())
			m_tInfo.CurrAnimState = ANIM_JUMP_ATTACK3;

		// keyinput이 있어야 다음 동작으로 넘어 가는건가 아니 근데 키를 눌러도 true 조건에 만족을 못 해서 안 넘어 가는 건가 ㅅㅂ
		//if (m_tInfo.prevAnimState == ANIM_JUMP_ATTACK3 && true == m_pModelCom->Get_AnimFinished())
		//{
		//	m_bJump = false;
		//	m_fGravity = 5.f;
		//	m_tInfo.CurrAnimState = ANIM_JUMP_LENDING;
		//}
		//
		//if (m_tInfo.prevAnimState == ANIM_JUMP_LENDING && true == m_pModelCom->Get_AnimFinished())
		//{
		//	m_tInfo.CurrAnimState = ANIM_IDEL;
		//	m_JumpAttack = true;
		//}

	}
}

void CPlayer_Body::Dash(_double TimeDelta)
{
	m_bDeah = true;
	m_tInfo.CurrAnimState = ANIM_RUN_L;
	
}

void CPlayer_Body::CombatWait()
{
	if (m_tInfo.prevAnimState == ANIM_ATTACK_COMBO3 && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnimState = ANIM_COMBAT_WAIT;
		m_AttackCheck = false;

	}

	if (false == m_AttackCheck && m_tInfo.prevAnimState == ANIM_COMBAT_WAIT && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnimState = ANIM_IDEL;
	}

	if (m_tInfo.prevAnimState == ANIM_JUMP_ATTACK3 && true == m_pModelCom->Get_AnimFinished())
	{
		m_bJump = false;
		m_fGravity = 5.f;
		m_tInfo.CurrAnimState = ANIM_JUMP_LENDING;
	}

	if (m_tInfo.prevAnimState == ANIM_JUMP_LENDING && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnimState = ANIM_IDEL;
		m_JumpAttack = true;
	}

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

HRESULT CPlayer_Body::Add_Parts() 
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

HRESULT CPlayer_Body::Add_Weapon()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBonePtr = m_pModelCom->Get_BonePtr("Weapon_Hand_R"); // 전투가 아닐 때 붙이는 뼈 == Weapon_Spine_R
	CBone* pBonePtrL = m_pModelCom->Get_BonePtr("Weapon_Hand_L");

	if (nullptr == pBonePtr || nullptr == pBonePtrL)
		return E_FAIL;

	CSwordShield::WEAPONDESC WeaponDesc = { pBonePtr, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CSwordShield::WEAPON_SWORD };
	CSwordShield::WEAPONDESC WeaponDesc1 = { pBonePtrL, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CSwordShield::WEAPON_SHILED };
	Safe_AddRef(pBonePtr);
	Safe_AddRef(pBonePtrL);

	CGameObject* pWeapon = pInstance->Clone_GameObject_Add_Layer(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	CGameObject* pShield = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc1);

	if (nullptr == pWeapon || nullptr == pShield)
		return E_FAIL;

	m_vecWeapon[WEAPON_SS].push_back(pWeapon);
	m_vecWeapon[WEAPON_SHIELD].push_back(pShield);

	RELEASE_INSTANCE(CGameInstance);

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
