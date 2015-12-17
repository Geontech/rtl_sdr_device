
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

const uint32_t NUM_IF_GAINS_E4000 = 6; // Per rtl-sdr.h


string int_array_to_char(int int_array[], int size_of_array, string token){
	stringstream returnstring;
	for (int temp = 0; temp < size_of_array; temp++) {
		returnstring << int_array[temp];
		if (temp != size_of_array - 1) returnstring << token;
	}

	return returnstring.str();
}


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
	if (NULL != _rtl_device) {
		rtlsdr_cancel_async(_rtl_device);
		rtlsdr_close(_rtl_device);
	}

	if (NULL != _worker) {
		_worker->interrupt();
		_worker->join();
		delete _worker;
	}

	if (NULL != _buffer) {
		delete _buffer;
	}
}

/*******************************
 * Property listener callback and helpers
 ******************************/
void rtl_sdr_device_i::_construct()
{
	// Initialize properties
	_rtl_device = NULL;
	_worker = NULL;
	_buffer = new circular_buffer<short>();

	// Initial value and register MessageEvent callback.
	_switch_pattern = 0;
	message_in->registerMessage("switch_pattern",
			this, &rtl_sdr_device_i::_receivedMessage);
}

int rtl_sdr_device_i::_setRtlDevice() {
	int callValue = 0;

	LOG_DEBUG(rtl_sdr_device_i, "Attempting to connect to RTL #" << device_index);

	callValue = rtlsdr_open(&_rtl_device, device_index);

	// Verify handle validity
	if (0 > callValue) {
		_rtl_device = NULL;
		uint32_t count = rtlsdr_get_device_count();
		if (0 == count) {
			LOG_ERROR(rtl_sdr_device_i, "No RTL-SDR devices found.");
		}
		else {
			LOG_WARN(rtl_sdr_device_i, "RTL-SDR at chosen device_index not found.");
		}
	}
	else {
		LOG_INFO(rtl_sdr_device_i, "RTL Device Connected");
	}

	return callValue;
}

int rtl_sdr_device_i::_setCenterFrequency()
{
	if (not _rtl_device) return 0;

	uint32_t cf = rtlsdr_get_center_freq(_rtl_device);

	int call = rtlsdr_set_center_freq(_rtl_device, center_frequency);
	if (0 > call) {
		if (0 == cf) {
			LOG_WARN(rtl_sdr_device_i, "Unable to get/set center frequency.");
		}
		else {
			LOG_WARN(rtl_sdr_device_i, "Unable to set new center frequency: " << center_frequency << "Hz");

			if (0 > rtlsdr_set_center_freq(_rtl_device, cf)) {
				LOG_WARN(rtl_sdr_device_i, "Failed to restore previous center frequency: " << center_frequency << "Hz");
			}

			mutex::scoped_lock lock(_theMutex);
			center_frequency = cf;
		}
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Center frequency set to: " << center_frequency);
	}
	return call;
}

int rtl_sdr_device_i::_setFrequencyCorrection()
{
	if (not _rtl_device) return 0;

	int ppm = rtlsdr_get_freq_correction(_rtl_device);
	int call = rtlsdr_set_freq_correction(_rtl_device, frequency_correction);

	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set Frequency Correction to: " << frequency_correction << "ppm");

		if (0 != rtlsdr_set_freq_correction(_rtl_device, ppm)) {
			LOG_WARN(rtl_sdr_device_i, "Failed to reset previous Frequency Correction value: " << frequency_correction << "ppm");
		}

		mutex::scoped_lock lock(_theMutex);
		frequency_correction = ppm;
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Frequency Correction set to: " << frequency_correction << "ppm");
	}
	return call;
}

int rtl_sdr_device_i::_setTunerGain()
{	if (not _rtl_device) return 0;

	int *gains;
	int count = rtlsdr_get_tuner_gains(_rtl_device, NULL);
	string gainStr;

	if (0 < count) {
		gains = new int[count];
		count = rtlsdr_get_tuner_gains(_rtl_device, gains);
		gainStr = int_array_to_char(gains, count, ", ");

		count  = rtlsdr_set_tuner_gain(_rtl_device, tuner_gain);
		if (0 != count) {
			LOG_WARN(rtl_sdr_device_i, "Unable to set Tuner Gain. Valid Values:\n" << gainStr);
		}
		else {
			LOG_DEBUG(rtl_sdr_device_i, "Set Tuner gain to: " << tuner_gain << "/10 dB")
		}
	}
	else {
		LOG_WARN(rtl_sdr_device_i, "Unable to get Tuner Gain(s). Check device");
	}

	return count;
}

int rtl_sdr_device_i::_setIFGains()
{
	if (not _rtl_device) return 0;

	int call = 0;

	// Only applicable to E4000
	if (RTLSDR_TUNER_E4000 == rtlsdr_get_tuner_type(_rtl_device)) {
		for (uint32_t i = 0; i < NUM_IF_GAINS_E4000; i++) {
			call = rtlsdr_set_tuner_if_gain(_rtl_device, i+1, if_gains[i]);
			if (0 != call) {
				LOG_WARN(rtl_sdr_device_i, "Failed to set #" << i+1 << " IF Gain: " << if_gains[i] << "/10 dB");
			}
			else {
				LOG_DEBUG(rtl_sdr_device_i, "IF Gain #" << i+1 << " set to: " << "/10 dB");
			}
		}
	}

	return call;
}

int rtl_sdr_device_i::_setGainMode()
{
	if (not _rtl_device) return 0;

	int call = 0;

	call = rtlsdr_set_tuner_gain_mode(_rtl_device, gain_mode);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set Gain Mode.");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Set Gain Mode to: " << gain_mode);
	}

	return call;
}

int rtl_sdr_device_i::_setSampleRate()
{
	if (not _rtl_device) return 0;

	int call = 0;
	uint32_t presentSampleRate = 0;

	presentSampleRate = rtlsdr_get_sample_rate(_rtl_device);
	if (0 == presentSampleRate) {
		LOG_WARN(rtl_sdr_device_i, "Unable to get present sample rate.");
	}

	call = rtlsdr_set_sample_rate(_rtl_device, sample_rate);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set sample rate: " << sample_rate << "Hz");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Sample rate set to: " << sample_rate << "Hz");

		// Update SRI
		mutex::scoped_lock lock(_theMutex);
		_doUpdateSRI = true;
	}

	return call;
}

int rtl_sdr_device_i::_setAGCMode()
{
	if (not _rtl_device) return 0;

	int call = rtlsdr_set_agc_mode(_rtl_device, agc_mode);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set AGC Mode");
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Set AGC Mode to: " << agc_mode);
	}
	return call;
}

int rtl_sdr_device_i::_setDirectSampling()
{
	if (not _rtl_device) return 0;

	int dsMode = rtlsdr_get_direct_sampling(_rtl_device);
	int call = 0;
	if (-1 == dsMode) {
		LOG_WARN(rtl_sdr_device_i, "Unable to get Direct Sampling mode");
		call = dsMode;
	}
	else {
		call = rtlsdr_set_direct_sampling(_rtl_device, direct_sampling);
		if (0 != call) {
			LOG_WARN(rtl_sdr_device_i,"Unable to set direct sampling mode.");
			rtlsdr_set_direct_sampling(_rtl_device, dsMode);
			mutex::scoped_lock lock(_theMutex);
			direct_sampling = dsMode;
		}
		else {
			LOG_DEBUG(rtl_sdr_device_i, "Direct sampling mode set to: " << direct_sampling);
		}
	}
	return call;
}

int rtl_sdr_device_i::_setOffsetTuning()
{
	if (not _rtl_device) return 0;

	int call = 0,
		mode = rtlsdr_get_offset_tuning(_rtl_device);

	if (-1 == mode) {
		LOG_WARN(rtl_sdr_device_i, "Unable to get offset tuning mode.");
		call = mode;
	}

	call = rtlsdr_set_offset_tuning(_rtl_device, offset_tuning);
	if (0 != call) {
		LOG_WARN(rtl_sdr_device_i, "Unable to set offset tuning mode.");
		rtlsdr_set_offset_tuning(_rtl_device, mode);
		mutex::scoped_lock lock(_theMutex);
		offset_tuning = mode;
	}
	else {
		LOG_DEBUG(rtl_sdr_device_i, "Offset tuning mode set to: " << offset_tuning);
	}

	return call;
}

int rtl_sdr_device_i::_setBufferSize() {
	// This function stops and restarts the service to make sure the rtlsdr read function
	// uses the right size of buffer.
	LOG_TRACE(rtl_sdr_device_i, "Resizing buffer...");
	_buffer->set_capacity(buffer_size * buffer_num);
	LOG_TRACE(rtl_sdr_device_i, "Successfully resized buffer.");
	if (started()) {
		stop();
		start();
	}
	return 0;
}

// If the value changed, update the SRI on the next pushPacket.
void rtl_sdr_device_i::_receivedMessage(const std::string &id,
		const switch_pattern_struct &msg) {
	mutex::scoped_lock lock(_theMutex);
	_doUpdateSRI = (_switch_pattern != msg.value);
	_switch_pattern = msg.value;
}

/***********************************************************************************************

    Basic functionality:

        The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.
        
    SRI:
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
	uint32_t n = 0;
	std::vector<short> shortOut;

	// If data received, copy the buffer and release it.
    if (NULL == _rtl_device) {
    	LOG_WARN(rtl_sdr_device_i, "RTL-SDR Device not connected");
    }
    else {
    	mutex::scoped_lock lock(_theMutex);
    	n = _buffer->size();
    	if (0 < n) {
			shortOut.resize(n);
			for (size_t i = 0; i < n; i++) {
				shortOut[i] = (*_buffer)[i];
			}
    		_buffer->clear();

    		returnValue = NORMAL;
    	}
    }

    if (NORMAL == returnValue) {
    	// Push SRI?  Details changed or we're in antenna switching pattern mode.
    	// TODO: Figure out why we must always push SRI
    	if (_doUpdateSRI) {
			BULKIO::StreamSRI SRI = bulkio::sri::create(stream_id);
			SRI.mode = 1;
			SRI.xdelta = 1.0 / sample_rate;
			SRI.xstart = 0.0;
			unsigned long size = SRI.keywords.length();
			SRI.keywords.length(size + 1);
			SRI.keywords[size].id = CORBA::string_dup("switch_pattern");
			SRI.keywords[size].value <<= _switch_pattern;
			dataShort_Out->pushSRI(SRI);

	    	_theMutex.lock();
			_doUpdateSRI = false;
			_theMutex.unlock();
		}

    	// TX
        BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();
    	dataShort_Out->pushPacket(shortOut, tstamp, _isNewStream, stream_id);

    	_theMutex.lock();
		_isNewStream = false;
		_theMutex.unlock();
    }

    return returnValue;
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
			if ("reserve" == id) {
				if (reserve) {
					LOG_TRACE(rtl_sdr_device_i, "RTL already reserved");
					throw CF::Device::InvalidCapacity();
				}
				else {
					reserve = true;
				}
			}
			else {
				property->setValue(capacities[ii].value);
			}
		}
	}
	catch (...) {
		LOG_DEBUG(rtl_sdr_device_i, "Allocation failed.  Deallocating...");
		deallocateCapacity(capacities);
		retVal = false;
	}
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
			if (("reserve" == id) && reserve) {
				reserve = false;
			}
			else {
				// All others try to set the value.
				property->setValue(capacities[ii].value);
			}
		}
		catch (...) {
			LOG_DEBUG(rtl_sdr_device_i, "ERROR WHEN DEALLOCATING. SKIPPING...");
		}
	}
}

void rtl_sdr_device_i::initialize()
	throw (CF::LifeCycle::InitializeError, CORBA::SystemException) {
	rtl_sdr_device_base::initialize();
	_setRtlDevice();
}

void rtl_sdr_device_i::configure (const CF::Properties& configProperties)
    throw (CF::PropertySet::PartialConfiguration,
           CF::PropertySet::InvalidConfiguration, CORBA::SystemException) {
	rtl_sdr_device_base::configure(configProperties);
	for (CORBA::ULong i = 0; i < configProperties.length(); ++i) {
		const std::string id = (const char*) configProperties[i].id;
		if ("center_frequency" == id) {
			_setCenterFrequency();
		}
		else if ("frequency_correction" == id) {
			_setFrequencyCorrection();
		}
		else if ("tuner_gain" == id) {
			_setTunerGain();
		}
		else if ("if_gains" == id) {
			_setIFGains();
		}
		else if ("gain_mode" == id) {
			_setGainMode();
		}
		else if ("sample_rate" == id) {
			_setSampleRate();
		}
		else if ("agc_mode" == id) {
			_setAGCMode();
		}
		else if ("direct_sampling" == id) {
			_setDirectSampling();
		}
		else if ("offset_tuning" == id) {
			_setOffsetTuning();
		}
		else if (("buffer_size" == id) ||
				 ("buffer_num" == id)) {
			_setBufferSize();
		}
		else if ("stream_id" == id) {
			mutex::scoped_lock lock(_theMutex);
			_doUpdateSRI = true;
		}
	}

	if (not started()) start();
}

void rtl_sdr_device_i::start() throw (CF::Resource::StartError, CORBA::SystemException) {
	// Call base
	rtl_sdr_device_base::start();

	if (not _rtl_device) {
		LOG_WARN(rtl_sdr_device_i, "No RTL Device Instance Connected.");
		stop();
	}
	else {
		try {
			mutex::scoped_lock lock(_theMutex);

			// Setup
			_buffer->clear();
			_isNewStream = true;
			_doUpdateSRI = true;
			_worker = new thread(&rtl_sdr_device_i::_worker_fn, this);

			setUsageState(CF::Device::ACTIVE);
		}
		catch (...) {
			stop();
			throw;
		}
	}
}

void rtl_sdr_device_i::stop() throw (CF::Resource::StopError, CORBA::SystemException) {
	// Call base
	rtl_sdr_device_base::stop();

	if (NULL != _rtl_device) {
		rtlsdr_cancel_async(_rtl_device);
	}

	// Shutdown
	if (NULL != _worker) {
		_worker->interrupt();
		_worker->join();
		delete _worker;
		_worker = NULL;
	}

	setUsageState(CF::Device::IDLE);
}

/* Threaded read functions from the RTL device */
void rtl_sdr_device_i::_worker_fn() {
	// MANDATORY BEFORE STARTING THE READ PROCESS!
	rtlsdr_reset_buffer(_rtl_device);

	// This thread blocks here.
	rtlsdr_read_async(_rtl_device, &rtl_sdr_device_i::_worker_cb_fn, this, buffer_num, buffer_size);
}

/* Copy buf to local circular buffer and release the mutex. */
void rtl_sdr_device_i::_worker_process_fn(unsigned char *buf, uint32_t len) {
	if ((0 < len) && (NULL != buf)) {
		mutex::scoped_lock lock(_theMutex);
		for (uint32_t i = 0; i < len; i++)
			_buffer->push_back(((short)buf[i]) - 127);
	}
}

/* STATIC Pass-through function */
void rtl_sdr_device_i::_worker_cb_fn(unsigned char *buf, uint32_t len, void *ctx) {
	if ((NULL != ctx) && (NULL != buf) && (0 < len)) {
		rtl_sdr_device_i *inst = reinterpret_cast<rtl_sdr_device_i*>(ctx);
		inst->_worker_process_fn(buf, len);
	}
}
