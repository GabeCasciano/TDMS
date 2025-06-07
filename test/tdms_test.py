from nptdms import TdmsFile

# Open the TDMS file
tdms_file = TdmsFile.read("tdms_test.tdms")

# List all groups
for group in tdms_file.groups():
    print("Group:", group.name)

    # List all channels in the group
    for channel in group.channels():
        print("  Channel:", channel.name)
        print("    Data:", channel[:10])  # Print first 10 samples
