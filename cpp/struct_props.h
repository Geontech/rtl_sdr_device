#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>
#include <ossie/PropertyInterface.h>

struct tuner_allocation_struct {
    tuner_allocation_struct ()
    {
        center_frequency = 0.0;
        bandwidth = 150000.0;
        bandwidth_tolerance = 5.0;
        sample_rate = 256E3;
        sample_rate_tolerance = 5.0;
        device_control = false;
        frequency_correction = 0;
        agc_mode = 0;
        tuner_gain = 0;
    };

    static std::string getId() {
        return std::string("FRONTEND::tuner_allocation");
    };

    std::string tuner_type;
    std::string allocation_id;
    double center_frequency;
    double bandwidth;
    double bandwidth_tolerance;
    double sample_rate;
    double sample_rate_tolerance;
    bool device_control;
    std::string group_id;
    std::string rf_flow_id;
    CORBA::Long frequency_correction;
    CORBA::Long agc_mode;
    CORBA::Long tuner_gain;
};

inline bool operator>>= (const CORBA::Any& a, tuner_allocation_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::tuner_allocation::tuner_type", props[idx].id)) {
            if (!(props[idx].value >>= s.tuner_type)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::allocation_id", props[idx].id)) {
            if (!(props[idx].value >>= s.allocation_id)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::center_frequency", props[idx].id)) {
            if (!(props[idx].value >>= s.center_frequency)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::bandwidth", props[idx].id)) {
            if (!(props[idx].value >>= s.bandwidth)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::bandwidth_tolerance", props[idx].id)) {
            if (!(props[idx].value >>= s.bandwidth_tolerance)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::sample_rate", props[idx].id)) {
            if (!(props[idx].value >>= s.sample_rate)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::sample_rate_tolerance", props[idx].id)) {
            if (!(props[idx].value >>= s.sample_rate_tolerance)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::device_control", props[idx].id)) {
            if (!(props[idx].value >>= s.device_control)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::group_id", props[idx].id)) {
            if (!(props[idx].value >>= s.group_id)) return false;
        }
        if (!strcmp("FRONTEND::tuner_allocation::rf_flow_id", props[idx].id)) {
            if (!(props[idx].value >>= s.rf_flow_id)) return false;
        }
        if (!strcmp("rtl_device::allocation::frequency_correction", props[idx].id)) {
            if (!(props[idx].value >>= s.frequency_correction)) return false;
        }
        if (!strcmp("rtl_device::allocation::agc_mode", props[idx].id)) {
            if (!(props[idx].value >>= s.agc_mode)) return false;
        }
        if (!strcmp("rtl_device::allocation::tuner_gain", props[idx].id)) {
            if (!(props[idx].value >>= s.tuner_gain)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const tuner_allocation_struct& s) {
    CF::Properties props;
    props.length(13);
    props[0].id = CORBA::string_dup("FRONTEND::tuner_allocation::tuner_type");
    props[0].value <<= s.tuner_type;
    props[1].id = CORBA::string_dup("FRONTEND::tuner_allocation::allocation_id");
    props[1].value <<= s.allocation_id;
    props[2].id = CORBA::string_dup("FRONTEND::tuner_allocation::center_frequency");
    props[2].value <<= s.center_frequency;
    props[3].id = CORBA::string_dup("FRONTEND::tuner_allocation::bandwidth");
    props[3].value <<= s.bandwidth;
    props[4].id = CORBA::string_dup("FRONTEND::tuner_allocation::bandwidth_tolerance");
    props[4].value <<= s.bandwidth_tolerance;
    props[5].id = CORBA::string_dup("FRONTEND::tuner_allocation::sample_rate");
    props[5].value <<= s.sample_rate;
    props[6].id = CORBA::string_dup("FRONTEND::tuner_allocation::sample_rate_tolerance");
    props[6].value <<= s.sample_rate_tolerance;
    props[7].id = CORBA::string_dup("FRONTEND::tuner_allocation::device_control");
    props[7].value <<= s.device_control;
    props[8].id = CORBA::string_dup("FRONTEND::tuner_allocation::group_id");
    props[8].value <<= s.group_id;
    props[9].id = CORBA::string_dup("FRONTEND::tuner_allocation::rf_flow_id");
    props[9].value <<= s.rf_flow_id;
    props[10].id = CORBA::string_dup("rtl_device::allocation::frequency_correction");
    props[10].value <<= s.frequency_correction;
    props[11].id = CORBA::string_dup("rtl_device::allocation::agc_mode");
    props[11].value <<= s.agc_mode;
    props[12].id = CORBA::string_dup("rtl_device::allocation::tuner_gain");
    props[12].value <<= s.tuner_gain;
    a <<= props;
};

inline bool operator== (const tuner_allocation_struct& s1, const tuner_allocation_struct& s2) {
    if (s1.tuner_type!=s2.tuner_type)
        return false;
    if (s1.allocation_id!=s2.allocation_id)
        return false;
    if (s1.center_frequency!=s2.center_frequency)
        return false;
    if (s1.bandwidth!=s2.bandwidth)
        return false;
    if (s1.bandwidth_tolerance!=s2.bandwidth_tolerance)
        return false;
    if (s1.sample_rate!=s2.sample_rate)
        return false;
    if (s1.sample_rate_tolerance!=s2.sample_rate_tolerance)
        return false;
    if (s1.device_control!=s2.device_control)
        return false;
    if (s1.group_id!=s2.group_id)
        return false;
    if (s1.rf_flow_id!=s2.rf_flow_id)
        return false;
    if (s1.frequency_correction!=s2.frequency_correction)
        return false;
    if (s1.agc_mode!=s2.agc_mode)
        return false;
    if (s1.tuner_gain!=s2.tuner_gain)
        return false;
    return true;
};

inline bool operator!= (const tuner_allocation_struct& s1, const tuner_allocation_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<tuner_allocation_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    tuner_allocation_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct listener_allocation_struct {
    listener_allocation_struct ()
    {
    };

    static std::string getId() {
        return std::string("FRONTEND::listener_allocation");
    };

    std::string existing_allocation_id;
    std::string listener_allocation_id;
};

inline bool operator>>= (const CORBA::Any& a, listener_allocation_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::listener_allocation::existing_allocation_id", props[idx].id)) {
            if (!(props[idx].value >>= s.existing_allocation_id)) return false;
        }
        if (!strcmp("FRONTEND::listener_allocation::listener_allocation_id", props[idx].id)) {
            if (!(props[idx].value >>= s.listener_allocation_id)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const listener_allocation_struct& s) {
    CF::Properties props;
    props.length(2);
    props[0].id = CORBA::string_dup("FRONTEND::listener_allocation::existing_allocation_id");
    props[0].value <<= s.existing_allocation_id;
    props[1].id = CORBA::string_dup("FRONTEND::listener_allocation::listener_allocation_id");
    props[1].value <<= s.listener_allocation_id;
    a <<= props;
};

inline bool operator== (const listener_allocation_struct& s1, const listener_allocation_struct& s2) {
    if (s1.existing_allocation_id!=s2.existing_allocation_id)
        return false;
    if (s1.listener_allocation_id!=s2.listener_allocation_id)
        return false;
    return true;
};

inline bool operator!= (const listener_allocation_struct& s1, const listener_allocation_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<listener_allocation_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    listener_allocation_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct tuner_status_struct {
    tuner_status_struct ()
    {
        center_frequency = 0;
        bandwidth = 150000;
        sample_rate = 256e3;
        gain_mode = 1;
        agc_mode = 0;
        tuner_gain = -1;
        offset_tuning = false;
        frequency_correction = 0;
        direct_sampling = 0;
        enabled = true;
    };

    static std::string getId() {
        return std::string("FRONTEND::tuner_status");
    };

    std::string tuner_type;
    std::string allocation_id_csv;
    double center_frequency;
    double bandwidth;
    double sample_rate;
    std::string group_id;
    std::string rf_flow_id;
    CORBA::Long gain_mode;
    CORBA::Long agc_mode;
    CORBA::Long tuner_gain;
    bool offset_tuning;
    CORBA::Long frequency_correction;
    CORBA::Long direct_sampling;
    std::string device_name;
    std::string device_serial;
    bool enabled;
};

inline bool operator>>= (const CORBA::Any& a, tuner_status_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FRONTEND::tuner_status::tuner_type", props[idx].id)) {
            if (!(props[idx].value >>= s.tuner_type)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::allocation_id_csv", props[idx].id)) {
            if (!(props[idx].value >>= s.allocation_id_csv)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::center_frequency", props[idx].id)) {
            if (!(props[idx].value >>= s.center_frequency)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::bandwidth", props[idx].id)) {
            if (!(props[idx].value >>= s.bandwidth)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::sample_rate", props[idx].id)) {
            if (!(props[idx].value >>= s.sample_rate)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::group_id", props[idx].id)) {
            if (!(props[idx].value >>= s.group_id)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::rf_flow_id", props[idx].id)) {
            if (!(props[idx].value >>= s.rf_flow_id)) return false;
        }
        if (!strcmp("rtl_device::gain_mode", props[idx].id)) {
            if (!(props[idx].value >>= s.gain_mode)) return false;
        }
        if (!strcmp("rtl_device::agc_mode", props[idx].id)) {
            if (!(props[idx].value >>= s.agc_mode)) return false;
        }
        if (!strcmp("rtl_device::tuner_gain", props[idx].id)) {
            if (!(props[idx].value >>= s.tuner_gain)) return false;
        }
        if (!strcmp("rtl_device::status::offset_tuning", props[idx].id)) {
            if (!(props[idx].value >>= s.offset_tuning)) return false;
        }
        if (!strcmp("rtl_device::status::frequency_correction", props[idx].id)) {
            if (!(props[idx].value >>= s.frequency_correction)) return false;
        }
        if (!strcmp("rtl_device::status::direct_sampling", props[idx].id)) {
            if (!(props[idx].value >>= s.direct_sampling)) return false;
        }
        if (!strcmp("rtl_device::status::device_name", props[idx].id)) {
            if (!(props[idx].value >>= s.device_name)) return false;
        }
        if (!strcmp("rtl_device::status::device_serial", props[idx].id)) {
            if (!(props[idx].value >>= s.device_serial)) return false;
        }
        if (!strcmp("FRONTEND::tuner_status::enabled", props[idx].id)) {
            if (!(props[idx].value >>= s.enabled)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const tuner_status_struct& s) {
    CF::Properties props;
    props.length(16);
    props[0].id = CORBA::string_dup("FRONTEND::tuner_status::tuner_type");
    props[0].value <<= s.tuner_type;
    props[1].id = CORBA::string_dup("FRONTEND::tuner_status::allocation_id_csv");
    props[1].value <<= s.allocation_id_csv;
    props[2].id = CORBA::string_dup("FRONTEND::tuner_status::center_frequency");
    props[2].value <<= s.center_frequency;
    props[3].id = CORBA::string_dup("FRONTEND::tuner_status::bandwidth");
    props[3].value <<= s.bandwidth;
    props[4].id = CORBA::string_dup("FRONTEND::tuner_status::sample_rate");
    props[4].value <<= s.sample_rate;
    props[5].id = CORBA::string_dup("FRONTEND::tuner_status::group_id");
    props[5].value <<= s.group_id;
    props[6].id = CORBA::string_dup("FRONTEND::tuner_status::rf_flow_id");
    props[6].value <<= s.rf_flow_id;
    props[7].id = CORBA::string_dup("rtl_device::gain_mode");
    props[7].value <<= s.gain_mode;
    props[8].id = CORBA::string_dup("rtl_device::agc_mode");
    props[8].value <<= s.agc_mode;
    props[9].id = CORBA::string_dup("rtl_device::tuner_gain");
    props[9].value <<= s.tuner_gain;
    props[10].id = CORBA::string_dup("rtl_device::status::offset_tuning");
    props[10].value <<= s.offset_tuning;
    props[11].id = CORBA::string_dup("rtl_device::status::frequency_correction");
    props[11].value <<= s.frequency_correction;
    props[12].id = CORBA::string_dup("rtl_device::status::direct_sampling");
    props[12].value <<= s.direct_sampling;
    props[13].id = CORBA::string_dup("rtl_device::status::device_name");
    props[13].value <<= s.device_name;
    props[14].id = CORBA::string_dup("rtl_device::status::device_serial");
    props[14].value <<= s.device_serial;
    props[15].id = CORBA::string_dup("FRONTEND::tuner_status::enabled");
    props[15].value <<= s.enabled;
    a <<= props;
};

inline bool operator== (const tuner_status_struct& s1, const tuner_status_struct& s2) {
    if (s1.tuner_type!=s2.tuner_type)
        return false;
    if (s1.allocation_id_csv!=s2.allocation_id_csv)
        return false;
    if (s1.center_frequency!=s2.center_frequency)
        return false;
    if (s1.bandwidth!=s2.bandwidth)
        return false;
    if (s1.sample_rate!=s2.sample_rate)
        return false;
    if (s1.group_id!=s2.group_id)
        return false;
    if (s1.rf_flow_id!=s2.rf_flow_id)
        return false;
    if (s1.gain_mode!=s2.gain_mode)
        return false;
    if (s1.agc_mode!=s2.agc_mode)
        return false;
    if (s1.tuner_gain!=s2.tuner_gain)
        return false;
    if (s1.offset_tuning!=s2.offset_tuning)
        return false;
    if (s1.frequency_correction!=s2.frequency_correction)
        return false;
    if (s1.direct_sampling!=s2.direct_sampling)
        return false;
    if (s1.device_name!=s2.device_name)
        return false;
    if (s1.device_serial!=s2.device_serial)
        return false;
    if (s1.enabled!=s2.enabled)
        return false;
    return true;
};

inline bool operator!= (const tuner_status_struct& s1, const tuner_status_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<tuner_status_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    tuner_status_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

inline bool operator== (const std::vector<tuner_status_struct>& s1, const std::vector<tuner_status_struct>& s2) {
    if (s1.size() != s2.size()) {
        return false;
    }
    for (unsigned int i=0; i<s1.size(); i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
};

inline bool operator!= (const std::vector<tuner_status_struct>& s1, const std::vector<tuner_status_struct>& s2) {
    return !(s1==s2);
};

template<> inline short StructSequenceProperty<tuner_status_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    std::vector<tuner_status_struct> tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

#endif
