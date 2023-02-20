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
	void Invalidate_Transform(_double TrackPosition, _uint* pCurrKeyFrame, const vector<class CBone*>& Bones);

private:
	char m_szName[MAX_PATH] = "";
	_uint m_iBoneIndex = { 0 };

private: 
	_uint m_iNumKeyFrames = { 0 };
	 vector<KEYFRAME> m_vecKeyFrame;

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free()override;
};

END