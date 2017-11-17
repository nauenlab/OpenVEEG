#include "stdafx.h"
#include "DeckLinkAPI_h.h"

#include <boost/config.hpp>
#include <boost/thread.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "lsl_cpp.h"
#include <comutil.h>
#include <comdef.h>
#include "ComPtr.h"
#include "DeckLinkCapture.h"
#include "DeckLinkInputCallback.h"
#include "DOpenCv.h"


DeckLinkCapture::DeckLinkCapture(ComPtr<IDeckLink> deckLink)
	: deckLink_(deckLink),
	error_(S_OK)
{
	IDeckLinkInput* deckLinkInput;
	frameCount = 0;
	error_ = deckLink_->QueryInterface(IID_IDeckLinkInput,
		reinterpret_cast<void**>(&deckLinkInput));
	if (SUCCEEDED(error_))
		deckLinkInput_ = ComPtr<IDeckLinkInput>(deckLinkInput);
	else {
		errorString_ = "Could not get the callback.";
		return;
	}
}

DeckLinkCapture::DeckLinkCapture(DeckLinkCapture&& other) BOOST_NOEXCEPT
{
	deckLink_.swap(other.deckLink_);
	deckLinkInput_.swap(other.deckLinkInput_);
	deckLinkInputCallback_.swap(other.deckLinkInputCallback_);
	grabbedVideoFrame_.swap(other.grabbedVideoFrame_);
	errorString_.swap(other.errorString_);
	frameCount = 0;
	error_ = other.error_;
	other.error_ = S_OK;
}

DeckLinkCapture::~DeckLinkCapture()
{
	if (deckLink_)
		stop();
}

DeckLinkCapture& DeckLinkCapture::operator=(DeckLinkCapture&& other) BOOST_NOEXCEPT
{
	deckLink_ = other.deckLink_;
	other.deckLink_.reset();

	deckLinkInput_ = other.deckLinkInput_;
	other.deckLinkInput_.reset();

	deckLinkInputCallback_ = other.deckLinkInputCallback_;
	other.deckLinkInputCallback_.reset();

	grabbedVideoFrame_ = other.grabbedVideoFrame_;
	other.grabbedVideoFrame_.reset();

	errorString_.swap(other.errorString_);
	other.errorString_.erase();

	error_ = other.error_;
	other.error_ = S_OK;

	return *this;
}

bool DeckLinkCapture::doesSupportVideoMode(BMDDisplayMode displayMode,
	BMDPixelFormat pixelFormat)
{
	BMDDisplayModeSupport support;
	error_ = deckLinkInput_->DoesSupportVideoMode(displayMode, pixelFormat, 0,
		&support, nullptr);
	if (SUCCEEDED(error_))
		return support == bmdDisplayModeSupported ||
		support == bmdDisplayModeSupportedWithConversion;
}

bool DeckLinkCapture::Init()
{
	IDeckLinkAttributes*            deckLinkAttributes = NULL;
	IDeckLinkDisplayModeIterator*   displayModeIterator = NULL;
	IDeckLinkDisplayMode*           displayMode = NULL;
	BSTR							deviceNameBSTR = NULL;

	// Get input interface
	if (deckLink_.get()->QueryInterface(IID_IDeckLinkInput, (void**)&m_deckLinkInput) != S_OK)
		return false;
	IDeckLink* m_deckLink = deckLink_.get();
	// Check if input mode detection is supported.
	if (m_deckLink->QueryInterface(IID_IDeckLinkAttributes, (void**)&deckLinkAttributes) == S_OK)
	{
		if (deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &m_supportsFormatDetection) != S_OK)
			m_supportsFormatDetection = false;

		deckLinkAttributes->Release();
	}

	// Retrieve and cache mode list
	if (m_deckLinkInput->GetDisplayModeIterator(&displayModeIterator) == S_OK)
	{
		while (displayModeIterator->Next(&displayMode) == S_OK)
			m_modeList.push_back(displayMode);

		displayModeIterator->Release();
	}

	//Initialize LSL
	lsl_streaminfo info = lsl_create_streaminfo("VideoStream_0", "VideoStream_0", 1, 30.00, cft_int32, "");
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name", "frames");
	lsl_append_child_value(chn, "unit", "number");
	lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
	lsl_append_child_value(sync, "can_drop_samples", "true");
	outlet = lsl_create_outlet(info, 0, 360);

	return true;
}


bool DeckLinkCapture::start(BOOL m_record, int videoModeIndex) {
	BMDVideoInputFlags		videoInputFlags = bmdVideoInputFlagDefault;

	BOOL m_applyDetectedInputMode = TRUE;
	BMDDisplayMode temp;
	// Enable input video mode detection if the device supports it
	if (m_supportsFormatDetection == TRUE)
		videoInputFlags |= bmdVideoInputEnableFormatDetection;

	// Get the IDeckLinkDisplayMode from the given index
	if ((videoModeIndex < 0) || (videoModeIndex >= m_modeList.size()))
	{
		if (videoModeIndex == -1) {
			temp = bmdModeHD1080p30;
		}
		else {
			return false;
		}
	}
	else {
		temp = m_modeList[videoModeIndex]->GetDisplayMode();
	}

	// Set capture callback
	ComPtr<DeckLinkInputCallback> callback(new DeckLinkInputCallback(m_deckLinkInput ,m_applyDetectedInputMode));
	error_ = deckLinkInput_->SetCallback(callback.get());
	if (FAILED(error_)) {
		errorString_ = "Error calling SetCallback.";
		return false;
	}
	m_deckLinkInput->SetCallback(callback.get());

	// Set the video input mode
	if (m_deckLinkInput->EnableVideoInput(temp, bmdFormat8BitYUV, videoInputFlags) != S_OK) //This line must be edited to fit the correct video input mode of the camera i.e. "bmdFormat8BitYUV" must be changed.
	{
		return false;
	}
	deckLinkInputCallback_ = callback;
	if (m_record) {

		// Start the capture
		if (m_deckLinkInput->StartStreams() != S_OK)
		{
			return false;
		}
		return true;
	}
	return true;
}

void DeckLinkCapture::stop()
{
	error_ = deckLinkInput_->StopStreams();
	if (FAILED(error_)) {
		errorString_ = "Error stopping streams.";
	}
	deckLinkInputCallback_.reset();
	grabbedVideoFrame_.reset();
}

bool DeckLinkCapture::grab()
{
	if (!deckLinkInputCallback_)
		return false;
	frameCount++;
	grabbedVideoFrame_ = deckLinkInputCallback_.get()->getVideoFrame();
	if (outlet) {
		double timestamp = lsl_local_clock();
		int sample[1];
		sample[0] = frameCount;
		lsl_push_sample_itp(outlet, sample, timestamp, 1);
	}
	return true;
}

bool DeckLinkCapture::retrieve(cv::Mat& videoFrame)
{
	if (!deckLinkInputCallback_ || !grabbedVideoFrame_) {
		videoFrame.release();
		return false;
	}

	if (!VideoFrameToMat(grabbedVideoFrame_, videoFrame)) {
		error_ = E_FAIL;
		errorString_ = "Error converting";
		videoFrame.release();
		return false;
	}

	return true;
}

bool DeckLinkCapture::read(cv::Mat& videoFrame)
{
	if (grab())
		return retrieve(videoFrame);
	else {
		videoFrame.release();
		return false;
	}
}

DeckLinkCapture& DeckLinkCapture::operator >> (cv::Mat& videoFrame)
{
	read(videoFrame);
	return *this;
}
