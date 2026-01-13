#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Kinect.h>
#include <vector>
#include <iostream>

class KinectSensor {
public:
    KinectSensor();
    ~KinectSensor();

    bool initialize();
    void update();
    void shutdown();

    // Accessor for your game to get the raw depth data
    const std::vector<UINT16>& getDepthData() const { return m_depthBuffer; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    // Kinect API Pointers
    IKinectSensor* m_pKinectSensor = nullptr;
    IDepthFrameReader* m_pDepthFrameReader = nullptr;

    // Data Storage
    std::vector<UINT16> m_depthBuffer;
    int m_width = 512;
    int m_height = 424;

    // Safety release helper
    template<class Interface>
    inline void SafeRelease(Interface*& pInterfaceToRelease) {
        if (pInterfaceToRelease != nullptr) {
            pInterfaceToRelease->Release();
            pInterfaceToRelease = nullptr;
        }
    }
};