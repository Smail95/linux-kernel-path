FILE = trace.json
FUNCTION = plot
PY_PATH = plot_json.py
CC = python3
TOADD =   {}]}]}}

TO = {}]}]}}

# CAN BE MODIFIED !!!
DISK_DEVICE = sda6			#device type to analyze 
TORECORD = aqu-sz			#data filed to record from DISK_DEVICE statistics 

run:	
	echo $(TOADD) >> $(FILE); \
	$(CC) $(PY_PATH) $(FUNCTION) $(FILE) $(DISK_DEVICE) $(TORECORD) 
	

clean:
	rm -f *.o *~  *.csv trace.json


