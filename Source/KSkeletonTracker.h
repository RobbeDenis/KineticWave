#pragma once

#define WIN32_LEAN_AND_MEAN 
#define INC_OLE2
#include <Windows.h>
#include <NuiApi.h>
#include <unordered_map>

class KSkeletonTracker
{
public:
	KSkeletonTracker();
	~KSkeletonTracker();

	HRESULT Init();
	void Update();
	void ShutDown();

	void SetAngle(LONG angleDegrees);
	void AddJointForTracking(unsigned joint);
	const Vector4& GetTrackingJointPosition(unsigned joint) const;

private:
	void UpdateSkeletonData(NUI_SKELETON_FRAME* pSkeletonFrame);
	void CopySkeletonData(const NUI_SKELETON_DATA& skeleton);

	HANDLE m_hNextSkeletonEvent;
	INuiSensor* m_pNuiSensor;
	std::unordered_map<unsigned, Vector4> m_TrackingJointsMap;
};

