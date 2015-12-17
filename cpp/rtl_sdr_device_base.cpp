#include "rtl_sdr_device_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the device class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl) :
    ExecutableDevice_impl(devMgr_ior, id, lbl, sftwrPrfl),
    serviceThread(0)
{
    construct();
}

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev) :
    ExecutableDevice_impl(devMgr_ior, id, lbl, sftwrPrfl, compDev),
    serviceThread(0)
{
    construct();
}

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities) :
    ExecutableDevice_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities),
    serviceThread(0)
{
    construct();
}

rtl_sdr_device_base::rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev) :
    ExecutableDevice_impl(devMgr_ior, id, lbl, sftwrPrfl, capacities, compDev),
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
    DigitalTuner_provides = new FRONTEND_DigitalTuner_In_i("DigitalTuner_provides", this);
    oid = ossie::corba::RootPOA()->activate_object(DigitalTuner_provides);
    dataShort_Out = new bulkio::OutShortPort("dataShort_Out");
    oid = ossie::corba::RootPOA()->activate_object(dataShort_Out);

    registerInPort(DigitalTuner_provides);
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
        if (!strcmp(_id,"DigitalTuner_provides")) {
            FRONTEND_DigitalTuner_In_i *ptr = dynamic_cast<FRONTEND_DigitalTuner_In_i *>(p_in->second);
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

    delete(DigitalTuner_provides);
    delete(dataShort_Out);

    ExecutableDevice_impl::releaseObject();
}

void rtl_sdr_device_base::loadProperties()
{
    addProperty(device_kind,
                "FRONTEND",
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

    addProperty(group_id_global,
                "rtls",
                "group_id_global",
                "",
                "readwrite",
                "",
                "external",
                "execparam");

    addProperty(tuner_allocation,
                tuner_allocation_struct(),
                "FRONTEND::tuner_allocation",
                "tuner_allocation",
                "readwrite",
                "",
                "external",
                "allocation");

    addProperty(listener_allocation,
                listener_allocation_struct(),
                "FRONTEND::listener_allocation",
                "listener_allocation",
                "readwrite",
                "",
                "external",
                "allocation");

    addProperty(tuner_statuses,
                "tuner_statuses",
                "tuner_statuses",
                "readonly",
                "",
                "external",
                "configure");

}
