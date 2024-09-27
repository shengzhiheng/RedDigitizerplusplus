/*H
* FILENAME: caen_rc.hpp
*
* DESCRIPTION: 
*    A wrapper for RedDigitizer++ library, exposing functions used by 
*    run control in C-compatible syntax, so it can be imported in python
*
* AUTHOR: Zhiheng Sheng
*
* DATE: 2024-09-26
*
*H*/

#ifndef CAEN_RC_HPP
#define CAEN_RC_HPP
#pragma once

// include red digitizer plus plus library
#include "red_digitizer_helper.hpp"
#include "logger_helpers.hpp"

// declare to be C compatible, so it can be imported into python
#ifdef __cplusplus
extern "C" {
#endif

// create class and attempt to connect to digitizer
// For definition of variables and cosntants, see red_digitizer_helper.hpp
void* caen_init(int model, int conn_type, int link_num, int conet_node, unsigned int vme_addr) {
    try {
        CAENDigitizerModel caen_model = static_cast<CAENDigitizerModel>(model);
        CAENConnectionType caen_ct = static_cast<CAENConnectionType>(connection_type);

        // Use the default Logger and EventBufferSize
        auto caen_instance = new CAEN<>(std::make_shared<iostream_wrapper>(), caen_model, caen_ct, link_num, conet_node, vme_base_address);

        return static_cast<void*>(caen_instance);  // Return a generic void* pointer
    } catch (const std::exception& ex) {
        std::cerr << "Error creating CAEN object: " << ex.what() << std::endl;
        return nullptr;
    }
}

// destroy caen object
void caen_destroy(void* caen_handle){
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        delete caen_instance; // invokes destructor
    }
}

// checks if the caen is connected
int caen_is_connected(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->IsConnected() ? 1 : 0;  // Return 1 for true, 0 for false
    }
    return 0;  // Return 0 if caen_handle is null
}

// get board info
const void* caen_get_board_info(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return static_cast<const void*>(&caen_instance->GetBoardInfo());
    }
    return nullptr;
}

// get group configuration
const void* caen_get_group_configs(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return static_cast<const void*>(&caen_instance->GetGroupConfigurations());
    }
    return nullptr;
}

// get group configurations
const void* caen_get_group_configs(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return static_cast<const void*>(&caen_instance->GetGroupConfigurations());
    }
    return nullptr;
}

// get number of events
unsigned int caen_get_number_of_events(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->GetNumberOfEvents();
    }
    return 0;
}

// get current max buffer
unsigned int caen_get_current_max_buffer(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->GetCurrentPossibleMaxBuffer();
    }
    return 0;
}

// setup caen
void caen_setup(void* caen_handle, const void* global_config, const void* group_configs) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        const CAENGlobalConfig* config = static_cast<const CAENGlobalConfig*>(global_config);
        const std::array<CAENGroupConfig, 8>* groups = static_cast<const std::array<CAENGroupConfig, 8>*>(group_configs);
        caen_instance->Setup(*config, *groups);
    }
}

// reset caen
void caen_reset(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->Reset();
    }
}

// enable acquisition
void caen_enable_acquisition(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->EnableAcquisition();
    }
}

// disable acquisition
void caen_disable_acquisition(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->DisableAcquisition();
    }
}

// write register
void caen_write_register(void* caen_handle, uint32_t addr, uint32_t value) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->WriteRegister(addr, value);
    }
}

// read register
void caen_read_register(void* caen_handle, uint32_t addr, uint32_t* value) {
    if (caen_handle && value) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->ReadRegister(addr, *value);
    }
}

// write bits
void caen_write_bits(void* caen_handle, uint32_t addr, uint32_t value, uint8_t pos, uint8_t len) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->WriteBits(addr, value, pos, len);
    }
}

// software trigger
void caen_software_trigger(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->SoftwareTrigger();
    }
}

// get event in buffer
unsigned int caen_get_events_in_buffer(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->GetEventsInBuffer();
    }
    return 0;
}

// retrieve data until events
int caen_retrieve_data_until_n_events(void* caen_handle, unsigned int n) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->RetrieveDataUntilNEvents(n) ? 1 : 0;
    }
    return 0;
}

// decode event
const void* caen_decode_event(void* caen_handle, unsigned int i) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->DecodeEvent(i).get();  // Assuming DecodeEvent returns a pointer to event
    }
    return nullptr;
}

// decode events
void caen_decode_events(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->DecodeEvents();
    }
}

// clear data
void caen_clear_data(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->ClearData();
    }
}

// get waveform
const void* caen_get_waveform(void* caen_handle, std::size_t i) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->GetWaveform(i).get();  // Assuming GetWaveform returns a shared pointer
    }
    return nullptr;
}

// close out extern C
#ifdef __cplusplus
}
#endif

#endif // for CAEN_RC_HPP
