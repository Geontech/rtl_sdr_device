#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>
#include <ossie/PropertyInterface.h>

struct switch_pattern_struct {
    switch_pattern_struct ()
    {
    };

    static std::string getId() {
        return std::string("switch_pattern");
    };

    CORBA::Long value;
};

inline bool operator>>= (const CORBA::Any& a, switch_pattern_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("value", props[idx].id)) {
            if (!(props[idx].value >>= s.value)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const switch_pattern_struct& s) {
    CF::Properties props;
    props.length(1);
    props[0].id = CORBA::string_dup("value");
    props[0].value <<= s.value;
    a <<= props;
};

inline bool operator== (const switch_pattern_struct& s1, const switch_pattern_struct& s2) {
    if (s1.value!=s2.value)
        return false;
    return true;
};

inline bool operator!= (const switch_pattern_struct& s1, const switch_pattern_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<switch_pattern_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    switch_pattern_struct tmp;
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
