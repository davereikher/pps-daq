#!/usr/bin/python
import struct
import sys
import matplotlib.pyplot as plt

def get_sampling_frequency(numeric):
	if numeric == 0:
		return 5.0
	elif numeric == 1:
		return 2.5
	elif numeric == 2:
		return 1.0


def parse_event(data):
	header = struct.unpack("<IIII", data[:16])
	if (header[0] & 0xf0000000 == 0xa0000000):
		print "header ok"
	fld_event_size_bytes = (header[0] & 0x0fffffff) * 4
	fld_board_id = (header[1] & 0xf8000000) >> 5
	fld_pattern = (header[1] & 0x003fff00) >> 8
	fld_group_mask = header[1] & 0x0000ffff
	fld_event_counter = header[2] & 0x003fffff
	fld_event_time_tag = header[3]

	samples_on_channel_0 = []

	#Here we're looking only at group 0
	data_remainder = data[16:]
	event_description_word = struct.unpack("<I", data_remainder[:4])[0]
	print "Event description word: {}".format(hex(event_description_word))
	fld_grp_start_index_cell = event_description_word & 0x3ff00000 >> 20
	fld_grp_freq = event_description_word & 0x00030000 >> 16
	fld_grp_tr = event_description_word & 0x00001000 >> 12
	fld_grp_size_ch = event_description_word & 0x00000fff
	print "Start index cell: {}".format(fld_grp_start_index_cell)
	print "Frequency: {} GS/s".format(get_sampling_frequency(fld_grp_freq))
	print "TR present in readout: {}".format(fld_grp_tr)
	print "Samples for each channel: {}".format(fld_grp_size_ch)
	data_remainder = data_remainder[4:]

	#Collect all samples on channel 0:
	for i in range(fld_grp_size_ch/4):
	#	raw_input("Enter...")
		sample_on_channel_0 = struct.unpack("<I", data_remainder[:4])[0] & 0x00000fff
	#	print struct.unpack("<I", data_remainder[:4])[0], sample_on_channel_0
		data_remainder = data_remainder[12:]
		samples_on_channel_0.extend([sample_on_channel_0])
	#print samples_on_channel_0
	return samples_on_channel_0

if __name__=="__main__":
	data = open(sys.argv[1], "rb").read()
	samples_on_channel_0 = parse_event(data)
	plt.plot(samples_on_channel_0)
	
	plt.show()	
	print "Event size = {}".format(fld_event_size_bytes)
	print "Board id = {}".format(fld_board_id)
	print "Pattern = {}".format(fld_pattern)
	print "Group mask = {}".format(hex(fld_group_mask))
	print "Event counter = {}".format(fld_event_counter)
	print "Event time tag = {}".format(fld_event_time_tag)




