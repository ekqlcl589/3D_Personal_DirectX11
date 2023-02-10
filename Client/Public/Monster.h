#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Transform.h"
// 추후 몬스터들의 부모로 사용하고 지금은 테스트 용으로 만들어 둠
BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CMonster final :	public CGameObject
{
public:
	typedef struct MonsterState
	{
		_float3 fPos;
		_uint iMonsterNum;
		_float3 fScale;

		CTransform::TRANSFORM_DESC transformDesc;
	}MONSTERSTATE;

private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	MONSTERSTATE m_MonsterState;
	_uint iId = { 0 };

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END