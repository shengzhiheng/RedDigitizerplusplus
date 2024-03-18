/*
 * Author: Hector Hawley Herrera
 *
 * This example assumes a digitizer with no grouping.
 * For an example with grouping, look at SingleAcquisitionGroups
 * */

#include "RedDigitizer++/red_digitizer_helper.hpp"

using namespace RedDigitizer;

int main() {
    std::shared_ptr<iostream_wrapper> logger = std::make_shared<iostream_wrapper>();
    std::shared_ptr<CAENWaveforms<uint16_t>> waveforms;

    {
        CAEN resource(logger,
                      CAENDigitizerModel::DT5730B,  // CAEN Model
                      CAENConnectionType::USB,      // Connection type
                      0,                            // Link number.
                      0,                            // Conet Node.
                      0);                           // VME Address

        // Setup
        CAENGlobalConfig global_config;
        global_config.RecordLength = 1000;
        global_config.SWTriggerMode = CAEN_DGTZ_TriggerMode_t::CAEN_DGTZ_TRGMODE_ACQ_ONLY;

        std::array<CAENGroupConfig, 8> group_configs;
        group_configs[0].Enabled = true;
        group_configs[0].TriggerMask[0] = true;

        resource.Setup(global_config, group_configs);

        resource.EnableAcquisition();
        // waveform now points to the internal waveform resource
        // This HAS to be called after EnableAcquisition
        waveforms = resource.GetWaveform(0);

        // Force an acquisition
        resource.SoftwareTrigger();

        // Reads the data from the digitizers
        resource.RetrieveData();
        // This decodes and writes to internal buffers
        // Now waveform has the data as well!
        if (resource.GetNumberOfEvents() > 0) {
            resource.DecodeEvents();

            // Clears CAEN digitizer internal buffers
            resource.ClearData();
        }

        resource.DisableAcquisition();
    }

    // Now CAEN resources are open and waveform remains viable.
    // Do whatever you want with waveform

    std::cout << "CAEN time tag: " << waveforms->getInfo().TriggerTimeTag << std::endl;
    std::cout << "Waveform size: " << waveforms->getTotalSize() << std::endl;
    // To get data from channel 0
    auto waveform = waveforms->get(0);
    for(auto count : waveform) {
        std::cout << count << std::endl;
    }
}