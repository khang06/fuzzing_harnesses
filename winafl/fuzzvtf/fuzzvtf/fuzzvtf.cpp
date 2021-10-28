#include <stdio.h>
#include "vtf.h"

extern "C" __declspec(noinline, dllexport) void fuzzme(char* path);
void fuzzme(char* path) {
    printf("Loading VTF...\n");
    auto vtf = CreateVTFTexture();
    if (!vtf) {
        printf("VTF creation failed, wtf?\n");
        return;
    }

    auto file = fopen(path, "rb");
    if (!file) {
        printf("File open failed, wtf?\n");
        return;
    }

    printf("Loading input...\n");
    fseek(file, 0, SEEK_END);
    auto size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto* data = new uint8[size];
    fread(data, size, 1, file);
    fclose(file);

    printf("Unserializing VTF...\n");
    CUtlBuffer buf(data, size, 8);
    if (vtf->Unserialize(buf)) {
        printf("\n");
        printf("Width: %d\n", vtf->Width());
        printf("Height: %d\n", vtf->Height());
        printf("Depth: %d\n", vtf->Depth());
        printf("MipCount: %d\n", vtf->MipCount());
        printf("FaceCount: %d\n", vtf->FaceCount());
        printf("FrameCount: %d\n", vtf->FrameCount());
        printf("\n");
    } else {
        printf("Failed to unserialize...\n");
    }

    printf("Seems fine for now, destroying VTF and file buffer...\n");
    DestroyVTFTexture(vtf);
    //buf.Purge();
    // i'm sure it's fine...
    delete[] data;

    printf("All good!\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: fuzzvtf.exe [vtf file]\n");
        return 1;
    }

    fuzzme(argv[1]);

    return 0;
}