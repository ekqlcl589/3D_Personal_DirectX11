#include "stdafx.h"
#include "..\Public\PlayerRageEffectTexture.h"
#include "GameInstance.h"
#include "PlayerRageAddEffect.h"
PlayerRageEffectTexture::PlayerRageEffectTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBlendObject(pDevice, pContext)
{
}

PlayerRageEffectTexture::PlayerRageEffectTexture(const PlayerRageEffectTexture & rhs)
	: CBlendObject(rhs)
{
}

HRESULT PlayerRageEffectTexture::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT PlayerRageEffectTexture::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	memcpy(&m_vPosition, pArg, sizeof(_vector));

	_float3 fPos;

	XMStoreFloat3(&fPos, m_vPosition);

	fPos.y += 2.0f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));

	return S_OK;
}

void PlayerRageEffectTexture::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);
	
		m_fFrame += 11.f * TimeDelta;
		if (m_fFrame >= 11.0f)
		{
			m_fFrame = 0.f;
			iCnt++;
		}

		if (iCnt >= 5)
			Set_Dead();

	}
}

void PlayerRageEffectTexture::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		//Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT PlayerRageEffectTexture::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	/* Apply함수를 호출하기전에 내가 사용하고자하는 셰이더 전역변수로 필요한값을 모두 던져야하낟. */
	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT PlayerRageEffectTexture::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fSpeed = 5.f;
	TransformDesc.fRotation = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instance_Rage"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RageEffect"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT PlayerRageEffectTexture::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_Transformfloat4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPos(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_DiffuseTexture", _uint(m_fFrame))))
		return E_FAIL;

	return S_OK;
}

PlayerRageEffectTexture * PlayerRageEffectTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	PlayerRageEffectTexture*		pInstance = new PlayerRageEffectTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerRageEffectTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PlayerRageEffectTexture::Clone(void* pArg)
{
	PlayerRageEffectTexture*		pInstance = new PlayerRageEffectTexture(*this);

	/* 우ㅏㅓㄴ형데이터 외에 사본에게 필요한 추가 초기화 데이터를 처리한ㄷ.,, */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerRageEffectTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PlayerRageEffectTexture::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
