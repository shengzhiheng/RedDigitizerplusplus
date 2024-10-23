#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <span>
#include "include/RedDigitizer++/red_digitizer_helper.hpp"

namespace py = pybind11;

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
    
    py::enum_<CAEN_DGTZ_TriggerPolarity_t>(m, "CAEN_DGTZ_TriggerPolarity")
        .value("TriggerOnRisingEdge", CAEN_DGTZ_TriggerPolarity_t::CAEN_DGTZ_TriggerOnRisingEdge)
        .value("TriggerOnFallingEdge", CAEN_DGTZ_TriggerPolarity_t::CAEN_DGTZ_TriggerOnFallingEdge)
        ;

    py::class_<RedDigitizer::CAENGlobalConfig>(m, "CAENGlobalConfig")
        .def(py::init<>())  // Default constructor
        .def_readwrite("MaxEventsPerRead", &RedDigitizer::CAENGlobalConfig::MaxEventsPerRead)
        .def_readwrite("RecordLength", &RedDigitizer::CAENGlobalConfig::RecordLength)
        .def_readwrite("PostTriggerPorcentage", &RedDigitizer::CAENGlobalConfig::PostTriggerPorcentage)
        .def_readwrite("EXTAsGate", &RedDigitizer::CAENGlobalConfig::EXTAsGate)
        .def_readwrite("EXTTriggerMode", &RedDigitizer::CAENGlobalConfig::EXTTriggerMode)
        .def_readwrite("SWTriggerMode", &RedDigitizer::CAENGlobalConfig::SWTriggerMode)
        .def_readwrite("CHTriggerMode", &RedDigitizer::CAENGlobalConfig::CHTriggerMode)
        .def_readwrite("AcqMode", &RedDigitizer::CAENGlobalConfig::AcqMode)
        .def_readwrite("IOLevel", &RedDigitizer::CAENGlobalConfig::IOLevel)
        .def_readwrite("TriggerOverlappingEn", &RedDigitizer::CAENGlobalConfig::TriggerOverlappingEn)
        .def_readwrite("MemoryFullModeSelection", &RedDigitizer::CAENGlobalConfig::MemoryFullModeSelection)
        .def_readwrite("TriggerPolarity", &RedDigitizer::CAENGlobalConfig::TriggerPolarity)
        .def_readwrite("DecimationFactor", &RedDigitizer::CAENGlobalConfig::DecimationFactor)
        .def_readwrite("MajorityLevel", &RedDigitizer::CAENGlobalConfig::MajorityLevel)
        .def_readwrite("MajorityCoincidenceWindow", &RedDigitizer::CAENGlobalConfig::MajorityCoincidenceWindow)
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