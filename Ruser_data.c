#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USER_DATA_BIN_PATH "./UserData.bin"

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

void ReadUserData(const char *filename, int index, UserData *userData, ParkingSpace *parkingSpace)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("File opening failed");
        exit(1);
    }

    Header header = {0};
    fread(&header, sizeof(header), 1, file);

    if (index >= 0 && index < header.UserDataCount)
    {
        Offset offset = {
            .UserDataOffset = sizeof(Header) + (index * sizeof(UserData)) + (index * sizeof(ParkingSpace)),
            .ParkingSpaceDataOffset = sizeof(Header) + ((index + 1) * sizeof(UserData)) + (index * sizeof(ParkingSpace))};

        fseek(file, offset.UserDataOffset, SEEK_SET);
        fread(userData, sizeof(UserData), 1, file);

        userData->Name[sizeof(userData->Name) - 1] = '\0';

        fseek(file, offset.ParkingSpaceDataOffset, SEEK_SET);
        fread(parkingSpace, sizeof(ParkingSpace), 1, file);
    }

    fclose(file);
}

int main()
{
    const char *filename = USER_DATA_BIN_PATH;

    Header header = {0};
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("File opening failed");
        return 1;
    }
    fread(&header, sizeof(header), 1, file);

    int totalDataCount = header.UserDataCount;

    UserData *userDataArray = malloc(sizeof(UserData) * totalDataCount);
    ParkingSpace *parkingSpaceArray = malloc(sizeof(ParkingSpace) * totalDataCount);

    if (!userDataArray || !parkingSpaceArray)
    {
        perror("Memory allocation failed");
        fclose(file);
        free(userDataArray);
        free(parkingSpaceArray);
        return 1;
    }

    for (int i = 0; i < totalDataCount; i++)
    {
        ReadUserData(filename, i, &userDataArray[i], &parkingSpaceArray[i]);
        printf("UserData %d: %s, %s, %s\n", i, userDataArray[i].Name, userDataArray[i].CarType, userDataArray[i].CarNumber);
        printf("ParkingSpace %d: %d\n", i, parkingSpaceArray[i].ParkingSpace);
    }

    free(userDataArray);
    free(parkingSpaceArray);
    fclose(file);

    return 0;
}
