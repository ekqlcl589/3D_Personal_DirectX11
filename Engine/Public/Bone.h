#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBone final :
	public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT Initialize(aiNode* pAINode, CBone* pParent);
	void Invalidate_CombinedMatrix();

public:
	const char* Get_Name() const { return m_szName; }

	_float4x4 Get_CombinedTransformMatrix() const { return m_CombinedTransformationMatrix; }

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

	_float4x4 Get_OffsetMatrix() const { return m_OffsetMatrix; }

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix); }

private:
	_float4x4 m_TransformationMatrix; // 내 뼈 만의 행렬
	_float4x4 m_CombinedTransformationMatrix; // 내 뼈 * 부모 뼈의 행렬 
	_float4x4 m_OffsetMatrix;
	CBone* m_pParent = { nullptr };
	char m_szName[MAX_PATH] = ""; // 특정 뼈를 찾기 위한 캐릭터형 변수 -> 무기 를 몸통의 "웨폰핸드"에 붙인다

public:
	static CBone* Create(aiNode* pAINode, CBone* pParent);
	virtual void Free()override;
};

END