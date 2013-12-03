/*
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include "SubsystemObject.h"
#include "Subsystem.h"
#include "InstanceConfigurableElement.h"
#include "ParameterBlackboard.h"
#include "ParameterAccessContext.h"
#include "MappingContext.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <stdarg.h>

CSubsystemObject::CSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement)
    : _pInstanceConfigurableElement(pInstanceConfigurableElement),
      _uiDataSize(pInstanceConfigurableElement->getFootPrint()),
      _pucBlackboardLocation(NULL),
      _uiAccessedIndex(0)
{
    // Syncer
    _pInstanceConfigurableElement->setSyncer(this);
}

CSubsystemObject::~CSubsystemObject()
{
    _pInstanceConfigurableElement->unsetSyncer();
}

string CSubsystemObject::getFormattedMappingValue() const
{
    // Default formatted mapping value is empty
    return "";
}

// Blackboard data location
uint8_t* CSubsystemObject::getBlackboardLocation() const
{
    return _pucBlackboardLocation;
}

// Size
uint32_t CSubsystemObject::getSize() const
{
    return _uiDataSize;
}

// Conversion utility
uint32_t CSubsystemObject::asInteger(const string& strValue)
{
    return strtoul(strValue.c_str(), NULL, 0);
}

string CSubsystemObject::asString(uint32_t uiValue)
{
    ostringstream ostr;

    ostr << uiValue;

    return ostr.str();
}

// Default back synchronization
void CSubsystemObject::setDefaultValues(CParameterBlackboard& parameterBlackboard) const
{
    string strError;

    // Create access context
    CParameterAccessContext parameterAccessContext(strError, &parameterBlackboard, false);

    // Just implement back synchronization with default values
    _pInstanceConfigurableElement->setDefaultValues(parameterAccessContext);
}

// Synchronization
bool CSubsystemObject::sync(CParameterBlackboard& parameterBlackboard, bool bBack, string& strError)
{
    // Get blackboard location
    _pucBlackboardLocation = parameterBlackboard.getLocation(_pInstanceConfigurableElement->getOffset());
    // Access index init
    _uiAccessedIndex = 0;

#ifdef SIMULATION
    return true;
#endif

    // Retrieve subsystem
    const CSubsystem* pSubsystem = _pInstanceConfigurableElement->getBelongingSubsystem();

    // Get it's health insdicator
    bool bIsSubsystemAlive = pSubsystem->isAlive();

    // Check subsystem health
    if (!bIsSubsystemAlive) {

        strError = "Susbsystem not alive";
    }

    // Synchronize to/from HW
    if (!bIsSubsystemAlive || !accessHW(bBack, strError)) {

        strError = string("Unable to ") + (bBack ? "back" : "forward") + " synchronize configurable element " +
                _pInstanceConfigurableElement->getPath() + ": " + strError;

        log_warning(strError);

        // Fall back to parameter default initialization
        if (bBack) {

           setDefaultValues(parameterBlackboard);
        }
        return false;
    }

    return true;
}

// Sync to/from HW
bool CSubsystemObject::sendToHW(string& strError)
{
    strError = "Send to HW interface not implemented at subsystem level";

    return false;
}

bool CSubsystemObject::receiveFromHW(string& strError)
{
    (void)strError;

    // Back synchronization is not supported at subsystem level.
    // Rely on blackboard content

    return true;
}

// Fall back HW access
bool CSubsystemObject::accessHW(bool bReceive, string& strError)
{
    // Default access fall back
    if (bReceive) {

        return receiveFromHW(strError);
    } else {

        return sendToHW(strError);
    }
}

// Blackboard access from subsystems
void CSubsystemObject::blackboardRead(void* pvData, uint32_t uiSize)
{
    assert(_uiAccessedIndex + uiSize <= _uiDataSize);

    memcpy(pvData, _pucBlackboardLocation + _uiAccessedIndex, uiSize);

    _uiAccessedIndex += uiSize;
}

void CSubsystemObject::blackboardWrite(const void* pvData, uint32_t uiSize)
{
    assert(_uiAccessedIndex + uiSize <= _uiDataSize);

    memcpy(_pucBlackboardLocation + _uiAccessedIndex, pvData, uiSize);

    _uiAccessedIndex += uiSize;
}

// Logging
void CSubsystemObject::log_info(const string& strMessage, ...) const
{
    char *pacBuffer;
    va_list listPointer;

    va_start(listPointer, strMessage);

    vasprintf(&pacBuffer,  strMessage.c_str(), listPointer);

    va_end(listPointer);

    _pInstanceConfigurableElement->log_info(pacBuffer);

    free(pacBuffer);
}

void CSubsystemObject::log_warning(const string& strMessage, ...) const
{
    char *pacBuffer;
    va_list listPointer;

    va_start(listPointer, strMessage);

    vasprintf(&pacBuffer,  strMessage.c_str(), listPointer);

    va_end(listPointer);

    _pInstanceConfigurableElement->log_warning(pacBuffer);

    free(pacBuffer);
}

// Configurable element retrieval
const CInstanceConfigurableElement* CSubsystemObject::getConfigurableElement() const
{
    return _pInstanceConfigurableElement;
}
// Belonging Subsystem retrieval
const CSubsystem* CSubsystemObject::getSubsystem() const
{
    return _pInstanceConfigurableElement->getBelongingSubsystem();
}
