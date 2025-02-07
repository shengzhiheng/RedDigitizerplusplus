/*
T

*/

#ifndef RD_HELPER_H
#define RD_HELPER_H
#pragma once

// C STD includes
// C 3rd party includes
// C++ STD includes
#include <cstdint>
#include <cwchar>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <cmath>
#include <chrono>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <span>

// C++ 3rd party includes
#include <CAENComm.h>
#include <CAENDigitizer.h>

// my includes
#include "logger_helpers.hpp"

namespace RedDigitizer {

enum class CAENDigitizerFamilies {
#ifndef NDEBUG
    DEBUG = 0,
#endif
    x721, x724, x725,
    x731,
    x730, // Actually supported
    x740, // Actually supported
    x742, x743,
    x751,
    x761,
    x780, x781, x782,
    x790
};

// Translates the CAEN API error code to a string.
// Definition is found at the end of this file.
static std::string translate_caen_error_code(const CAEN_DGTZ_ErrorCode&);

// This list is incomplete
enum class CAENConnectionType {
    USB,
    A4818,
    OpticalLink,    // Not supported
    Ethernet_V4718, // Only available for V4718 (not supported)
    USB_V4718       // not supported
};

// This list is incomplete
enum class CAENDigitizerModel {
#ifndef NDEBUG
    DEBUG = 0,
#endif
    DT5730B,
    DT5740D,
    V1740D
};

// All the constants that never change for a given digitizer
struct CAENDigitizerModelConstants {
    // ADC resolution in bits
    uint32_t ADCResolution = 8;
    // In S/s
    double AcquisitionRate = 10e6;
    // In S/ch
    uint32_t MemoryPerChannel = 0;
    // Total number of channels
    uint8_t NumChannels = 1;
    // If 0, digitizer does not deal in groups
    uint8_t NumberOfGroups = 0;
    // Number of channels per group
    uint8_t NumChannelsPerGroup = 1;

    // Max number of internal buffers the digitizer can have.
    uint32_t MaxNumBuffers = 1024;

    // Constant to transform Nloc to Recordlength
    float NLOCToRecordLength = 1.0f;

    // Voltage ranges the digitizer has.
    std::vector<double> VoltageRanges = {};
};

// This is here so we can transform string to enums
// If the key does not exist, this will throw an error
const static inline std::unordered_map<std::string, CAENDigitizerModel>
    CAENDigitizerModelsMap {
#ifndef NDEBUG
        {"DEBUG", CAENDigitizerModel::DEBUG},
#endif
        {"DT5730B", CAENDigitizerModel::DT5730B},
        {"DT5740D", CAENDigitizerModel::DT5740D},
        {"V1740D", CAENDigitizerModel::V1740D}
};

// These links all the enums with their constants or properties that
// are fixed per digitizer
const static inline std::unordered_map<CAENDigitizerModel, CAENDigitizerModelConstants>
    CAENDigitizerModelsConstantsMap {
        #ifndef NDEBUG
        {CAENDigitizerModel::DEBUG, CAENDigitizerModelConstants {
            8,          // ADCResolution
            100e3,      //  AcquisitionRate
            1024ul,     // MemoryPerChannel
            1,          // NumChannels
            0,          // NumberOfGroups, 0 -> no groups
            1,          // NumChannelsPerGroup
            1024,       // MaxNumBuffers
            10.0f,      // NLOCToRecordLength
            {1.0}       // VoltageRanges
        }},
        #endif
        {CAENDigitizerModel::DT5730B, CAENDigitizerModelConstants {
            14,         // ADCResolution
            500e6,      //  AcquisitionRate
            static_cast<uint32_t>(5.12e6),  // MemoryPerChannel
            8,          // NumChannels
            0,          // NumberOfGroups, 0 -> no groups
            8,          // NumChannelsPerGroup
            1024,       // MaxNumBuffers
            10.0f,      // NLOCToRecordLength
            {0.5, 2.0}  // VoltageRanges
        }},
        {CAENDigitizerModel::DT5740D, CAENDigitizerModelConstants {
            12,         // ADCResolution
            62.5e6,     //  AcquisitionRate
            static_cast<uint32_t>(192e3),  // MemoryPerChannel
            32,         // NumChannels
            4,          // NumChannelsPerGroup
            8,          // NumberOfGroups
            1024,       // MaxNumBuffers
            1.5f,       // NLOCToRecordLength
            {2.0, 10.0} // VoltageRanges
        }},
        {CAENDigitizerModel::V1740D, CAENDigitizerModelConstants {
            12,         // ADCResolution
            62.5e6,     //  AcquisitionRate
            static_cast<uint32_t>(192e3),  // MemoryPerChannel
            64,         // NumChannels
            8,          // NumChannelsPerGroup
            8,          // NumberOfGroups
            1024,       // MaxNumBuffers
            1.5f,       // NLOCToRecordLength
            {2.0}       // VoltageRanges
        }}
    // This is a C++20 higher feature so lets keep everything 17 compliant
    // CAENDigitizerModelsConstants_map {
    //     {CAENDigitizerModel::DT5730B, CAENDigitizerModelConstants{
    //         .ADCResolution = 14,
    //         .AcquisitionRate = 500e6,
    //         .MemoryPerChannel = static_cast<uint32_t>(5.12e6),
    //         .NumChannels = 8,
    //         .NumberOfGroups = 0,
    //         .NumChannelsPerGroup = 8,
    //         .NLOCToRecordLength = 10,
    //         .VoltageRanges = {0.5, 2.0}
    //     }},
    //     {CAENDigitizerModel::DT5740D, CAENDigitizerModelConstants{
    //         .ADCResolution = 12,
    //         .AcquisitionRate = 62.5e6,
    //         .MemoryPerChannel = static_cast<uint32_t>(192e3),
    //         .NumChannels = 32,
    //         .NumberOfGroups = 4,
    //         .NumChannelsPerGroup = 8,
    //         .NLOCToRecordLength = 1.5,
    //         .VoltageRanges = {2.0, 10.0}
    //     }}
};

// A helper function to use the constants in python
std::unordered_map<CAENDigitizerModel, CAENDigitizerModelConstants> GetCAENDigitizerModelConstants() {
    return CAENDigitizerModelsConstantsMap;
}

struct CAENGlobalConfig {
    // X730 max buffers is 1024
    // X740 max buffers is 1024
    uint32_t MaxEventsPerRead = 512;

    // Record length in samples
    uint32_t RecordLength = 100;

    // In %
    uint32_t PostTriggerPorcentage = 50;

    // If enabled, the trigger acquisition only happens whenever
    // EXT or TRG-IN is high.
    bool EXTAsGate = false;

    // External Trigger Mode
    // Trigger Mode comes in four flavors:
    // CAEN_DGTZ_TRGMODE_DISABLED
    //    is disabled
    // CAEN_DGTZ_TRGMODE_EXTOUT_ONLY
    //    is used to generate the trigger output
    // CAEN_DGTZ_TRGMODE_ACQ_ONLY
    //    is used to generate the acquisition trigger
    // CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT
    //    is used to generate the acquisition trigger and trigger output
    CAEN_DGTZ_TriggerMode_t EXTTriggerMode
        = CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_ACQ_ONLY;

    // Software Trigger Mode
    // see External Trigger Mode for description
    CAEN_DGTZ_TriggerMode_t SWTriggerMode
        = CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_ACQ_ONLY;

    // Channel Trigger Mode
    // see External Trigger Mode for description
    CAEN_DGTZ_TriggerMode_t CHTriggerMode
        = CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_ACQ_ONLY;

    /* Acquisition mode comes in four flavors:
     * - CAEN_DGTZ_SW_CONTROLLED
     *  Start/stop of the run takes place on software
     *  command (by calling CAEN_DGTZ_SWStartAcquisition )
     * - CAEN_DGTZ_FIRST_TRG_CONTROLLED
     *  Run starts on the first trigger pulse (rising edge on
     *  TRG-IN) actual triggers start from the second pulse from there
     * - CAEN_DGTZ_S_IN_CONTROLLED
     *  S-IN/GPI controller (depends on the board). Acquisition
     *  starts on edge of the GPI/S-IN
     *  CAEN_DGTZ_LVDS_CONTROLLED
     VME ONLY, like S_IN_CONTROLLER but uses LVDS. */
    CAEN_DGTZ_AcqMode_t AcqMode
        = CAEN_DGTZ_AcqMode_t::CAEN_DGTZ_SW_CONTROLLED;

    // Voltage I/O level for the digital channels
    CAEN_DGTZ_IOLevel_t IOLevel
        = CAEN_DGTZ_IOLevel_t::CAEN_DGTZ_IOLevel_NIM;

    // This feature is for x730, x740
    // True = disabled, False = enabled
    bool TriggerOverlappingEn = false;

    // false -> normal, the board is full whenever all buffers are full.
    // true  -> always one buffer free. The board is full whenever Nb-1 buffers
    // are full, where Nb is the overall number of numbers of buffers allocated.
    bool MemoryFullModeSelection = true;

    // Triggering edge option
    // 0L/CAEN_DGTZ_TriggerOnRisingEdge -> Rising edge,
    // 1L/CAEN_DGTZ_TriggerOnFallingEdge -> Falling edge
    CAEN_DGTZ_TriggerPolarity_t TriggerPolarity =
        CAEN_DGTZ_TriggerPolarity_t::CAEN_DGTZ_TriggerOnRisingEdge;

    // Only available for x740 and x724 digitizer families.
    // Reduces the effective sampling frequency by
    //  {SAMPLE_RATE}/2^{DecimationFactor}
    // Using the average of the consecutive 2*DecimationFactor
    // samples for the output value. Trigger is processed after decimation
    uint16_t DecimationFactor = 0;

    // From CAEN API: For a MajorityLevel, m, the trigger fires when at
    // least m+1 of the enabled trigger requests are over-threshold inside
    // the majority coincidence window. Max is 7.
    uint32_t MajorityLevel = 0;
    // In units of trigger clock. Only 4 LSB bits are counted.
    uint32_t MajorityCoincidenceWindow = 0;

    // TODO(Any): there are also majority values for TRG-OUT
};

// Help structure to link an array of booleans to a single uint8_t
// without the use of other C++ features which makes it trickier to use
struct ChannelsMask {
    const static std::size_t kNumCHs = 8;
    std::array<bool, kNumCHs> CH
        = {false, false, false, false, false, false, false, false};

    uint8_t get() const noexcept {
        uint8_t out = 0u;
        for(std::size_t i = 0; i < CH.size(); i++) {
            out |= static_cast<uint8_t>(CH[i]) << i;
        }
        return out;
    }

    bool& operator[](const std::size_t& iter) noexcept {
        return CH[iter];
    }

    [[nodiscard]] const bool& at(const std::size_t& iter) const {
        if(iter > kNumCHs) {
            throw std::invalid_argument("iter is higher than 8.");
        }

        return CH[iter];
    }

    friend std::ostream& operator<<(std::ostream& os, const ChannelsMask& mask) {
        os << "[";
        for (std::size_t i = 0; i < kNumCHs; ++i) {
            os << (mask.CH[i] ? "1" : "0");
            if (i < kNumCHs - 1) {
                os << ", ";
            }
        }
        os << "] (0x" << std::hex << static_cast<int>(mask.get()) << ")";
        return os;
    }
};

// As a general case, this holds all the configuration values for a channel
// if a digitizer does not support groups, i.e x730, group = channel
struct CAENGroupConfig {
    // Is this group/channel enabled?
    bool Enabled = false;

    // Mask of channels within the group enabled to trigger
    // If Channel, if its != 0 then its enabled
    ChannelsMask TriggerMask;

    // Mask of enabled channels within the group
    // Ignored for single channels.
    ChannelsMask AcquisitionMask;

    // DC offsets of each channel in the group in ADC counts.
    // Usually the DC offset is a 16bit DAC. Check documentation.
    uint32_t DCOffset = 0x8000;
    std::array<uint8_t, 8> DCCorrections = {0, 0, 0, 0, 0, 0, 0, 0};

    // For digitizers that support several ranges.
    // 0 = 2Vpp, 1 = 0.5Vpp
    // DC range of the group or channel
    uint8_t DCRange = 0;

    // In ADC counts
    uint32_t TriggerThreshold = 0;
};

// Events structure: holds the raw data of the event, the info (timestamp),
// and the pointer to the point in the original buffer.
// This uses CAEN functions to allocate memory, so if handle does not
// have an associated digitizer to it, it will not work.
// Create events only after a setup(...) call has been made for best
// results.
class CAENEvent {
    int _handle = 0;
    // Possibly the most memory unsafe part of all CAEN software.
    // Remember kids, do not write functions that take void**
    // that modify their internal values

    // A pointer to the location in the CAENData (see below)
    // DOES NOT MANAGE DATA
    char* DataPtr = nullptr;
    // The reinterpreted data as uint16_ts with sizes.
    // Must be pointer because that is what the CAEN API expects.
    CAEN_DGTZ_UINT16_EVENT_t* Data = nullptr;
    // Contains the Info.
    CAEN_DGTZ_EventInfo_t Info = CAEN_DGTZ_EventInfo_t{};

    CAEN_DGTZ_ErrorCode _err_code = CAEN_DGTZ_ErrorCode::CAEN_DGTZ_Success;
 public:
    // Creating an Event using a handle allocates memory
    explicit CAENEvent(const int& handle) : _handle(handle) {
        // This will only allocate memory if handle does
        // has an associated digitizer to it.
        _err_code = CAEN_DGTZ_AllocateEvent(_handle,
                                            reinterpret_cast<void**>(&Data));
    }

    // If handle is released before this event is freed,
    // it will cause a memory leak, maybe?
    ~CAENEvent() {
        if (Data) {
            _err_code = CAEN_DGTZ_FreeEvent(_handle,
                                            reinterpret_cast<void**>(&Data));
            if(_err_code < 0) {
                std::cout << "Fatal error at ~CAENEvent()"
                        "Failed to free memory with error: " <<
                        translate_caen_error_code(_err_code) << "\n";
            }
        }
        Data = nullptr;
    }

    [[nodiscard]] const CAEN_DGTZ_ErrorCode& getError() const {
        return _err_code;
    }

    // A read-only access to the event data.
    [[nodiscard]] const CAEN_DGTZ_UINT16_EVENT_t* getData() const {
        return Data;
    }

    // A read-only access to the event info.
    [[nodiscard]] const CAEN_DGTZ_EventInfo_t& getInfo() const {
        return Info;
    }

    // This is a stupid function, char* is mutable so any interfacing
    // code will be unsafe by default. Blame CAEN, not me. Take max precautions.
    //
    // This grabs the data from data_ptr, CAEN magic happens and returns
    // the info of the event, and a pointer to its location in data_ptr
    CAEN_DGTZ_ErrorCode getEventInfo(char* data_ptr,
                                     const uint32_t& data_size,
                                     const int32_t& i) noexcept {
        _err_code = CAEN_DGTZ_GetEventInfo(_handle,
                                           data_ptr,
                                           data_size,
                                           i,
                                           &Info,
                                           &DataPtr);
        return _err_code;
    }

    // Decodes the information at the pointer found in getEventInfo
    //
    // Cannot decode without calling getEventInfo at least once. If
    // someone the data used in data_ptr is destroyed this will return an error
    CAEN_DGTZ_ErrorCode decodeEvent() {
        _err_code = CAEN_DGTZ_DecodeEvent(_handle,
                              DataPtr,
                              reinterpret_cast<void**>(&Data));
        return _err_code;
    }
};

// CAENWaveforms is the final stage of the digitizer data. It is optional
// and the benefit is that it provides a data format which lifetime is not
// dependent on CAEN API.
//
// Default DataType is uint16_t, the other option is uint8_t if memory is a
// priority in your implementation.
template <typename DataType = uint16_t>
//requires std::is_same_v<DataType, uint16_t> or std::is_same_v<DataType, uint8_t>
class CAENWaveforms {
    std::vector<std::size_t> _en_chs = {};
    std::size_t _num_en_chs = 0;
    uint32_t _record_length = 0;
    CAEN_DGTZ_EventInfo_t _info = CAEN_DGTZ_EventInfo_t{};
 public:
    CAENWaveforms() = default;
    CAENWaveforms(const CAENDigitizerModelConstants& model_constants,
                  const CAENGlobalConfig& gp_config,
                  const std::array<CAENGroupConfig, 8>& groups) :
            _en_chs{_get_en_chs(model_constants, groups)},
            _num_en_chs{_en_chs.size()},
            _record_length{gp_config.RecordLength},
            _data(_num_en_chs*_record_length)
    { }

    ~CAENWaveforms() = default;

    const uint32_t& getRecordLength() const {
        return _record_length;
    }
    const std::size_t getTotalSize() const {
        return _data.size();
    }
    const std::size_t& getNumEnabledChannels() const {
        return _num_en_chs;
    }
    const std::vector<std::size_t>& getEnabledChannels() const {
        return _en_chs;
    }
    const CAEN_DGTZ_EventInfo_t& getInfo() const {
        return _info;
    }

    // Copies values from event into the internal buffer
    // Does not copy if record length does not match the size
    void copy(const std::unique_ptr<CAENEvent>& event) {
        const CAEN_DGTZ_UINT16_EVENT_t* data = event->getData();

        if (data->ChSize[0] != _record_length) {
            return;
        }

        _info = event->getInfo();
        for (std::size_t ch_index = 0; ch_index < _en_chs.size(); ch_index++) {
            // We get the actual CAEN Channel number
            const auto& en_ch = _en_chs[ch_index];
            // Get the size and data from the CAEN data structure
            // The size must be the record length. There is one exception if
            // Overlapping waveforms is enabled. But that is a dangerous
            // configuration anyways.
            auto ch_size = data->ChSize[en_ch];
            auto ch_data = data->DataChannel[en_ch];

            // Now we copy to our own structure
            for(std::size_t i = 0; i < ch_size; i++) {
                _data[_record_length*ch_index + i] = ch_data[i];
            }
        }
    }

    // Does not copy if both waveforms do not match in enabled channels,
    // number of enabled channels or record length
    void copy(const CAENWaveforms<DataType>& other) {
        if (other.getNumEnabledChannels() != _en_chs) {
            return;
        }

        if (other.getEnabledChannels() != _num_en_chs) {
            return;
        }

        if (other.getRecordLength() != _record_length) {
            return;
        }

        auto other_data = other.getData();
        _info = other.getInfo();
        for(std::size_t i = 0; i < _data.size(); i++) {
            _data[i] = other_data[i];
        }
    }

    // Returns the data of channel (ch)
    std::vector<DataType> get(const std::size_t& ch) {
        return std::vector<DataType>(_data.cbegin() + _record_length*ch,
                                     _data.cbegin() + _record_length*(ch+1));
    }

    [[nodiscard]] std::span<DataType> getData() noexcept {
        return std::span(_data.data(), _data.size());
    }

 private:
    // Raw waveform data as one continuous 1-D array
    std::vector<DataType> _data;
    // Gets a vector with the numbers of the channels as per CAEN specification.
    // Takes into account if the digitizer has groups or not.
    std::vector<std::size_t> _get_en_chs(
            const CAENDigitizerModelConstants& model_constants,
            const std::array<CAENGroupConfig, 8>& groups) {
        std::vector<std::size_t> out;
        for(std::size_t group_num = 0; group_num < groups.size(); group_num++) {
            const auto& group = groups[group_num];
            if (not group.Enabled) {
                continue;
            }

            // If the digitizer does not support groups, group_num = ch
            if(model_constants.NumberOfGroups == 0) {
                out.push_back(group_num);
                continue;
            }

            // Othewise, calculate using the AcquisitionMask
            for(std::size_t ch = 0; ch < model_constants.NumChannelsPerGroup; ch++) {
                if(group.AcquisitionMask.at(ch)) {
                    out.push_back(ch + model_constants.NumChannelsPerGroup * group_num);
                }
            }
        }
        return out;
    }
};

template<typename Logger = iostream_wrapper,
         size_t EventBufferSize = 1024>
class CAEN {
    // Holds the CAEN raw data, the size of the buffer, the size
    // of the data and number of events
    // pointer Buffer is meant to be allocated using CAEN functions
    // after a setup(...) call.
    class CAENData {
        std::shared_ptr<Logger> _logger;
        CAEN_DGTZ_ErrorCode _err_code = CAEN_DGTZ_ErrorCode::CAEN_DGTZ_Success;

        char* _caen_malloc(const int& handle) noexcept {
            char* tmp_ptr = nullptr;
            _err_code = CAEN_DGTZ_MallocReadoutBuffer(handle, &tmp_ptr,
                                                      &TotalSizeBuffer);
            return tmp_ptr;
        }
     public:
        // Unsafe C buffer. Only this class has access to CAENData
        // Therefore all the damage is limited to us not the user.
        char* Buffer = nullptr;
        uint32_t TotalSizeBuffer = 0;
        uint32_t DataSize = 0;
        uint32_t NumEvents = 0;

        CAENData() = default;
        CAENData(std::shared_ptr<Logger> logger, const int& handle) :
            _logger{logger},
            Buffer{_caen_malloc(handle)} { }

        CAENData& operator=(CAENData&& other) noexcept {
            TotalSizeBuffer = other.TotalSizeBuffer;
            DataSize = other.DataSize;
            NumEvents = other.NumEvents;
            std::swap(Buffer, other.Buffer);
            return *this;
        }

        const CAEN_DGTZ_ErrorCode& getError() {
            return _err_code;
        }

        ~CAENData() {
            if(Buffer != nullptr) {
                _err_code = CAEN_DGTZ_FreeReadoutBuffer(&Buffer);

                if(_err_code < 0) {
                    _logger->error("Fatal error at CAENData -> _clear_data_mem."
                                   "Failed to free memory with error: {}",
                                   translate_caen_error_code(_err_code));
                }
            }
            Buffer = nullptr;
        }
    };

    //TODO(Any): change this to be a global variable probably inside a namespace
    static inline std::unordered_map<int, bool> _connection_info_map;

    // THis is a number that the CAEN API uses to manage its own resources.
    int _caen_api_handle = -1;
    bool _is_connected = false;
    bool _is_acquiring = false;

    // CAEN ENUMS. Holds the latest error thrown by any of the CAEN APIs funcs
    CAEN_DGTZ_ErrorCode _err_code = CAEN_DGTZ_ErrorCode::CAEN_DGTZ_Success;

    bool _has_error = false;
    bool _has_warning = false;

    // Communicated with the outside world: errors, warnings and debug msgs
    // Assumes it is a pointer of any form and this class does not manage
    // its deletion
    std::shared_ptr<Logger> _logger;

    using CAENData_ptr = std::unique_ptr<CAENData>;
    // This holds the latest raw digitizer data.
    // unique_ptr because only this class should manage this resource;
    // Its lifetime is the same as the acquisition is enabled.
    // Only 1 required at a time.
    // TODO(Any): for multiprocessing of data this is where I would start.
    //  - adding a circular buffer of CAEN data will allow to further
    //  - acquire more CAENData while the previous CAENData is being
    //  - processed
    CAENData_ptr _caen_raw_data;
    // Contains information about the configuration of the digitizer
    CAENGlobalConfig _global_config;
    // Contains information about all of the groups, see CAENGroupConfig struct
    // for more information. Currently, it is only possible to have up to
    // 8 groups from any digitizer.
    std::array<CAENGroupConfig, 8> _group_configs;

    // BoardInfo obtained during setup(...). It is a CAEN API struct.
    // Contains information about the digitizer such as firmware version,
    // family, and others.
    CAEN_DGTZ_BoardInfo_t _board_info = {};
    // Local variable that holds the current max buffers given the current
    // setup. Read directly from CAEN API.
    uint32_t _current_max_buffers = 0;

    // unique_ptr because only this class should manage this resource;
    // Its life-time is as long as acquisition is enabled.
    using CAENEvent_ptr = std::unique_ptr<CAENEvent>;
    // There can only be a max of 1024 of events at a time, but this
    // can be tweeked in teh future.
    std::array<CAENEvent_ptr, EventBufferSize> _events;
    // shared_ptr as anyone can manage this resource even if CAEN
    // is no longer in use. Its lifetime is independent of CAEN
    using CAENWaveforms_ptr = std::shared_ptr<CAENWaveforms<uint16_t>>;
    std::array<CAENWaveforms_ptr, EventBufferSize> _waveforms;

    // Translates the connection info data to a single number that should
    // be unique.
    constexpr uint64_t _hash_connection_info(const CAENConnectionType& ct,
        const int& ln, const int& cn, const uint32_t& addr) noexcept {
        uint64_t id = static_cast<uint64_t>(addr & 0x0000FFFF) << 32;
        id |= (cn & 0x000000FF) << 16;
        id |= (ln & 0x000000FF) << 8;

        // TODO(Any): missing enum cases
        switch (ct) {
        case CAENConnectionType::A4818:
            id |= 1;
        break;
        case CAENConnectionType::USB:
        default:
            id |= 0;
        }

        return id;
    }

    // Private helper function to wrap the logic behind checking for an error
    // and printing the error message
    void _print_if_err(std::string_view CAEN_func_name,
                       std::string_view location,
                       std::string_view extra_msg = "") noexcept {

        const std::string expression_str =
            "{} at {} in CAEN API function named {} with CAEN API message: {}. "
            "Additional message: {}";

        switch (_err_code) {
        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_Success:
            break;

        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_ChannelBusy:
        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_FunctionNotAllowed:
        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_Timeout:
        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_DigitizerAlreadyOpen:
        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_DPPFirmwareNotSupported:
        case CAEN_DGTZ_ErrorCode::CAEN_DGTZ_NotYetImplemented:
            _has_warning = true;
            _logger->warn(expression_str, "Warning", location,
                CAEN_func_name, translate_caen_error_code(_err_code),
                extra_msg);
            break;
        default:
            _has_error = true;
            _logger->error(expression_str, "Error", location,
                CAEN_func_name, translate_caen_error_code(_err_code),
                extra_msg);
        }
    }

    // Gets the family given a model.
    CAENDigitizerFamilies _get_family(const CAENDigitizerModel& model) {
        switch(model) {
        case CAENDigitizerModel::DT5740D:
        case CAENDigitizerModel::V1740D: 
            return CAENDigitizerFamilies::x740;
        default:
        case CAENDigitizerModel::DT5730B: 
            return CAENDigitizerFamilies::x730;
        }
    }

 public:
    // Family 
    const CAENDigitizerFamilies Family;
    // Model
    const CAENDigitizerModel Model;
    // Constants associated with the model.
    const CAENDigitizerModelConstants ModelConstants;
    // Type of connection (USB, VME...)
    const CAENConnectionType ConnectionType;
    //
    const int LinkNum;
    //
    const int ConetNode;
    //
    const uint32_t VMEBaseAddress;

    // Creation of this class attempts a connection to CAEN.
    // If succesful, it will take the resource and other attempts
    // will fail.
    // Use IsConnected() member function will indicate if its connected
    //
    // Description of parameters:
    // logger ->    logger used to print information (debug, warning, etc)
    //              defaults iostream_wrapper but can be user configured.
    // ct ->        Physical communication channel. CAEN_DGTZ_USB,
    //              CAEN_DGTZ_PCI_OpticalLink being the most expected options
    // ln -> Link number. In the case of USB is the link number assigned by the
    //  PC when you connect the cable to the device. 0 for the first device and
    //  so on. There is no fix correspondence between the USB port and the
    //  link number.
    //  For CONET, indicates which link of A2818 or A3818 is used.
    // cn -> Conet Node. Identifies which device in the daisy chain is being
    //  addressed.
    // addr -> VME Base address of the board. Only for VME models.
    //  0 in all other cases
    CAEN(std::shared_ptr<Logger> logger, const CAENDigitizerModel& model,
         const CAENConnectionType& ct, const int& ln, const int& cn,
         const uint32_t& addr) :
        _logger{logger},
        Family{_get_family(model)},
        Model{model},
        ModelConstants{CAENDigitizerModelsConstantsMap.at(model)},
        ConnectionType{ct},
        LinkNum{ln},
        ConetNode{cn},
        VMEBaseAddress{addr}
    {
        if (not logger) {
            throw std::invalid_argument("Logger is not an existing resource.");
        }

        // We turn Connection type, linknum, conectnode and VMEBaseAddress
        // into a single container so we know we are not repeating connections.
        auto id = _hash_connection_info(ct, ln, cn, addr);

        // C++17 way to check if the item exists
        // If we move to C++20, we could use _handle_map.contains(...)
        try {
            _connection_info_map.at(id);
            // Past this point it means the connection already exists
            _logger->warn("Resource is already on use. Calling this function "
                "will restart the resource.");
        } catch (...) {
            _connection_info_map[id] = true;
        }

        // If the item exists, pass on the creation
        // TODO(Any): it is missing the other types of connections
        switch (ct) {
        case CAENConnectionType::A4818:
            _err_code = CAEN_DGTZ_OpenDigitizer(
                CAEN_DGTZ_ConnectionType::CAEN_DGTZ_USB_A4818,
                ln, cn, addr, &_caen_api_handle);
        break;

        case CAENConnectionType::USB:
        default:
            _err_code = CAEN_DGTZ_OpenDigitizer(
                CAEN_DGTZ_ConnectionType::CAEN_DGTZ_USB,
                ln, 0, 0, &_caen_api_handle);
        break;
        }

        _is_connected = (_err_code == CAEN_DGTZ_ErrorCode::CAEN_DGTZ_Success)
                        and (_caen_api_handle >= 0);
        if (not _is_connected) {
            _caen_api_handle = -1;
            _has_error = true;
            _print_if_err("CAEN_DGTZ_OpenDigitizer", __FUNCTION__, "Failed "
                "to open the port to the digitizer.");
        } else {
            _logger->info("Connected resource with handle {0} with "
                "link number {1}, conet node {2} and VME address {3}.",
                _caen_api_handle, ln, cn, addr);
        }
    }

    ~CAEN() {
        auto id = _hash_connection_info(
            ConnectionType, LinkNum, ConetNode, VMEBaseAddress);
        _connection_info_map.erase(id);

        if (_is_connected) {
            _logger->info("Going to disconnect resource with handle {0} with "
            "link number {1}, conet node {2} and VME address {3}.",
            _caen_api_handle, LinkNum, ConetNode, VMEBaseAddress);

            _err_code = CAEN_DGTZ_SWStopAcquisition(_caen_api_handle);
            _print_if_err("CAEN_DGTZ_SWStopAcquisition", __FUNCTION__,
                "Failed to stop acquisition during cleaning of "
                "resources, what went wrong?");

            // Before closing, we clear all memory.
            for(auto& event : _events) {
                event.reset();
            }
            _caen_raw_data.reset();

            _err_code = CAEN_DGTZ_CloseDigitizer(_caen_api_handle);
            _print_if_err("CAEN_DGTZ_CloseDigitizer", __FUNCTION__,
                "Failed to close digitizer, whaaat?");
        }
        // Everything should be cleared by their own destructors!
    }

    // Check whenever the port is connected.
    bool IsConnected() noexcept { return _is_connected; }
    // Check if there is an error.
    bool HasError() noexcept { return _has_error; }
    // Check if there is a warning.
    bool HasWarning() noexcept { return _has_warning; }
    // Resets warning flag.
    void ResetWarning() noexcept { _has_warning = false; }

    const auto& GetBoardInfo() noexcept { return _board_info; }
    const auto& GetGlobalConfiguration() noexcept { return _global_config; }
    const auto& GetGroupConfigurations() noexcept { return _group_configs; }
    const auto& GetNumberOfEvents() noexcept {
        return _caen_raw_data->NumEvents;
    }
    const auto& GetCurrentPossibleMaxBuffer() noexcept {
        return _current_max_buffers;
    }

    // Using CAENGlobalConfig and the array of CAENGroupConfig
    // the digitizer is setup to specification.
    // No memory allocation is done during this step.
    void Setup(const CAENGlobalConfig&,
               const std::array<CAENGroupConfig, 8>&) noexcept;
    // Reset. Returns all internal registers to defaults. It also releases
    // any dynamic memory.
    void Reset() noexcept;
    // Enables the acquisition and allocates the memory for the data.
    // Does not enable acquisition if there are errors.
    void EnableAcquisition() noexcept;
    // Disables the acquisition.
    // Does not disables acquisition if there are errors.
    void DisableAcquisition() noexcept;
    // Writes to register ADDR with VALUE
    // Does write to register if there are errors.
    void WriteRegister(const uint32_t& addr, const uint32_t& value) noexcept;
    // Reads contents of register ADDR into value
    // Does not modify value if there are errors.
    void ReadRegister(const uint32_t& addr, uint32_t& value) noexcept;
    // Write arbitrary bits of any length at any position,
    // Keeps the other bits unchanged,
    // Following instructions at
    // https://stackoverflow.com/questions/11815894/how-to-read-write-arbitrary-bits-in-c-c
    void WriteBits(const uint32_t& addr,
        const uint32_t& value, uint8_t pos, uint8_t len = 1) noexcept;
    // Forces a software trigger in the digitizer.
    // Does not trigger if there are errors.
    void SoftwareTrigger() noexcept;
    // Asks CAEN how many events are in the buffer
    // Returns 0 if there are errors.
    uint32_t GetEventsInBuffer() noexcept;
    // Runs a bunch of commands to retrieve the buffer and process it
    // using CAEN functions.
    // Does not retrieve data if there are errors or is not acquiring.
    void RetrieveData() noexcept;
    // Returns true if data was read successfully
    // Does not retrieve data if there are errors, is not acquiring,
    // or events in buffer are less than n.
    // n cannot be bigger than the max number of buffers allowed
    bool RetrieveDataUntilNEvents(const uint32_t& n) noexcept;
    // Decodes event i from the data retrieved by any call from RetrieveData
    // If i is out of bounds, returns the event at the end of the buffer.
    // if there is an error during acquisition, this returns a nullptr;
    auto DecodeEvent(const uint32_t& i) noexcept;
    // Decodes the latest acquired events.
    // If there are errors it does nothing.
    void DecodeEvents() noexcept;
    // Clears the digitizer buffer. It stops the acquisition and resumes it
    // after clearing the data without doing any reallocation of memory.
    void ClearData() noexcept;
    // Returns a const pointer to the event held @ index i.
    // If i value is higher the latest acquired number of events, it returns
    // the last event. Use GetNumberOfevents() to check for the number
    // of events in memory
    auto GetWaveform(const std::size_t& i) noexcept {
        if (i >= _current_max_buffers) {
            return _waveforms[_current_max_buffers - 1];
        }

        return _waveforms[i];
    }

    auto GetWaveforms() noexcept {
        return _waveforms;
    }

    // Returns a const pointer to CAENEvent. Its lifespans its
    // managed by CAEN
    const CAENEvent* GetEvent(const std::size_t& i) noexcept {
        if (i >= _current_max_buffers) {
            return _events[_current_max_buffers - 1].get();
        }

        return _events[i].get();
    }

    uint32_t GetCommTransferRate() noexcept {
        if(ConnectionType == CAENConnectionType::USB){
            return 15000000u;  // S/s
        } else if (ConnectionType == CAENConnectionType::A4818) {
            return 40000000u;
        }

        return 0u;
    }

    // Returns the channel voltage range. If channel does not exist
    // returns 0
    double GetVoltageRange(const uint8_t& gr_n) noexcept {
        try {
            auto config = _group_configs[gr_n];
            return ModelConstants.VoltageRanges[config.DCRange];
        } catch(...) {
            return 0.0;
        }
        return 0.0;
    }

    // Turns a time (in nanoseconds) into counts
    // Reminder that some digitizers take data in chunks or only allow
    // some record lengths, so some record lengths are not possible.
    // Ex: x5730 record length can only be multiples of 10
    uint32_t TimeToRecordLength(const double& nsTime) noexcept {
        return static_cast<uint32_t>(nsTime*1e-9*GetCommTransferRate());
    }

    // Turns a voltage threshold counts to ADC cts
    uint32_t VoltThresholdCtsToADCCts(const uint32_t& cts) noexcept {
        const auto kVthresholdRangeCts = static_cast<uint32_t>(std::exp2(16));
        const uint32_t ADCRange = std::exp2(ModelConstants.ADCResolution);
        return ADCRange*cts / kVthresholdRangeCts;
    }

    // Turns a voltage (V) into trigger counts
    uint32_t VoltToThresholdCounts(const double& volt) noexcept {
        const auto kVthresholdRangeCts = static_cast<uint32_t>(std::exp2(16));
        return static_cast<uint32_t>(volt / kVthresholdRangeCts);
    }

    // Turns a voltage (V) into count offset
    uint32_t VoltOffsetToCountOffset(const double& volt) noexcept {
        uint32_t ADCRange = std::exp2(ModelConstants.ADCResolution);
        return static_cast<uint32_t>(volt / ADCRange);
    }
};

/// General CAEN functions
template<typename T, size_t N>
void CAEN<T, N>::Reset() noexcept {
    if (_has_error) {
        return;
    }

    _err_code = CAEN_DGTZ_Reset(_caen_api_handle);
    _print_if_err("CAEN_DGTZ_Reset", __FUNCTION__);

    _caen_raw_data.reset();
    for(auto& event : _events){
        event.reset();
    }
}

template<typename T, size_t N>
void CAEN<T, N>::Setup(const CAENGlobalConfig& global_config,
    const std::array<CAENGroupConfig, 8>& gr_configs) noexcept {
    if (_has_error or not _is_connected) {
        return;
    }
    // First, we disable acquisition just to make sure.
    // as some parameters can only be changed while the acquisition
    // is disabled.
    DisableAcquisition();
    // Second, we put the digitizer in a known state.
    Reset();

    int& handle = _caen_api_handle;

    // Now, global configuration
    // Global config
    _global_config = global_config;

    _err_code = CAEN_DGTZ_GetInfo(handle, &_board_info);
    _print_if_err("CAEN_DGTZ_GetInfo", __FUNCTION__);

    _err_code = CAEN_DGTZ_SetMaxNumEventsBLT(handle, _global_config.MaxEventsPerRead);
    _print_if_err("CAEN_DGTZ_SetMaxNumEventsBLT", __FUNCTION__);

    _err_code = CAEN_DGTZ_SetRecordLength(handle, _global_config.RecordLength);
    _print_if_err("CAEN_DGTZ_SetRecordLength", __FUNCTION__);

    // We need to ask the digitizer what is the actual record length is using
    // to keep an accurate account of it.
    _err_code = CAEN_DGTZ_GetRecordLength(handle, &_global_config.RecordLength);
    _print_if_err("CAEN_DGTZ_SetRecordLength", __FUNCTION__,
                  "reverting back to the provided record length");
    // if it fails, we write back the provided record length
    if (_err_code < 0) {
        _global_config.RecordLength = global_config.RecordLength;
    }

    // So far, all digitizer have the register 0x800C point to the
    // exponent of the number of buffers currently used.
    // TODO(Any): check if 0x800C is the register for all families
    ReadRegister(0x800C, _current_max_buffers);
    _current_max_buffers = std::exp2(_current_max_buffers);

    if (Family == CAENDigitizerFamilies::x740 or Family == CAENDigitizerFamilies::x724) {
        if(_global_config.DecimationFactor < 1) {
            _global_config.DecimationFactor = 1;
        } else if (_global_config.DecimationFactor > 128) {
            _global_config.DecimationFactor = 128;
        }

        // Next lines is to round to the next power of 2
        // as CAEN_DGTZ_SetDecimationFactor only allows
        // powers of 2 up to 128
        uint16_t out = std::log2(_global_config.DecimationFactor);
        _global_config.DecimationFactor = 1 << out;

        _err_code = CAEN_DGTZ_SetDecimationFactor(handle, _global_config.DecimationFactor);
    }

    if (Model == CAENDigitizerModel::V1740D) {
        _print_if_err("CAEN_DGTZ_SetPostTriggerSize", __FUNCTION__);
        uint32_t posttrigval = 0.01*_global_config.PostTriggerPorcentage*_global_config.RecordLength*_global_config.DecimationFactor;
        WriteRegister(0x8114, posttrigval);
    } else {
        _err_code = CAEN_DGTZ_SetPostTriggerSize(handle, _global_config.PostTriggerPorcentage);
        _print_if_err("CAEN_DGTZ_SetPostTriggerSize", __FUNCTION__);
    }

    _err_code = CAEN_DGTZ_SetSWTriggerMode(handle, _global_config.SWTriggerMode);
    _print_if_err("CAEN_DGTZ_SetSWTriggerMode", __FUNCTION__);

    _err_code = CAEN_DGTZ_SetExtTriggerInputMode(handle, _global_config.EXTTriggerMode);
    _print_if_err("CAEN_DGTZ_SetExtTriggerInputMode", __FUNCTION__);

    _err_code = CAEN_DGTZ_SetAcquisitionMode(handle, _global_config.AcqMode);
    _print_if_err("CAEN_DGTZ_SetAcquisitionMode", __FUNCTION__);

    // Trigger polarity
    // These digitizers do not support channel-by-channel trigger pol
    // so we treat it like a global config, and use 0 as a placeholder.
    _err_code = CAEN_DGTZ_SetTriggerPolarity(handle, 0, _global_config.TriggerPolarity);
    _print_if_err("CAEN_DGTZ_SetTriggerPolarity", __FUNCTION__);
    WriteBits(0x8000, _global_config.TriggerPolarity, 6);

    _err_code = CAEN_DGTZ_SetIOLevel(handle, _global_config.IOLevel);
    _print_if_err("CAEN_DGTZ_SetIOLevel", __FUNCTION__);


    // Board config register
    // 0 = Trigger overlapping not allowed
    // 1 = trigger overlapping allowed
    // WriteBits(0x8000, _global_config.TriggerOverlappingEn, 1);

    WriteBits(0x8100, _global_config.MemoryFullModeSelection, 5);

    // Global Trigger mask. So far seems to be applicable for digitizers
    // with and without groups, huh!
    constexpr uint32_t kGlobalTriggerMaskAddr = 0x810C;
    WriteBits(kGlobalTriggerMaskAddr, _global_config.MajorityCoincidenceWindow, 20, 4);
    WriteBits(kGlobalTriggerMaskAddr, _global_config.MajorityLevel, 24, 3);

    // Channel stuff
    _group_configs = gr_configs;
    if (Family == CAENDigitizerFamilies::x730) {
        // For DT5730B, there are no groups only channels so we take
        // each configuration as a channel
        // First, we make the channel mask
        uint32_t channel_mask = 0;
        for (std::size_t ch = 0; ch < gr_configs.size(); ch++) {
            channel_mask |= _group_configs[ch].Enabled << ch;
        }

        uint32_t trg_mask = 0;
        for (std::size_t ch = 0; ch < gr_configs.size(); ch++) {
            trg_mask = _group_configs[ch].TriggerMask.get();
            bool has_trig_mask = trg_mask > 0;
            trg_mask |=  has_trig_mask << ch;
        }

        // Then enable those channels
        _err_code = CAEN_DGTZ_SetChannelEnableMask(handle, channel_mask);
        _print_if_err("CAEN_DGTZ_SetChannelEnableMask", __FUNCTION__);

        // Then enable if they are part of the trigger
        _err_code = CAEN_DGTZ_SetChannelSelfTrigger(handle,
                                                    _global_config.CHTriggerMode,
                                                    trg_mask);
        _print_if_err("CAEN_DGTZ_SetChannelSelfTrigger", __FUNCTION__);

        for (std::size_t ch = 0; ch < gr_configs.size(); ch++) {
            auto ch_config = gr_configs[ch];

            // Trigger stuff
            // Self Channel trigger
            _err_code = CAEN_DGTZ_SetChannelTriggerThreshold(handle,
                                                             ch,
                                                             ch_config.TriggerThreshold);
            _print_if_err("CAEN_DGTZ_SetChannelTriggerThreshold", __FUNCTION__);

            _err_code = CAEN_DGTZ_SetChannelDCOffset(handle, ch, ch_config.DCOffset);
            _print_if_err("CAEN_DGTZ_SetChannelDCOffset", __FUNCTION__);

            // Writes to the registers that holds the DC range
            // For 5730 it is the register 0x1n28
            WriteRegister(0x1028 | (ch & 0x0F) << 8, ch_config.DCRange & 0x0001);
        }

    } else if (Family == CAENDigitizerFamilies::x740) {
        uint32_t group_mask = 0;
        uint16_t num_grps = 4;
        for (std::size_t grp_n = 0; grp_n < num_grps; grp_n++) {
            group_mask |= gr_configs[grp_n].Enabled << grp_n;
        }
        for (std::size_t grp_n = num_grps; grp_n < 8; grp_n++) {
            _group_configs[grp_n].Enabled = false;
        }

        _err_code = CAEN_DGTZ_SetGroupEnableMask(handle, group_mask);
        _print_if_err("CAEN_DGTZ_SetGroupEnableMask", __FUNCTION__);

        _err_code = CAEN_DGTZ_SetGroupSelfTrigger(handle,
                                                  _global_config.CHTriggerMode,
                                                  group_mask);
        _print_if_err("CAEN_DGTZ_SetGroupSelfTrigger", __FUNCTION__);

        for (std::size_t grp_n = 0; grp_n < num_grps; grp_n++) {
            auto gr_config = gr_configs[grp_n];
            // Trigger stuff

            // This guy is does not work under V1740D unless in firmware
            // version 4.17
            _err_code = CAEN_DGTZ_SetGroupTriggerThreshold(handle,
                                                           grp_n,
                                                           gr_config.TriggerThreshold);
            _print_if_err("CAEN_DGTZ_SetGroupTriggerThreshold", __FUNCTION__);

            _err_code = CAEN_DGTZ_SetGroupDCOffset(handle,
                                                   grp_n,
                                                   gr_config.DCOffset);
            _print_if_err("CAEN_DGTZ_SetGroupDCOffset", __FUNCTION__);

            // Set the mask for channels enabled for self-triggering
            auto trig_mask = gr_config.TriggerMask.get();
            _err_code = CAEN_DGTZ_SetChannelGroupMask(handle,
                                                      grp_n,
                                                      trig_mask);
            _print_if_err("CAEN_DGTZ_SetChannelGroupMask", __FUNCTION__);

           // Set acquisition mask
           auto acq_mask = gr_config.AcquisitionMask.get();
           WriteBits(0x10A8 | (grp_n << 8), acq_mask, 0, 8);

            // DCCorrections should be of length
            // NumberofChannels / NumberofGroups.
            // set individual channel 8-bitDC offset
            // on 12 bit LSB scale, same as threshold
            uint32_t word = 0;
            for (int ch = 0; ch < 4; ch++) {
                word += gr_config.DCCorrections[ch] << (ch * 8);
            }

            WriteRegister(0x10C0 | (grp_n << 8), word);
            word = 0;
            for (int ch = 4; ch < 8; ch++) {
                word += gr_config.DCCorrections[ch] << ((ch - 4) * 8);
            }
            WriteRegister(0x10C4 | (grp_n << 8), word);
        }

        bool trg_out = false;
        // TODO(Any): these configuration bits look like more complex than they
        //  - are, maybe they should be expanded to be its own struct?
        // For 740, to use TRG-IN as Gate / anti-veto
        if (_global_config.EXTAsGate) {
            WriteBits(kGlobalTriggerMaskAddr, 1, 27);  // TRG-IN AND internal trigger,
            // and to serve as gate
            WriteBits(0x811C, 1, 10);  // TRG-IN as gate
        } else {
            WriteBits(kGlobalTriggerMaskAddr, 0, 27);
            WriteBits(0x811C, 0, 10); 
        }

        if (trg_out) {
            WriteBits(0x811C, 0, 15);  // TRG-OUT based on internal signal
            WriteBits(0x811C, 0b00, 16, 2);  // TRG-OUT based
            // on internal signal
        }
        WriteBits(0x811C, 0b01, 21, 2);

        // read_register(res, 0x8110, word);
        // word |= 1; // enable group 0 to participate in GPO
        // write_register(res, 0x8110, word);

    } else {
        // custom error message if not above models
        _err_code = CAEN_DGTZ_ErrorCode::CAEN_DGTZ_BadBoardType;
        _print_if_err("setup", __FUNCTION__,
                      "This API does not support your model/family."
                      " Maybe help writing the support code? :)");

    }
}

template<typename T, size_t N>
void CAEN<T, N>::EnableAcquisition() noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    int& handle = _caen_api_handle;

    // By using reset() for CAENData and CAENEvent we release memory, too.
    // so by calling enable acquisition we also free memory and re-allocate.

    // We need a single data buffer to hold the incoming Data
    _caen_raw_data.reset(new CAENData{_logger, handle});
    _err_code = _caen_raw_data->getError();
    _print_if_err("CAENData", __FUNCTION__);

    // Allocates all the memory for the internal events buffer
    std::generate(_events.begin(), _events.end(), [h = handle](){
        return std::make_unique<CAENEvent>(h);
    });

    std::generate(_waveforms.begin(), _waveforms.end(),
                  [constants = ModelConstants,
                   global = _global_config,
                   groups = _group_configs]() {
                    return std::make_shared<CAENWaveforms<uint16_t>>(constants,
                                                   global,
                                                   groups);
    });

    _err_code = CAEN_DGTZ_ClearData(handle);
    _print_if_err("CAEN_DGTZ_ClearData", __FUNCTION__);

    _err_code = CAEN_DGTZ_SWStartAcquisition(handle);
    _print_if_err("CAEN_DGTZ_SWStartAcquisition", __FUNCTION__);

    if (not _has_error) {
        _is_acquiring = true;
    } else {
        // Try to disable the acquisition just in case
        _err_code = CAEN_DGTZ_SWStopAcquisition(_caen_api_handle);
        _print_if_err("CAEN_DGTZ_SWStopAcquisition",
                      __FUNCTION__);
        _is_acquiring = false;
    }
}

template<typename T, size_t N>
void CAEN<T, N>::DisableAcquisition() noexcept {
    if (_has_error or not _is_connected or not _is_acquiring) {
        return;
    }

    _err_code = CAEN_DGTZ_SWStopAcquisition(_caen_api_handle);
    _print_if_err("CAEN_DGTZ_SWStopAcquisition", __FUNCTION__);
    // To avoid false positives
    if (not _has_error) {
        _is_acquiring = false;
    }
}

template<typename T, size_t N>
void CAEN<T, N>::WriteRegister(const uint32_t& addr, const uint32_t& value) noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    _err_code = CAEN_DGTZ_WriteRegister(_caen_api_handle, addr, value);
    _print_if_err("CAEN_DGTZ_WriteRegister", __FUNCTION__, "Failed to write "
                                                           "register " +
                                                           std::to_string(addr));
}

template<typename T, size_t N>
void CAEN<T, N>::ReadRegister(const uint32_t& addr, uint32_t& value) noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    _err_code = CAEN_DGTZ_ReadRegister(_caen_api_handle, addr, &value);
    _print_if_err("CAEN_DGTZ_ReadRegister", __FUNCTION__, "Failed to read "
                                                          "register " +
                                                          std::to_string(addr));
}

template<typename T, size_t N>
void CAEN<T, N>::WriteBits(const uint32_t& addr,
    const uint32_t& value, uint8_t pos, uint8_t len) noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    // First read the register
    uint32_t read_word = 0;
    _err_code = CAEN_DGTZ_ReadRegister(_caen_api_handle, addr, &read_word);
    _print_if_err("CAEN_DGTZ_ReadRegister", __FUNCTION__, "Failed to read "
                                                          "register " +
                                                          std::to_string(addr));

    uint32_t bit_mask = ~(((1 << len) - 1) << pos);
    read_word = read_word & bit_mask; //mask the register value

    // Get the lowest bits of value and shifted to the correct position
    uint32_t value_bits = (value & ((1 << len) - 1)) << pos;
    // Combine masked value read from register with new bits
    _err_code = CAEN_DGTZ_WriteRegister(_caen_api_handle, addr, read_word | value_bits);
    _print_if_err("CAEN_DGTZ_WriteRegister", __FUNCTION__, "Failed to write "
                                                           "register " +
                                                           std::to_string(addr));
}

template<typename T, size_t N>
void CAEN<T, N>::SoftwareTrigger() noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    _err_code = CAEN_DGTZ_SendSWtrigger(_caen_api_handle);
    _print_if_err("CAEN_DGTZ_SendSWtrigger", __FUNCTION__);
}

template<typename T, size_t N>
uint32_t CAEN<T, N>::GetEventsInBuffer() noexcept {
    if (_has_error or not _is_connected) {
        return 0;
    }

    uint32_t events = 0;
    // For 5730 it is the register 0x812C
    // TODO(Any): expand to include any registers depending on the model
    // or family.
    ReadRegister(0x812C, events);

    return events;
}

template<typename T, size_t N>
void CAEN<T, N>::RetrieveData() noexcept {
    int& handle = _caen_api_handle;

    if (_has_error or not _is_connected or not _is_acquiring) {
        return;
    }

    // UNSAFE CODE AHEAD
    _err_code = CAEN_DGTZ_ReadData(handle,
        CAEN_DGTZ_ReadMode_t::CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
        _caen_raw_data->Buffer,
        &_caen_raw_data->DataSize);
    _print_if_err("CAEN_DGTZ_ReadData", __FUNCTION__);

    _err_code = CAEN_DGTZ_GetNumEvents(handle,
                                       _caen_raw_data->Buffer,
                                       _caen_raw_data->DataSize,
                                       &_caen_raw_data->NumEvents);
    // END OF UNSAFE CODE
    _print_if_err("CAEN_DGTZ_GetNumEvents", __FUNCTION__);
}

template<typename T, size_t N>
bool CAEN<T, N>::RetrieveDataUntilNEvents(const uint32_t& n) noexcept {
    if (_has_error or not _is_connected or not _is_acquiring) {
        return false;
    }

    if (n >= _current_max_buffers) {
        if (GetEventsInBuffer() < _current_max_buffers) {
            return false;
        }
    } else if (GetEventsInBuffer() < n) {
        return false;
    }

    // There is a weird quirk related to this function that BLOCKS
    // the software essentially killing it.
    // If TriggerOverlapping is allowed sometimes this function
    // returns more data than Buffer it can hold, why? Idk
    // but so far with the software as is, it won't work with that
    // so dont do it!

    RetrieveData();

    return true;
}

template<typename T, size_t N>
auto CAEN<T, N>::DecodeEvent(const uint32_t& i) noexcept {
    if (i > _caen_raw_data->NumEvents) {
        return _waveforms[_caen_raw_data->NumEvents - 1];
    }

    if (_has_error or not _is_connected) {
        return _waveforms[_caen_raw_data->NumEvents - 1];
    }

    _err_code = _events[i]->getEventInfo(_caen_raw_data->Buffer,
                                        _caen_raw_data->DataSize,
                                        i);
    _print_if_err("CAEN_DGTZ_GetEventInfo",
                  __FUNCTION__,
                  "at event " + std::to_string(i));
    // Cannot decode without getting event info
    _err_code = _events[i]->decodeEvent();
    _print_if_err("CAEN_DGTZ_DecodeEvent",
                  __FUNCTION__,
                  "at event " + std::to_string(i));

    _waveforms[i]->copy(_events[i]);

    return _waveforms[i];
}

template<typename T, size_t N>
void CAEN<T, N>::DecodeEvents() noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    for (uint32_t i = 0; i < _caen_raw_data->NumEvents; i++) {
        _err_code = _events[i]->getEventInfo(_caen_raw_data->Buffer,
                                            _caen_raw_data->DataSize,
                                            i);
        _print_if_err("CAEN_DGTZ_GetEventInfo",
                      __FUNCTION__,
                      "at event " + std::to_string(i));
        // Cannot decode without getting event info
        _err_code = _events[i]->decodeEvent();
        _print_if_err("CAEN_DGTZ_DecodeEvent",
                      __FUNCTION__,
                      "at event " + std::to_string(i));

        _waveforms[i]->copy(_events[i]);
    }
}

template<typename T, size_t N>
void CAEN<T, N>::ClearData() noexcept {
    if (_has_error or not _is_connected) {
        return;
    }

    int& handle = _caen_api_handle;
    _err_code = CAEN_DGTZ_SWStopAcquisition(handle);
    _print_if_err("CAEN_DGTZ_SWStopAcquisition", __FUNCTION__);
    _err_code = CAEN_DGTZ_ClearData(handle);
    _print_if_err("CAEN_DGTZ_ClearData", __FUNCTION__);
    _err_code = CAEN_DGTZ_SWStartAcquisition(handle);
    _print_if_err("CAEN_DGTZ_SWStartAcquisition", __FUNCTION__);
}

std::string translate_caen_error_code(const CAEN_DGTZ_ErrorCode& err) {
    switch(err) {
        case CAEN_DGTZ_Success:
            return "Success";
        case CAEN_DGTZ_CommError:
            return "Communication error";
        default:
        case CAEN_DGTZ_GenericError:
            return "Unspecified error";
        case CAEN_DGTZ_InvalidParam:
            return "Invalid parameter";
        case CAEN_DGTZ_InvalidLinkType:
            return "Invalid link type";
        case CAEN_DGTZ_InvalidHandle:
            return "Invalid device handle";
        case CAEN_DGTZ_MaxDevicesError:
            return "Maximum number of devices exceeded";
        case CAEN_DGTZ_BadBoardType:
            return "The operation is not allowed on this type of board";
        case CAEN_DGTZ_BadInterruptLev:
            return "The interrupt level is not allowed";
        case CAEN_DGTZ_BadEventNumber:
            return "The event number is bad ";
        case CAEN_DGTZ_ReadDeviceRegisterFail:
            return "Unable to read the registry";
        case CAEN_DGTZ_WriteDeviceRegisterFail:
            return "Unable to write into the registry";
        case CAEN_DGTZ_InvalidChannelNumber:
            return "The channel number is invalid";
        case CAEN_DGTZ_ChannelBusy:
            return "The Channel is busy";
        case CAEN_DGTZ_FPIOModeInvalid:
            return "Invalid FPIO Mode";
        case CAEN_DGTZ_WrongAcqMode:
            return "Wrong acquisition mode";
        case CAEN_DGTZ_FunctionNotAllowed:
            return "This function is not allowed for this module";
        case CAEN_DGTZ_Timeout:
            return "Communication Timeout";
        case CAEN_DGTZ_InvalidBuffer:
            return "The buffer is invalid";
        case CAEN_DGTZ_EventNotFound:
            return "The event is not found";
        case CAEN_DGTZ_InvalidEvent:
            return "The event is invalid";
        case CAEN_DGTZ_OutOfMemory:
            return "Out of memory";
        case CAEN_DGTZ_CalibrationError:
            return "Unable to calibrate the board";
        case CAEN_DGTZ_DigitizerNotFound:
            return "Unable to open the digitizer";
        case CAEN_DGTZ_DigitizerAlreadyOpen:
            return "The Digitizer is already open";
        case CAEN_DGTZ_DigitizerNotReady:
            return "The Digitizer is not ready to operate";
        case CAEN_DGTZ_InterruptNotConfigured:
            return "The Digitizer has not the IRQ configured";
        case CAEN_DGTZ_DigitizerMemoryCorrupted:
            return "The digitizer flash memory is corrupted";
        case CAEN_DGTZ_DPPFirmwareNotSupported:
            return "The digitizer dpp firmware is not supported in this lib version";
        case CAEN_DGTZ_InvalidLicense:
            return "Invalid Firmware License";
        case CAEN_DGTZ_InvalidDigitizerStatus:
            return "The digitizer is found in a corrupted status";
        case CAEN_DGTZ_UnsupportedTrace:
            return "The given trace is not supported by the digitizer";
        case CAEN_DGTZ_InvalidProbe:
            return "The given probe is not supported for the given digitizer's trace";
        case CAEN_DGTZ_UnsupportedBaseAddress:
            return "The Base Address is not supported, it's a Desktop device?";
        case CAEN_DGTZ_NotYetImplemented:
            return "The function is not yet implemented";
    }
}
/// End Data Acquisition functions
//
/// Mathematical functions



/// End Mathematical functions
//
/// File functions

// Saves the digitizer data in the Binary format SBC collboration is using
// This only writes the header at the beginning of the file.
// Meant to be written once.
//std::string sbc_init_file(CAEN&) noexcept;
//
//// Saves the digitizer data in the Binary format SBC collboration is using
//std::string sbc_save_func(CAENEvent&, CAEN&) noexcept;

/// End File functions

}   // namespace RedDigitizer
#endif // RD_HELPER_H
