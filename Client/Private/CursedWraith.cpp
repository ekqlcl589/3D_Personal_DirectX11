#include "stdafx.h"
#include "..\Public\CursedWraith.h"
#include "GameInstance.h"
#include "MonsterWeapon.h"

CCursedWraith::CCursedWraith(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CCursedWraith::CCursedWraith(const CCursedWraith & rhs)
	: CMonster(rhs)
{
}

HRESULT CCursedWraith::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursedWraith::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Coll()))
		return	E_FAIL;

	_float3 fPosition = { 5.f, 0.f, 5.f }; // �ӽ� ��ġ��
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&fPosition));

	m_tInfo._MaxHp = 1000.f;
	m_tInfo._Hp = 1000.f;

	m_tInfo.CurrAnim = CW_Wait;
	m_pModelCom->SetUp_Animation(m_tInfo.CurrAnim);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(195.0f));

	return S_OK;
}

void CCursedWraith::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Animation_State(TimeDelta);

	//Attack_Go(TimeDelta); // �ִϸ��̼��� ���� �Ǹ鼭 ������ ���� �ϴϱ� �߰��� �´�?

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

void CCursedWraith::LateTick(_double TimeDelta)
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

HRESULT CCursedWraith::Render()
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

void CCursedWraith::OnCollision(CGameObject * pObj)
{
}

void CCursedWraith::Animation_State(_double TimeDelta)
{
}

void CCursedWraith::Animation(CURSEDWRAITHSTATE eType)
{
	if (m_tInfo.CurrAnim != m_tInfo.PrevAnim)
	{
		switch (eType)
		{
		case Engine::CW_Air:
			break;
		case Engine::CW_Avoid:
			break;
		case Engine::CW_Run:
			break;
		case Engine::CW_Start:
			break;
		case Engine::CW_Wait:
			break;
		case Engine::CW_Walk:
			break;
		case Engine::CW_Down_F_F:
			break;
		case Engine::CW_DIE:
			break;
		case Engine::CW_RTBLOW_AIR_F:
			break;
		case Engine::CW_RTBLOW_AIR_FALL_F:
			break;
		case Engine::CW_RBBLOW_AIR_LENDING:
			break;
		case Engine::CW_RTBLOW_AIR_R:
			break;
		case Engine::CW_RTBLOW_DOWN_F:
			break;
		case Engine::CW_RTCHASE_BIG_F:
			break;
		case Engine::CW_RTDOWN_AIR_LENDING_F_F:
			break;
		case Engine::CW_RTDOWN_F_F:
			break;
		case Engine::CW_RTSTAND_BIG_F:
			break;
		case Engine::CW_SKILL_01:
			break;
		case Engine::CW_SKILL_02:
			break;
		case Engine::CW_SKILL_03:
			break;
		case Engine::CW_SKILL_04:
			break;
		case Engine::CW_SKILL_05:
			break;
		case Engine::CW_SKILL_07:
			break;
		case Engine::CW_SKILL_09_1:
			break;
		case Engine::CW_SKILL_09_2:
			break;
		case Engine::CW_STANDUP_F:
			break;
		case Engine::CW_STUN_LOOP:
			break;
		case Engine::CW_ANIMEND:
			break;
		default:
			break;
		}
		m_pModelCom->SetUp_Animation(m_iAnimIndex);
		m_tInfo.PrevAnim = m_tInfo.CurrAnim;
	}
}

void CCursedWraith::Avoid(_double TimeDelta)
{
}

HRESULT CCursedWraith::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss3"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCursedWraith::SetUp_ShaderResources()
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

HRESULT CCursedWraith::Add_Coll()
{
	return S_OK;
}

CCursedWraith * CCursedWraith::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCursedWraith*		pInstance = new CCursedWraith(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCursedWraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCursedWraith::Clone(void * pArg)
{
	CCursedWraith*		pInstance = new CCursedWraith(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCursedWraith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCursedWraith::Free()
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
