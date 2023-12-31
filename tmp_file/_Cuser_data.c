
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int TotalDataSize;
    int UserDataCount;
    int ParkingSpaceDataCount;
    int UserDataSize;
    int ParkingSpaceSize;
} Header;

typedef struct
{
    int UserDataOffset;
    int ParkingSpaceDataOffset;
} Offset;

typedef struct
{
    char Name[13];
    char CarType[20];
    char CarNumber[12];
} UserData;

typedef struct
{
    int ParkingSpace;
} ParkingSpace;

void Initialize_bin_file(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("File opening failed");
        return;
    }

    Header header = {0, 0, 0, sizeof(UserData), sizeof(ParkingSpace)};
    fwrite(&header, sizeof(header), 1, file);
    fclose(file);
}

void AddData(const char *filename, UserData *userData, ParkingSpace *parkingSpace)
{
    // 파일을 업데이트 모드로 열기. 파일이 없으면 새로 초기화합니다.
    FILE *file = fopen(filename, "rb+");
    if (!file)
    {
        Initialize_bin_file(filename);
        file = fopen(filename, "rb+");
        if (!file)
        {
            perror("File opening failed on second attempt");
            return; // 파일 열기 실패시 함수 종료
        }
    }

    // 헤더 정보 읽기
    Header header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, file);

    // 파일 끝으로 이동하여 데이터 추가
    fseek(file, 0, SEEK_END);
    fwrite(userData, sizeof(UserData), 1, file);

    // "ParkingSpace" 초기화
    parkingSpace->ParkingSpace = 0;
    fwrite(parkingSpace, sizeof(ParkingSpace), 1, file);

    // 헤더 업데이트
    header.UserDataCount++;
    header.ParkingSpaceDataCount++;
    header.TotalDataSize += sizeof(UserData) + sizeof(ParkingSpace);

    // 헤더 정보를 파일 시작 부분에 다시 쓰기
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, file);

    // 파일 닫기
    fclose(file);
}

int main(int argc, char *argv[])
{
    const char *filename = "./UserData.bin";

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s Name CarType CarNumber\n", argv[0]);
        return 1;
    }

    UserData userData;
    strncpy(userData.Name, argv[1], sizeof(userData.Name) - 1);
    userData.Name[sizeof(userData.Name) - 1] = '\0';
    strncpy(userData.CarType, argv[2], sizeof(userData.CarType) - 1);
    userData.CarType[sizeof(userData.CarType) - 1] = '\0';
    strncpy(userData.CarNumber, argv[3], sizeof(userData.CarNumber) - 1);
    userData.CarNumber[sizeof(userData.CarNumber) - 1] = '\0';

    ParkingSpace parkingSpace = {0};

    AddData(filename, &userData, &parkingSpace);

    printf("Data added: %s, %s, %s\n", userData.Name, userData.CarType, userData.CarNumber);

    return 0;
}