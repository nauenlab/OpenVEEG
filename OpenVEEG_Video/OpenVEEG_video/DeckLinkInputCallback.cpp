#include "stdafx.h"
#include <atomic>
#include <mutex>

#include "DeckLinkAPI_h.h"

#include "ComPtr.h"
#include "DeckLinkInputCallback.h"

DeckLinkInputCallback::DeckLinkInputCallback(IDeckLinkInput* decklink, BOOL applyDetectedVideoMode)
{
	m_deckLinkInput = decklink;
	m_applyDetectedInputMode = applyDetectedVideoMode;
	refCounter_ = 1;
}

DeckLinkInputCallback::~DeckLinkInputCallback()
{
}

HRESULT DeckLinkInputCallback::QueryInterface(REFIID, LPVOID*)
{
	return S_OK; //This method should never be used.
}

ULONG DeckLinkInputCallback::AddRef()
{
	return ++refCounter_;
}

ULONG DeckLinkInputCallback::Release()
{
	ULONG refCounter = --refCounter_;
	if (refCounter == 0)
		delete this;
	return refCounter;
}

HRESULT DeckLinkInputCallback::VideoInputFormatChanged(
	BMDVideoInputFormatChangedEvents notificationEvents,
	IDeckLinkDisplayMode* newDisplayMode,
	BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
	BMDPixelFormat	pixelFormat = bmdFormat8BitYUV;

	// Restart capture with the new video mode if told to
	if (!m_applyDetectedInputMode)
		goto bail;

	if (detectedSignalFlags & bmdDetectedVideoInputRGB444)
		pixelFormat = bmdFormat10BitRGB;

	// Stop the capture
	m_deckLinkInput->StopStreams();

	// Set the video input mode
	if (m_deckLinkInput->EnableVideoInput(newDisplayMode->GetDisplayMode(), pixelFormat, bmdVideoInputEnableFormatDetection) != S_OK)
	{
		goto bail;
	}

	// Start the capture
	if (m_deckLinkInput->StartStreams() != S_OK)
	{
		goto bail;
	}

bail:
	return S_OK;
}

HRESULT DeckLinkInputCallback::VideoInputFrameArrived(
	IDeckLinkVideoInputFrame* videoFrame,
	IDeckLinkAudioInputPacket* audioPacket)
{
	std::lock_guard<std::mutex> lock(videoFrameMutex_);
	videoFrame_ = ComPtr<IDeckLinkVideoInputFrame>(videoFrame, true);
	videoFrameArrive_.notify_all();
	return S_OK;
}

ComPtr<IDeckLinkVideoInputFrame> DeckLinkInputCallback::getVideoFrame()
{
	std::unique_lock<std::mutex> lock(videoFrameMutex_);

	if (!videoFrame_)
		videoFrameArrive_.wait(lock);

	ComPtr<IDeckLinkVideoInputFrame> grabbedFrame_ = videoFrame_;
	videoFrame_.reset();

	return grabbedFrame_;
}