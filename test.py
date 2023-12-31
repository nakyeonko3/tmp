import subprocess
CUSER_DATA_FILE_PATH = "./Cuser_data"
EventID = "2"
CarNumber = "312314-k-dart-kim"
ParkingSpace = "25"

subprocess.run([CUSER_DATA_FILE_PATH, EventID, CarNumber, ParkingSpace])