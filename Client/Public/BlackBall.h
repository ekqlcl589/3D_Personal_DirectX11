#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CTransform;
class CModel;
class CBone;
class CCollider;

END

BEGIN(Client)

class CBlackBall final :
	public CGameObject
{
public:
	enum TYPE { TYPE_8, TYPE_3, TYPE_DDEBASI, TYPE_END };

	typedef struct tagBallDesc
	{
		_vector vPosition; // 생성될 위치의 몬스터 위치 
		_vector vLook; // 날아갈 방향
		_float3 fOriginPos; // 생성 위치
		TYPE eType;
	}BALLDESC;
private:
	CBlackBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CBlackBall(const CBlackBall& rhs);
	virtual ~CBlackBall() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CGameObject* pObj)override;
	virtual void EnterCollision(CGameObject * pObj)override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bStart = { false };

private:
	_vector m_vPosition;
	_vector m_vLook;
	_float3 m_fPos;

	_bool Dead = false;
	_float Damage = 0.f;
	_double m_LifeTime = 0.f;

	_bool m_bTakeHit = false;
	_bool m_Hit = false;

private:
	BALLDESC m_BallDesc;

public:
	static CBlackBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END