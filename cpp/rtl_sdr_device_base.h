#ifndef RTL_SDR_DEVICE_IMPL_BASE_H
#define RTL_SDR_DEVICE_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Device_impl.h>

#include "bulkio/bulkio.h"
#include "port_impl.h"
#include "struct_props.h"

#define NOOP 0
#define FINISH -1
#define NORMAL 1

class rtl_sdr_device_base;

template < typename TargetClass >
class ProcessThread
{
    public:
        ProcessThread(TargetClass *_target, float _delay) :
            target(_target)
        {
            _mythread = 0;
            _thread_running = false;
            _udelay = (__useconds_t)(_delay * 1000000);
        };

        // kick off the thread
        void start() {
            if (_mythread == 0) {
                _thread_running = true;
                _mythread = new boost::thread(&ProcessThread::run, this);
            }
        };

        // manage calls to target's service function
        void run() {
            int state = NORMAL;
            while (_thread_running and (state != FINISH)) {
                state = target->serviceFunction();
                if (state == NOOP) usleep(_udelay);
            }
        };

        // stop thread and wait for termination
        bool release(unsigned long secs = 0, unsigned long usecs = 0) {
            _thread_running = false;
            if (_mythread)  {
                if ((secs == 0) and (usecs == 0)){
                    _mythread->join();
                } else {
                    boost::system_time waitime= boost::get_system_time() + boost::posix_time::seconds(secs) +  boost::posix_time::microseconds(usecs) ;
                    if (!_mythread->timed_join(waitime)) {
                        return 0;
                    }
                }
                delete _mythread;
                _mythread = 0;
            }
    
            return 1;
        };

        virtual ~ProcessThread(){
            if (_mythread != 0) {
                release(0);
                _mythread = 0;
            }
        };

        void updateDelay(float _delay) { _udelay = (__useconds_t)(_delay * 1000000); };

    private:
        boost::thread *_mythread;
        bool _thread_running;
        TargetClass *target;
        __useconds_t _udelay;
        boost::condition_variable _end_of_run;
        boost::mutex _eor_mutex;
};

class rtl_sdr_device_base : public Device_impl
{
    public:
        rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl);
        rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, char *compDev);
        rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities);
        rtl_sdr_device_base(char *devMgr_ior, char *id, char *lbl, char *sftwrPrfl, CF::Properties capacities, char *compDev);

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        CORBA::Object_ptr getPort(const char* _id) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

        void loadProperties();

        virtual int serviceFunction() = 0;

    protected:
        ProcessThread<rtl_sdr_device_base> *serviceThread; 
        boost::mutex serviceThreadLock;

        // Member variables exposed as properties
        std::string device_kind;
        std::string device_model;
        CORBA::ULong center_frequency;
        CORBA::Long frequency_correction;
        CORBA::Long tuner_gain;
        CORBA::Long gain_mode;
        CORBA::ULong sample_rate;
        CORBA::Long agc_mode;
        CORBA::Long direct_sampling;
        bool offset_tuning;
        CORBA::ULong device_index;
        CORBA::ULong buffer_size;
        std::string stream_id;
        CORBA::ULong buffer_num;
        bool reserve;
        std::vector<CORBA::Long> if_gains;
        switch_pattern_struct switch_pattern;

        // Ports
        MessageConsumerPort *message_in;
        PropertyEventSupplier *propEvent;
        bulkio::OutShortPort *dataShort_Out;

    private:
        void construct();

};
#endif
