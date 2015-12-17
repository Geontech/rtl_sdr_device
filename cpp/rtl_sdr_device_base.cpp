#include "rtl_sdr_device_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the device class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl) :
    Device_impl(devMgr_ior, id, lbl, sftwrPrfl),
    serviceThread(0)
{
    construct();
}

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev) :
    Device_impl(devMgr_ior, id, lbl, sftwrPrfl, compDev),
    serviceThread(0)
{
    construct();
}

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities) :
    Device_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities),
    serviceThread(0)
{
    construct();
}

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev) :
    Device_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities, compDev),
    serviceThread(0)
{
    construct();
}

void rtl_sdr_device_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    message_in = new MessageConsumerPort("message_in");
    oid = ossie::corba::RootPOA()->activate_object(message_in);
    propEvent = new PropertyEventSupplier("propEvent");
    oid = ossie::corba::RootPOA()->activate_object(propEvent);
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("center_frequency"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("frequency_correction"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("tuner_gain"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("gain_mode"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("sample_rate"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("agc_mode"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("direct_sampling"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("offset_tuning"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("buffer_size"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("stream_id"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("buffer_num"));
    propEvent->registerProperty(this->_identifier, this->naming_service_name, this->getPropertyFromId("if_gains"));
    this->registerPropertyChangePort(propEvent);
    dataShort_Out = new bulkio::OutShortPort("dataShort_Out");
    oid = ossie::corba::RootPOA()->activate_object(dataShort_Out);

    registerInPort(message_in);
    registerOutPort(propEvent, propEvent->_this());
    registerOutPort(dataShort_Out, dataShort_Out->_this());
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void rtl_sdr_device_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void rtl_sdr_device_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        serviceThread = new ProcessThread<rtl_sdr_device_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void rtl_sdr_device_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr rtl_sdr_device_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {
        if (!strcmp(_id,"message_in")) {
            MessageConsumerPort *ptr = dynamic_cast<MessageConsumerPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void rtl_sdr_device_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the device running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    // deactivate ports
    releaseInPorts();
    releaseOutPorts();

    delete(message_in);
    delete(propEvent);
    delete(dataShort_Out);

    Device_impl::releaseObject();
}

void rtl_sdr_device_base::loadProperties()
{
    addProperty(device_kind,
                "receiver",
                "DCE:cdc5ee18-7ceb-4ae6-bf4c-31f983179b4d",
                "device_kind",
                "readonly",
                "",
                "eq",
                "allocation,configure");

    addProperty(device_model,
                "RTL2832",
                "DCE:0f99b2e4-9903-4631-9846-ff349d18ecfb",
                "device_model",
                "readonly",
                "",
                "eq",
                "allocation,configure");

    addProperty(center_frequency,
                10000000,
                "center_frequency",
                "",
                "readwrite",
                "Hz",
                "external",
                "configure,event");

    addProperty(frequency_correction,
                0,
                "frequency_correction",
                "",
                "readwrite",
                "PPM",
                "external",
                "configure,event");

    addProperty(tuner_gain,
                0,
                "tuner_gain",
                "",
                "readwrite",
                "tenths of dB",
                "external",
                "configure,event");

    addProperty(gain_mode,
                1,
                "gain_mode",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(sample_rate,
                256000,
                "sample_rate",
                "",
                "readwrite",
                "Hz",
                "external",
                "configure,event");

    addProperty(agc_mode,
                0,
                "agc_mode",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(direct_sampling,
                0,
                "direct_sampling",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(offset_tuning,
                false,
                "offset_tuning",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(device_index,
                0,
                "device_index",
                "",
                "readwrite",
                "",
                "external",
                "execparam");

    addProperty(buffer_size,
                65536,
                "buffer_size",
                "",
                "readwrite",
                "Bytes",
                "external",
                "configure,event");

    addProperty(stream_id,
                "rtl_sdr_1",
                "stream_id",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(buffer_num,
                8,
                "buffer_num",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(reserve,
                false,
                "reserve",
                "",
                "readwrite",
                "",
                "external",
                "allocation");

    // Set the sequence with its initial values
    if_gains.push_back(0);
    if_gains.push_back(0);
    if_gains.push_back(0);
    if_gains.push_back(0);
    if_gains.push_back(0);
    if_gains.push_back(0);
    addProperty(if_gains,
                if_gains,
                "if_gains",
                "",
                "readwrite",
                "",
                "external",
                "configure,event");

    addProperty(switch_pattern,
                switch_pattern_struct(),
                "switch_pattern",
                "",
                "readwrite",
                "",
                "external",
                "message");

}
