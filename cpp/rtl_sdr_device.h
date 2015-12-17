#ifndef rtl_sdr_device_IMPL_H
#define rtl_sdr_device_IMPL_H

#include "rtl_sdr_device_base.h"

#include <boost/circular_buffer.hpp>
#include "rtl-sdr.h"

class rtl_sdr_device_i;

using namespace boost;

class rtl_sdr_device_i : public rtl_sdr_device_base
{
    ENABLE_LOGGING
    public:
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl);
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev);
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities);
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev);
        ~rtl_sdr_device_i();
        int serviceFunction();

        // Implement start, stop, and configure
        void start() throw (CF::Resource::StartError, CORBA::SystemException);
        void stop() throw (CF::Resource::StopError, CORBA::SystemException);
        void configure (const CF::Properties& configProperties)
            throw (CF::PropertySet::PartialConfiguration,
                   CF::PropertySet::InvalidConfiguration,
                   CORBA::SystemException);

        // For hardware reservation flag
        CORBA::Boolean allocateCapacity(const CF::Properties & capacities)
                    throw (CORBA::SystemException,
                           CF::Device::InvalidCapacity,
                           CF::Device::InvalidState);
		void deallocateCapacity(const CF::Properties & capacities)
			throw (CORBA::SystemException,
				   CF::Device::InvalidCapacity,
				   CF::Device::InvalidState);

		// For grabbing an RTL Handle on launch (execparam).
		void initialize()
			throw (CF::LifeCycle::InitializeError,
				   CORBA::SystemException);

    private:
        // Constructor function.
        void _construct();

        // Device
        rtlsdr_dev_t *_rtl_device;
        int _setRtlDevice();

        // Storage and threaded worker
        circular_buffer<short> *_buffer;
        bool _isNewStream;
        bool _doUpdateSRI;
        thread *_worker;
        mutex _theMutex;

        // Local storage of the last-received switch pattern.
        long _switch_pattern;

        void _worker_fn();
        void _worker_process_fn(unsigned char *buf, uint32_t len);
        static void _worker_cb_fn(unsigned char *buf, uint32_t len, void *ctx);


        // Property listeners
        int _setCenterFrequency();
        int _setFrequencyCorrection();
        int _setTunerGain();
        int _setIFGains();
        int _setGainMode();
        int _setSampleRate();
        int _setAGCMode();
        int _setDirectSampling();
        int _setOffsetTuning();
        int _setBufferSize();

        void _receivedMessage(const std::string &id,
           const switch_pattern_struct &msg);

        inline void _setAllProperties() {
        	if (NULL != _rtl_device) {
            	LOG_INFO(rtl_sdr_device_i, "Setting all properties...");
				_setCenterFrequency();
				_setSampleRate();
				_setGainMode();
				_setTunerGain();
				_setFrequencyCorrection();
				_setIFGains();
				_setAGCMode();
				_setDirectSampling();
				_setOffsetTuning();
				_setBufferSize();
        	}
        }

};

#endif
