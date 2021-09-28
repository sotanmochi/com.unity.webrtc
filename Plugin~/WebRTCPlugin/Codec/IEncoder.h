#pragma once

namespace unity
{
namespace webrtc
{

    namespace webrtc = ::webrtc;
    enum class codecinitializationresult
    {
        notinitialized,
        success,
        drivernotinstalled,
        driverversiondoesnotsupportapi,
        apinotfound,
        encoderinitializationfailed
    };

    class iencoder {
    public:
        virtual ~iencoder() {};        
        virtual void initv() = 0;   //can throw exception. 
        virtual void setrates(uint32_t bitrate, int64_t framerate) = 0;
        virtual void updatesettings() = 0;
        virtual bool copybuffer(void* frame) = 0;
        virtual bool encodeframe(int64_t timestamp_us) = 0;
        virtual bool issupported() const = 0;
        virtual void setidrframe() = 0;
        virtual uint64 getcurrentframecount() const = 0;
        sigslot::signal1<const webrtc::videoframe&> captureframe;

        // todo(kazuki): remove this virtual method after refactoring dummyvideoencoder
        virtual void setencoderid(const uint32_t id) { m_encoderid = id;  }
        virtual uint32_t id() const { return m_encoderid; }

        codecinitializationresult getcodecinitializationresult() const { return m_initializationresult; }
    protected:
        codecinitializationresult m_initializationresult = codecinitializationresult::notinitialized;
        uint32_t m_encoderid;
    };
    
} // end namespace webrtc
} // end namespace unity
