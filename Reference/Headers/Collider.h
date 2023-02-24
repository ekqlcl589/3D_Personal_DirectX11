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
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	HRESULT Initialize_Prototype(TYPE eType);
	HRESULT Initialize(void* pArg)override;

	void Update(_fmatrix TransformMatrix);

#ifdef _DEBUG

	HRESULT Render();

#endif // _DEBUG

private:
	TYPE m_eType = { TYPE_END };
	BoundingBox* m_pAABB = { nullptr };
	BoundingBox* m_pOriginAABB = { nullptr };
	BoundingOrientedBox* m_pOBB = { nullptr };
	BoundingOrientedBox* m_pOriginOBB = { nullptr };
	BoundingSphere* m_pSphere = { nullptr };
	BoundingSphere* m_pOriginSphere = { nullptr };

	COLLIDERDESC m_ColliderDesc;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr }; // �츮�� ���� ���̴��� �ƴ϶� debugDraw���� ���� �������� ���̴��� ������ ���� Effect
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix); // AABB�� ȸ���ϸ� ���Ǵ� �浹ü�� ���� 

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free()override;
};

END