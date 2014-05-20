/*
 * Copyright (c) 2011-2014, fortiss GmbH.
 * Licensed under the Apache License, Version 2.0.
 *
 * Use, modification and distribution are subject to the terms specified
 * in the accompanying license file LICENSE.txt located at the root directory
 * of this software distribution. A copy is available at
 * http://chromosome.fortiss.org/.
 *
 * This file is part of CHROMOSOME.
 *
 * $Id: sensorKBComponentWrapper.c 7774 2014-03-11 17:38:48Z geisinger $
 */

/**
 * \file
 *         Component wrapper - implements interface of a component
 *              to the data handler
 *
 * \author
 *         This file has been generated by the CHROMOSOME Modeling Tool (XMT)
 *         (fortiss GmbH).
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "sensorMonitor/adv/sensorKB/include/sensorKBComponentWrapper.h"

#include "xme/core/dataHandler/include/dataHandler.h"
#include "xme/core/log.h"

#ifdef XME_MULTITHREAD
#include "xme/hal/include/tls.h"
#endif // #ifdef XME_MULTITHREAD

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \brief Structure for storing information about the output ports.
 */
typedef struct
{
    xme_core_dataManager_dataPacketId_t dataPacketId; ///< The data packet id.
    xme_core_component_portState_t state; ///< State of the port. Denotes whether the port is locked, whether data and/or attributes have been written to it and whether an error occurred.
} outputPort_t;

/******************************************************************************/
/***   Variables                                                            ***/
/******************************************************************************/
/**
 * \brief Number of times this component wrapper has been initialized.
 */
static uint16_t initializationCount = 0U;

/**
 * \brief Array storing information about the output ports.
 */
static outputPort_t outputPorts[] = {
    {XME_CORE_DATAMANAGER_DATAPACKETID_INVALID, { 0U } } // sensorKB.sensorValueOut
};

/**
 * \brief Size of outputPorts array.
 */
static const uint8_t outputPortCount = sizeof(outputPorts) / sizeof(outputPorts[0]);

#ifdef XME_MULTITHREAD
static xme_hal_tls_handle_t outputPortAccessed = XME_HAL_TLS_INVALID_TLS_HANDLE; ///< Bitmap storing whether a specific thread has accessed an output port in multithreading scenarios.
#endif // #ifdef XME_MULTITHREAD

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_status_t
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_init(void)
{
    if (0U == initializationCount)
    {
        #ifdef XME_MULTITHREAD
        if (XME_HAL_TLS_INVALID_TLS_HANDLE == outputPortAccessed)
        {
            outputPortAccessed = xme_hal_tls_alloc((outputPortCount + 7U) / 8U);
            XME_ASSERT(XME_HAL_TLS_INVALID_TLS_HANDLE != outputPortAccessed);
        }
        #endif // #ifdef XME_MULTITHREAD
    }

    initializationCount++;

    return XME_STATUS_SUCCESS;
}

void
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_fini(void)
{
    XME_ASSERT_NORVAL(initializationCount > 0U);
    initializationCount--;

    if (0U == initializationCount)
    {
        #ifdef XME_MULTITHREAD
        if (XME_HAL_TLS_INVALID_TLS_HANDLE != outputPortAccessed)
        {
            xme_hal_tls_free(outputPortAccessed);
        }
        #endif // #ifdef XME_MULTITHREAD
    }
}

xme_status_t
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_receivePort
(
    xme_core_dataManager_dataPacketId_t dataPacketId,
    sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_internalPortId_t componentInternalPortId
)
{
    XME_CHECK
    (
        outputPortCount > componentInternalPortId,
        XME_STATUS_INVALID_PARAMETER
    );

    {
        outputPorts[componentInternalPortId].dataPacketId = dataPacketId;
        outputPorts[componentInternalPortId].state.dataValid = 0;
        outputPorts[componentInternalPortId].state.attributesValid = 0;
        outputPorts[componentInternalPortId].state.locked = 0;
        outputPorts[componentInternalPortId].state.error = 0;
    }

    return XME_STATUS_SUCCESS;
}

void
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_completeWriteOperations(void)
{
    uint8_t outputPortIndex;

    for (outputPortIndex = 0U; outputPortIndex < outputPortCount; outputPortIndex++)
    {
        // This is a no-operation in case the port has not been accessed
        (void) sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_writeNextPacket(outputPortIndex);
    }
}

xme_status_t
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_writePortSensorValueOut
(
    const sensorMonitor_topic_sensorData_t* const data
)
{
    uint8_t outputPortIndex;
    xme_status_t status;
#ifdef XME_MULTITHREAD
    char* accessed;
#endif // #ifdef XME_MULTITHREAD

    outputPortIndex = ((uint8_t)SENSORMONITOR_ADV_SENSORKB_SENSORKBCOMPONENTWRAPPER_PORT_SENSORVALUEOUT);

    XME_CHECK_REC
    (
        NULL != data,
        XME_STATUS_SUCCESS,
        {
            outputPorts[outputPortIndex].state.dataValid = 0;
        }
    );

#ifdef XME_MULTITHREAD
    XME_ASSERT(outputPortAccessed != XME_HAL_TLS_INVALID_TLS_HANDLE);
    accessed = (char*) xme_hal_tls_get(outputPortAccessed);
    XME_ASSERT(NULL != accessed);

    if (!(accessed[outputPortIndex / 8U] & (1 << (outputPortIndex % 8U))))
#else // #ifdef XME_MULTITHREAD
    if (!outputPorts[outputPortIndex].state.locked)
#endif // #ifdef XME_MULTITHREAD
    {
        status = xme_core_dataHandler_startWriteOperation(outputPorts[outputPortIndex].dataPacketId);
        XME_CHECK(XME_STATUS_SUCCESS == status, XME_STATUS_INTERNAL_ERROR);
#ifdef XME_MULTITHREAD
        accessed[outputPortIndex / 8U] |= (1 << (outputPortIndex % 8U));
#endif // #ifdef XME_MULTITHREAD
        outputPorts[outputPortIndex].state.locked = 1;
    }

    status = xme_core_dataHandler_writeData
    (
        outputPorts[outputPortIndex].dataPacketId,
        (void*)data,
        (uint32_t)sizeof(sensorMonitor_topic_sensorData_t)
    );

    if (XME_STATUS_SUCCESS == status)
    {
        outputPorts[outputPortIndex].state.dataValid = 1;
    }
    else
    {
        outputPorts[outputPortIndex].state.error = 1;

        XME_LOG(XME_LOG_ERROR,
            "[sensorKBComponentWrapper] writePortSensorValueOut(): Writing to port failed. Data Handler returned error code %d.\n",
            status);
    }

    return status;
}

xme_status_t
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_writeOutputPortAttribute
(
    sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_internalPortId_t portId,
    xme_core_attribute_key_t attributeKey,
    const void* const buffer,
    uint32_t bufferSize
)
{
    xme_status_t status;
    uint8_t outputPortIndex;
#ifdef XME_MULTITHREAD
    char* accessed;
#endif // #ifdef XME_MULTITHREAD

    outputPortIndex = ((uint8_t)portId);

#ifdef XME_MULTITHREAD
    XME_ASSERT(outputPortAccessed != XME_HAL_TLS_INVALID_TLS_HANDLE);
    accessed = (char*) xme_hal_tls_get(outputPortAccessed);
    XME_ASSERT(NULL != accessed);

    if (!(accessed[outputPortIndex / 8U] & (1 << (outputPortIndex % 8U))))
#else // #ifdef XME_MULTITHREAD
    if (!outputPorts[outputPortIndex].state.locked)
#endif // #ifdef XME_MULTITHREAD
    {
        status = xme_core_dataHandler_startWriteOperation(outputPorts[outputPortIndex].dataPacketId);
        XME_CHECK(XME_STATUS_SUCCESS == status, XME_STATUS_INTERNAL_ERROR);
#ifdef XME_MULTITHREAD
        accessed[outputPortIndex / 8U] |= (1 << (outputPortIndex % 8U));
#endif // #ifdef XME_MULTITHREAD
        outputPorts[outputPortIndex].state.locked = 1;
    }

    status = xme_core_dataHandler_writeAttribute
    (
        outputPorts[outputPortIndex].dataPacketId,
        attributeKey,
        buffer,
        bufferSize
    );

    if (XME_STATUS_SUCCESS == status)
    {
        outputPorts[outputPortIndex].state.attributesValid = 1;
    }
    else
    {
        outputPorts[outputPortIndex].state.error = 1;

        XME_LOG
        (
            XME_LOG_ERROR,
            "%s:%d sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_writeOutputPortAttribute: Writing to port failed. Data Handler returned error code %d.",
            __FILE__,
            __LINE__,
            status
        );
    }

    return status;
}

xme_status_t
sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_writeNextPacket
(
    sensorMonitor_adv_sensorKB_sensorKBComponentWrapper_internalPortId_t portId
)
{
    xme_status_t status = XME_STATUS_SUCCESS;
#ifdef XME_MULTITHREAD
    char* accessed;
#endif // #ifdef XME_MULTITHREAD

    XME_ASSERT(portId < outputPortCount);

#ifdef XME_MULTITHREAD
    XME_ASSERT(outputPortAccessed != XME_HAL_TLS_INVALID_TLS_HANDLE);
    accessed = (char*) xme_hal_tls_get(outputPortAccessed);
    XME_ASSERT(NULL != accessed);

    // We need to complete the write operation if the port has been accessed
    if (accessed[portId / 8U] & (1 << (portId % 8U)))
#else // #ifdef XME_MULTITHREAD
    if (outputPorts[portId].state.locked)
#endif // #ifdef XME_MULTITHREAD
    {
        status = xme_core_dataHandler_completeWriteOperation(outputPorts[portId].dataPacketId);
        XME_CHECK_MSG
        (
            XME_STATUS_SUCCESS == status || XME_STATUS_NOT_FOUND == status || XME_STATUS_BUFFER_TOO_SMALL == status,
            XME_STATUS_INTERNAL_ERROR,
            XME_LOG_ERROR,
            "[sensorKBComponentWrapper] CompleteWriteOperation for port (interalPortId: %d, dataPacketId: %d) returned error code %d.\n",
            portId, outputPorts[portId], status
        );

#ifdef XME_MULTITHREAD
        accessed[portId / 8U] &= ~(1 << (portId % 8U));
#endif // #ifdef XME_MULTITHREAD
        outputPorts[portId].state.dataValid = 0;
        outputPorts[portId].state.attributesValid = 0;
        outputPorts[portId].state.locked = 0;
        outputPorts[portId].state.error = 0;

        if (XME_STATUS_BUFFER_TOO_SMALL == status)
        {
            XME_LOG
            (
                XME_LOG_WARNING,
                "[sensorKBComponentWrapper] Data loss on port (interalPortId: %d, dataPacketId: %d), because of full buffer. Oldest value was overwritten.\n",
                portId, outputPorts[portId]
            );
        }
    }

    return status;
}
