/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file 
 * distributed with this source distribution.
 * 
 * This file is part of GNUHAWK.
 * 
 * GNUHAWK is free software: you can redistribute it and/or modify is under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * GNUHAWK is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program.  If not, see http://www.gnu.org/licenses/.
 */


/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

 *******************************************************************************************/

#include "port_impl_customized.h"
#include "rtl_sdr_device.h"

// ----------------------------------------------------------------------------------------
// BULKIO_dataShort_Out_i definition
// ----------------------------------------------------------------------------------------

BULKIO_dataShort_Out_i_customized::BULKIO_dataShort_Out_i_customized(std::string port_name, rtl_sdr_device_base *_parent) :
BULKIO_dataShort_Out_i(port_name,_parent) {
}

BULKIO_dataShort_Out_i_customized::~BULKIO_dataShort_Out_i_customized() {
}

void BULKIO_dataShort_Out_i_customized::pushPacket(short* data, size_t length, BULKIO::PrecisionUTCTime& T, bool EOS, std::string& streamID) {
	if (refreshSRI) {
		if (currentSRIs.find(streamID) != currentSRIs.end()) {
			pushSRI(currentSRIs[streamID]);
		}
	}
	boost::mutex::scoped_lock lock(updatingPortsLock); // don't want to process while command information is coming in
	// Magic is below, make a new sequence using the data from the Iterator
	// as the data for the sequence.  The 'false' at the end is whether or not
	// CORBA is allowed to delete the buffer when the sequence is destroyed.
	PortTypes::ShortSequence seq = PortTypes::ShortSequence(length, length, data, false);
	if (active) {
		std::vector < std::pair < BULKIO::dataShort_var, std::string > >::iterator port;
		for (port = outConnections.begin(); port != outConnections.end(); port++) {
			if (! parent->is_connectionID_valid_for_streamID(streamID, port->second))
				continue;
			try {
				((*port).first)->pushPacket(seq, T, EOS, streamID.c_str());
				stats[(*port).second].update(length, 0, 0, streamID);
			} catch (...) {
				std::cout << "Call to pushPacket by BULKIO_dataShort_Out_i failed for connection: " << (*port).second << std::endl;
			}
		}
	}
};

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataShort_Out_i_customized::pushSRI(const BULKIO::StreamSRI& H) {
	std::vector < std::pair < BULKIO::dataShort_var, std::string > >::iterator i;

	boost::mutex::scoped_lock lock(updatingPortsLock); // don't want to process while command information is coming in
	if (active) {
		for (i = outConnections.begin(); i != outConnections.end(); ++i) {
			if (! parent->is_connectionID_valid_for_streamID(std::string(H.streamID), i->second))
				continue;
			try {
				((*i).first)->pushSRI(H);
			} catch (...) {
				std::cout << "Call to pushSRI by BULKIO_dataShort_Out_i failed" << std::endl;
			}
		}
	}

	currentSRIs[std::string(H.streamID)] = H;
	refreshSRI = false;

	return;
}

void BULKIO_dataShort_Out_i_customized::pushSRI_on_connection(const std::string& connection_name) {
	for(std::map<std::string, BULKIO::StreamSRI>::iterator iter = currentSRIs.begin(); iter != currentSRIs.end(); iter++){
		if ( parent->is_connectionID_valid_for_streamID(iter->first, connection_name)){
			pushSRI(iter->second);
		}
	}
}



// ----------------------------------------------------------------------------------------
// FRONTEND_DigitalTuner_In_i_customized definition
// ----------------------------------------------------------------------------------------

FRONTEND_DigitalTuner_In_i_customized::FRONTEND_DigitalTuner_In_i_customized(std::string port_name, rtl_sdr_device_base *_parent) :
				FRONTEND_DigitalTuner_In_i(port_name, _parent) {
	parent = static_cast<rtl_sdr_device_i *> (_parent);
}

FRONTEND_DigitalTuner_In_i_customized::~FRONTEND_DigitalTuner_In_i_customized() {
}


char* FRONTEND_DigitalTuner_In_i_customized::getTunerType(const char* id){
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return CORBA::string_dup(parent->_rtls[tunerNum].frontendStatus->tuner_type.c_str());
}

CORBA::Boolean FRONTEND_DigitalTuner_In_i_customized::getTunerDeviceControl(const char* id){
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) == parent->_rtls[tunerNum].controlAllocationID)
		return true;
	return false;
}

char* FRONTEND_DigitalTuner_In_i_customized::getTunerGroupId(const char* id){
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return CORBA::string_dup(parent->_rtls[tunerNum].frontendStatus->group_id.c_str());
}
char* FRONTEND_DigitalTuner_In_i_customized::getTunerRfFlowId(const char* id){
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return CORBA::string_dup(parent->_rtls[tunerNum].frontendStatus->rf_flow_id.c_str());

}

CF::Properties* FRONTEND_DigitalTuner_In_i_customized::getTunerStatus(const char* id){
    boost::mutex::scoped_lock lock(portAccess);
    CF::Properties* tmpVal = new CF::Properties();
    long tunerNum = parent->getTunerMapping(id);
    if (tunerNum < 0)
    	throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
    CORBA::Any prop;
    prop <<= *(parent->_rtls[tunerNum].frontendStatus);
    prop >>= tmpVal;

    CF::Properties_var tmp = new CF::Properties(*tmpVal);
    return tmp._retn();
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerCenterFrequency(const char* id, CORBA::Double freq) {
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) != parent->_rtls[tunerNum].controlAllocationID)
		throw FRONTEND::FrontendException(("ERROR:: ID: " + std::string(id) + " DOES NOT HAVE AUTHORIZATION TO MODIFY TUNER!").c_str());

	tuner_allocation_struct newParams = parent->feAlloc_from_feStatus(tunerNum);
	newParams.allocation_id = std::string(id);
	newParams.center_frequency = freq;
	try {
		parent->setupTuner(tunerNum, newParams);
	} catch (std::exception& e) {
		throw FRONTEND::FrontendException(("WARNING: Exception Caught during setupTuner: " + std::string(e.what())).c_str());
	}

}

CORBA::Double FRONTEND_DigitalTuner_In_i_customized::getTunerCenterFrequency(const char* id) {
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return parent->_rtls[tunerNum].frontendStatus->center_frequency;
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerBandwidth(const char* id, CORBA::Double bw) {
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) != parent->_rtls[tunerNum].controlAllocationID)
		throw FRONTEND::FrontendException(("ERROR:: ID: " + std::string(id) + " DOES NOT HAVE AUTHORIZATION TO MODIFY TUNER!").c_str());

	tuner_allocation_struct newParams = parent->feAlloc_from_feStatus(tunerNum);
	newParams.allocation_id = std::string(id);
	newParams.bandwidth = bw;
	try {
		parent->setupTuner(tunerNum, newParams);
	} catch (std::exception& e) {
		throw FRONTEND::FrontendException(("WARNING: Exception Caught during setupTuner: " + std::string(e.what())).c_str());
	}

}

CORBA::Double FRONTEND_DigitalTuner_In_i_customized::getTunerBandwidth(const char* id) {
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return parent->_rtls[tunerNum].frontendStatus->bandwidth;
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerAgcEnable(const char* id, CORBA::Boolean enable) {
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);

	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) != parent->_rtls[tunerNum].controlAllocationID)
		throw FRONTEND::FrontendException(("ERROR:: ID: " + std::string(id) + " DOES NOT HAVE AUTHORIZATION TO MODIFY TUNER!").c_str());

	tuner_allocation_struct newParams = parent->feAlloc_from_feStatus(tunerNum);
	newParams.allocation_id = std::string(id);
	newParams.agc_mode = enable;

	try {
		parent->setupTuner(tunerNum, newParams);
	} catch (std::exception& e) {
		throw FRONTEND::FrontendException(("WARNING: Exception Caught during setupTuner: " + std::string(e.what())).c_str());
	}
}

CORBA::Boolean FRONTEND_DigitalTuner_In_i_customized::getTunerAgcEnable(const char* id) {
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return parent->_rtls[tunerNum].frontendStatus->agc_mode;
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerGain(const char* id, CORBA::Float gain) {
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) != parent->_rtls[tunerNum].controlAllocationID)
		throw FRONTEND::FrontendException(("ERROR:: ID: " + std::string(id) + " DOES NOT HAVE AUTHORIZATION TO MODIFY TUNER!").c_str());

	parent->_rtls[tunerNum].frontendStatus->tuner_gain = (CORBA::Long) gain;
	tuner_allocation_struct newParams = parent->feAlloc_from_feStatus(tunerNum);
	newParams.allocation_id = std::string(id);
	try {
		parent->setupTuner(tunerNum, newParams);
	} catch (std::exception& e) {
		throw FRONTEND::FrontendException(("WARNING: Exception Caught during setupTuner: " + std::string(e.what())).c_str());
	}
}

CORBA::Float FRONTEND_DigitalTuner_In_i_customized::getTunerGain(const char* id) {
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return (CORBA::Float) parent->_rtls[tunerNum].frontendStatus->tuner_gain;
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerEnable(const char* id, CORBA::Boolean enable) {
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) != parent->_rtls[tunerNum].controlAllocationID)
		throw FRONTEND::FrontendException(("ERROR:: ID: " + std::string(id) + " DOES NOT HAVE AUTHORIZATION TO MODIFY TUNER!").c_str());

	parent->_rtls[tunerNum].frontendStatus->enabled = enable;
	tuner_allocation_struct newParams = parent->feAlloc_from_feStatus(tunerNum);
	newParams.allocation_id = std::string(id);
	try {
		parent->setupTuner(tunerNum, newParams);
	} catch (std::exception& e) {
		throw FRONTEND::FrontendException(("WARNING: Exception Caught during setupTuner: " + std::string(e.what())).c_str());
	}
}

CORBA::Boolean FRONTEND_DigitalTuner_In_i_customized::getTunerEnable(const char* id) {
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return parent->_rtls[tunerNum].frontendStatus->enabled;
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerOutputSampleRate(const char* id, CORBA::Double sr) {
	boost::mutex::scoped_lock lock(portAccess);

	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	if(std::string(id) != parent->_rtls[tunerNum].controlAllocationID)
		throw FRONTEND::FrontendException(("ERROR:: ID: " + std::string(id) + " DOES NOT HAVE AUTHORIZATION TO MODIFY TUNER!").c_str());

	tuner_allocation_struct newParams = parent->feAlloc_from_feStatus(tunerNum);
	newParams.allocation_id = std::string(id);
	newParams.sample_rate = sr;
	try {
		parent->setupTuner(tunerNum, newParams);
	} catch (std::exception& e) {
		throw FRONTEND::FrontendException(("WARNING: Exception Caught during setupTuner: " + std::string(e.what())).c_str());
	}
}

CORBA::Double FRONTEND_DigitalTuner_In_i_customized::getTunerOutputSampleRate(const char* id) {
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());
	return parent->_rtls[tunerNum].frontendStatus->sample_rate;
}

void FRONTEND_DigitalTuner_In_i_customized::setTunerReferenceSource(const char* id,CORBA::Long source){
	boost::mutex::scoped_lock lock(portAccess);
	throw FRONTEND::NotSupportedException("setTunerReferenceSource ENABLE IS NOT CURRENTLY SUPPORTED");
}
CORBA::Long FRONTEND_DigitalTuner_In_i_customized::getTunerReferenceSource(const char* id){
	boost::mutex::scoped_lock lock(portAccess);
	long tunerNum = parent->getTunerMapping(id);
	if (tunerNum < 0)
		throw FRONTEND::FrontendException(("ERROR: ID: " + std::string(id) + " IS NOT ASSOCIATED WITH ANY TUNER!").c_str());

	// NOTE: The RTL 2832 we have only has an internal clock reference,
	// thus we always return 0.
	// if (parent->clock_ref == "INTERNAL")
	CORBA::Long ref = 0;
	return ref;
}
