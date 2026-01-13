#include "../../include/engine/KinectSensor.hpp"

KinectSensor::KinectSensor() {
    // Reserve space for 512x424 depth image
    m_depthBuffer.resize(m_width * m_height, 0);
}

KinectSensor::~KinectSensor() {
    shutdown();
}

bool KinectSensor::initialize() {
    HRESULT hr;

    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr) || !m_pKinectSensor) {
        std::cerr << "Failed to get default Kinect sensor!" << std::endl;
        return false;
    }

    hr = m_pKinectSensor->Open();
    if (SUCCEEDED(hr)) {
        IDepthFrameSource* pDepthFrameSource = nullptr;
        hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);

        if (SUCCEEDED(hr)) {
            hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
        }
        SafeRelease(pDepthFrameSource);
    }

    if (FAILED(hr) || !m_pDepthFrameReader) {
        std::cerr << "Failed to open Kinect Depth Reader!" << std::endl;
        return false;
    }

    std::cout << "Kinect Initialized Successfully." << std::endl;
    return true;
}

void KinectSensor::update() {
    if (!m_pDepthFrameReader) return;

    IDepthFrame* pDepthFrame = nullptr;
    HRESULT hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);

    if (SUCCEEDED(hr)) {
        IFrameDescription* pFrameDescription = nullptr;
        int nWidth = 0;
        int nHeight = 0;
        UINT nBufferSize = 0;
        UINT16* pBuffer = nullptr;

        // Get Description
        if (SUCCEEDED(pDepthFrame->get_FrameDescription(&pFrameDescription))) {
            pFrameDescription->get_Width(&nWidth);
            pFrameDescription->get_Height(&nHeight);
            SafeRelease(pFrameDescription);
        }

        // Access the raw buffer
        if (SUCCEEDED(pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer))) {
            // Check if size matches what we expect
            if (nWidth == m_width && nHeight == m_height) {
                // Copy data to our local vector so the game can use it safely
                // (pBuffer is only valid inside this scope, so we MUST copy)
                std::memcpy(m_depthBuffer.data(), pBuffer, nBufferSize * sizeof(UINT16));
            }
        }
    }

    SafeRelease(pDepthFrame);
}

void KinectSensor::shutdown() {
    SafeRelease(m_pDepthFrameReader);
    if (m_pKinectSensor) {
        m_pKinectSensor->Close();
    }
    SafeRelease(m_pKinectSensor);
}