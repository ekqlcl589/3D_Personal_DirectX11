#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPipeLine  final:
	public CBase
{
	DECLARE_SINGLETON(CPipeLine)
	
public:
	enum TRANSFORMSTATE {TS_VIEW, TS_PROJ, TS_END};

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);

public:
	_float4x4 Get_Transformfloat4x4(TRANSFORMSTATE eState);
	_float4x4 Get_Transformfloat4x4Inverse(TRANSFORMSTATE eState);
	_matrix Get_TransformMatrix(TRANSFORMSTATE eState);
	_matrix	Get_TransformMatrixInverse(TRANSFORMSTATE eState);
	_float4 Get_CamPos();

	void Set_TransformMatrix(TRANSFORMSTATE eState, _fmatrix TransformMatrix);

private:
	_float4x4 m_TransformMatrix[TS_END];
	_float4x4 m_TransformMatrixInverse[TS_END];
	_float4 m_vCamPos;

public:
	virtual void Free()override;
};

END