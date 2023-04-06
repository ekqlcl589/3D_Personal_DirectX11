#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
	CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	_float Get_CamDistance() const {
		return m_fCameraDistance;
	}

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	void Compute_CamDistance(_fvector vWorldPos);

private:
	_float				m_fCameraDistance = { 0.0f };


public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END