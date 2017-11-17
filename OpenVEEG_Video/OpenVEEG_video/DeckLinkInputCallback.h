#ifndef DeckLinkInputCallback_H
#define DeckLinkInputCallback_H
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "DeckLinkAPI_h.h"

#include "ComPtr.h"


class DeckLinkInputCallback : public IDeckLinkInputCallback
{
public:
	DeckLinkInputCallback(IDeckLinkInput* decklink ,BOOL applyDetectedVideoMode);
	~DeckLinkInputCallback();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(
		BMDVideoInputFormatChangedEvents notificationEvents,
		IDeckLinkDisplayMode* newDisplayMode,
		BMDDetectedVideoInputFormatFlags detectedSignalFlags);
	HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame,
		IDeckLinkAudioInputPacket* audioPacket);

	ComPtr<IDeckLinkVideoInputFrame> getVideoFrame();

private:
	std::atomic_ulong refCounter_;
	BOOL m_applyDetectedInputMode;
	IDeckLinkInput* m_deckLinkInput;
	std::mutex videoFrameMutex_;
	std::condition_variable videoFrameArrive_;
	ComPtr<IDeckLinkVideoInputFrame> videoFrame_;
};


class DeckLinkCapture;

#endif
