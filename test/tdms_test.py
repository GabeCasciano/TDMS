from nptdms import TdmsFile, TdmsWriter, RootObject, GroupObject, ChannelObject
import numpy as np

root_obj = RootObject(properties={"RootTestProp": "RootTestProptValue"})

group_obj = GroupObject(
    "TestGroup", properties={"TestGroupProperties": "TestGroupPropertiesValue"}
)

data = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
chan_obj = ChannelObject("TestGroup", "TestChannel", data, properties={})

with TdmsWriter("test_npTDMS.tdms") as tdms_writer:
    tdms_writer.write_segment([root_obj, group_obj, chan_obj])

    tdms_writer.write_segment([chan_obj])

# Open the TDMS file

tdms_file = TdmsFile.read("test_npTDMS.tdms")

# List all groups
for group in tdms_file.groups():
    print("Group:", group.name)

    # List all channels in the group
    for channel in group.channels():
        print("  Channel:", channel.name)
        print("    Data:", channel[:10])  # Print first 10 samples
