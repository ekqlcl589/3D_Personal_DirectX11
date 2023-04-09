#include "stdafx.h"
#include "..\Public\EffectMonster.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "AddCollision.h"

CEffectMonster::CEffectMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CEffectMonster::CEffectMonster(const CEffectMonster & rhs)
	: CMonster(rhs)
{
}

HRESULT CEffectMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Coll()))
		return E_FAIL;

	memcpy(&m_vPosition, pArg, sizeof(_vector));

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CTransform* pPlayerTransform = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	RELEASE_INSTANCE(CGameInstance);

	_float3 fPos;
	XMStoreFloat3(&fPos, m_vPosition);

	fPos.x += 3.f;
	//fPos.y += 5.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPos));
	
	m_pTransformCom->LookAt(m_vTargetPos);

	m_pModelCom->SetUp_Animation(0);

	m_fDissolveTime = 5.f;

	m_bFadeIn = true;

	return S_OK;
}

void CEffectMonster::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		for (_uint i = 0; i < WEAPON_END; i++)
		{
			for (auto& pWeapon : m_vecWeapon[i])
			{
				if (nullptr != pWeapon)
				{
					pWeapon->Tick(TimeDelta);
				}
			}
		}


		if (true == m_pModelCom->Get_AnimFinished())
		{
			m_fDissolveTime -= TimeDelta;
			
			fDissolveAmount = Lerp(1.f, 0.f, m_fDissolveTime / 5.f);
			m_bFadeIn = false;

			if (m_fDissolveTime <= 0.f)
				Set_Dead();
		}

	}
}

void CEffectMonster::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);

		for (_uint i = 0; i < WEAPON_END; i++)
		{
			for (auto& pWeapon : m_vecWeapon[i])
			{
				if (nullptr != pWeapon)
					pWeapon->LateTick(TimeDelta);
			}
		}
		
		if (m_bFadeIn)
			m_pModelCom->Play_Animation(TimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	}
}

HRESULT CEffectMonster::Render()
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

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Render();

#endif

	return S_OK;
}

HRESULT CEffectMonster::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Model_Effect_Monster"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY2, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectMonster::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->SetUp_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAmount", &fDissolveAmount, sizeof(float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectMonster::Add_Coll()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CBone* pBoneR = m_pModelCom->Get_BonePtr("Bone19");

	if (nullptr == pBoneR)// || nullptr == pBoneSpine)
		return E_FAIL;

	CAddCollision::COLLDESC WeaponDesc1 = { pBoneR, m_pModelCom->Get_LocalMatrix(), m_pTransformCom };
	Safe_AddRef(pBoneR);

	CGameObject* pWeaponR = pInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Coll"), &WeaponDesc1);

	if (nullptr == pWeaponR)
		return E_FAIL;

	m_vecWeapon[WEAPON_MONSTERR].push_back(pWeaponR);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEffectMonster * CEffectMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectMonster*		pInstance = new CEffectMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectMonster::Clone(void * pArg)
{
	CEffectMonster*		pInstance = new CEffectMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectMonster::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	for (_uint i = 0; i < WEAPON_END; ++i)
	{
		for (auto& pWeapon : m_vecWeapon[i])
			Safe_Release(pWeapon);

		m_vecWeapon[i].clear();
	}

	Safe_Release(m_pTextureCom);

}
