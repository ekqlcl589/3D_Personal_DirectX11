#include "stdafx.h"
#include "..\Public\Player_Skill_RockBreak.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "TwoHandedSword.h"
//
Player_Skill_RockBreak::Player_Skill_RockBreak(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

Player_Skill_RockBreak::Player_Skill_RockBreak(const Player_Skill_RockBreak & rhs)
	: CGameObject(rhs)
{
}

HRESULT Player_Skill_RockBreak::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Player_Skill_RockBreak::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bActive = true;

	m_pModelCom->SetUp_Animation(0);
	
	return S_OK;
}

void Player_Skill_RockBreak::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		if (true == m_pModelCom->Get_AnimFinished())
		{
			m_bFadeIn = false;
			Set_Dead();
		}

		//FadeInOut();
	}

}

void Player_Skill_RockBreak::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * CTwoHandedSword::WorldMatrix);
		//XMStoreFloat4x4(&m_WorldMatrix, XMMatrixRotationZ(270.f) * m_pTransformCom->Get_WorldMatrix() * CTwoHandedSword::WorldMatrix);

		//if (m_bFadeIn)
		m_pModelCom->Play_Animation(TimeDelta);

		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}

	}
}

HRESULT Player_Skill_RockBreak::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DepthTexture", i, aiTextureType_DIFFUSE);
		/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

		m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

_bool Player_Skill_RockBreak::FadeInOut()
{
	if (m_Alpha >= 100 && !m_bFadeIn)
		m_Alpha -= m_fFadeSpeed;

	if (m_Alpha < 255 && m_bFadeIn)
		m_Alpha += m_fFadeSpeed;
	else
		m_bFadeIn = false;

	if (m_Alpha > 255)
		m_Alpha = 255;

	if (m_Alpha < 100)
	{
		m_bDead = true;
		return true;
	}

	return false;
	//셰이더로 던져야 하는데 흠;
}

void Player_Skill_RockBreak::Set_Transform()
{
//	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
//
//	CGameObject* pWeapon = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_TS"));
//	RELEASE_INSTANCE(CGameInstance);
//
//	_float4x4 Weapon = static_cast<CTwoHandedSword*>(pWeapon)->Get_WeaponMatrix();
//
	//CTwoHandedSword::WorldMatrix; 

	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&Weapon));
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * CTwoHandedSword::WorldMatrix);
}

HRESULT Player_Skill_RockBreak::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RockBreak"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT Player_Skill_RockBreak::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

Player_Skill_RockBreak * Player_Skill_RockBreak::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	Player_Skill_RockBreak*		pInstance = new Player_Skill_RockBreak(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Player_Skill_RockBreak");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Player_Skill_RockBreak::Clone(void * pArg)
{
	Player_Skill_RockBreak*		pInstance = new Player_Skill_RockBreak(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : Player_Skill_RockBreak");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Player_Skill_RockBreak::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
