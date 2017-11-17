#include "stdafx.h"
#include "DeckLinkAPI_h.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "ComPtr.h"
#include "DOpenCv.h"
#include "DeckLinkCreateInstance.h"

#define MAT_REFCOUNT(mat) \
 (mat.u ? (mat.u->refcount) : 0)

class CvMatDeckLinkVideoFrame : public IDeckLinkVideoFrame
{
public:
	cv::Mat mat;

	CvMatDeckLinkVideoFrame(int row, int cols)
		: mat(row, cols, CV_8UC4)
	{}

	//
	// IDeckLinkVideoFrame
	//

	long STDMETHODCALLTYPE GetWidth()
	{
		return mat.rows;
	}
	long STDMETHODCALLTYPE GetHeight()
	{
		return mat.cols;
	}
	long STDMETHODCALLTYPE GetRowBytes()
	{
		return mat.step;
	}
	BMDPixelFormat STDMETHODCALLTYPE GetPixelFormat()
	{
		return bmdFormat8BitBGRA;
	}
	BMDFrameFlags STDMETHODCALLTYPE GetFlags()
	{
		return 0;
	}
	HRESULT STDMETHODCALLTYPE GetBytes(void **buffer)
	{
		*buffer = mat.data;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetTimecode(BMDTimecodeFormat format,
		IDeckLinkTimecode **timecode)
	{
		*timecode = nullptr; return S_OK;
	}
	HRESULT STDMETHODCALLTYPE GetAncillaryData(IDeckLinkVideoFrameAncillary **ancillary)
	{
		*ancillary = nullptr; return S_OK;
	}

	//
	// IDeckLinkVideoFrame
	//

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv)
	{
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		mat.addref(); return MAT_REFCOUNT(mat);
	}
	ULONG STDMETHODCALLTYPE Release()
	{
		mat.release();
		if (MAT_REFCOUNT(mat) == 0) delete this;
		return MAT_REFCOUNT(mat);
	}
};

bool VideoFrameToMat(ComPtr<IDeckLinkVideoInputFrame> in,
	cv::Mat& out)
{
	//Example code for how to do a manual conversion if necessary.
	/*
	IDeckLinkVideoInputFrame* videoFrame = in.get();
	int height = (int) videoFrame->GetHeight();
	int width = (int) videoFrame->GetWidth();
	void* frameBytes;
	videoFrame->GetBytes(&frameBytes);
	IplImage* m_RGB = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	unsigned char* dat = (unsigned char *)frameBytes;

	uint8_t r, g, b, y[6], cb[6], cr[6];


		for (int i = 0, j = 0; i < width*height; i += 2, j += 4) {
			cb[0] = (uint8_t)(dat[j]);
			y[0] = (uint8_t)(dat[j + 1]);
			cr[0] = (uint8_t)(dat[j + 2]);
			y[1] = (uint8_t)(dat[j + 3]);
			cb[1] = cb[0];
			cr[1] = cr[0];

			for (int k = 0; k < 2; k++) {
				r = (uint8_t)((1.164*(y[k] - 16)) + (1.793*(cr[k] - 128)));
				g = (uint8_t)((1.164*(y[k] - 16)) - (0.534*(cr[k] - 128)) - (0.213*(cb[k] - 128)));
				b = (uint8_t)((1.164*(y[k] - 16)) + (2.115*(cb[k] - 128)));
				if (r < 0) {
					r = 0;
				}
				else if (r > 255) {
					r = 255;
				}
				if (g < 0) {
					g = 0;
				}
				else if (g > 255) {
					g = 255;
				}
				if (b < 0) {
					b = 0;
				}
				else if (b > 255) {
					b = 255;
				}
				m_RGB->imageData[(3 * (i + k))] = r;
				m_RGB->imageData[(3 * (i + k)) + 1] = g;
				m_RGB->imageData[(3 * (i + k)) + 2] = b;
			}
		}
		out = cv::cvarrToMat(m_RGB);
		return true;
	*/
	switch (in->GetPixelFormat()) {
	case bmdFormat8BitYUV:
	{
		void* data;
		if (FAILED(in->GetBytes(&data)))
			return false;
		cv::Mat mat = cv::Mat(in->GetHeight(), in->GetWidth(), CV_8UC2, data,
			in->GetRowBytes());
		cv::cvtColor(mat, out, CV_YUV2BGR_UYVY);
		return true;
	}
	case bmdFormat8BitBGRA:
	{
		void* data;
		if (FAILED(in->GetBytes(&data)))
			return false;
		cv::Mat mat = cv::Mat(in->GetHeight(), in->GetWidth(), CV_8UC4, data);
		cv::cvtColor(mat, out, CV_BGRA2BGR);
		return true;
	}
	default:
	{
		ComPtr<IDeckLinkVideoConversion> deckLinkVC =
			CreateVideoConversionInstance();
		if (!deckLinkVC)
			return false;

		CvMatDeckLinkVideoFrame cvMatWrapper(in->GetHeight(), in->GetWidth());
		if (FAILED(deckLinkVC->ConvertFrame(in.get(), &cvMatWrapper)))
			return false;
		cv::cvtColor(cvMatWrapper.mat, out, CV_BGRA2BGR);
		return true;
	}
	}
}