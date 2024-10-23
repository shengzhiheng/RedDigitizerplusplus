import red_caen
import time

caen = red_caen.CAEN(
    red_caen.iostream_wrapper(), 
    red_caen.CAENDigitizerModel.DT5740D, 
    red_caen.CAENConnectionType.USB,
    0,
    0,
    0)

# global config
global_config = red_caen.CAENGlobalConfig()
global_config.MaxEventsPerRead = 500
global_config.RecordLength = 180
global_config.PostTriggerPorcentage = 50
global_config.EXTTriggerMode = red_caen.CAEN_DGTZ_TriggerMode.ACQ_ONLY
global_config.SWTriggerMode = red_caen.CAEN_DGTZ_TriggerMode.ACQ_ONLY
global_config.CHTriggerMode = red_caen.CAEN_DGTZ_TriggerMode.ACQ_ONLY
global_config.TriggerPolarity = red_caen.CAEN_DGTZ_TriggerPolarity.TriggerOnRisingEdge
global_config.EXTAsGate = False

# group config (need length 8 for the wrapper, but only first 4 is used)
group_configs = [red_caen.CAENGroupConfig() for i in range(8)]
for i in range(8):
    acq_mask = red_caen.ChannelsMask()
    trg_mask = red_caen.ChannelsMask()
    for ch in range(8):
        acq_mask[ch] = True
        trg_mask[ch] = True
    
    group_configs[i].Enabled = True
    group_configs[i].DCOffset = 0x8000
    group_configs[i].DCCorrections = [0,0,0,0,0,0,0,0]
    group_configs[i].DCRange = 0
    group_configs[i].TriggerThreshold = 2048
    group_configs[i].AcquisitionMask = acq_mask
    group_configs[i].TriggerMask = trg_mask

# set config
caen.Setup(global_config, group_configs)

caen.EnableAcquisition()
for i in range(100):
    caen.SoftwareTrigger()
    time.sleep(0.01)

time.sleep(2)

print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")

caen.RetrieveData()
caen.DecodeEvents()

print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")

# data = caen.GetEvent(1)
# print(data)

caen.DisableAcquisition()
