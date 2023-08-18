#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final :	public CComponent
{
public:
	enum TYPE {TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END};

public:
	typedef struct tagColliderDesc
	{
		_float3 vCenter;
		_float3 vScale;
		_float3 vRotation;
	}COLLIDERDESC;

	typedef struct tagOBBDesc
	{
		_float3 vCenter;
		_float3 vCenterDir[3];
		_float3 vAlignDir[3];
	}OBBDESC;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	HRESULT Initialize_Prototype(TYPE eType);
	HRESULT Initialize(void* pArg)override;

	void Update(_fmatrix TransformMatrix);

	_bool Collision(CCollider* pTargetCollider);

	_bool Collision_AABB(CCollider* pTargetCollider);
	_bool Collision_OBB(CCollider* pTargetCollider);
	_bool Collision_SPHERE(CCollider* pTargetCollider);

#ifdef _DEBUG

	HRESULT Render();

#endif // _DEBUG

public:
	COLLISIONSTATE Get_Collider() { return m_Type; }

private:
	TYPE m_eType = { TYPE_END };
	BoundingBox* m_pAABB = { nullptr };
	BoundingBox* m_pOriginAABB = { nullptr };
	BoundingOrientedBox* m_pOBB = { nullptr };
	BoundingOrientedBox* m_pOriginOBB = { nullptr };
	BoundingSphere* m_pSphere = { nullptr };
	BoundingSphere* m_pOriginSphere = { nullptr };

	COLLIDERDESC m_ColliderDesc;
	COLLISIONSTATE m_Type;
	_bool m_isColl = { false };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr }; // 우리가 만든 셰이더가 아니라 debugDraw에서 만든 정점들을 셰이더에 던지기 위한 Effect
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix); // AABB용 회전하면 ㅄ되는 충돌체를 위해 
	
	_float3 Compute_Min();
	_float3 Compute_Max();
	OBBDESC Compute_OBB();

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free()override;
};

END