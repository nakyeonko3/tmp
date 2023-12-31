from flask import Flask, request, Response, jsonify
from flask_cors import CORS
from user_data_management import UserDataManagement
import subprocess
import json

# 각 실행파일과 바이너리 파일 위치 상수 변수
USER_DATA_BIN_FILE_PATH="./UserData.bin"
CUSER_DATA_FILE_PATH="./Cuser_data"
RMAP_DATA_FILE_PATH="./Rmap_data"

app = Flask(__name__)
app.json.ensure_ascii = False
CORS(app)

user_data_management = UserDataManagement(user_data_file=USER_DATA_BIN_FILE_PATH)

@app.route("/UserInfo", methods=["POST"])
def handle_user():
    data = request.json
    EventID = data.get("EventID")

    if EventID == "1":
        UserData = data
        CarNumber = str(UserData['CarNumber'])
        Name = UserData["Name"]
        CarType = UserData["CarType"]
        
        if user_data_management.is_regisered_CarNumber(CarNumber):
            return jsonify({"message":f"차번호: {CarNumber}는 이미 등록된 데이터입니다."})

        try:
            result = subprocess.run([CUSER_DATA_FILE_PATH, EventID, Name, CarType, CarNumber])
        except Exception as e:
            print(e, "./Cuser_data 유저 데이터 등록 중 에러 발생")
            return {"message":"유저 데이터 등록 실패"}, 500
        else:
            user_data_management.register_data(UserData=UserData)
            return {"message":"유저 데이터 등록 완료"}, 200
            

    elif EventID == "2":
        ParkingSpace = data
        CarNumber = str(ParkingSpace["CarNumber"])
        ParkingSpaceNumber = str(ParkingSpace["ParkingSpace"])

        # match_and_register_data 함수를 호출하고 반환값을 result에 저장
        result = user_data_management.match_and_register_data(ParkingSpace= ParkingSpaceNumber, CarNumber=CarNumber)

        if user_data_management.is_regisered_ParkingSpaceNumber(ParkingSpaceNumber):
            return jsonify({"message":f"주자장 번호: {ParkingSpaceNumber}는 이미 등록된 데이터입니다."})
        try:
            result = subprocess.run([CUSER_DATA_FILE_PATH, EventID, CarNumber, ParkingSpaceNumber])
        except Exception as e:
            print(e)
            return jsonify({"message":f"주자장 번호 등록 실패"}), 500
        else:
            if result.returncode == 1:
                return jsonify({"message":f"주자장 번호 등록 실패"}), 500
            return jsonify({"message":f"주자장 번호 등록 성공, 주차번호:{CarNumber}, 주차장:{ParkingSpaceNumber}"}), 200

            


@app.route("/get-json-data", methods=["GET"])
def get_json_data():

    EventID = request.args.get("EventID")
    
    # 클라이언트로부터 받은 EventID 출력
    print(f"Received EventID from client: {EventID}")
    
    if EventID == "3":
        result = subprocess.run([RMAP_DATA_FILE_PATH], capture_output = True, text = True)
        if result.returncode == 0:
            print("returncode: ", result.returncode)
            json_data = result.stdout # stdout: Standard Output 줄임말, "./Rmap_data" 실행하고 저장하고 반환합니다.
            print("Command Output: ", result.stdout)
            return jsonify(json_data)
        else:
            return jsonify({"message": "데이터가 없습니다."}), 404 # 클라이언트가 요청한 데이터를 서버에서 찾을 수 없는 오류입니다.
    else:
        return jsonify({"message": "잘못된 EventID입니다."}), 404 # 클라이언트가 요청한 데이터를 서버에서 찾을 수 없는 오류입니다.

if __name__ == "__main__":
    app.run(host = "0.0.0.0", debug=True)