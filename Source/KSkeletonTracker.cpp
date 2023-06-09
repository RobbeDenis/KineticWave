#include "KSkeletonTracker.h"
#include <algorithm>

KSkeletonTracker::KSkeletonTracker()
    : m_pNuiSensor{nullptr}
    , m_hNextSkeletonEvent{}
{
}

KSkeletonTracker::~KSkeletonTracker()
{
    ShutDown();
}

void KSkeletonTracker::ShutDown()
{
    if (m_pNuiSensor != nullptr)
        m_pNuiSensor->NuiShutdown();
}

HRESULT KSkeletonTracker::Init()
{
    m_hNextSkeletonEvent = NULL;
    
    HRESULT hr = NuiCreateSensorByIndex(0, &m_pNuiSensor);
    if (SUCCEEDED(hr))
    {
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
        if (SUCCEEDED(hr))
        {
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);
        }
    }
    return hr;
}

void KSkeletonTracker::Update()
{
    if (m_pNuiSensor == nullptr)
        return;

    if (WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0))
    {
        NUI_SKELETON_FRAME skeletonFrame = { 0 };

        if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame)))
        {
            UpdateSkeletonData(&skeletonFrame);
        }
    }
}

void KSkeletonTracker::UpdateSkeletonData(NUI_SKELETON_FRAME* pSkeletonFrame)
{
    for (int i = 0; i < NUI_SKELETON_COUNT; ++i)
    {
        NUI_SKELETON_TRACKING_STATE trackingState = pSkeletonFrame->SkeletonData[i].eTrackingState;

        if (NUI_SKELETON_TRACKED == trackingState)
        {
            CopySkeletonData(pSkeletonFrame->SkeletonData[i]);
            break;
        }
    }
}

void KSkeletonTracker::CopySkeletonData(const NUI_SKELETON_DATA& skeleton)
{
    for (auto& [joint, position] : m_TrackingJointsMap)
    {
        position = skeleton.SkeletonPositions[joint];
    }
}

void KSkeletonTracker::SetAngle(LONG angleDegrees)
{
    if (m_pNuiSensor == nullptr)
        return;

    m_pNuiSensor->NuiCameraElevationSetAngle(std::clamp(angleDegrees, static_cast<LONG>(0), static_cast <LONG>(30)));
}

void KSkeletonTracker::AddJointForTracking(unsigned joint)
{
    m_TrackingJointsMap.emplace(std::make_pair(joint, Vector4{0.f, 0.f, 0.f, 0.f}));
}

const Vector4& KSkeletonTracker::GetTrackingJointPosition(unsigned joint) const
{
    if (m_pNuiSensor == nullptr)
        return { 0,0,0,0 };

    return m_TrackingJointsMap.at(joint);
}
