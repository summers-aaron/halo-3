// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// x3daudio.h: X3DAudio, the 3D positional audio math library: listener / emitter / cone / distance-curve
// descriptions, the per-emitter DSP settings it computes, and its two entry points.
#ifndef __X3DAUDIO_H__
#define __X3DAUDIO_H__
#include <windef.h>
#if defined(_XBOX)
#include <vectorintrinsics.h>
#endif
#include <d3d9types.h> // D3DVECTOR

// Speaker position bits of a channel mask.
#if !defined(_SPEAKER_POSITIONS_)
#define _SPEAKER_POSITIONS_
#define SPEAKER_FRONT_LEFT 0x00000001
#define SPEAKER_FRONT_RIGHT 0x00000002
#define SPEAKER_FRONT_CENTER 0x00000004
#define SPEAKER_LOW_FREQUENCY 0x00000008
#define SPEAKER_BACK_LEFT 0x00000010
#define SPEAKER_BACK_RIGHT 0x00000020
#define SPEAKER_FRONT_LEFT_OF_CENTER 0x00000040
#define SPEAKER_FRONT_RIGHT_OF_CENTER 0x00000080
#define SPEAKER_BACK_CENTER 0x00000100
#define SPEAKER_SIDE_LEFT 0x00000200
#define SPEAKER_SIDE_RIGHT 0x00000400
#define SPEAKER_TOP_CENTER 0x00000800
#define SPEAKER_TOP_FRONT_LEFT 0x00001000
#define SPEAKER_TOP_FRONT_CENTER 0x00002000
#define SPEAKER_TOP_FRONT_RIGHT 0x00004000
#define SPEAKER_TOP_BACK_LEFT 0x00008000
#define SPEAKER_TOP_BACK_CENTER 0x00010000
#define SPEAKER_TOP_BACK_RIGHT 0x00020000
#define SPEAKER_RESERVED 0x7FFC0000
#define SPEAKER_ALL 0x80000000
#endif

#if !defined(SPEAKER_STEREO)
#define SPEAKER_STEREO (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)
#define SPEAKER_5POINT1 (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)
#endif
#if defined(_XBOX)
#define SPEAKER_XBOX SPEAKER_5POINT1 // the console always mixes 5.1
#endif

#define X3DAUDIO_HANDLE_BYTESIZE 20
#define X3DAUDIO_PI 3.141592654f
#define X3DAUDIO_2PI 6.283185307f
#define X3DAUDIO_SPEED_OF_SOUND 343.5f // world units per second (metres)

// X3DAudioCalculate: which DSP settings to compute.
#define X3DAUDIO_CALCULATE_MATRIX 0x00000001
#define X3DAUDIO_CALCULATE_DELAY 0x00000002
#define X3DAUDIO_CALCULATE_LPF_DIRECT 0x00000004
#define X3DAUDIO_CALCULATE_LPF_REVERB 0x00000008
#define X3DAUDIO_CALCULATE_REVERB 0x00000010
#define X3DAUDIO_CALCULATE_DOPPLER 0x00000020
#define X3DAUDIO_CALCULATE_EMITTER_ANGLE 0x00000040
#define X3DAUDIO_CALCULATE_ZEROCENTER 0x00010000 // leave the center channel silent

// The Xbox library is linked statically.
#if defined(_XBOX) || defined(X3DAUDIOSTATIC)
#define X3DAUDIO_API_(type) EXTERN_C type STDAPICALLTYPE
#elif defined(X3DEXPORT)
#define X3DAUDIO_API_(type) EXTERN_C __declspec(dllexport) type STDAPICALLTYPE
#else
#define X3DAUDIO_API_(type) EXTERN_C __declspec(dllimport) type STDAPICALLTYPE
#endif
#define X3DAUDIO_IMP_(type) type STDMETHODCALLTYPE

typedef float FLOAT32;
typedef D3DVECTOR X3DAUDIO_VECTOR; // left-handed: x right, y up, z forward
typedef BYTE X3DAUDIO_HANDLE[X3DAUDIO_HANDLE_BYTESIZE];

// A piecewise-linear curve of a DSP setting over normalized distance.
typedef struct X3DAUDIO_DISTANCE_CURVE_POINT
{
    FLOAT32 Distance;   // normalized, [0, 1]
    FLOAT32 DSPSetting;
} X3DAUDIO_DISTANCE_CURVE_POINT, *LPX3DAUDIO_DISTANCE_CURVE_POINT;

typedef struct X3DAUDIO_DISTANCE_CURVE
{
    X3DAUDIO_DISTANCE_CURVE_POINT *pPoints;
    UINT32 PointCount;
} X3DAUDIO_DISTANCE_CURVE, *LPX3DAUDIO_DISTANCE_CURVE;

// Angles in radians; inside the inner angle the inner values apply, outside the outer angle the outer ones.
typedef struct X3DAUDIO_CONE
{
    FLOAT32 InnerAngle;
    FLOAT32 OuterAngle;
    FLOAT32 InnerVolume;
    FLOAT32 OuterVolume;
    FLOAT32 InnerLPF;
    FLOAT32 OuterLPF;
    FLOAT32 InnerReverb;
    FLOAT32 OuterReverb;
} X3DAUDIO_CONE, *LPX3DAUDIO_CONE;

typedef struct X3DAUDIO_LISTENER
{
    X3DAUDIO_VECTOR OrientFront;
    X3DAUDIO_VECTOR OrientTop;
    X3DAUDIO_VECTOR Position;
    X3DAUDIO_VECTOR Velocity;
    X3DAUDIO_CONE *pCone; // null: omnidirectional
} X3DAUDIO_LISTENER, *LPX3DAUDIO_LISTENER;

typedef struct X3DAUDIO_EMITTER
{
    X3DAUDIO_CONE *pCone;
    X3DAUDIO_VECTOR OrientFront;
    X3DAUDIO_VECTOR OrientTop;
    X3DAUDIO_VECTOR Position;
    X3DAUDIO_VECTOR Velocity;
    FLOAT32 InnerRadius;
    FLOAT32 InnerRadiusAngle;
    UINT32 ChannelCount;
    FLOAT32 ChannelRadius;      // multi-channel emitters: channels sit on a circle of this radius
    FLOAT32 *pChannelAzimuths;
    X3DAUDIO_DISTANCE_CURVE *pVolumeCurve;
    X3DAUDIO_DISTANCE_CURVE *pLFECurve;
    X3DAUDIO_DISTANCE_CURVE *pLPFDirectCurve;
    X3DAUDIO_DISTANCE_CURVE *pLPFReverbCurve;
    X3DAUDIO_DISTANCE_CURVE *pReverbCurve;
    FLOAT32 CurveDistanceScaler; // world distance that normalized distance 1 maps to
    FLOAT32 DopplerScaler;
} X3DAUDIO_EMITTER, *LPX3DAUDIO_EMITTER;

// Results; the caller provides the matrix (Src x Dst) and delay arrays.
typedef struct X3DAUDIO_DSP_SETTINGS
{
    FLOAT32 *pMatrixCoefficients;
    FLOAT32 *pDelayTimes;
    UINT32 SrcChannelCount;
    UINT32 DstChannelCount;
    FLOAT32 LPFDirectCoefficient;
    FLOAT32 LPFReverbCoefficient;
    FLOAT32 ReverbLevel;
    FLOAT32 DopplerFactor;
    FLOAT32 EmitterToListenerAngle;
    FLOAT32 EmitterToListenerDistance;
    FLOAT32 EmitterVelocityComponent;
    FLOAT32 ListenerVelocityComponent;
} X3DAUDIO_DSP_SETTINGS, *LPX3DAUDIO_DSP_SETTINGS;

X3DAUDIO_API_(void) X3DAudioInitialize(UINT32 SpeakerChannelMask, FLOAT32 SpeedOfSound, X3DAUDIO_HANDLE Instance);
X3DAUDIO_API_(void) X3DAudioCalculate(const X3DAUDIO_HANDLE Instance, const X3DAUDIO_LISTENER *pListener,
    const X3DAUDIO_EMITTER *pEmitter, UINT32 Flags, X3DAUDIO_DSP_SETTINGS *pDSPSettings);

#endif // __X3DAUDIO_H__
