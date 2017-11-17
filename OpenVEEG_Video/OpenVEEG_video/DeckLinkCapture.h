#include <boost/config.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "lsl_cpp.h"
#include "DeckLinkAPI_h.h"
#include <vector>
#include "ComPtr.h"
#include "DeckLinkInputCallback.h"

using namespace lsl;

class DeckLinkCapture
{
public:
	DeckLinkCapture(ComPtr<IDeckLink> deckLink);
	DeckLinkCapture(DeckLinkCapture&& other) BOOST_NOEXCEPT;
	~DeckLinkCapture();

	DeckLinkCapture& operator=(DeckLinkCapture&& other) BOOST_NOEXCEPT;

	bool doesSupportVideoMode(BMDDisplayMode displayMode,
		BMDPixelFormat pixelFormat);

	bool Init();

	bool start(BOOL m_record, int videoModeIndex);
	void stop();
	lsl_outlet outlet;
	std::vector<IDeckLinkDisplayMode*> m_modeList;
	bool grab();
	bool retrieve(cv::Mat& videoFrame);

	bool read(cv::Mat& videoFrame);
	DeckLinkCapture& operator >> (cv::Mat& videoFrame);

	HRESULT error() const;
	const std::string& errorString() const;
	std::string m_deviceName;
private:
	int frameCount;
	ComPtr<IDeckLink> deckLink_;
	ComPtr<IDeckLinkInput> deckLinkInput_;
	ComPtr<DeckLinkInputCallback> deckLinkInputCallback_;
	ComPtr<IDeckLinkVideoInputFrame> grabbedVideoFrame_;
	IDeckLinkInput* m_deckLinkInput;
	BOOL m_supportsFormatDetection;
	HRESULT error_;
	int width;
	int height;
	std::string errorString_;
	std::vector<IDeckLinkDisplayMode*> modeList;
	friend HRESULT DeckLinkInputCallback::VideoInputFrameArrived(
		IDeckLinkVideoInputFrame* videoFrame,
		IDeckLinkAudioInputPacket* audioPacket);

	DeckLinkCapture(DeckLinkCapture const&) = delete;
	DeckLinkCapture& operator=(DeckLinkCapture const&) = delete;
};

inline HRESULT DeckLinkCapture::error() const
{
	return error_;
}

inline const std::string& DeckLinkCapture::errorString() const
{
	return errorString_;
}
