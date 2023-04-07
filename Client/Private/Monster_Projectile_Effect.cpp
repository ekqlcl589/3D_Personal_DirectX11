#include "stdafx.h"
#include "..\Public\Monster_Projectile_Effect.h"
#include "GameInstance.h"
//
Monster_Projectile_Effect::Monster_Projectile_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

Monster_Projectile_Effect::Monster_Projectile_Effect(const Monster_Projectile_Effect & rhs)
	: CGameObject(rhs)
{
}

HRESULT Monster_Projectile_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Monster_Projectile_Effect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bActive = true;

	m_pModelCom->SetUp_Animation(0);
	
	memcpy(&m_vPosition, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);

	//m_pModelCom->Set_AnimTick(10.0);

	m_fDissolveTime = 3.f;

	return S_OK;
}

void Monster_Projectile_Effect::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		if (true == m_pModelCom->Get_AnimFinished())
		{
			m_fDissolveTime -= TimeDelta;

			fDissolveAmount = Lerp(1.f, 0.f, m_fDissolveTime / 3.f);
			m_bFadeIn = false;

			if (m_fDissolveTime <= 0.f)
				Set_Dead();
		}

	}

}

void Monster_Projectile_Effect::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		if (m_bFadeIn)
			m_pModelCom->Play_Animation(TimeDelta);

		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}

	}
}

HRESULT Monster_Projectile_Effect::Render()
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

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

_bool Monster_Projectile_Effect::FadeInOut()
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

void Monster_Projectile_Effect::Set_Transform()
{
	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * CTwoHandedSword::WorldMatrix);
}

HRESULT Monster_Projectile_Effect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Projectile_Effect"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT Monster_Projectile_Effect::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAmount", &fDissolveAmount, sizeof(float))))
		return E_FAIL;

	return S_OK;
}

Monster_Projectile_Effect * Monster_Projectile_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	Monster_Projectile_Effect*		pInstance = new Monster_Projectile_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Monster_Projectile_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Monster_Projectile_Effect::Clone(void * pArg)
{
	Monster_Projectile_Effect*		pInstance = new Monster_Projectile_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : Monster_Projectile_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Monster_Projectile_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
