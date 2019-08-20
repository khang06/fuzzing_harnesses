#include <Windows.h>
#include <dia2.h>
#include <cstdio>
#include <shlwapi.h>

typedef HRESULT(__stdcall* pDllGetClassObject)(
    _In_  REFCLSID rclsid,
    _In_  REFIID   riid,
    _Out_ LPVOID*   ppv
    );

HRESULT STDMETHODCALLTYPE NoRegCoCreate(const __wchar_t* dllName,
    REFCLSID   rclsid,
    REFIID     riid,
    void**     ppv)
{
    HRESULT hr;
    HMODULE hModule = LoadLibraryExW(dllName, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    pDllGetClassObject DllGetClassObject;
    if (hModule && (DllGetClassObject = (pDllGetClassObject)GetProcAddress(hModule, "DllGetClassObject")))
    {
        IClassFactory* classFactory;
        hr = DllGetClassObject(rclsid, IID_IClassFactory, (LPVOID*)&classFactory);
        if (SUCCEEDED(hr))
        {
            hr = classFactory->CreateInstance(nullptr, riid, ppv);
            classFactory->AddRef();
        }
    }
    else
    {
        hr = GetLastError();
        if (hr > 0)
            hr |= REASON_LEGACY_API;
    }
    return hr;
}

wchar_t* charToWChar(const char* text) {
    size_t size = strlen(text) + 1;
    wchar_t* wa = new wchar_t[size];
    mbstowcs(wa, text, size);
    return wa;
}

IDiaDataSource* data_source;
IDiaSession* session;
IStream* stream;

extern "C" __declspec(dllexport) void fuzzme(const char* input);

void fuzzme(const char* input) {
    auto conv = charToWChar(input);
    FILE* file = fopen(input, "rb");
    fseek(file, 0, SEEK_END);
    DWORD size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[size];
    fread(data, size, 1, file);
    fclose(file);
    stream = SHCreateMemStream((const BYTE*)data, size);
    if (FAILED(data_source->loadDataFromIStream(stream))) {
        stream->Release();
        delete[] data;
        delete[] conv;
        return;
    }
    if (FAILED(data_source->openSession(&session))) {
        stream->Release();
        delete[] data;
        delete[] conv;
        return;
    }

    session->Release();
    stream->Release();
    delete[] data;
    delete[] conv;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("retard\n");
        return 1;
    }
    CoInitialize(NULL);
    NoRegCoCreate(L"msdia140.dll", __uuidof(DiaSource), __uuidof(IDiaDataSource), (LPVOID*)&data_source);
    fuzzme(argv[1]);
    data_source->Release();
    return 0;
}