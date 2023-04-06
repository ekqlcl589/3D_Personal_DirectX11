#include "stdafx.h"
#include "..\Public\Player_Rage_Arma.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "TwoHandedSword.h"
//
CPlayer_Rage_Arma::CPlayer_Rage_Arma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer_Rage_Arma::CPlayer_Rage_Arma(const CPlayer_Rage_Arma & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_Rage_Arma::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Rage_Arma::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bActive = true;

	return S_OK;
}

void CPlayer_Rage_Arma::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* p = GET_INSTANCE(CGameInstance);
	CGameObject* pOwner = nullptr;

	pOwner = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_bActive = static_cast<CTSPlayer*>(pOwner)->Get_Info().fSkill;

	//m_Time = static_cast<CTSPlayer*>(pOwner)->Get_Info().m_FSkill;
	if (m_bActive)
	{
		m_Time -= 1.0 * TimeDelta;// static_cast<CTSPlayer*>(pOwner)->Get_Info().m_FSkill;

		if (m_Time <= 0.f)
			m_Time = 0.f;
	}

	RELEASE_INSTANCE(CGameInstance);

}

void CPlayer_Rage_Arma::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Set_Transform();

	FadeInOut();

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CPlayer_Rage_Arma::Render()
{
	if (m_bActive)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NoiseTexture", i, aiTextureType_NORMALS);
			/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

			//m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

			m_pShaderCom->Begin(0);

			m_pModelCom->Render(i);
		}
	}

	return S_OK;
}

_bool CPlayer_Rage_Arma::FadeInOut()
{
	if (m_Alpha >= 100.f && !m_bFadeIn)
		m_Alpha -= m_fFadeSpeed;

	if (m_Alpha < 255.f && m_bFadeIn)
		m_Alpha += m_fFadeSpeed;
	else
		m_bFadeIn = false;

	if (m_Alpha > 255.f)
		m_Alpha = 255.f;

	if (m_Alpha < 100.f)
	{
		m_bDead = true;
		return true;
	}

	return false;
	//셰이더로 던져야 하는데 흠;
}

void CPlayer_Rage_Arma::Set_Transform()
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

HRESULT CPlayer_Rage_Arma::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Arma"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel_Test"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Rage_Arma::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	//if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;

	float time = m_Time;

	float scale = 0.5f;
	float Tiling = 0.1f;

	_float3 Velocity = { 1.f, 1.f, 1.f };

	m_pShaderCom->Set_RawValue("time", &time, sizeof(float));

	m_pShaderCom->Set_RawValue("dissolveNoiseScale", &scale, sizeof(float));
	m_pShaderCom->Set_RawValue("dissolveNoiseTiling", &Tiling, sizeof(float));
	
	m_pShaderCom->Set_RawValue("dissolveNoiseVelocity", &Velocity, sizeof(_float3));


	return S_OK;
}

CPlayer_Rage_Arma * CPlayer_Rage_Arma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Rage_Arma*		pInstance = new CPlayer_Rage_Arma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Rage_Arma");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Rage_Arma::Clone(void * pArg)
{
	CPlayer_Rage_Arma*		pInstance = new CPlayer_Rage_Arma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer_Rage_Arma");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Rage_Arma::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
}
