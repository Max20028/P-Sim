#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3d11.h>
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\directxmath.h>
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\wincodec.h>


static IWICImagingFactory* _GetWIC()
{
    static IWICImagingFactory* s_Factory = nullptr;

    if ( s_Factory )
        return s_Factory;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory),
        (LPVOID*)&s_Factory
        );

    if ( FAILED(hr) )
    {
        s_Factory = nullptr;
        return nullptr;
    }

    return s_Factory;
}

void CreateTextureFromWIC( _In_ ID3D11Device* d3dDevice, _In_opt_ ID3D11DeviceContext* d3dContext, _In_ IWICBitmapFrameDecode *frame, _Out_opt_ ID3D11Resource** texture, _Out_opt_ ID3D11ShaderResourceView** textureView, _In_opt_ size_t maxsize) {

}

//https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-how-to
//Followed this, didn't copy their junk
void CreateShaderRescViewTextureFromFile(ID3D11Device* dev, ID3D11DeviceContext* devcon, LPCWSTR filename, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, size_t maxsize ) {
    //Create the imaging factory
    IWICImagingFactory* imagingfactory = _GetWIC();

    //Dont know what the decoder does
    IWICBitmapDecoder* decoder;
    HRESULT hr = imagingfactory->CreateDecoderFromFilename( filename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder );

    //Not sure about this either, but i think maybe i understand
    IWICBitmapFrameDecode* frame;
    decoder->GetFrame(0, &frame);

    CreateTextureFromWIC( dev, devcon, frame, texture, textureView, maxsize );
}