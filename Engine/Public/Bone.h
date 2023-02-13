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

private:
	_float4x4 m_TransformationMatrix; // 내 뼈 만의 행렬
	_float4x4 m_CombinedTransformationMatrix; // 내 뼈 * 부모 뼈의 행렬 

private:
	char m_szName[MAX_PATH] = ""; // 특정 뼈를 찾기 위한 캐릭터형 변수 -> 무기 를 몸통의 "웨폰핸드"에 붙인다

public:
	static CBone* Create(aiNode* pAINode);
	virtual void Free()override;
};

END