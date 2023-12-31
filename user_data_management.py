class UserDataManagement:
    def __init__(self, user_data_file="./UserData.bin"):
        self.user_data_file = user_data_file
        self.user_data = {}
    
    def is_regisered_CarNumber(self, CarNumber):
        return CarNumber in self.user_data

    def is_regisered_ParkingSpaceNumber(self, ParkingSpaceNumber):
        return ParkingSpaceNumber in self.user_data

    def search_data(self, CarNumber):
        if CarNumber in self.user_data:
            return self.user_data[CarNumber]

    def register_data(self, UserData):
        CarNumber = UserData["CarNumber"]
        UserData["ParkingSpace"] = ""  # 주차 공간 정보 초기화
        self.user_data[CarNumber] = UserData
        return "데이터가 등록되었습니다."
    
    def match_and_register_data(self, CarNumber, ParkingSpace):
        if CarNumber in self.user_data:
            self.user_data[CarNumber]["ParkingSpace"] = ParkingSpace
            return "주차 공간이 등록되었습니다."
        else:
            return "데이터를 다시 입력해주세요."