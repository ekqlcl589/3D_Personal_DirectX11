#include "stdafx.h"
#include "..\Public\SkillIconRage.h"

#include "GameInstance.h"
#include "TSPlayer.h"

CSkillIconRage::CSkillIconRage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CSkillIconRage::CSkillIconRage(const CSkillIconRage & rhs)
	: CUI(rhs)
	, m_fPosition(rhs.m_fPosition)
{
}

HRESULT CSkillIconRage::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillIconRage::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_fPosition));
	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixScaling(100.f, 100.f, 1.f) * XMMatrixTranslation(m_fX - m_fPosition.x, -m_fY + m_fPosition.y, 0.1f));

	XMStoreFloat4x4(&m_ViewMatrix,
		XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f));


	return S_OK;
}

void CSkillIconRage::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayer = pInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	m_CoolTime = static_cast<CTSPlayer*>(pPlayer)->Get_Info().m_RageSkill;

	cout << m_CoolTime << endl;

	RELEASE_INSTANCE(CGameInstance);

}

void CSkillIconRage::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CSkillIconRage::Render()
{
	if (FAILED(SetUp_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillIconRage::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rage_Skill"),
		TEXT("Com_ESKILL_Texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillIconRage::SetUp_ShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	float uiCoolTime = m_CoolTime*0.04f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fData", &uiCoolTime, sizeof(float))))
		return E_FAIL;

	return S_OK;
}

CSkillIconRage * CSkillIconRage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _float3 fPos)
{
	CSkillIconRage * pInstance = new CSkillIconRage(pDevice, pContext);
	pInstance->m_fPosition = fPos;

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSkillIconRage Create Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkillIconRage::Clone(void * pArg)
{
	CSkillIconRage * pInstance = new CSkillIconRage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillIconRage Clone Fail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillIconRage::Free()
{
	Safe_Release(m_pShaderCom);
	__super::Free();
}