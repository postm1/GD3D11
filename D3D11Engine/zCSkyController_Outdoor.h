#pragma once
#include "pch.h"
#include "HookedFunctions.h"
#include "zCPolygon.h"
#include "Engine.h"
#include "GothicAPI.h"
//#include "zCWorld.h"
#include "zCObject.h"

class zCSkyPlanet {
public:
    int vtbl;
    void* mesh; // 0
    XMFLOAT4 color0; // 4 
    XMFLOAT4 color1; // 20
    float		size; // 36
    XMFLOAT3 pos; // 40
    XMFLOAT3 rotAxis; // 52
};

enum zESkyLayerMode {
    zSKY_LAYER_MODE_POLY,
    zSKY_LAYER_MODE_BOX
};

class zCSkyLayerData {
public:
    zESkyLayerMode SkyMode;
    zCTexture* Tex;
    char zSTring_TexName[20];

    float TexAlpha;
    float TexScale;
    XMFLOAT2 TexSpeed;
};

class zCSkyState {
public:
    float Time;
    XMFLOAT3	PolyColor;
    XMFLOAT3	FogColor;
    XMFLOAT3	DomeColor1;
    XMFLOAT3	DomeColor0;
    float FogDist;
    int	SunOn;
    int	CloudShadowOn;
    zCSkyLayerData	Layer[2];
};

enum zTWeather {
    zTWEATHER_SNOW,
    zTWEATHER_RAIN
};

class zCSkyLayer;

typedef void( __thiscall* zCSkyControllerRenderSkyPre )(void*);
typedef void( __thiscall* zCSkyControllerRenderSkyPost )(void*, int);
class zCSkyController : public zCObject {
public:
    void RenderSkyPre() {
        DWORD* vtbl = reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(this));

        zCSkyControllerRenderSkyPre fn = reinterpret_cast<zCSkyControllerRenderSkyPre>(vtbl[GothicMemoryLocations::zCSkyController::VTBL_RenderSkyPre]);
        fn( this );
    }

    void RenderSkyPost() {
        DWORD* vtbl = reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(this));

        zCSkyControllerRenderSkyPost fn = reinterpret_cast<zCSkyControllerRenderSkyPost>(vtbl[GothicMemoryLocations::zCSkyController::VTBL_RenderSkyPost]);
        fn( this, 1 );
    }

    DWORD* PolyLightCLUTPtr;
    float cloudShadowScale;
    BOOL ColorChanged;
    zTWeather Weather;
};

// Controler - Typo in Gothic
class zCSkyController_Outdoor : public zCSkyController {
public:
    /** Hooks the functions of this Class */
    static void Hook() {
        // Overwrite the rain-renderfunction and particle-updates
        DWORD dwProtect;
        if ( VirtualProtect( reinterpret_cast<void*>(GothicMemoryLocations::zCSkyController_Outdoor::LOC_ProcessRainFXNOPStart),
            GothicMemoryLocations::zCSkyController_Outdoor::LOC_ProcessRainFXNOPEnd
            - GothicMemoryLocations::zCSkyController_Outdoor::LOC_ProcessRainFXNOPStart,
            PAGE_EXECUTE_READWRITE, &dwProtect ) ) {

            REPLACE_RANGE( GothicMemoryLocations::zCSkyController_Outdoor::LOC_ProcessRainFXNOPStart, GothicMemoryLocations::zCSkyController_Outdoor::LOC_ProcessRainFXNOPEnd - 1, INST_NOP );
        }

        // Replace the check for the lensflare with nops
        if ( VirtualProtect( reinterpret_cast<void*>(GothicMemoryLocations::zCSkyController_Outdoor::LOC_SunVisibleStart),
            GothicMemoryLocations::zCSkyController_Outdoor::LOC_SunVisibleEnd - GothicMemoryLocations::zCSkyController_Outdoor::LOC_SunVisibleStart,
            PAGE_EXECUTE_READWRITE, &dwProtect ) ) {

            REPLACE_RANGE( GothicMemoryLocations::zCSkyController_Outdoor::LOC_SunVisibleStart, GothicMemoryLocations::zCSkyController_Outdoor::LOC_SunVisibleEnd - 1, INST_NOP );
        }
    }

    /** Updates the rain-weight and sound-effects */
    void ProcessRainFX() {
        reinterpret_cast<void( __fastcall* )( zCSkyController_Outdoor* )>
            ( GothicMemoryLocations::zCSkyController_Outdoor::ProcessRainFX )( this );
    }

    /** Returns the rain-fx weight */
    float GetRainFXWeight() {
        return *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_OutdoorRainFXWeight ));
    }

    /** Returns the currently active weather type */
    zTWeather GetWeatherType() {
#ifdef BUILD_GOTHIC_2_6_fix
        return *reinterpret_cast<zTWeather*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_WeatherType ));
#else
        return zTWeather::zTWEATHER_RAIN;
#endif
    }

    float GetTimeStartRain() {
        return *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_TimeStartRain ));
    }

    void SetTimeStartRain( float timeStartRain ) {
        *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_TimeStartRain )) = timeStartRain;
    }

    float GetTimeStopRain() {
        return *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_TimeStopRain ));
    }

    void SetTimeStopRain( float timeStopRain ) {
        *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_TimeStopRain )) = timeStopRain;
    }

    int GetRenderLighting() {
#ifndef BUILD_GOTHIC_1_08k
        return *reinterpret_cast<int*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_RenderLightning ));
#else
        return 0;
#endif
    }

    void SetRenderLighting( int renderLighting ) {
#ifndef BUILD_GOTHIC_1_08k
        *reinterpret_cast<int*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_RenderLightning )) = renderLighting;
#else
        (void)renderLighting;
#endif
    }

    int GetRainingCounter() {
#ifndef BUILD_GOTHIC_1_08k
        return *reinterpret_cast<int*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_RainingCounter ));
#else
        return 0;
#endif
    }

    void SetRainingCounter( int rainCtr ) {
#ifndef BUILD_GOTHIC_1_08k
        *reinterpret_cast<int*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_RainingCounter )) = rainCtr;
#else
        (void)rainCtr;
#endif
    }

    float GetLastMasterTime() {
        return *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_LastMasterTime ));
    }

    void SetLastMasterTime( float lastMasterTime ) {
        *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_LastMasterTime )) = lastMasterTime;
    }

    /** Returns the master-time wrapped between 0 and 1 */
    float GetMasterTime() {
        return *reinterpret_cast<float*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_MasterTime ));
    }

    int GetUnderwaterFX() {
        return reinterpret_cast<int( __fastcall* )( zCSkyController_Outdoor* )>
            ( GothicMemoryLocations::zCSkyController_Outdoor::GetUnderwaterFX )( this );
    }

    XMFLOAT3 GetOverrideColor() {
#ifndef BUILD_GOTHIC_1_08k
        return *reinterpret_cast<XMFLOAT3*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_OverrideColor ));
#else
        return XMFLOAT3( 0, 0, 0 );
#endif
    }

    bool GetOverrideFlag() {
#ifndef BUILD_GOTHIC_1_08k
        return *reinterpret_cast<int*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_OverrideFlag )) != 0;
#else
        return 0;
#endif
    }

    /** Returns the sun position in world coords */
    XMFLOAT3 GetSunWorldPosition( float timeScale = 1.0f ) {
        //float angle = GetMasterTime() * XM_2PI; // Get mastertime into rad, 0 and 12 are now at the horizon, 18 is in the sky
        //angle += XM_PIDIV2; // 12 is now in the sky, 18 horizon
        float angle;
        if ( timeScale <= -1 ) {
            angle = 4.71375f;
        } else {
            angle = ((GetMasterTime() * timeScale - 0.3f) * 1.25f + 0.5f) * XM_2PI;
        }

        constexpr XMVECTORF32 sunPos = { -60, 0, 100, 0 };
        XMFLOAT3 rotAxis = XMFLOAT3( 1, 0, 0 );

        XMFLOAT3 pos;
        //XMVector3NormalizeEst leads to jumping shadows dueto reduced accuracy in combination with XMStoreFloat3( &LightDir, XMVector3NormalizeEst( XMLoadFloat3( &LightDir ) ) ); but setting this mentioned code line in this comment to non Est does not influence if this active code line before the comment is Est or not
        XMStoreFloat3( &pos, XMVector3TransformNormal( XMVector3Normalize( sunPos ), XMMatrixTranspose( XMLoadFloat4x4( &(HookedFunctions::OriginalFunctions.original_Alg_Rotation3DNRad( rotAxis, -angle )) ) ) ) );

        return pos;
    }

    void SetCameraLocationHint( int hint ) {
        reinterpret_cast<void( __fastcall* )( zCSkyController_Outdoor*, int, int )>
            ( GothicMemoryLocations::zCSkyController_Outdoor::SetCameraLocationHint )( this, 0, hint );
    }

    zCSkyState* GetMasterState() {
        return reinterpret_cast<zCSkyState*>(THISPTR_OFFSET( GothicMemoryLocations::zCSkyController_Outdoor::Offset_MasterState ));
    }
};

class zCMesh;
class zCSkyLayer {
public:
    zCMesh* SkyPolyMesh;
    zCPolygon* SkyPoly;
    XMFLOAT2 SkyTexOffs;
    zCMesh* SkyDomeMesh;
    zESkyLayerMode SkyMode;
};
