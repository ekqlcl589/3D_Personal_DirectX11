#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class CEffectMonster final :
	public CMonster
{
private:
	CEffectMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectMonster(const CEffectMonster& rhs);
	virtual ~CEffectMonster() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

	HRESULT Add_Coll();

	_bool m_bFadeIn = true;
	_vector m_vPosition;

public:
	static CEffectMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END