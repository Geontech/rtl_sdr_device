#ifndef PORT_H
#define PORT_H

#include "ossie/Port_impl.h"
#include <queue>
#include <list>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>

class rtl_sdr_device_base;
class rtl_sdr_device_i;

#define CORBA_MAX_TRANSFER_BYTES omniORB::giopMaxMsgSize()

#include <ossie/MessageInterface.h>
#include <ossie/PropertyInterface.h>
#include "struct_props.h"

#endif
