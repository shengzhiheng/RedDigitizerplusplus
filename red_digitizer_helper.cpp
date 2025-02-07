#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <span>
#include "include/RedDigitizer++/red_digitizer_helper.hpp"

namespace py = pybind11;

struct CAEN_DGTZ_UINT16_EVENT_Python {
    std::vector<uint32_t> ch_size;
    std::vector<std::vector<uint16_t>> data_channel;
};

CAEN_DGTZ_UINT16_EVENT_Python convertToPythonData(const CAEN_DGTZ_UINT16_EVENT_t* data) {
    CAEN_DGTZ_UINT16_EVENT_Python pyData;
    pyData.ch_size = std::vector<uint32_t>(data->ChSize, data->ChSize + 32);

    for (size_t i = 0; i < 32; ++i) {
        std::vector<uint16_t> channelData(data->DataChannel[i], 
                                          data->DataChannel[i] + data->ChSize[i]);
        pyData.data_channel.push_back(channelData);
    }
    return pyData;
}

PYBIND11_MODULE(red_caen, m) {
    m.doc() = "Python bindings for RedDigitizer++";

    py::class_<RedDigitizer::CAENDigitizerModelConstants>(m, "CAENDigitizerModelConstants")
        .def_readonly("ADCResolution", &RedDigitizer::CAENDigitizerModelConstants::ADCResolution)
        .def_readonly("AcquisitionRate", &RedDigitizer::CAENDigitizerModelConstants::AcquisitionRate)
        .def_readonly("MemoryPerChannel", &RedDigitizer::CAENDigitizerModelConstants::MemoryPerChannel)
        .def_readonly("NumChannels", &RedDigitizer::CAENDigitizerModelConstants::NumChannels)
        .def_readonly("NumberOfGroups", &RedDigitizer::CAENDigitizerModelConstants::NumberOfGroups)
        .def_readonly("NumChannelsPerGroup", &RedDigitizer::CAENDigitizerModelConstants::NumChannelsPerGroup)
        .def_readonly("MaxNumBuffers", &RedDigitizer::CAENDigitizerModelConstants::MaxNumBuffers)
        .def_readonly("NLOCToRecordLength", &RedDigitizer::CAENDigitizerModelConstants::NLOCToRecordLength)
        .def_readonly("VoltageRanges", &RedDigitizer::CAENDigitizerModelConstants::VoltageRanges);

    // Bind the getter function to access the map as a dictionary
    m.def("GetCAENDigitizerModelConstants", &RedDigitizer::GetCAENDigitizerModelConstants);

    py::enum_<RedDigitizer::CAENDigitizerFamilies>(m, "CAENDigitizerFamilies")
#ifndef NDEBUG
        .value("DEBUG", RedDigitizer::CAENDigitizerFamilies::DEBUG)
#endif
        .value("x721", RedDigitizer::CAENDigitizerFamilies::x721)
        .value("x724", RedDigitizer::CAENDigitizerFamilies::x724)
        .value("x725", RedDigitizer::CAENDigitizerFamilies::x725)
        .value("x731", RedDigitizer::CAENDigitizerFamilies::x731)
        .value("x730", RedDigitizer::CAENDigitizerFamilies::x730)  // Actually supported
        .value("x740", RedDigitizer::CAENDigitizerFamilies::x740)  // Actually supported
        .value("x742", RedDigitizer::CAENDigitizerFamilies::x742)
        .value("x743", RedDigitizer::CAENDigitizerFamilies::x743)
        .value("x751", RedDigitizer::CAENDigitizerFamilies::x751)
        .value("x761", RedDigitizer::CAENDigitizerFamilies::x761)
        .value("x780", RedDigitizer::CAENDigitizerFamilies::x780)
        .value("x781", RedDigitizer::CAENDigitizerFamilies::x781)
        .value("x782", RedDigitizer::CAENDigitizerFamilies::x782)
        .value("x790", RedDigitizer::CAENDigitizerFamilies::x790)
        ;

    py::enum_<RedDigitizer::CAENConnectionType>(m, "CAENConnectionType")
        .value("USB", RedDigitizer::CAENConnectionType::USB)
        .value("A4818", RedDigitizer::CAENConnectionType::A4818)
        ;

    py::enum_<RedDigitizer::CAENDigitizerModel>(m, "CAENDigitizerModel")
        .value("DT5730B", RedDigitizer::CAENDigitizerModel::DT5730B)
        .value("DT5740D", RedDigitizer::CAENDigitizerModel::DT5740D)
        .value("V1740D", RedDigitizer::CAENDigitizerModel::V1740D)
        ;
    
    py::enum_<CAEN_DGTZ_TriggerMode_t>(m, "CAEN_DGTZ_TriggerMode")
        .value("DISABLED", CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_DISABLED)
        .value("EXTOUT_ONLY", CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_EXTOUT_ONLY)
        .value("ACQ_ONLY", CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_ACQ_ONLY)
        .value("ACQ_AND_EXTOUT", CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT)
        ;
    
    py::class_<RedDigitizer::CAENEvent>(m, "CAENEvent")
        .def(py::init<int>(), py::arg("handle"))
        .def("get_data", [](const RedDigitizer::CAENEvent &self) {
            const auto* data = self.getData();
            return convertToPythonData(data);  // Convert to a Python-usable structure
        })
        .def("get_info", &RedDigitizer::CAENEvent::getInfo)
        // return info in a dictioanry format
        .def("get_info_dict", [](const RedDigitizer::CAENEvent &self) -> py::dict {
            const CAEN_DGTZ_EventInfo_t& info = self.getInfo();
            py::dict info_dict;
            info_dict["EventCounter"] = info.EventCounter;
            info_dict["EventSize"] = info.EventSize;
            info_dict["BoardId"] = info.BoardId;
            info_dict["Pattern"] = info.Pattern;
            info_dict["TriggerTimeTag"] = info.TriggerTimeTag;
            info_dict["ChannelMask"] = info.ChannelMask;
            return info_dict;
        })
        ;

    py::class_<CAEN_DGTZ_UINT16_EVENT_Python>(m, "CAENEventData")
        .def_readonly("ch_size", &CAEN_DGTZ_UINT16_EVENT_Python::ch_size)
        .def_readonly("data_channel", &CAEN_DGTZ_UINT16_EVENT_Python::data_channel)
        ;
    
    py::enum_<CAEN_DGTZ_TriggerPolarity_t>(m, "CAEN_DGTZ_TriggerPolarity")
        .value("Rising", CAEN_DGTZ_TriggerPolarity_t::CAEN_DGTZ_TriggerOnRisingEdge)
        .value("Falling", CAEN_DGTZ_TriggerPolarity_t::CAEN_DGTZ_TriggerOnFallingEdge)
        ;

    py::enum_<CAEN_DGTZ_IOLevel_t>(m, "CAEN_DGTZ_IOLevel")
        .value("NIM", CAEN_DGTZ_IOLevel_t::CAEN_DGTZ_IOLevel_NIM)
        .value("TTL", CAEN_DGTZ_IOLevel_t::CAEN_DGTZ_IOLevel_TTL)
        ;
    
    py::class_<CAEN_DGTZ_EventInfo_t>(m, "EventInfo")
        .def_readwrite("EventSize", &CAEN_DGTZ_EventInfo_t::EventSize)
        .def_readwrite("BoardId", &CAEN_DGTZ_EventInfo_t::BoardId)
        .def_readwrite("Pattern", &CAEN_DGTZ_EventInfo_t::Pattern)
        .def_readwrite("ChannelMask", &CAEN_DGTZ_EventInfo_t::ChannelMask)
        .def_readwrite("EventCounter", &CAEN_DGTZ_EventInfo_t::EventCounter)
        .def_readwrite("TriggerTimeTag", &CAEN_DGTZ_EventInfo_t::TriggerTimeTag)
        .def("__str__", [](const CAEN_DGTZ_EventInfo_t &info) {
            // Specify a way to print all configuration
            std::ostringstream oss;
            oss << "CAEN Event Info:\n"
                << "  Event Size: \t\t" << info.EventSize << "\n"
                << "  Board ID: \t\t" << info.BoardId << "\n"
                << "  Pattern: \t\t" << info.Pattern << "\n"
                << "  Channel Mask: \t" << info.ChannelMask << "\n"
                << "  Event Counter: \t" << info.EventCounter << "\n"
                << "  Trigger Time Tag: \t" << info.TriggerTimeTag << "\n";
            return oss.str();
        })
        ;

    py::class_<RedDigitizer::CAENWaveforms<uint16_t>>(m, "CAENWaveforms")
        .def("GetRecordLength", &RedDigitizer::CAENWaveforms<uint16_t>::getRecordLength)
        .def("GetTotalSize", &RedDigitizer::CAENWaveforms<uint16_t>::getTotalSize)
        .def("GetData", &RedDigitizer::CAENWaveforms<uint16_t>::getData, py::return_value_policy::reference);


    py::class_<RedDigitizer::CAENGlobalConfig>(m, "CAENGlobalConfig")
        .def(py::init<>())  // Default constructor
        .def_readwrite("MaxEventsPerRead", &RedDigitizer::CAENGlobalConfig::MaxEventsPerRead)
        .def_readwrite("RecordLength", &RedDigitizer::CAENGlobalConfig::RecordLength)
        .def_readwrite("PostTriggerPorcentage", &RedDigitizer::CAENGlobalConfig::PostTriggerPorcentage)
        .def_readwrite("EXTAsGate", &RedDigitizer::CAENGlobalConfig::EXTAsGate)
        .def_readwrite("EXTTriggerMode", &RedDigitizer::CAENGlobalConfig::EXTTriggerMode)
        .def_readwrite("SWTriggerMode", &RedDigitizer::CAENGlobalConfig::SWTriggerMode)
        .def_readwrite("CHTriggerMode", &RedDigitizer::CAENGlobalConfig::CHTriggerMode)
        .def_readwrite("TriggerPolarity", &RedDigitizer::CAENGlobalConfig::TriggerPolarity)
        .def_readwrite("IOLevel", &RedDigitizer::CAENGlobalConfig::IOLevel)
        .def_readwrite("AcqMode", &RedDigitizer::CAENGlobalConfig::AcqMode)
        .def_readwrite("TriggerOverlappingEn", &RedDigitizer::CAENGlobalConfig::TriggerOverlappingEn)
        .def_readwrite("DecimationFactor", &RedDigitizer::CAENGlobalConfig::DecimationFactor)
        .def_readwrite("MajorityLevel", &RedDigitizer::CAENGlobalConfig::MajorityLevel)
        .def_readwrite("MajorityCoincidenceWindow", &RedDigitizer::CAENGlobalConfig::MajorityCoincidenceWindow)
        .def_readwrite("MemoryFullModeSelection", &RedDigitizer::CAENGlobalConfig::MemoryFullModeSelection)
        .def("__str__", [](const RedDigitizer::CAENGlobalConfig &config) {
            // Specify a way to print all configuration
            std::ostringstream oss;
            oss << "CAEN Global Configuration:\n"
                << "  MaxEventsPerRead: \t\t" << config.MaxEventsPerRead << "\n"
                << "  RecordLength: \t\t" << config.RecordLength << "\n"
                << "  PostTriggerPorcentage: \t" << config.PostTriggerPorcentage << "\n"
                << "  EXTTriggerMode: \t\t" << config.EXTTriggerMode << "\n"
                << "  SWTriggerMode: \t\t" << config.SWTriggerMode << "\n"
                << "  CHTriggerMode: \t\t" << config.CHTriggerMode << "\n"
                << "  TriggerPolarity: \t\t" << (config.TriggerPolarity ? "Falling" : "Rising") << "\n"
                << "  EXTAsGate: \t\t\t" << (config.EXTAsGate ? "True" : "False") << "\n"
                << "  IOLevel: \t\t\t" << (config.IOLevel ? "TTL" : "NIM") << "\n"
                << "  AcqMode: \t\t\t" << config.AcqMode << "\n"
                << "  TriggerOverlappingEn: \t" << (config.TriggerOverlappingEn ? "True" : "False") << "\n"
                << "  MajorityLevel: \t\t" << config.MajorityLevel << "\n"
                << "  MajorityCoincidenceWindow: \t" << config.MajorityCoincidenceWindow << "\n"
                << "  MemoryFullModeSelection: \t" << (config.MemoryFullModeSelection ? "True" : "False");
            return oss.str();
        })
        ;

    py::class_<RedDigitizer::CAENGroupConfig>(m, "CAENGroupConfig")
        .def(py::init<>())
        .def(py::init<const RedDigitizer::CAENGroupConfig&>())
        .def_readwrite("Enabled", &RedDigitizer::CAENGroupConfig::Enabled)
        .def_readwrite("DCOffset", &RedDigitizer::CAENGroupConfig::DCOffset)
        .def_readwrite("DCRange", &RedDigitizer::CAENGroupConfig::DCRange)
        .def_readwrite("DCCorrections", &RedDigitizer::CAENGroupConfig::DCCorrections)
        .def_readwrite("TriggerThreshold", &RedDigitizer::CAENGroupConfig::TriggerThreshold)
        .def_readwrite("AcquisitionMask", &RedDigitizer::CAENGroupConfig::AcquisitionMask)
        .def_readwrite("TriggerMask", &RedDigitizer::CAENGroupConfig::TriggerMask)
        .def("__str__", [](const RedDigitizer::CAENGroupConfig &config) {
            // Specify a way to print all configuration
            std::ostringstream oss;
            oss << "CAENGroupConfig:\n"
                << "  Enabled: \t\t" << (config.Enabled ? "True" : "False") << "\n"
                << "  DCOffset: \t\t" << config.DCOffset << " (0x" << std::hex << config.DCOffset << ")\n"
                << "  DCRange: \t\t" << (config.DCRange ? "0.5 Vpp" : "2 Vpp") << "\n"
                << "  DCCorrections: \t[";
            for (size_t i = 0; i < config.DCCorrections.size(); ++i) {
                oss << static_cast<int>(config.DCCorrections[i]);
                if (i < config.DCCorrections.size() - 1) {
                    oss << ", ";
                }
            }
            oss << "]\n"
                << "  TriggerThreshold: \t" << config.TriggerThreshold << "\n"
                << "  AcquisitionMask: \t" << config.AcquisitionMask << "\n"
                << "  TriggerMask: \t\t" << config.TriggerMask << "\n";
            return oss.str();
        })
        ;

    py::class_<RedDigitizer::ChannelsMask>(m, "ChannelsMask")
    .def(py::init<>())
    .def("__len__", [](const RedDigitizer::ChannelsMask &self) {
        return RedDigitizer::ChannelsMask::kNumCHs;
    })
    .def("__getitem__", [](const RedDigitizer::ChannelsMask &self, std::size_t i) {
        if (i >= RedDigitizer::ChannelsMask::kNumCHs)
            throw py::index_error("Index out of range");
        return self.CH[i];
    })
    .def("__setitem__", [](RedDigitizer::ChannelsMask &self, std::size_t i, bool value) {
        if (i >= RedDigitizer::ChannelsMask::kNumCHs)
            throw py::index_error("Index out of range");
        self.CH[i] = value;
    })
    .def("__str__", [](const RedDigitizer::ChannelsMask &self) {
        // Specify a way to print value for each channel
        std::ostringstream oss;
        oss << "[";
        for (std::size_t i = 0; i < RedDigitizer::ChannelsMask::kNumCHs; ++i) {
            oss << (self.CH[i] ? "1" : "0");
            if (i < RedDigitizer::ChannelsMask::kNumCHs - 1) {
                oss << ", ";
            }
        }
        oss << "] (0x" << std::hex << static_cast<int>(self.get()) << ")";
        return oss.str();
    })
    .def("get", &RedDigitizer::ChannelsMask::get)
    .def("at", &RedDigitizer::ChannelsMask::at)
    .def_static("num_channels", []() {
        return RedDigitizer::ChannelsMask::kNumCHs;
    })
    ;

    py::class_<RedDigitizer::CAEN<>, std::shared_ptr<RedDigitizer::CAEN<>>>(m, "CAEN")
        .def(py::init<std::shared_ptr<RedDigitizer::iostream_wrapper>, RedDigitizer::CAENDigitizerModel, RedDigitizer::CAENConnectionType, int, int, uint32_t>())
        .def("IsConnected", &RedDigitizer::CAEN<>::IsConnected)
        .def("Setup", &RedDigitizer::CAEN<>::Setup,
            py::arg("global_config"), py::arg("group_configs"))
        .def("GetGlobalConfiguration", &RedDigitizer::CAEN<>::GetGlobalConfiguration, py::return_value_policy::copy)
        .def("GetGroupConfigurations", &RedDigitizer::CAEN<>::GetGroupConfigurations, py::return_value_policy::copy)
        .def("SoftwareTrigger", &RedDigitizer::CAEN<>::SoftwareTrigger)
        .def("GetBoardInfo", &RedDigitizer::CAEN<>::GetBoardInfo, py::return_value_policy::reference_internal)
        .def("RetrieveData", &RedDigitizer::CAEN<>::RetrieveData)
        .def("RetrieveDataUntilNEvents", &RedDigitizer::CAEN<>::RetrieveDataUntilNEvents,
            py::arg("n"))
        .def("DecodeEvents", &RedDigitizer::CAEN<>::DecodeEvents)
        .def("ClearData", &RedDigitizer::CAEN<>::ClearData)
        .def("GetNumberOfEvents", &RedDigitizer::CAEN<>::GetNumberOfEvents)
        .def("GetCurrentPossibleMaxBuffer", &RedDigitizer::CAEN<>::GetCurrentPossibleMaxBuffer)
        .def("GetEvent", &RedDigitizer::CAEN<>::GetEvent,
            py::arg("i"), py::return_value_policy::reference_internal)
        .def("GetEvents", &RedDigitizer::CAEN<>::GetEvents,
            py::return_value_policy::reference_internal)
        .def("GetEventsInfo", &RedDigitizer::CAEN<>::GetEventsInfo,
            py::return_value_policy::reference_internal)
        .def("GetEventsInfoDict", [](RedDigitizer::CAEN<>& self) -> py::dict {
            auto events_info = self.GetEventsInfo();
            py::list event_counters;
            py::list event_sizes;
            py::list board_ids;
            py::list patterns;
            py::list trigger_time_tags;
            py::list channel_masks;
            // loop through events to extract info
            for (const auto& info : events_info) {
                event_counters.append(info->EventCounter);
                event_sizes.append(info->EventSize);
                board_ids.append(info->BoardId);
                patterns.append(info->Pattern);
                trigger_time_tags.append(info->TriggerTimeTag);
                channel_masks.append(info->ChannelMask);
            }
            // save data in a dictionary
            py::dict info_dict;
            info_dict["EventCounter"] = event_counters;
            info_dict["EventSize"] = event_sizes;
            info_dict["BoardId"] = board_ids;
            info_dict["Pattern"] = patterns;
            info_dict["ChannelMask"] = channel_masks;
            info_dict["TriggerTimeTag"] = trigger_time_tags;
            return info_dict;
        })
        .def("GetEventsInBuffer", &RedDigitizer::CAEN<>::GetEventsInBuffer)
        .def("GetWaveform", [](RedDigitizer::CAEN<>& self, std::size_t i) {
            auto waveform = self.GetWaveform(i);
            if (waveform) {
                auto data = waveform->getData();  // Get the std::span<uint16_t>
                
                // Cast size_t to pybind11::ssize_t (which is long int)
                pybind11::array::ShapeContainer shape = {static_cast<pybind11::ssize_t>(waveform->getNumEnabledChannels()), 
                                                        static_cast<pybind11::ssize_t>(waveform->getRecordLength())};
                pybind11::array::StridesContainer strides = {waveform->getRecordLength() * sizeof(uint16_t), sizeof(uint16_t)};
                
                return py::array_t<uint16_t>(
                    std::move(shape),  // Shape
                    std::move(strides),  // Strides
                    data.data()  // Raw data pointer from std::span
                );
            }
            return py::array_t<uint16_t>();
        })
        .def("GetWaveforms", [](RedDigitizer::CAEN<>& self) -> py::array_t<uint16_t> {
            // Get the array of waveform pointers
            auto waveforms = self.GetWaveforms(); // std::array<std::shared_ptr<...>, N>
            const size_t num_waveforms = waveforms.size();
        
            // Find the first valid waveform to obtain the shape.
            size_t valid_idx = 0;
            while (valid_idx < num_waveforms && !waveforms[valid_idx]) {
                ++valid_idx;
            }
            if (valid_idx == num_waveforms) {
                throw std::runtime_error("No valid waveform found");
            }
            auto first_waveform = waveforms[valid_idx];
        
            // Get the shape from the first valid waveform.
            const size_t channels = first_waveform->getNumEnabledChannels();
            const size_t record_length = first_waveform->getRecordLength();
        
            // Total number of elements in the 3D array: [num_waveforms, channels, record_length].
            const size_t total_elems = num_waveforms * channels * record_length;
        
            // Allocate a contiguous buffer to hold all the waveform data.
            uint16_t* buffer = new uint16_t[total_elems];
        
            // For each waveform, copy its data into the correct position in the contiguous buffer.
            for (size_t i = 0; i < num_waveforms; ++i) {
                if (waveforms[i]) {
                    auto data = waveforms[i]->getData(); // returns std::span<uint16_t>
                    // Verify that the data size is as expected.
                    if (data.size() != channels * record_length) {
                        delete[] buffer;
                        throw std::runtime_error("Waveform size mismatch");
                    }
                    // Copy the waveform data into the block for waveform i.
                    std::memcpy(buffer + i * channels * record_length,
                                data.data(),
                                channels * record_length * sizeof(uint16_t));
                } else {
                    // If a waveform is missing, fill its block with zeros.
                    std::memset(buffer + i * channels * record_length,
                                0,
                                channels * record_length * sizeof(uint16_t));
                }
            }
        
            // Create a capsule to ensure that the buffer is freed when the NumPy array is garbage-collected.
            py::capsule free_buffer(buffer, [](void* f) {
                uint16_t* buf = reinterpret_cast<uint16_t*>(f);
                delete[] buf;
            });
        
            // Set up the shape and strides for a 3D array.
            // Shape: [num_waveforms, channels, record_length]
            py::array::ShapeContainer shape = {
                static_cast<py::ssize_t>(num_waveforms),
                static_cast<py::ssize_t>(channels),
                static_cast<py::ssize_t>(record_length)
            };
        
            // Strides (in bytes): 
            // - To go to the next waveform, skip over channels*record_length elements.
            // - To go to the next channel, skip over record_length elements.
            // - To go to the next sample, skip sizeof(uint16_t) bytes.
            py::array::StridesContainer strides = {
                static_cast<py::ssize_t>(channels * record_length * sizeof(uint16_t)),
                static_cast<py::ssize_t>(record_length * sizeof(uint16_t)),
                static_cast<py::ssize_t>(sizeof(uint16_t))
            };
        
            // Return a NumPy array that takes ownership of the buffer.
            return py::array_t<uint16_t>(shape, strides, buffer, free_buffer);
        })        
        .def("EnableAcquisition", &RedDigitizer::CAEN<>::EnableAcquisition)
        .def("DisableAcquisition", &RedDigitizer::CAEN<>::DisableAcquisition)
        ;

    // Add a wrapper for iostream_wrapper if needed
    py::class_<RedDigitizer::iostream_wrapper, std::shared_ptr<RedDigitizer::iostream_wrapper>>(m, "iostream_wrapper")
        .def(py::init<>());
}