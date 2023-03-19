#include "stdafx.h"
#include "..\Public\GrudgeWraith.h"
#include "GameInstance.h"
#include "MonsterWeapon.h"

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

	m_tInfo._MaxHp = 1000.f;
	m_tInfo._Hp = 1000.f;

	m_tInfo.CurrAnim = G_Wait;
	m_pModelCom->SetUp_Animation(m_tInfo.CurrAnim);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(195.0f));

	return S_OK;
}

void CGrudgeWraith::Tick(_double TimeDelta)
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

void CGrudgeWraith::LateTick(_double TimeDelta)
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

HRESULT CGrudgeWraith::Render()
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

void CGrudgeWraith::OnCollision(CGameObject * pObj)
{
}

HRESULT CGrudgeWraith::Add_Coll()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBoneR = m_pModelCom->Get_BonePtr("Bip001-R-Finger01");
	CBone* pBoneSpine = m_pModelCom->Get_BonePtr("Bip001-Spine");

	if (nullptr == pBoneR || nullptr == pBoneSpine)
		return E_FAIL;

	CMonsterWeapon::WEAPONDESC WeaponDesc1 = { pBoneR, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_R, CMonsterWeapon::OWNER_WRAITH };
	CMonsterWeapon::WEAPONDESC WeaponDesc2 = { pBoneSpine, m_pModelCom->Get_LocalMatrix(), m_pTransformCom, CMonsterWeapon::WEAPON_MONSTER_BODY, CMonsterWeapon::OWNER_WRAITH };
	Safe_AddRef(pBoneR);
	Safe_AddRef(pBoneSpine);

	CGameObject* pWeaponR = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc1);
	CGameObject* pWeaponSpine = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Monster_Weapon"), &WeaponDesc2);

	if (nullptr == pWeaponR || nullptr == pWeaponSpine)
		return E_FAIL;

	m_vecWeapon[WEAPON_MONSTERR].push_back(pWeaponR);
	m_vecWeapon[WEAPON_MONSTERBODY].push_back(pWeaponSpine);

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

	Skill01(TimeDelta);
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
}

void CGrudgeWraith::Combat_Wait(_double TimeDelta)
{
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

	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 35.0)
		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if(m_tInfo.PrevAnim == G_Skill01_3 && m_AnimTimeAcc >= 19.0)
		m_pTransformCom->Go_Straight(0.5 * TimeDelta);

	if (m_tInfo.PrevAnim == G_Skill01_3 && m_AnimTimeAcc >= 40.0)
		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CGrudgeWraith::Run(_double TimeDelta)
{
}

void CGrudgeWraith::Skill01(_double TimeDelta)
{
	if (m_tInfo.PrevAnim == G_Wait && true == m_pModelCom->Get_AnimFinished())
	{
		m_tInfo.CurrAnim = G_Skill01_1;
		m_bAttack = true;
	}

	int RandSkill = 0;

	RandSkill = rand() % 2;

	switch (RandSkill)
	{
	case 0:
		Skill01_1();
		break;

	case 1:
		Skill01_2();
		break;

	default:
		break;
	}

	if (m_tInfo.PrevAnim == G_Skill01_2&& m_AnimTimeAcc >= (m_AnimDuration / 2) + 37.0)
	{
		m_bAttack = false;
		m_tInfo.CurrAnim = G_Wait;
	} 

	if(m_tInfo.PrevAnim == G_Skill01_3 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 41.0)
	{
		m_bAttack = false;
		m_tInfo.CurrAnim = G_Wait;
	} 

}

void CGrudgeWraith::Skill01_1()
{
	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 48.0)
	{
		m_tInfo.CurrAnim = G_Skill01_2;
	}

}

void CGrudgeWraith::Skill01_2()
{
	if (m_tInfo.PrevAnim == G_Skill01_1 && m_AnimTimeAcc >= (m_AnimDuration / 2) + 48.0)
	{
		m_tInfo.CurrAnim = G_Skill01_3;
	}

}

void CGrudgeWraith::Skill02(_double TimeDelta)
{
}

void CGrudgeWraith::Skill04(_double TimeDelta)
{
}

void CGrudgeWraith::Skill05(_double TimeDelta)
{
}

void CGrudgeWraith::Skill07(_double TimeDelta)
{
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
