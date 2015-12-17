#ifndef rtl_sdr_device_IMPL_H
#define rtl_sdr_device_IMPL_H

#include "rtl_sdr_device_base.h"
#include "port_impl_customized.h"

#include <boost/circular_buffer.hpp>
#include "rtl-sdr.h"

class rtl_sdr_device_i;

using namespace boost;


const int NUM_BUFFERS = 16;
const int BUFFER_SIZE = 65384;

// Based on the USRP_UHD example but made a bit more 1.9.0 looking.

// Contains information on each detected RTL instance
struct individualRTL {
	individualRTL() {
		lock = NULL;
		controlAllocationID.clear();
		frontendStatus = NULL;
		outputBuffer.resize(NUM_BUFFERS*BUFFER_SIZE / 2); // bytes / 2 since it contains shorts.
		rtlHandle = NULL;
		sri = bulkio::sri::create("");
		sri.mode = 1; // Complex data
		newSRI = false;
	}

	~individualRTL() {
		frontendStatus = NULL;
		rtlHandle = NULL;
		if (NULL != lock) {
			lock->unlock();
			// delete lock;
		}
	}

	tuner_status_struct *frontendStatus;
	BULKIO::StreamSRI sri;
	bool newSRI;
	std::vector<short> outputBuffer;
	std::string controlAllocationID;
	boost::mutex *lock;

	rtlsdr_dev_t *rtlHandle;
};

class rtl_sdr_device_i : public rtl_sdr_device_base
{
    ENABLE_LOGGING
	typedef std::vector<std::string> stringVector;
	typedef std::map<std::string, size_t> string_number_mapping;
	typedef std::map<size_t, thread*> number_threadPtr_mapping;
	typedef std::map<std::string, std::string> string_string_mapping;
	typedef std::map<std::string, stringVector > string_stringVector_mapping;
	typedef boost::mutex::scoped_lock exclusive_lock;
	friend class FRONTEND_DigitalTuner_In_i_customized;

    public:
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl);
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev);
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities);
        rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev);
        ~rtl_sdr_device_i();
        int serviceFunction();

        // Overrides for life cycle functions (config, init, alloc/dealloc, start/stop)
    	void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
        void start() throw (CF::Resource::StartError, CORBA::SystemException);
        void stop() throw (CF::Resource::StopError, CORBA::SystemException);
        void configure (const CF::Properties& configProperties)
            throw (CF::PropertySet::PartialConfiguration,
                   CF::PropertySet::InvalidConfiguration,
                   CORBA::SystemException);
        CORBA::Boolean allocateCapacity(const CF::Properties & capacities)
            throw (CORBA::SystemException,
                   CF::Device::InvalidCapacity,
                   CF::Device::InvalidState);
        void deallocateCapacity(const CF::Properties & capacities)
            throw (CORBA::SystemException,
                   CF::Device::InvalidCapacity,
                   CF::Device::InvalidState);
        CF::Device::UsageType updateUsageState();

        // Mapping and translation helpers. External string identifiers to internal numerical identifiers
		long addTunerMapping(const tuner_allocation_struct &frontend_alloc);
		long addTunerMapping(const listener_allocation_struct &frontend_listener_alloc);
		bool removeTunerMapping(std::string allocation_id);
		long getTunerMapping(std::string allocation_id);
		bool is_connectionID_valid_for_tunerNum( const size_t & tunerNum, const std::string & connectionID);
		bool is_connectionID_valid_for_streamID( const std::string & streamID, const std::string & connectionID);
		bool is_freq_valid(double req_cf, double req_bw, double req_sr, double cf, double bw, double sr);


        bool setupTuner(size_t id, const tuner_allocation_struct &params) throw (std::logic_error);
        bool enableTuner(size_t tuner_id, bool enable);
		bool removeTuner(size_t tuner_id);

    private:
        /* PROPERTIES */
        // RTL Device Management
        std::vector<individualRTL> _rtls;
        number_threadPtr_mapping _workers;

        // from USRP -- Keep service function thread safe
        mutex _prop_lock;

        // from USRP -- Provides mapping from unique allocation ID to internal tuner (channel) number
    	string_number_mapping allocationID_to_tunerNum;
    	string_number_mapping streamID_to_tunerNum;
    	mutex allocationID_MappingLock;

        /* METHODS */
        // Constructor function to open connections to all RTLs
        void _construct();
    	std::string _create_allocation_id_csv(size_t tunerNum);

        // configures a device for a specific index in the detected list of RTLs.
        void _connect_RTL(uint32_t rtl_index, individualRTL &dev);
        void _cancel_all_transfers();

        // Setting properties
        int _setCenterFrequency(const individualRTL &dev, uint32_t cf);
        int _setFrequencyCorrection(const individualRTL &dev, int ppm);
        int _setTunerGain(const individualRTL &dev, int gain);
        int _setGainMode(const individualRTL &dev, bool mode);
        int _setSampleRate(individualRTL &dev, uint32_t rate);
        int _setAGCMode(const individualRTL &dev, bool mode);
        int _setDirectSampling(const individualRTL &dev, int directSampling);
        int _setOffsetTuning(const individualRTL &dev, int mode);

        // Managing threaded data flow
        void _worker_fn(individualRTL *dev);
        static void _worker_cb_fn(unsigned char *buf, uint32_t len, void *ctx);
        void _clear_all_workers();

        inline tuner_allocation_struct feAlloc_from_feStatus(size_t tuner_id){
			tuner_allocation_struct newStruct;
			newStruct.tuner_type= _rtls[tuner_id].frontendStatus->tuner_type;
			newStruct.allocation_id= _rtls[tuner_id].controlAllocationID;
			newStruct.center_frequency= _rtls[tuner_id].frontendStatus->center_frequency;
			newStruct.bandwidth= _rtls[tuner_id].frontendStatus->bandwidth;
			newStruct.sample_rate= _rtls[tuner_id].frontendStatus->sample_rate;
			// FIXME: If it's important to have device_control relayed, then add it to tuner_status, but it's not required.
			// newStruct.device_control= _rtls[tuner_id].frontendStatus->device_control;
			newStruct.group_id = _rtls[tuner_id].frontendStatus->group_id;
			newStruct.rf_flow_id= _rtls[tuner_id].frontendStatus->rf_flow_id;
			return newStruct;
		}


        /* OVERLOAD PORTS */
        BULKIO_dataShort_Out_i_customized *dataShort_Out;
        FRONTEND_DigitalTuner_In_i_customized *DigitalTuner_provides;
};

#endif
