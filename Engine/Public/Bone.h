#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final :
	public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT Initialize(aiNode* pAINode);

public:
	const char* Get_Name() const { return m_szName; }

	_float4x4 Get_CombinedTransformMatrix() const { return m_CombinedTransformationMatrix; }

private:
	_float4x4 m_TransformationMatrix; // �� �� ���� ���
	_float4x4 m_CombinedTransformationMatrix; // �� �� * �θ� ���� ��� 
	char m_szName[MAX_PATH] = ""; // Ư�� ���� ã�� ���� ĳ������ ���� -> ���� �� ������ "�����ڵ�"�� ���δ�

public:
	static CBone* Create(aiNode* pAINode);
	virtual void Free()override;
};

END