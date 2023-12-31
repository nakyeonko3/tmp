#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#define USER_DATA_BIN_PATH "./UserData.bin"

// 데이터 헤더를 위한 구조체 정의
typedef struct
{
    int TotalDataSize;
    int UserDataCount;
    int SelectParkingSpaceDataCount;
    int UserDataSize;
    int SelectParkingSpaceSize;
} Header;

// 데이터 오프셋을 위한 구조체 정의
typedef struct
{
    int UserDataOffset;
    int SelectParkingSpaceDataOffset;
} Offset;

// 사용자 데이터 구조체 정의
typedef struct
{
    char Name[13];
    char CarType[20];
    char CarNumber[12];
} UserData;

// 선택한 주차 공간 데이터 구조체 정의
typedef struct
{
    int ParkingSpace;
} ParkingSpace;

FILE *read_byte_binfile(const char *filename)
{
    FILE *file = fopen(filename, "r+b");
    if (!file)
    {
        perror("File opening failed");
        exit(1);
    }
    return file;
}

// 데이터 파일 초기화 및 헤더 작성
void Initialize_bin_file(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("File opening failed");
        return;
    }

    Header header = {0, 0, 0, sizeof(UserData), sizeof(ParkingSpace)};
    fwrite(&header, sizeof(header), 1, file); // 파일 시작 부분에 헤더 저장
    printf("Initialize_bin_file 초기화");

    fclose(file);
}

int search_data(const char *filename, const char *CarNumber)
{
    FILE *file = fopen(filename, "r+b");
    if (!file)
    {
        perror("File opening failed");
        return 0;
    }

    Header header;
    fread(&header, sizeof(header), 1, file);

    // 사용자 데이터 읽기 (CarNumber에 해당하는 데이터 검색)
    UserData userData;

    for (int i = 0; i < header.UserDataCount; ++i)
    {
        fread(&userData, sizeof(UserData), 1, file);
        if (strcmp(userData.CarNumber, CarNumber) == 0)
        {
            fclose(file);
            return 1; // 원하는 데이터를 찾았을 때 원하는 결과 문자열로 반환
        }
    }

    fclose(file);
    return 0; // 원하는 데이터를 찾지 못했을 때 기본 메시지 반환
}

// 사용자 데이터 등록 함수
void register_data(const char *filename, const char *Name, const char *CarType, const char *CarNumber)
{
    FILE *file = fopen(filename, "r+b"); // 읽기/쓰기 모드로 파일 열기 ('b'는 바이너리 모드)
    if (!file)
    {
        perror("File opening failed");
        return;
    }

    Header header;
    Offset offset;

    // 헤더 정보 읽기
    fread(&header, sizeof(header), 1, file);

    // 현재 오프셋 계산
    offset.UserDataOffset = sizeof(Header) + (header.UserDataCount * sizeof(UserData)) + (header.UserDataCount * sizeof(ParkingSpace));
    //   offset.SelectParkingSpaceDataOffset = offset.UserDataOffset + (header.SelectParkingSpaceDataCount * sizeof(ParkingSpace));

    // 오프셋으로 이동 후 사용자 데이터 쓰기
    fseek(file, offset.UserDataOffset, SEEK_SET);

    UserData userData;
    strncpy(userData.Name, Name, sizeof(userData.Name));
    strncpy(userData.CarType, CarType, sizeof(userData.CarType));
    strncpy(userData.CarNumber, CarNumber, sizeof(userData.CarNumber));

    fwrite(&userData, sizeof(UserData), 1, file);

    // 헤더 업데이트
    header.TotalDataSize += sizeof(UserData);
    header.UserDataCount++;

    // 파일 시작으로 이동 후 헤더 정보 업데이트
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, file);
    printf("Data added: %s, %s, %s\n", userData.Name, userData.CarType, userData.CarNumber);

    fclose(file);
}

int UpdateParkingSpace(const char *filename, const char *CarNumberToUpdate, const char *NewParkingSpace)
{
    FILE *file = fopen(filename, "r+b");

    if (!file)
    {
        perror("File opening failed");
        return 1;
    }

    Header header;
    fread(&header, sizeof(header), 1, file);

    UserData userData;
    int found = 0; // Flag to track if the CarNumberToUpdate is found
    int count = 0; // 바이너리 파일 안의 carNumber가 저장된 위치

    for (int i = 0; i < header.UserDataCount; ++i)
    {
        int size = sizeof(header) + i * sizeof(ParkingSpace) + i * sizeof(UserData);
        fseek(file, size, SEEK_SET);
        fread(&userData, sizeof(UserData), 1, file);
        count++;
        if (strcmp(userData.CarNumber, CarNumberToUpdate) == 0)
        {
            found = 1;
            break;
        }
    }

    if (found)
    {
        ParkingSpace parkingSpace;
        parkingSpace.ParkingSpace = atoi(NewParkingSpace);

        int parkingSpaceOffset = sizeof(Header) + (count * sizeof(UserData)) + ((count - 1) * sizeof(ParkingSpace));

        fseek(file, parkingSpaceOffset, SEEK_SET);

        fwrite(&parkingSpace, sizeof(ParkingSpace), 1, file);

        printf("CarNumber: %s ", CarNumberToUpdate);
        printf(" updated ParkingSpaceNumber: %d\n", parkingSpace.ParkingSpace);
    }
    else
    {
        printf("Car Number %s not found.\n", CarNumberToUpdate);
        exit(1);
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[])
{

    const char *filename = USER_DATA_BIN_PATH;

    if (argc < 2)
    {
        return 0;
    }

    int flag = atoi(argv[1]);

    if (flag == 1)
    {
        if (argc != 5)
        {
            printf("argc =! 5, argments is not 4");
            return 1;
        }
        register_data(filename, argv[2], argv[3], argv[4]);
        return 0;
    }

    if (flag == 2)
    {
        if (argc != 4)
        {
            printf("argc =! 3, argments is not 3");
            return 1;
        }
        UpdateParkingSpace(filename, argv[2], argv[3]);
        return 0;
    }

    if (flag == 3)
    {
        Initialize_bin_file(filename); // 파일 초기화
        return 1;
    }

    return 1;
}