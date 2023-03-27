#include "stdafx.h"
#include "..\Public\Player_Basic_Combo.h"
#include "GameInstance.h"
#include "TSPlayer.h"
#include "TwoHandedSword.h"
//
CPlayer_Basic_Combo::CPlayer_Basic_Combo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer_Basic_Combo::CPlayer_Basic_Combo(const CPlayer_Basic_Combo & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_Basic_Combo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Basic_Combo::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bActive = true;

	m_pModelCom->SetUp_Animation(0);

	return S_OK;
}

void CPlayer_Basic_Combo::Tick(_double TimeDelta)
{
	CGameInstance* p = GET_INSTANCE(CGameInstance);
	CGameObject* pOwner = nullptr;

	pOwner = p->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_bActive = static_cast<CTSPlayer*>(pOwner)->Get_Attack();

	RELEASE_INSTANCE(CGameInstance);

	if (m_bActive)
	{
		__super::Tick(TimeDelta);

		if (nullptr != m_pColliderCom)
			m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		if (m_pModelCom->Get_AnimTimeAcc() / (m_pModelCom->Get_AnimDuration() / 2) + 14.9)
			m_AnimStart = false;
	}
}

void CPlayer_Basic_Combo::LateTick(_double TimeDelta)
{
	if (m_bActive)
	{
		__super::LateTick(TimeDelta);

		Set_Transform();

		if(m_AnimStart)
			m_pModelCom->Play_Animation(TimeDelta);

		if (nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}

	}
}

HRESULT CPlayer_Basic_Combo::Render()
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
			m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DepthTexture", i, aiTextureType_DIFFUSE);
			/*m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);
			m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);*/

			m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

			m_pShaderCom->Begin(0);

			m_pModelCom->Render(i);
		}
	}

	return S_OK;
}

_bool CPlayer_Basic_Combo::FadeInOut()
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
}

void CPlayer_Basic_Combo::Set_Transform()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CTransform* pWeaponTrasnfrom = static_cast<CTransform*>(pInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_TS"), TEXT("Com_Transform")));

	CGameObject* pWeapon = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Weapon_TS"));

	RELEASE_INSTANCE(CGameInstance);


	_vector vPosition = pWeaponTrasnfrom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = XMVector3Normalize(pWeaponTrasnfrom->Get_State(CTransform::STATE_LOOK));
	_vector vRight = XMVector3Normalize(pWeaponTrasnfrom->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(pWeaponTrasnfrom->Get_State(CTransform::STATE_UP));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);

	_matrix WeaponMatrix = pWeaponTrasnfrom->Get_WorldMatrix();

	_float4x4 Weapon = static_cast<CTwoHandedSword*>(pWeapon)->Get_WeaponMatrix();

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&Weapon));
}

HRESULT CPlayer_Basic_Combo::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Basic"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Basic_Combo::SetUp_ShaderResources()
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

	//if (FAILED(pInstance->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CPlayer_Basic_Combo * CPlayer_Basic_Combo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Basic_Combo*		pInstance = new CPlayer_Basic_Combo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Basic_Combo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Basic_Combo::Clone(void * pArg)
{
	CPlayer_Basic_Combo*		pInstance = new CPlayer_Basic_Combo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer_Basic_Combo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Basic_Combo::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
