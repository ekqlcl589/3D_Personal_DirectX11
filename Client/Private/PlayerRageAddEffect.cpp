#include "stdafx.h"
#include "..\Public\PlayerRageAddEffect.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "TwoHandedSword.h"
#include "Level_Mgr.h"

CPlayerRageAddEffect::CPlayerRageAddEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerRageAddEffect::CPlayerRageAddEffect(const CPlayerRageAddEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerRageAddEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerRageAddEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixRotationY(90.f) * m_pTransformCom->Get_WorldMatrix() * CTwoHandedSword::WorldMatrix);

	_matrix World = XMLoadFloat4x4(&m_WorldMatrix);

	_vector vPos = World.r[3];

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.0f));

	m_fDissolveTime = 4.f;

	m_eCollisionState = OBJ_PLAYER_RAGESKILL;

	m_iObjID = 14;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_Collider(m_eCollisionState, Set_ObjID(m_iObjID), this);

	RELEASE_INSTANCE(CGameInstance);

	m_UVData = 25.f;

	return S_OK;
}

void CPlayerRageAddEffect::Tick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::Tick(TimeDelta);

		//m_UVData -= TimeDelta;


		//if (m_UVData <= 0.f)
		//	m_UVData = 10.f;

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(XMLoadFloat4x4(&m_WorldMatrix));

		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		CLevel_Mgr* L = GET_INSTANCE(CLevel_Mgr);

		CGameObject* pPlayer = nullptr;

		if (L->Get_LevelIndex() == LEVEL_GAMEPLAY)
			pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		else if (L->Get_LevelIndex() == LEVEL_GAMEPLAY2)
			pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY2, TEXT("Layer_Player"));

		RELEASE_INSTANCE(CLevel_Mgr);

		RELEASE_INSTANCE(CGameInstance);

		m_bActive = static_cast<CTSPlayer*>(pPlayer)->Get_RageState();
		m_UVData -= 2.0 * TimeDelta;

		if (!m_bActive)
		{
			m_fDissolveTime -= TimeDelta;

			fDissolveAmount = Lerp(1.f, 0.f, m_fDissolveTime / 4.f);

			if (m_fDissolveTime <= 0.f)
				Set_Dead();

		}

	}

}

void CPlayerRageAddEffect::LateTick(_double TimeDelta)
{
	if (!m_bDead)
	{
		__super::LateTick(TimeDelta);


		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}
	}
}

HRESULT CPlayerRageAddEffect::Render()
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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayerRageAddEffect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Rage_Add_Effect"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vScale = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vScale.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerRageAddEffect::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAmount", &fDissolveAmount, sizeof(float))))
		return E_FAIL;


	float uiCoolTime = m_UVData;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVData", &uiCoolTime, sizeof(float))))
		return E_FAIL;

	bool bUVOn = true;

	if (uiCoolTime <= 0.f)
		bUVOn = false;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVOn", &bUVOn, sizeof(bool))))
		return E_FAIL;

	return S_OK;
}

CPlayerRageAddEffect * CPlayerRageAddEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerRageAddEffect*		pInstance = new CPlayerRageAddEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerRageAddEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerRageAddEffect::Clone(void * pArg)
{
	CPlayerRageAddEffect*		pInstance = new CPlayerRageAddEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerRageAddEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerRageAddEffect::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pRendererCom);
}
