#pragma once
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract :
	public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		CTransform::TRANSFORM_DESC Transform_Desc;
		_float3 vEye;
		_float3 vAt;
		_float3 vAxisY;
		_float vFov;
		_float vAspect;
		_float vNear;
		_float vFar;

	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	class CTransform* m_Transform = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	CAMERADESC m_CameraDesc;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free()override;
	
};


END