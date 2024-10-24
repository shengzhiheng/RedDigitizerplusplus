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

// using RedDigitizer namespace declarations
using RedDigitizer::CAEN;
using RedDigitizer::CAENDigitizerModel;
using RedDigitizer::CAENConnectionType;
using RedDigitizer::CAENGlobalConfig;
using RedDigitizer::CAENGroupConfig;
using RedDigitizer::iostream_wrapper;

// declare to be C compatible, so it can be imported into python
#ifdef __cplusplus
extern "C" {
#endif

// create class and attempt to connect to digitizer
// For definition of variables and cosntants, see red_digitizer_helper.hpp
void* caen_init(int model, int conn_type, int link_num, int conet_node, unsigned int vme_addr) {
    try {
        CAENDigitizerModel caen_model = static_cast<CAENDigitizerModel>(model);
        CAENConnectionType caen_ct = static_cast<CAENConnectionType>(conn_type);

        // Use the default Logger and EventBufferSize
        auto caen_instance = new CAEN<>(std::make_shared<iostream_wrapper>(), caen_model, caen_ct, link_num, conet_node, vme_addr);

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

// checks for error
int caen_has_error(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->HasError() ? 1 : 0;  // Return 1 for true, 0 for false
    }
    return 0;
}

// checks for warning
int caen_has_warning(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return caen_instance->HasWarning() ? 1 : 0;  // Return 1 for true, 0 for false
    }
    return 0;
}

// reset warning flag
void caen_reset_warning(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->ResetWarning();
    }
}

// get board info
const void* caen_get_board_info(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return static_cast<const void*>(&caen_instance->GetBoardInfo());
    }
    return nullptr;
}

// get global configuration
const void* caen_get_global_configs(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        return static_cast<const void*>(&caen_instance->GetGlobalConfiguration());
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

// retrieve data
void caen_retrieve_data(void* caen_handle) {
    if (caen_handle) {
        CAEN<>* caen_instance = static_cast<CAEN<>*>(caen_handle);
        caen_instance->RetrieveData();
    }
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

// get waveform record length
uint32_t wf_get_record_length(void* waveform_handle) {
    if (waveform_handle) {
        auto* waveform = static_cast<CAEN<T, N>::CAENWaveforms<uint16_t>*>(waveform_handle);
        return waveform->getRecordLength();
    }
    return 0;
}

// get total size
uint32_t wf_get_total_size(void* waveform_handle) {
    if (waveform_handle) {
        auto* waveform = static_cast<CAEN<T, N>::CAENWaveforms<uint16_t>*>(waveform_handle);
        return waveform->getTotalSize();
    }
    return 0;
}

// get number of enabled channels
uint32_t wf_get_num_enabled_channels(void* waveform_handle) {
    if (waveform_handle) {
        auto* waveform = static_cast<CAEN<T, N>::CAENWaveforms<uint16_t>*>(waveform_handle);
        return waveform->getNumEnabledChannels();
    }
    return 0;
}

// get waveform data of one channel
uint16_t* wf_get_channel_data(void* waveform_handle, std::size_t ch, std::size_t* out_size) {
    if (!waveform_handle) return nullptr;
    CAEN<T, N>::CAENWaveforms<uint16_t>* waveform = static_cast<CAEN<T, N>::CAENWaveforms<uint16_t>*>(waveform_handle);

    // Get the data vector for the given channel
    std::vector<uint16_t> data = waveform->get(ch);

    // Set the size of the output array
    *out_size = data.size();

    // Allocate memory for the data to be returned (caller needs to free this)
    uint16_t* data_array = new uint16_t[data.size()];
    std::copy(data.begin(), data.end(), data_array);

    return data_array;  // Return the pointer to the data
}
// get waveform for all channels
template<typename T, size_t N>
uint16_t* wf_get_data(void* waveform_handle, std::size_t* out_size) {
    if (!waveform_handle) return nullptr;
    CAEN<T, N>::CAENWaveforms<uint16_t>* waveform = static_cast<CAEN<T, N>::CAENWaveforms<uint16_t>*>(waveform_handle);

    // Get the data span
    std::span<uint16_t> data_span = waveform->getData();

    // Set the size of the output array
    *out_size = data_span.size();

    // Allocate memory for the data to be returned (caller needs to free this)
    uint16_t* data_array = new uint16_t[data_span.size()];
    std::copy(data_span.begin(), data_span.end(), data_array);

    return data_array;  // Return the pointer to the data
}

// close out extern C
#ifdef __cplusplus
}
#endif

#endif // for CAEN_RC_HPP
