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

    IColorFrameSource* pColorSource = nullptr;
    hr = m_pKinectSensor->get_ColorFrameSource(&pColorSource);

    if (SUCCEEDED(hr)) {
        hr = pColorSource->OpenReader(&m_pColorReader);
    }
    SafeRelease(pColorSource);

    if (FAILED(hr) || !m_pColorReader) {
        std::cerr << "Failed to open Kinect Color Reader!" << std::endl;
        return false;
    }

    // Allocate memory for 1920x1080 pixels (4 bytes per pixel: RGBA)
    m_colorBuffer.resize(m_colorWidth * m_colorHeight * 4);

    return true;

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

    if (!m_pColorReader) return;

    IColorFrame* pColorFrame = nullptr;
    hr = m_pColorReader->AcquireLatestFrame(&pColorFrame);

    if (SUCCEEDED(hr)) {
        IFrameDescription* pDescription = nullptr;
        pColorFrame->get_FrameDescription(&pDescription);

        // Safety check resolution
        int width, height;
        pDescription->get_Width(&width);
        pDescription->get_Height(&height);

        if (width == m_colorWidth && height == m_colorHeight) {
            // CONVERT to RGBA
            // This function converts the raw Kinect YUY2 format into standard RGBA
            // that you can upload directly to an OpenGL texture.
            hr = pColorFrame->CopyConvertedFrameDataToArray(
                (UINT)m_colorBuffer.size(),
                m_colorBuffer.data(),
                ColorImageFormat_Rgba
            );
        }

        SafeRelease(pDescription);
    }
    SafeRelease(pColorFrame);
}

void KinectSensor::shutdown() {
    SafeRelease(m_pDepthFrameReader);
    SafeRelease(m_pColorReader);
    if (m_pKinectSensor) {
        m_pKinectSensor->Close();
    }
    SafeRelease(m_pKinectSensor);
}