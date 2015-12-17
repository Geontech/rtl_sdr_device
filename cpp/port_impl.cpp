/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    Source: rtl_sdr_device.spd.xml

*******************************************************************************************/

#include "rtl_sdr_device.h"

// ----------------------------------------------------------------------------------------
// BULKIO_dataShort_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataShort_Out_i::BULKIO_dataShort_Out_i(std::string port_name, rtl_sdr_device_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<rtl_sdr_device_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataShort_Out_i::~BULKIO_dataShort_Out_i()
{
}

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
void BULKIO_dataShort_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataShort_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataShort_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}

// ----------------------------------------------------------------------------------------
// FRONTEND_DigitalTuner_In_i definition
// ----------------------------------------------------------------------------------------
FRONTEND_DigitalTuner_In_i::FRONTEND_DigitalTuner_In_i(std::string port_name, rtl_sdr_device_base *_parent) : 
Port_Provides_base_impl(port_name)
{
    parent = static_cast<rtl_sdr_device_i *> (_parent);
}

FRONTEND_DigitalTuner_In_i::~FRONTEND_DigitalTuner_In_i()
{
}

char* FRONTEND_DigitalTuner_In_i::getTunerType(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    char* tmpVal;
    // TODO: Fill in this function

    return CORBA::string_dup(tmpVal);
}

CORBA::Boolean FRONTEND_DigitalTuner_In_i::getTunerDeviceControl(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Boolean tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

char* FRONTEND_DigitalTuner_In_i::getTunerGroupId(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    char* tmpVal;
    // TODO: Fill in this function

    return CORBA::string_dup(tmpVal);
}

char* FRONTEND_DigitalTuner_In_i::getTunerRfFlowId(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    char* tmpVal;
    // TODO: Fill in this function

    return CORBA::string_dup(tmpVal);
}

CF::Properties* FRONTEND_DigitalTuner_In_i::getTunerStatus(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CF::Properties* tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerCenterFrequency(const char* id, CORBA::Double freq)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Double FRONTEND_DigitalTuner_In_i::getTunerCenterFrequency(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Double tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerBandwidth(const char* id, CORBA::Double bw)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Double FRONTEND_DigitalTuner_In_i::getTunerBandwidth(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Double tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerAgcEnable(const char* id, CORBA::Boolean enable)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Boolean FRONTEND_DigitalTuner_In_i::getTunerAgcEnable(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Boolean tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerGain(const char* id, CORBA::Float gain)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Float FRONTEND_DigitalTuner_In_i::getTunerGain(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Float tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerReferenceSource(const char* id, CORBA::Long source)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Long FRONTEND_DigitalTuner_In_i::getTunerReferenceSource(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Long tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerEnable(const char* id, CORBA::Boolean enable)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Boolean FRONTEND_DigitalTuner_In_i::getTunerEnable(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Boolean tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}

void FRONTEND_DigitalTuner_In_i::setTunerOutputSampleRate(const char* id, CORBA::Double sr)
{
    boost::mutex::scoped_lock lock(portAccess);
    // TODO: Fill in this function
}

CORBA::Double FRONTEND_DigitalTuner_In_i::getTunerOutputSampleRate(const char* id)
{
    boost::mutex::scoped_lock lock(portAccess);
    CORBA::Double tmpVal;
    // TODO: Fill in this function

    return tmpVal;
}



