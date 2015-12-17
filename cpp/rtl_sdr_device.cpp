
/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

 	Source: rtl_sdr_device.spd.xml
 	Generated on: Tue Sep 24 09:35:57 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

**************************************************************************/

#include "rtl_sdr_device.h"
PREPARE_LOGGING(rtl_sdr_device_i)


using namespace std;


rtl_sdr_device_i::rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl) :
    rtl_sdr_device_base(devMgr_ior, id, lbl, sftwrPrfl)
{
	_construct();
}

rtl_sdr_device_i::rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev) :
    rtl_sdr_device_base(devMgr_ior, id, lbl, sftwrPrfl, compDev)
{
	_construct();
}

rtl_sdr_device_i::rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities) :
    rtl_sdr_device_base(devMgr_ior, id, lbl, sftwrPrfl, capacities)
{
	_construct();
}

rtl_sdr_device_i::rtl_sdr_device_i(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev) :
    rtl_sdr_device_base(devMgr_ior, id, lbl, sftwrPrfl, capacities, compDev)
{
	_construct();
}

rtl_sdr_device_i::~rtl_sdr_device_i()
{
	// TODO: Reconsider how to better handle stop -> destruction
	_cancel_all_transfers();
	for (size_t i = 0; i < _rtls.size(); i++) {
		if (NULL != _rtls[i].rtlHandle) {
			rtlsdr_close(_rtls[i].rtlHandle);
		}
	}
	_rtls.clear();

	_clear_all_workers();
}

/***********************************************************************************************

    Basic functionality:

					The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.

    SRId
        To create a StreamSRI object, use the following code:
        	stream_id = "";
	    	sri = BULKIO::StreamSRI();
	    	sri.hversion = 1;
	    	sri.xstart = 0.0;
	    	sri.xdelta = 0.0;
	    	sri.xunits = BULKIO::UNITS_TIME;
	    	sri.subsize = 0;
	    	sri.ystart = 0.0;
	    	sri.ydelta = 0.0;
	    	sri.yunits = BULKIO::UNITS_NONE;
	    	sri.mode = 0;
	    	sri.streamID = this->stream_id.c_str();

	Time:
	    To create a PrecisionUTCTime object, use the following code:
	        struct timeval tmp_time;
	        struct timezone tmp_tz;
	        gettimeofday(&tmp_time, &tmp_tz);
	        double wsec = tmp_time.tv_sec;
	        double fsec = tmp_time.tv_usec / 1e6;;
	        BULKIO::PrecisionUTCTime tstamp = BULKIO::PrecisionUTCTime();
	        tstamp.tcmode = BULKIO::TCM_CPU;
	        tstamp.tcstatus = (short)1;
	        tstamp.toff = 0.0;
	        tstamp.twsec = wsec;
	        tstamp.tfsec = fsec;

    Ports:

        Data is passed to the serviceFunction through the getPacket call (BULKIO only).
        The dataTransfer class is a port-specific class, so each port implementing the
        BULKIO interface will have its own type-specific dataTransfer.

        The argument to the getPacket function is a floating point number that specifies
        the time to wait in seconds. A zero value is non-blocking. A negative value
        is blocking.

        Each received dataTransfer is owned by serviceFunction and *MUST* be
        explicitly deallocated.

        To send data using a BULKIO interface, a convenience interface has been added
        that takes a std::vector as the data input

        NOTE: If you have a BULKIO dataSDDS port, you must manually call
              "port->updateStats()" to update the port statistics when appropriate.

        Example:
            // this example assumes that the component has two ports:
            //  A provides (input) port of type BULKIO::dataShort called short_in
            //  A uses (output) port of type BULKIO::dataFloat called float_out
            // The mapping between the port and the class is found
            // in the component base class header file

            BULKIO_dataShort_In_i::dataTransfer *tmp = short_in->getPacket(-1);
            if (not tmp) { // No data is available
                return NOOP;
            }

            std::vector<float> outputData;
            outputData.resize(tmp->dataBuffer.size());
            for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
                outputData[i] = (float)tmp->dataBuffer[i];
            }

            // NOTE: You must make at least one valid pushSRI call
            if (tmp->sriChanged) {
                float_out->pushSRI(tmp->SRI);
            }
            float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

            delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
            return NORMAL;

        Interactions with non-BULKIO ports are left up to the component developer's discretion

    Properties:

        Properties are accessed directly as member variables. For example, if the
        property name is "baudRate", it may be accessed within member functions as
        "baudRate". Unnamed properties are given a generated name of the form
        "prop_n", where "n" is the ordinal number of the property in the PRF file.
        Property types are mapped to the nearest C++ type, (e.g. "string" becomes
        "std::string"). All generated properties are declared in the base class
        (rtl_sdr_device_base).

        Simple sequence properties are mapped to "std::vector" of the simple type.
        Struct properties, if used, are mapped to C++ structs defined in the
        generated file "struct_props.h". Field names are taken from the name in
        the properties file; if no name is given, a generated name of the form
        "field_n" is used, where "n" is the ordinal number of the field.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            //  - A boolean called scaleInput

            if (scaleInput) {
                dataOut[i] = dataIn[i] * scaleValue;
            } else {
                dataOut[i] = dataIn[i];
            }

        A callback method can be associated with a property so that the method is
        called each time the property value changes.  This is done by calling
        setPropertyChangeListener(<property name>, this, &rtl_sdr_device::<callback method>)
        in the constructor.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue

        //Add to rtl_sdr_device.cpp
        rtl_sdr_device_i::rtl_sdr_device_i(const char *uuid, const char *label) :
            rtl_sdr_device_base(uuid, label)
        {
            setPropertyChangeListener("scaleValue", this, &rtl_sdr_device_i::scaleChanged);
        }

        void rtl_sdr_device_i::scaleChanged(const std::string& id){
            std::cout << "scaleChanged scaleValue " << scaleValue << std::endl;
        }

        //Add to rtl_sdr_device.h
        void scaleChanged(const std::string&);


************************************************************************************************/
int rtl_sdr_device_i::serviceFunction()
{
	int returnValue = NOOP;

	// If data received, copy the buffer and release it.
	for (std::vector<individualRTL>::iterator rtl = _rtls.begin(); rtl != _rtls.end(); rtl++) {
		exclusive_lock lock(*rtl->lock);
		if (rtl->frontendStatus->enabled) {
			// Valid operating RTL
			if (rtl->newSRI) {
				dataShort_Out->pushSRI(rtl->sri);
				rtl->newSRI = false;
			}

			// If data is available and the stream should be pushed, push.
			if (0 < rtl->outputBuffer.size()) {
				returnValue = NORMAL;

				// Push...
				BULKIO::PrecisionUTCTime stamp = bulkio::time::utils::now();
				std::string streamID = std::string(rtl->sri.streamID);
				dataShort_Out->pushPacket(&rtl->outputBuffer[0], rtl->outputBuffer.size(), stamp, false, streamID);
			}
		}
		// Clear the buffer (ensure no overflows/bad_alloc on disabled-but-running RTL threads)
		rtl->outputBuffer.clear();
	}
    return returnValue;
}

void rtl_sdr_device_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException) {
	// As of RH 1.9.0, Devices (still) need to start themselves.
	start();
}

void rtl_sdr_device_i::start() throw (CF::Resource::StartError, CORBA::SystemException) {
	if (0 == _rtls.size()) {
		LOG_WARN(rtl_sdr_device_i, "No RTL Device Instances Connected.");
	}

	// Call base
	rtl_sdr_device_base::start();

	// Setup -- maintaining 1-1 index ratio of workers to RTLs.
	for (size_t i = 0; i < _rtls.size(); i++) {
		thread *t = NULL;
		if (NULL != _rtls[i].rtlHandle) {
			LOG_TRACE(rtl_sdr_device_i, "Resetting buffer for RTL: " << _rtls[i].frontendStatus->device_name);
			rtlsdr_reset_buffer(_rtls[i].rtlHandle);
			LOG_TRACE(rtl_sdr_device_i, "Starting async read for RTL: " << _rtls[i].frontendStatus->device_name);
			t = new thread(&rtlsdr_read_async, _rtls[i].rtlHandle, &rtl_sdr_device_i::_worker_cb_fn, &_rtls[i], NUM_BUFFERS, BUFFER_SIZE);
		}
		_workers.insert(std::pair<size_t, thread*>(i, t));
	}
}

void rtl_sdr_device_i::stop() throw (CF::Resource::StopError, CORBA::SystemException) {
	// Call base
	rtl_sdr_device_base::stop();

	// Shutdown
	_cancel_all_transfers();

	// Wipe all worker threads
	_clear_all_workers();
}

void rtl_sdr_device_i::configure (const CF::Properties& configProperties)
    throw (CF::PropertySet::PartialConfiguration,
           CF::PropertySet::InvalidConfiguration, CORBA::SystemException) {
	rtl_sdr_device_base::configure(configProperties);
}

CORBA::Boolean rtl_sdr_device_i::allocateCapacity(const CF::Properties & capacities)
    throw (CORBA::SystemException,
           CF::Device::InvalidCapacity,
           CF::Device::InvalidState) {
	CORBA::Boolean retVal = true;
	CORBA::ULong ii;
	try {
		for (ii = 0; ii < capacities.length(); ++ii) {
			const std::string id = (const char*) capacities[ii].id;
			PropertyInterface* property = getPropertyFromId(id);
			if(!property)
				throw CF::Device::InvalidCapacity("UNKNOWN PROPERTY", capacities);
			property->setValue(capacities[ii].value);
			if ("FRONTEND::tuner_allocation" == id){
				LOG_TRACE(rtl_sdr_device_i,"ATTEMPTING ALLOCATION FOR TUNER TYPE: " << tuner_allocation.tuner_type);
				if(tuner_allocation.tuner_type == "RX_DIGITIZER") {
					long tunerNum = addTunerMapping(tuner_allocation);
					if (0 > tunerNum) {
						LOG_TRACE(rtl_sdr_device_i, "No tuner available for use.");
						throw CF::Device::InvalidCapacity();
					}

					// Initialize the tuner
					try{
						setupTuner(tunerNum, tuner_allocation);
					}catch(const std::logic_error &e) {
						LOG_TRACE(rtl_sdr_device_i, "SETUP TUNER FAILED WITH ERROR: " << e.what());
						throw e;
					};
				}
				else {
					throw CF::Device::InvalidCapacity("UNKNOWN FRONTEND TUNER TYPE", capacities);
				}
			}
			else if ("FRONTEND::listener_allocation" == id) {
				if(addTunerMapping(listener_allocation) < 0)
					throw CF::Device::InvalidCapacity("UNKNOWN CONTROL ALLOCATION ID PROPERTY", capacities);
			}
		}
	}
	catch (...) {
		LOG_DEBUG(rtl_sdr_device_i,"Allocation failed.  Deallocating...");
		deallocateCapacity(capacities);
		retVal = false;
	};
	return retVal;
}

void rtl_sdr_device_i::deallocateCapacity(const CF::Properties & capacities)
    throw (CORBA::SystemException,
           CF::Device::InvalidCapacity,
           CF::Device::InvalidState) {
	// Returns values for valid queries in the same order as requested
	for (CORBA::ULong ii = 0; ii < capacities.length(); ++ii) {
		try{
			const std::string id = (const char*) capacities[ii].id;
			PropertyInterface* property = getPropertyFromId(id);
			if(!property)
				throw CF::Device::InvalidCapacity("UNKNOWN PROPERTY", capacities);
			property->setValue(capacities[ii].value);
			if (id == "FRONTEND::tuner_allocation"){
				// Try to remove control of the device
				long tunerNum = getTunerMapping(tuner_allocation.allocation_id);
				if (tunerNum < 0)
					throw CF::Device::InvalidState();
				if(_rtls[tunerNum].controlAllocationID == tuner_allocation.allocation_id){
					_rtls[tunerNum].controlAllocationID.clear();
				}
				removeTunerMapping(tuner_allocation.allocation_id);
			}
			else if (id == "FRONTEND::listener_allocation") {
				removeTunerMapping(listener_allocation.listener_allocation_id);
			}
			else {
				LOG_TRACE(rtl_sdr_device_i,"WARNING: UNKNOWN ALLOCATION PROPERTY \""+ std::string(property->name) + "\". IGNORING!");
			}
		}
		catch(...){
			LOG_DEBUG(rtl_sdr_device_i,"ERROR WHEN DEALLOCATING. SKIPPING...");
		}
	}
}

CF::Device::UsageType rtl_sdr_device_i::updateUsageState() {
	size_t tunerAllocated = 0;
	for (size_t i = 0; i < _rtls.size(); i++) {
		if (!_rtls[i].controlAllocationID.empty())
			tunerAllocated++;
	}
	// If no tuners are allocated, device is idle
	if (tunerAllocated == 0)
		return CF::Device::IDLE;
	// If all tuners are allocated, device is busy
	if (tunerAllocated == _rtls.size())
		return CF::Device::BUSY;
	// Else, device is active
	return CF::Device::ACTIVE;
}

long rtl_sdr_device_i::addTunerMapping(const tuner_allocation_struct &frontend_alloc) {
	long NO_VALID_TUNER = -1;

	// Do not allocate if allocation ID has already been used
	if(0 <= getTunerMapping(frontend_alloc.allocation_id)) {
		LOG_TRACE(rtl_sdr_device_i, "Allocation ID already used");
		return NO_VALID_TUNER;
	}

	// Next, try to allocate a new tuner
	exclusive_lock lock(allocationID_MappingLock);
	for (long tunerNum = 0; tunerNum < long(tuner_statuses.size()); tunerNum++) {
		if(_rtls[tunerNum].frontendStatus->tuner_type != frontend_alloc.tuner_type)
			continue;

		bool freq_valid = is_freq_valid(frontend_alloc.center_frequency, frontend_alloc.bandwidth, frontend_alloc.sample_rate,
				tuner_statuses[tunerNum].center_frequency, tuner_statuses[tunerNum].bandwidth, tuner_statuses[tunerNum].sample_rate);

		LOG_TRACE(rtl_sdr_device_i, "Allocation initial state for tuner: " << tunerNum
				<< "\n\tFreqValid:              " << freq_valid
				<< "\n\tDevice Control:         " << frontend_alloc.device_control
				<< "\n\tControl Alloc ID Empty: " << _rtls[tunerNum].controlAllocationID.empty());

		//listen
		if (!frontend_alloc.device_control && !_rtls[tunerNum].controlAllocationID.empty() && freq_valid) {
			LOG_TRACE(rtl_sdr_device_i, "Adding allocation ID as listener.");
			allocationID_to_tunerNum.insert(std::pair<std::string, size_t > (frontend_alloc.allocation_id, tunerNum));
			_rtls[tunerNum].frontendStatus->allocation_id_csv = _create_allocation_id_csv(tunerNum);
			return tunerNum;
		}
		//control
		else if (frontend_alloc.device_control && _rtls[tunerNum].controlAllocationID.empty()) {
			LOG_TRACE(rtl_sdr_device_i, "Adding allocation ID as controller.");
			_rtls[tunerNum].controlAllocationID = frontend_alloc.allocation_id;
			allocationID_to_tunerNum.insert(std::pair<std::string, size_t > (frontend_alloc.allocation_id, tunerNum));
			_rtls[tunerNum].frontendStatus->allocation_id_csv = _create_allocation_id_csv(tunerNum);
			return tunerNum;
		}

	}
	return NO_VALID_TUNER;
}

long rtl_sdr_device_i::addTunerMapping(const listener_allocation_struct &frontend_listener_alloc) {
	long NO_VALID_TUNER = -1;

	// Do not allocate if allocation ID has already been used
	int tuner_num = NO_VALID_TUNER;
	if ((tuner_num = getTunerMapping(frontend_listener_alloc.existing_allocation_id)) < 0) {
		LOG_TRACE(rtl_sdr_device_i, "Allocation ID already used on tuner: " << tuner_num);
		return NO_VALID_TUNER;
	}

	allocationID_to_tunerNum.insert(std::pair<std::string, size_t > (frontend_listener_alloc.listener_allocation_id, tuner_num));
	_rtls[tuner_num].frontendStatus->allocation_id_csv = _create_allocation_id_csv(tuner_num);
	return tuner_num;
}

bool rtl_sdr_device_i::removeTunerMapping(std::string allocation_id) {
	bool retVal = false;
	exclusive_lock lock(allocationID_MappingLock);
	long tunerNum = allocationID_to_tunerNum[allocation_id];
	if(allocationID_to_tunerNum.erase(allocation_id) > 0) {
		retVal = true;
		if (_rtls[tunerNum].controlAllocationID.empty() && _rtls[tunerNum].frontendStatus->allocation_id_csv.empty())
			enableTuner(tunerNum, false);
	}

	return retVal;

}

long rtl_sdr_device_i::getTunerMapping(std::string allocation_id) {
	long NO_VALID_TUNER = -1;

	exclusive_lock lock(allocationID_MappingLock);
	string_number_mapping::iterator iter = allocationID_to_tunerNum.find(allocation_id);
	if (iter != allocationID_to_tunerNum.end()) {
		LOG_TRACE(rtl_sdr_device_i, "Found allocation ID.  Tuner number: " << NO_VALID_TUNER);
		return iter->second;
	}

	return NO_VALID_TUNER;
}

bool rtl_sdr_device_i::is_connectionID_valid_for_tunerNum( const size_t & tunerNum, const std::string & connectionID) {
	bool retVal = true;
	std::map<std::string, size_t>::iterator iter =  allocationID_to_tunerNum.find(connectionID);
	if(iter == allocationID_to_tunerNum.end())
		retVal = false;
	if(iter->second != tunerNum)
		retVal = false;
	return retVal;
}

bool rtl_sdr_device_i::is_connectionID_valid_for_streamID( const std::string & streamID, const std::string & connectionID) {
	bool retVal = true;
	string_number_mapping::iterator iter = streamID_to_tunerNum.find(streamID);
	if (iter == streamID_to_tunerNum.end())
		retVal = false;
	else
		retVal = is_connectionID_valid_for_tunerNum(iter->second, connectionID);
	return retVal;
}

// Verifies frequency settings.
// FIXME: RTL has no bandwidth setting so the property isn't actually used which
// will probably cause problems here.
bool rtl_sdr_device_i::is_freq_valid(double req_cf, double req_bw, double req_sr, double cf, double bw, double sr){
	double req_min_bw_sr = std::min(req_bw,req_sr);
	double min_bw_sr = std::min(bw,sr);
	if( (req_cf + req_min_bw_sr/2 <= cf + min_bw_sr/2) && (req_cf - req_min_bw_sr/2 >= cf - min_bw_sr/2) )
		return true;
	return false;
};

bool rtl_sdr_device_i::setupTuner(size_t tuner_id, const tuner_allocation_struct &tuner_req) throw (std::logic_error) {
	if(tuner_req.allocation_id != _rtls[tuner_id].controlAllocationID) {
		LOG_TRACE(rtl_sdr_device_i, "Allocation ID is not the controller")
		return false;
	}

	// If the freq has changed (change in stream) or the tuner is disabled, then set it as disabled
	individualRTL* tuner_curr = &_rtls[tuner_id];
	bool isTunerEnabled = tuner_curr->frontendStatus->enabled;
	if (!isTunerEnabled || tuner_curr->frontendStatus->center_frequency != tuner_req.center_frequency)
		enableTuner(tuner_id, false);

	{
		tuner_curr->lock->lock();
		if(tuner_req.group_id != group_id_global )
			throw std::logic_error("setupTuner(): CAN NOT ALLOCATE A TUNER WITH THAT GROUP ID!");

		if(!tuner_req.rf_flow_id.empty() && tuner_req.rf_flow_id != _rtls[tuner_id].frontendStatus->rf_flow_id )
			throw std::logic_error("setupTuner(): CAN NOT ALLOCATE A TUNER WITH RF FLOW ID = " + tuner_req.rf_flow_id + " !");

		_rtls[tuner_id].frontendStatus->group_id = tuner_req.group_id;

		//Check Validity
		if (tuner_req.bandwidth <= 0)
			throw std::logic_error("setupTuner(): INVALID BANDWIDTH");
		if (tuner_req.sample_rate <= 0)
			throw std::logic_error("setupTuner(): INVALID RATE");

		tuner_curr->lock->unlock();
		if (_rtls[tuner_id].frontendStatus->tuner_type == "RX_DIGITIZER") {
			// TODO: Handle bandwidth
			// Interpreting AGC Mode as Gain Mode.  If True, Gain Mode -> Auto and gain = 0.
			// Otherwise, use the commanded gain value and set Gain Mode -> Manual.
			// Documentation says AGC Mode on this library may cause noise.
			CORBA::Long gain = 0;
			if (false == tuner_req.agc_mode) {
				gain = tuner_req.tuner_gain;
			}
			_setAGCMode(_rtls[tuner_id], false);
			_setGainMode(_rtls[tuner_id], tuner_req.agc_mode);
			_setTunerGain(_rtls[tuner_id], gain);
			_setFrequencyCorrection(_rtls[tuner_id], tuner_req.frequency_correction);
			_setCenterFrequency(_rtls[tuner_id], tuner_req.center_frequency);
			_setSampleRate(_rtls[tuner_id], tuner_req.sample_rate);
		}
		else {
			throw std::logic_error("setupTuner(): INVALID TUNER TYPE. MUST BE RX_DIGITIZER");
		}
	}

	if (isTunerEnabled)
		enableTuner(tuner_id, true);

	return true;
}


bool rtl_sdr_device_i::enableTuner(size_t tuner_id, bool enable) {
	// Lock the tuner
	exclusive_lock lock(*_rtls[tuner_id].lock);
	LOG_TRACE(rtl_sdr_device_i, "enableTuner called on #" << tuner_id << " to enable: " << enable);

	bool prev_enabled = _rtls[tuner_id].frontendStatus->enabled;
	_rtls[tuner_id].frontendStatus->enabled = enable;

	// If going from disabled to enabled
	if (!prev_enabled && enable) {
		// Start Streaming Now
		LOG_TRACE(rtl_sdr_device_i, "Tuner transitioned to enabled");
		long freq = _rtls[tuner_id].frontendStatus->center_frequency;
		char centerFreq_str[16];
		sprintf(centerFreq_str, "%ld", freq);
		string streamID = "tuner_" + tuner_statuses[tuner_id].device_serial;
		_rtls[tuner_id].sri.streamID = CORBA::string_dup(streamID.c_str());
		streamID_to_tunerNum.insert(std::make_pair(std::string(_rtls[tuner_id].sri.streamID), tuner_id));
		LOG_TRACE(rtl_sdr_device_i, "Tuner #" << tuner_id << " pushing SRI: " << _rtls[tuner_id].sri.streamID);
		_rtls[tuner_id].newSRI = true;
		dataShort_Out->pushSRI(_rtls[tuner_id].sri);
	}

	// If going from enabled to disabled
	if (prev_enabled && !enable && !std::string(_rtls[tuner_id].sri.streamID).empty()) {
		LOG_TRACE(rtl_sdr_device_i, "Tuner transitioned to disabled");
		std::string streamID = std::string(_rtls[tuner_id].sri.streamID);
		BULKIO::PrecisionUTCTime stamp = bulkio::time::utils::now();
		dataShort_Out->pushPacket(_rtls[tuner_id].outputBuffer, stamp, true, streamID);
		streamID_to_tunerNum.erase(streamID);
	}

	return true;
}

void rtl_sdr_device_i::_construct()
{
	/**
	 * NOTE: The following code is an example of how one can de-register ports and re-instantiate with different classes. In this case
	 * 		 I have added port_impl_customized.* files that contain custom port code that is not overwritten during the REDHAWK code re-generation process.
	 * 		 In a majority of cases, the following is never done and the port_impl.* files are modified directly.
	 */


	// Releasing / De-registering all ports
	releaseInPorts();
	releaseOutPorts();

	// Create new port instances for custom ports
	// NOTE: The ECLIPSE IDE will mark both these lines as errors to implement _do_get_interface...it will still compile though.
	DigitalTuner_provides = new FRONTEND_DigitalTuner_In_i_customized("DigitalTuner_provides", this);	/** CUSTOM PORT */
	dataShort_Out = new BULKIO_dataShort_Out_i_customized("dataShort_Out", this);

	// Re-register all ports
	registerInPort(DigitalTuner_provides);
	registerOutPort(dataShort_Out, dataShort_Out->_this());

	// Count and resize vectors to connect RTLs
	uint32_t rtlCount = rtlsdr_get_device_count();
	tuner_statuses.resize(rtlCount);
	_rtls.resize(rtlCount);

	for (uint32_t i = 0; i < rtlCount; i++){
		_rtls[i].lock = new mutex();
		_rtls[i].frontendStatus = &tuner_statuses[i];
		_connect_RTL(i, _rtls[i]);
	}
	if (0 == rtlCount) {
		LOG_INFO(rtl_sdr_device_i, "No RTL devices connected.");
	}
	else {
		LOG_INFO(rtl_sdr_device_i, "Connected to " << rtlCount << " RTL device" << ((1 < rtlCount) ? "s." : ". "));
	}
}

// Locates the tuner's allocation ID CSV and updates it.
std::string rtl_sdr_device_i::_create_allocation_id_csv(size_t tunerNum) {
	std::string alloc_id_csv = "";
	for(string_number_mapping::iterator it = allocationID_to_tunerNum.begin();
			it != allocationID_to_tunerNum.end();
			it++){
		if(it->second == tunerNum)
			alloc_id_csv += it->first + ",";
	}
	if(!alloc_id_csv.empty())
		alloc_id_csv.erase(alloc_id_csv.size()-1);
	return alloc_id_csv;
}

// connects to and pulls all present values (if possible) for the status of each
// attached RTL.
void rtl_sdr_device_i::_connect_RTL(uint32_t rtl_index, individualRTL &dev) {
	exclusive_lock(*dev.lock);
	if (0 > rtlsdr_open(&dev.rtlHandle, rtl_index)) {
		LOG_WARN(rtl_sdr_device_i, "Unable to connect to RTL #" << rtl_index);
		dev.rtlHandle = NULL;
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Connected to RTL #" << rtl_index);
		if (NULL != dev.frontendStatus) {
			// Get current values and update frontend_status
			dev.frontendStatus->tuner_type = "RX_DIGITIZER";
			dev.frontendStatus->enabled = true;
			dev.frontendStatus->agc_mode = false;
			_setAGCMode(dev, false);
			// TODO: dev.frontendStatus->bandwidth
			dev.frontendStatus->center_frequency = (double) rtlsdr_get_center_freq(dev.rtlHandle);
			dev.frontendStatus->device_name = std::string(rtlsdr_get_device_name(rtl_index));
			char serial[256] = {};
			rtlsdr_get_device_usb_strings(rtl_index, NULL, NULL, serial);
			dev.frontendStatus->device_serial = std::string(serial);
			dev.frontendStatus->direct_sampling =  rtlsdr_get_direct_sampling(dev.rtlHandle);
			dev.frontendStatus->frequency_correction = rtlsdr_get_freq_correction(dev.rtlHandle);
			dev.frontendStatus->gain_mode = true;
			_setGainMode(dev, (bool) dev.frontendStatus->gain_mode);
			dev.frontendStatus->offset_tuning = rtlsdr_get_offset_tuning(dev.rtlHandle);
			dev.frontendStatus->rf_flow_id.clear();
			dev.frontendStatus->sample_rate = rtlsdr_get_sample_rate(dev.rtlHandle);
			// FIXME: E4000 supports 6 gain values but IDL only supports 1 tuner gain.
			int gains[6] = {0},
			    count = 0;
			count = rtlsdr_get_tuner_gains(dev.rtlHandle, gains);
			if (1 < count)
				LOG_DEBUG(rtl_sdr_device_i, "Some RTLs support multiple gains, only 1 used");
			dev.frontendStatus->tuner_gain = gains[0];
		}
	}
}

void rtl_sdr_device_i::_cancel_all_transfers() {
	for (size_t i = 0; i < _rtls.size(); i++) {
		if (NULL != _rtls[i].rtlHandle) {
			LOG_TRACE(rtl_sdr_device_i, "Cancelling async RTL transfer: #" << i);
			rtlsdr_cancel_async(_rtls[i].rtlHandle);
		}
	}
}

int rtl_sdr_device_i::_setCenterFrequency(const individualRTL &dev, uint32_t cf)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set center frequency.");
	int call = rtlsdr_set_center_freq(dev.rtlHandle, cf);
	if (0 > call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set center frequency.");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Center frequency set to: " << cf);
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->center_frequency = (double) cf;
	}
	return call;
}

int rtl_sdr_device_i::_setFrequencyCorrection(const individualRTL &dev, int ppm)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set frequency correction.");
	int call = rtlsdr_set_freq_correction(dev.rtlHandle, ppm);

	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set Frequency Correction to: " << ppm << "ppm");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Frequency Correction set to: " << ppm << "ppm");
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->frequency_correction = ppm;
	}
	return call;
}

// FIXME: One of the potential RTL devices can accept 6 gain values
// vs. just 1 as in the IDL.  This method implements a single gain copied
// as many times as the attached device accepts.
int rtl_sdr_device_i::_setTunerGain(const individualRTL &dev, int gain)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set tuner gain.");
	int count  = rtlsdr_set_tuner_gain(dev.rtlHandle, gain);
	if (0 != count) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set Tuner Gain:" << gain << "/10 dB");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Set Tuner gain to: " << gain << "/10 dB");
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->tuner_gain = gain;
	}

	return count;
}

int rtl_sdr_device_i::_setGainMode(const individualRTL &dev, bool mode)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set gain mode.");
	int call = rtlsdr_set_tuner_gain_mode(dev.rtlHandle, mode);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set Gain Mode.");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Set Gain Mode to: " << mode);
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->gain_mode = mode;
	}

	return call;
}

int rtl_sdr_device_i::_setSampleRate(individualRTL &dev, uint32_t rate)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set sample rate.");
	int call = rtlsdr_set_sample_rate(dev.rtlHandle, rate);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set sample rate: " << rate << "Hz");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Sample rate set to: " << rate << "Hz");
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->sample_rate = rate;
		dev.sri.xdelta = 1.0 / rate;
		dev.sri.subsize = (long) rate;
		dev.newSRI = true;
	}

	return call;
}

int rtl_sdr_device_i::_setAGCMode(const individualRTL &dev, bool mode)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set AGC Mode.");
	int call = rtlsdr_set_agc_mode(dev.rtlHandle, mode);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set AGC Mode");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Set AGC Mode to: " << mode);
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->agc_mode = mode;
	}
	return call;
}

int rtl_sdr_device_i::_setDirectSampling(const individualRTL &dev, int directSampling)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set direct sampling mode.");
	int call = rtlsdr_set_direct_sampling(dev.rtlHandle, directSampling);

	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i,"Unable to set direct sampling mode.");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Direct sampling mode set to: " << directSampling);
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->direct_sampling = directSampling;
	}
	return call;
}

int rtl_sdr_device_i::_setOffsetTuning(const individualRTL &dev, int mode)
{
	if (not dev.rtlHandle)
		return 0;

	LOG_TRACE(rtl_sdr_device_i, "Attempting to set offset tuning mode.");
	int call = rtlsdr_set_offset_tuning(dev.rtlHandle, mode);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set offset tuning mode.");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Offset tuning mode set to: " << mode);
		exclusive_lock lock(*dev.lock);
		dev.frontendStatus->offset_tuning = mode;
	}

	return call;
}

// Interrupt, join, and delete all workers.
void rtl_sdr_device_i::_clear_all_workers() {
	for (size_t i = 0; i < _workers.size(); i++) {
		if (NULL != _workers[i]) {
			LOG_TRACE(rtl_sdr_device_i, "Interrupting helper thread #" << i);
			_workers[i]->interrupt();
			LOG_TRACE(rtl_sdr_device_i, "Joining helper thread #" << i);
			_workers[i]->join();
			delete _workers[i];
			LOG_TRACE(rtl_sdr_device_i, "Deleting helper thread #" << i);
		}
	}
	_workers.clear();
}

/* Threaded read functions from the RTL device */
void rtl_sdr_device_i::_worker_fn(individualRTL *dev) {
	// MANDATORY BEFORE STARTING THE READ PROCESS!
	LOG_TRACE(rtl_sdr_device_i, "Resetting buffer for RTL: " << dev->frontendStatus->device_name);
	rtlsdr_reset_buffer(dev->rtlHandle);

	// This thread blocks here.
	LOG_TRACE(rtl_sdr_device_i, "Starting async read for RTL: " << dev->frontendStatus->device_name);
	rtlsdr_read_async(dev->rtlHandle, &rtl_sdr_device_i::_worker_cb_fn, dev, NUM_BUFFERS, BUFFER_SIZE);
	LOG_TRACE(rtl_sdr_device_i, "Async read exiting for RTL: " << dev->frontendStatus->device_name);
}

/* STATIC Copy all data from buffer to the RTL's output buffer, remove DC offset. */
void rtl_sdr_device_i::_worker_cb_fn(unsigned char *buf, uint32_t len, void *ctx) {
	individualRTL *dev = reinterpret_cast<individualRTL*>(ctx);

	exclusive_lock lock(*dev->lock);

	// Copy buffer and compensate for DC offset
	if ((NULL != dev) && (NULL != buf) && (0 < len)) {
		for (uint32_t i = 0; i < len; i++)
			dev->outputBuffer.push_back(buf[i] - 127);
	}
}
