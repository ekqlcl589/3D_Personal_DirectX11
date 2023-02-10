#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final :
	public CComponent
{
public:
	enum STATE {STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float fSpeed;
		_float fRotation;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }

	void Set_State(STATE eState, _fvector vState);

	_float3 Get_Scale();
	void Set_Scale(const _float3& vScale);
	void Scaling(const _float3 & vScale);

	void Set_TransformDesc(const TRANSFORM_DESC& TransformDesc)
	{
		m_TransformDesc = TransformDesc;
	}

	_matrix Get_WorldMatrixInverse() { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }
	_matrix Get_WorldMatrix() { return XMLoadFloat4x4(&m_WorldMatrix); }
public:
	HRESULT Initialize_Prototype()override;
	HRESULT Initialize(void* pArg)override;

public:
	void Go_Straight(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_Back(_double TimeDelta);

	void Rotation(_fvector vAxis, _float fAngle);
	void Turn(_fvector vAxis, _double TimeDelta);

	void LookAt(_fvector vTargetPos);
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDist = 0.1f);
	_float Compute_Distance(_fvector vTargetPos);

public:
	HRESULT SetUp_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

private:
	_float4x4 m_WorldMatrix;
	TRANSFORM_DESC m_TransformDesc;
	_float3 m_fAngle = { 0.f, 0.f, 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg)override;
	virtual void Free()override;
};

END