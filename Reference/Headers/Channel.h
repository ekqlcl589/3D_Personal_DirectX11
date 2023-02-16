#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void Invalidate_Transform(_double TrackPosition);

private:
	char m_szName[MAX_PATH] = "";
	class CBone* m_pBone = { nullptr };

private: 
	_uint m_iNumKeyFrames = { 0 };
	 vector<KEYFRAME> m_vecKeyFrame;

private:
	_uint m_iCurrKeyFrame = { 0 };

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free()override;
};

END