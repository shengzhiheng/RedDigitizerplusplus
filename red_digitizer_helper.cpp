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
        .def("get_data", [](const RedDigitizer::CAENEvent &self) {
            const auto* data = self.getData();
            return convertToPythonData(data);  // Convert to a Python-usable structure
        })
        .def("get_info", &RedDigitizer::CAENEvent::getInfo)
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
        .def("getRecordLength", &RedDigitizer::CAENWaveforms<uint16_t>::getRecordLength)
        .def("getTotalSsize", &RedDigitizer::CAENWaveforms<uint16_t>::getTotalSize)
        .def("getData", &RedDigitizer::CAENWaveforms<uint16_t>::getData, py::return_value_policy::reference);


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
        .def("GetEvent", &RedDigitizer::CAEN<>::GetEvent,
            py::arg("i"))
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
        .def("EnableAcquisition", &RedDigitizer::CAEN<>::EnableAcquisition)
        .def("DisableAcquisition", &RedDigitizer::CAEN<>::DisableAcquisition)
        ;

    // Add a wrapper for iostream_wrapper if needed
    py::class_<RedDigitizer::iostream_wrapper, std::shared_ptr<RedDigitizer::iostream_wrapper>>(m, "iostream_wrapper")
        .def(py::init<>());
}