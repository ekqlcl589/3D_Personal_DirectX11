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

void CAncient_StonGolem::OnCollision(CGameObject * pObj, _bool* pColl)
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
			cout << "Į ���� �¾Ƽ� ���� ü�� : " << m_eType._Hp << endl;
			m_isColl = true;
			pColl = false;
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
		case Engine::S_RESPAN: //  w ��𰬴� ����
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
		cout << "���� �ִ� �ε��� :" << m_CurrAnim << endl;
		m_PrevAnim = m_CurrAnim;
	}

}

_uint CAncient_StonGolem::Set_State(_double TimeDelta)
{
	if (m_eType._Hp <= 1.f)
	{
		m_CurrAnim = S_SKILL10_1; // ������ ��ų ����ε� �״°� ���� ��� �̰ɷ� ��ü 

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


	if (true == m_bPlayerChecck) //  �÷��̾�� Ư�� �Ÿ� ���Ϸ� ���ͼ� ���� �ϸ� ������ ���� ���߿� �� ������ �� �ָ��� �� ��� Ȱ��ȭ �ϰ� ī�޶� ���ͷ� �̵�
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
		Set_Skill04(TimeDelta); // ���� �� ���� ���� 1
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
				vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // �ù� �̰� �� �̴�� ��� ���� �Ǵ� �� �ƴϰ���?
			}

		}

	}


	Set_Skill07(TimeDelta);
	
	// ���� ���� �̷��� ������ ������ �ʾƼ� ���� ������� ���� 

	if (m_eType._Hp <= 500.f)
		Set_Skill01(TimeDelta); // ü���� �� ���Ϸ� �������� 
	
	if(m_eType._Hp <= 250.f)
		Set_Skill05(TimeDelta); // ü���� 25% ���Ϸ� �������� ���� ��ũ���鼭 ü�� ȸ�� ���� ��� 
	////�ִϸ��̼��� ��� �����ٸ� ���� �߰� 

	return OBJ_NOEVENT;
}

void CAncient_StonGolem::Run(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); // �ڱ� ��ġ�� ���� 
	// 5���� �� �� battack false -> �÷��̾ �������
	// 5 ���� ������ battack true -> ����, ���� �� �÷��̾ �־����� �ϴ� ���� ����� �� ���� 
	// Chase to Target �� ù ���� ���� 4_1 �� �� ��� �Ǵ� �Լ� 
	if (true == m_bCheck && false == m_bAttack) // �̰� �ؿ��� �ߵ��ϴ� ���� ���� 7, 9 ������ ������ �� ���� ���ɼ��� ���� 
	{
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 10.f) // Ÿ�ٰ��� �Ÿ��� 10 ���� ������ 
		{
			m_pTransformCom->Chase_Tatget(m_vTargetPos, 5.f, TimeDelta);
			m_CurrAnim = S_RUN;
		}
		if (m_pTransformCom->Compute_Distance(m_vTargetPos) <= 5.f)
		{
			m_CurrAnim = S_WAIT; // ������ �� �ǰ� �Ѿ�� ���� 
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); // �ڱ� ��ġ�� ���� 

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
		// �ִϸ��̼� ������ ������ �μ����� ����Ʈ ����
	}

	if (m_PrevAnim == S_SKILL04_2 && true == m_pModelCom->Get_AnimFinished())
	{
		// ����Ʈ FadeIn,Out ��  ����ó�� 
		m_AnimDuration;  
		m_AnimTimeAcc; // �̰� duration�� ������ �Ǹ� �÷��̾ ���� �������� �ϰ� 
		//�� 80�� �� ����Ǹ� ������ ���߰� ���ڸ��� ���� �־�� �� 
		m_CurrAnim = S_SKILL04_3;
	}
	
	if (m_PrevAnim == S_SKILL04_3 && true == m_pModelCom->Get_AnimFinished())
	{
		m_bSkill4 = true;
		m_bAttack = false; // false�� �� �� ������ �Ÿ��� �־����ٸ� ���� ���� 

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
		m_bAttack = true; // ���⼭ �ٽ� true�� �༭ �Ÿ��� �־ �� �� Ʈ�簡 �ƴٸ� ���� ��� ������� ���ϰ� 
		//m_bTest = true; // ���� 7���� ������ ���� �̵� �ϴ� 
		m_CurrAnim = S_SKILL07;

	}

	if (m_PrevAnim == S_SKILL07 && true == m_pModelCom->Get_AnimFinished())
	{
		m_dLerpTime = 0.f;
		m_bTest = false;
		m_bAttack = false; // �ٽ� false;
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
