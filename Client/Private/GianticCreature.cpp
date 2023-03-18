#include "stdafx.h"
#include "..\Public\GianticCreature.h"
#include "GameInstance.h"
#include "MonsterWeapon.h"

CGianticCreature::CGianticCreature(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CGianticCreature::CGianticCreature(const CGianticCreature & rhs)
	: CMonster(rhs)
{
}

HRESULT CGianticCreature::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGianticCreature::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Coll()))
		return	E_FAIL;

	_float3 fPosition = { 5.f, 0.f, 10.f }; // 임시 위치값
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_tInfo._MaxHp = 1000.f;
	m_tInfo._Hp = 1000.f;

	m_tInfo.CurrAnim = C_WAIT; // 일단 정상 생성되는 지 확인
	m_pModelCom->SetUp_Animation(m_tInfo.CurrAnim);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(195.0f));

	return S_OK;
}

void CGianticCreature::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Animation_State(TimeDelta);

	Animation(m_tInfo.CurrAnim);

	//Attack_Go(TimeDelta);

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

void CGianticCreature::LateTick(_double TimeDelta)
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

HRESULT CGianticCreature::Render()
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

void CGianticCreature::OnCollision(CGameObject * pObj)
{
	//MonsterWeapon 에서 처리 
}

HRESULT CGianticCreature::Add_Coll()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

 	CBone* pBoneL = m_pModelCom->Get_BonePtr("L_Finger2");
	CBone* pBoneR = m_pModelCom->Get_BonePtr("R_Finger2");
	CBone* pBoneSpine = m_pModelCom->Get_BonePtr("Spine");

	if (nullptr == pBoneL || nullptr == pBoneR || nullptr == pBoneSpine)
		return E_FAIL;

	CMonsterWeapon::WEAPONDESC WeaponDesc = { pBoneL, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_L, CMonsterWeapon::OWNER_CREATURE };
	CMonsterWeapon::WEAPONDESC WeaponDesc1 = { pBoneR, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_R, CMonsterWeapon::OWNER_CREATURE };
	CMonsterWeapon::WEAPONDESC WeaponDesc2 = { pBoneSpine, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_BODY, CMonsterWeapon::OWNER_CREATURE };
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

void CGianticCreature::Animation_State(_double TimeDelta)
{
}

void CGianticCreature::Animation(CREATURESTATE eType)
{
	if (m_tInfo.CurrAnim != m_tInfo.PrevAnim)
	{
		switch (eType)
		{
		case Engine::C_DIE:
			m_iAnimIndex = 0;
			break;
		case Engine::C_RTDOWN_LOOP:
			m_iAnimIndex = 1;
			break;
		case Engine::C_RTDOWN_START:
			m_iAnimIndex = 2;
			break;
		case Engine::C_RTSTAND_END:
			m_iAnimIndex = 3;
			break;
		case Engine::C_RTSTAND_LOOP:
			m_iAnimIndex = 4;
			break;
		case Engine::C_RTSTAND_START:
			m_iAnimIndex = 5;
			break;
		case Engine::C_SKILL01_1:
			m_iAnimIndex = 6;
			break;
		case Engine::C_SKILL01_2:
			m_iAnimIndex = 7;
			break;
		case Engine::C_SKILL02:
			m_iAnimIndex = 8;
			break;
		case Engine::C_SKILL03_L:
			m_iAnimIndex = 9;
			break;
		case Engine::C_SKILL03_R:
			m_iAnimIndex = 10;
			break;
		case Engine::C_SKILL04:
			m_iAnimIndex = 11;
			break;
		case Engine::C_SKILL05:
			m_iAnimIndex = 12;
			break;
		case Engine::C_SKILL07:
			m_iAnimIndex = 13;
			break;
		case Engine::C_SP_SKILL01_1:
			m_iAnimIndex = 14;
			break;
		case Engine::C_SP_SKILL01_2_1:
			m_iAnimIndex = 15;
			break;
		case Engine::C_SP_SKILL01_2_2:
			m_iAnimIndex = 16;
			break;
		case Engine::C_SP_SKILL01_2_3:
			m_iAnimIndex = 17;
			break;
		case Engine::C_SP_SKILL01_END:
			m_iAnimIndex = 18;
			break;
		case Engine::C_SP_SKILL01_WAIT:
			m_iAnimIndex = 19;
			break;
		case Engine::C_SP_SKILL01_WALK:
			m_iAnimIndex = 20;
			break;
		case Engine::C_SP_SKILL02:
			m_iAnimIndex = 21;
			break;
		case Engine::C_SP_SKILL03:
			m_iAnimIndex = 22;
			break;
		case Engine::C_SP_SKILL03_LOOP:
			m_iAnimIndex = 23;
			break;
		case Engine::C_RTDOWN_ATTACK:
			m_iAnimIndex = 24;
			break;
		case Engine::C_START:
			m_iAnimIndex = 25;
			break;
		case Engine::C_WAIT:
			m_iAnimIndex = 26;
			break;
		case Engine::C_WALK:
			m_iAnimIndex = 27;
			break;
		case Engine::C_ANIM_END:
			break;
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);
		m_tInfo.PrevAnim = m_tInfo.CurrAnim;
	}
}

void CGianticCreature::Use_Skill(_double TimeDelta)
{
}

void CGianticCreature::Combat_Wait(_double TimeDelta)
{
}

void CGianticCreature::Attack_Go(_double TimeDelta)
{
}

HRESULT CGianticCreature::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss1"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CGianticCreature::SetUp_ShaderResources()
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

CGianticCreature * CGianticCreature::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGianticCreature*		pInstance = new CGianticCreature(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGianticCreature");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGianticCreature::Clone(void * pArg)
{
	CGianticCreature*		pInstance = new CGianticCreature(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGianticCreature");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGianticCreature::Free()
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
