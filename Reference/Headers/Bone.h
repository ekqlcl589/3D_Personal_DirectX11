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
	_float4x4 m_TransformationMatrix; // �� �� ���� ���
	_float4x4 m_CombinedTransformationMatrix; // �� �� * �θ� ���� ��� 
	_float4x4 m_OffsetMatrix;
	CBone* m_pParent = { nullptr };
	char m_szName[MAX_PATH] = ""; // Ư�� ���� ã�� ���� ĳ������ ���� -> ���� �� ������ "�����ڵ�"�� ���δ�

public:
	static CBone* Create(aiNode* pAINode, CBone* pParent);
	virtual void Free()override;
};

END