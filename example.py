import red_caen
import time
import matplotlib.pyplot as plt
import numpy as np
import sbcbinaryformat as bf

caen = red_caen.CAEN(
    red_caen.iostream_wrapper(), 
    red_caen.CAENDigitizerModel.DT5740D, 
    red_caen.CAENConnectionType.USB,
    0,
    0,
    0)

model_constants = red_caen.GetCAENDigitizerModelConstants()

# global config
global_config = red_caen.CAENGlobalConfig()
global_config.MaxEventsPerRead = 500
global_config.RecordLength = 180
global_config.PostTriggerPorcentage = 50
global_config.EXTTriggerMode = red_caen.CAEN_DGTZ_TriggerMode.ACQ_ONLY
global_config.SWTriggerMode = red_caen.CAEN_DGTZ_TriggerMode.ACQ_ONLY
global_config.CHTriggerMode = red_caen.CAEN_DGTZ_TriggerMode.ACQ_ONLY
global_config.TriggerPolarity = red_caen.CAEN_DGTZ_TriggerPolarity.Falling
global_config.IOLevel = red_caen.CAEN_DGTZ_IOLevel.TTL
global_config.EXTAsGate = False
global_config.TriggerOverlappingEn = False
global_config.MemoryFullModeSelection = True
global_config.DecimationFactor = 0
global_config.MajorityLevel = 0
global_config.MajorityCoincidenceWindow = 0

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
    group_configs[i].TriggerThreshold = 0x800
    group_configs[i].AcquisitionMask = acq_mask
    group_configs[i].TriggerMask = trg_mask

# set config
caen.Setup(global_config, group_configs)

global_back = caen.GetGlobalConfiguration()
print(global_back)


groups_back = caen.GetGroupConfigurations()
# for g in groups_back:
#     print(g)

caen.EnableAcquisition()
for i in range(2000):
    caen.SoftwareTrigger()
    time.sleep(0.001)

time.sleep(2)

caen.RetrieveDataUntilNEvents(caen.GetEventsInBuffer()) # constant in parameter, and check if it's successful
print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")
caen.DecodeEvents()

while caen.GetEventsInBuffer()>=500:
    print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")
    caen.RetrieveDataUntilNEvents(500)
    caen.DecodeEvents()

caen.DisableAcquisition()

event = caen.GetEvent(0)
print(event.get_info())
data = event.get_data()

# print(data)
# bf()

print(data.ch_size)
# plt.figure()
# for i in range(32):
#     plt.plot(np.array(data.data_channel[i]))
# plt.show()

caen.RetrieveDataUntilNEvents(caen.GetEventsInBuffer())
caen.DecodeEvents()
print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")

event = caen.GetEvent(0)
print(event.get_data())
# print(wf)
print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")

caen.ClearData()
print(caen.GetCurrentPossibleMaxBuffer())
print(f"buffer: {caen.GetEventsInBuffer()}, retrieved: {caen.GetNumberOfEvents()}")
print("All done!")

