// dear raingui, v1.83
// (internal structures/api)

// You may use this file to debug, understand or extend RainGui features but we don't provide any guarantee of forward compatibility!
// Set:
//   #define RAINGUI_DEFINE_MATH_OPERATORS
// To implement maths operators for ImVec2 (disabled by default to not collide with using IM_VEC2_CLASS_EXTRA along with your own math types+operators)

/*

Index of this file:

// [SECTION] Header mess
// [SECTION] Forward declarations
// [SECTION] Context pointer
// [SECTION] STB libraries includes
// [SECTION] Macros
// [SECTION] Generic helpers
// [SECTION] ImDrawList support
// [SECTION] Widgets support: flags, enums, data structures
// [SECTION] Columns support
// [SECTION] Multi-select support
// [SECTION] Docking support
// [SECTION] Viewport support
// [SECTION] Settings support
// [SECTION] Metrics, Debug
// [SECTION] Generic context hooks
// [SECTION] RainGuiContext (main raingui context)
// [SECTION] RainGuiWindowTempData, RainGuiWindow
// [SECTION] Tab bar, Tab item support
// [SECTION] Table support
// [SECTION] RainGui internal API
// [SECTION] ImFontAtlas internal API
// [SECTION] Test Engine specific hooks (raingui_test_engine)

*/

#pragma once
#ifndef RAINGUI_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#ifndef RAINGUI_VERSION
#error Must include raingui.h before raingui_internal.h
#endif

#include <stdio.h>      // FILE*, sscanf
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

// Enable SSE intrinsics if available
#if defined __SSE__ || defined __x86_64__ || defined _M_X64
#define RAINGUI_ENABLE_SSE
#include <immintrin.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251)     // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when RAINGUI_API is set to__declspec(dllexport)
#pragma warning (disable: 26812)    // The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3). [MSVC Static Analyzer)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).

#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok, for ImFloorSigned()
#pragma clang diagnostic ignored "-Wunused-function"                // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"             // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"              // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"      // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

// Legacy defines
#ifdef RAINGUI_DISABLE_FORMAT_STRING_FUNCTIONS            // Renamed in 1.74
#error Use RAINGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
#endif
#ifdef RAINGUI_DISABLE_MATH_FUNCTIONS                     // Renamed in 1.74
#error Use RAINGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
#endif

// Enable stb_truetype by default unless FreeType is enabled.
// You can compile with both by defining both RAINGUI_ENABLE_FREETYPE and RAINGUI_ENABLE_STB_TRUETYPE together.
#ifndef RAINGUI_ENABLE_FREETYPE
#define RAINGUI_ENABLE_STB_TRUETYPE
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

struct ImBitVector;                 // Store 1-bit per value
struct ImRect;                      // An axis-aligned rectangle (2 points)
struct ImDrawDataBuilder;           // Helper to build a ImDrawData instance
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct RainGuiColorMod;               // Stacked color modifier, backup of modified data so we can restore it
struct RainGuiContext;                // Main Dear RainGui context
struct RainGuiContextHook;            // Hook for extensions like RainGuiTestEngine
struct RainGuiDataTypeInfo;           // Type information associated to a RainGuiDataType enum
struct RainGuiGroupData;              // Stacked storage data for BeginGroup()/EndGroup()
struct RainGuiInputTextState;         // Internal state of the currently focused/edited text input box
struct RainGuiLastItemDataBackup;     // Backup and restore IsItemHovered() internal data
struct RainGuiMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct RainGuiNavItemData;            // Result of a gamepad/keyboard directional navigation move query result
struct RainGuiMetricsConfig;          // Storage for ShowMetricsWindow() and DebugNodeXXX() functions
struct RainGuiNextWindowData;         // Storage for SetNextWindow** functions
struct RainGuiNextItemData;           // Storage for SetNextItem** functions
struct RainGuiOldColumnData;          // Storage data for a single column for legacy Columns() api
struct RainGuiOldColumns;             // Storage data for a columns set for legacy Columns() api
struct RainGuiPopupData;              // Storage for current popup stack
struct RainGuiSettingsHandler;        // Storage for one type registered in the .ini file
struct RainGuiStackSizes;             // Storage of stack sizes for debugging/asserting
struct RainGuiStyleMod;               // Stacked style modifier, backup of modified data so we can restore it
struct RainGuiTabBar;                 // Storage for a tab bar
struct RainGuiTabItem;                // Storage for a tab item (within a tab bar)
struct RainGuiTable;                  // Storage for a table
struct RainGuiTableColumn;            // Storage for one column of a table
struct RainGuiTableTempData;          // Temporary storage for one table (one per table in the stack), shared between tables.
struct RainGuiTableSettings;          // Storage for a table .ini settings
struct RainGuiTableColumnsSettings;   // Storage for a column .ini settings
struct RainGuiWindow;                 // Storage for one window
struct RainGuiWindowTempData;         // Temporary storage for one window (that's the data which in theory we could ditch at the end of the frame, in practice we currently keep it for each window)
struct RainGuiWindowSettings;         // Storage for a window .ini settings (we keep one of those even if the actual window wasn't instanced during this session)

// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
typedef int RainGuiLayoutType;            // -> enum RainGuiLayoutType_         // Enum: Horizontal or vertical
typedef int RainGuiItemFlags;             // -> enum RainGuiItemFlags_          // Flags: for PushItemFlag()
typedef int RainGuiItemAddFlags;          // -> enum RainGuiItemAddFlags_       // Flags: for ItemAdd()
typedef int RainGuiItemStatusFlags;       // -> enum RainGuiItemStatusFlags_    // Flags: for DC.LastItemStatusFlags
typedef int RainGuiOldColumnFlags;        // -> enum RainGuiOldColumnFlags_     // Flags: for BeginColumns()
typedef int RainGuiNavHighlightFlags;     // -> enum RainGuiNavHighlightFlags_  // Flags: for RenderNavHighlight()
typedef int RainGuiNavDirSourceFlags;     // -> enum RainGuiNavDirSourceFlags_  // Flags: for GetNavInputAmount2d()
typedef int RainGuiNavMoveFlags;          // -> enum RainGuiNavMoveFlags_       // Flags: for navigation requests
typedef int RainGuiNextItemDataFlags;     // -> enum RainGuiNextItemDataFlags_  // Flags: for SetNextItemXXX() functions
typedef int RainGuiNextWindowDataFlags;   // -> enum RainGuiNextWindowDataFlags_// Flags: for SetNextWindowXXX() functions
typedef int RainGuiSeparatorFlags;        // -> enum RainGuiSeparatorFlags_     // Flags: for SeparatorEx()
typedef int RainGuiTextFlags;             // -> enum RainGuiTextFlags_          // Flags: for TextEx()
typedef int RainGuiTooltipFlags;          // -> enum RainGuiTooltipFlags_       // Flags: for BeginTooltipEx()

typedef void (*RainGuiErrorLogCallback)(void* user_data, const char* fmt, ...);

//-----------------------------------------------------------------------------
// [SECTION] Context pointer
// See implementation of this variable in raingui.cpp for comments and details.
//-----------------------------------------------------------------------------

#ifndef GRainGui
extern RAINGUI_API RainGuiContext* GRainGui;  // Current implicit context pointer
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries includes
//-------------------------------------------------------------------------

namespace ImStb
{

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING             RainGuiInputTextState
#define STB_TEXTEDIT_CHARTYPE           ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   (-1.0f)
#define STB_TEXTEDIT_UNDOSTATECOUNT     99
#define STB_TEXTEDIT_UNDOCHARCOUNT      999
#include "imstb_textedit.h"

} // namespace ImStb

//-----------------------------------------------------------------------------
// [SECTION] Macros
//-----------------------------------------------------------------------------

// Debug Logging
#ifndef RAINGUI_DEBUG_LOG
#define RAINGUI_DEBUG_LOG(_FMT,...)       printf("[%05d] " _FMT, GRainGui->FrameCount, __VA_ARGS__)
#endif

// Debug Logging for selected systems. Remove the '((void)0) //' to enable.
//#define RAINGUI_DEBUG_LOG_POPUP         RAINGUI_DEBUG_LOG // Enable log
//#define RAINGUI_DEBUG_LOG_NAV           RAINGUI_DEBUG_LOG // Enable log
#define RAINGUI_DEBUG_LOG_POPUP(...)      ((void)0)       // Disable log
#define RAINGUI_DEBUG_LOG_NAV(...)        ((void)0)       // Disable log

// Static Asserts
#if (__cplusplus >= 201100) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201100)
#define IM_STATIC_ASSERT(_COND)         static_assert(_COND, "")
#else
#define IM_STATIC_ASSERT(_COND)         typedef char static_assertion_##__line__[(_COND)?1:-1]
#endif

// "Paranoid" Debug Asserts are meant to only be enabled during specific debugging/work, otherwise would slow down the code too much.
// We currently don't have many of those so the effect is currently negligible, but onward intent to add more aggressive ones in the code.
//#define RAINGUI_DEBUG_PARANOID
#ifdef RAINGUI_DEBUG_PARANOID
#define IM_ASSERT_PARANOID(_EXPR)       IM_ASSERT(_EXPR)
#else
#define IM_ASSERT_PARANOID(_EXPR)
#endif

// Error handling
// Down the line in some frameworks/languages we would like to have a way to redirect those to the programmer and recover from more faults.
#ifndef IM_ASSERT_USER_ERROR
#define IM_ASSERT_USER_ERROR(_EXP,_MSG) IM_ASSERT((_EXP) && _MSG)   // Recoverable User Error
#endif

// Misc Macros
#define IM_PI                           3.14159265358979323846f
#ifdef _WIN32
#define IM_NEWLINE                      "\r\n"   // Play it nice with Windows users (Update: since 2018-05, Notepad finally appears to support Unix-style carriage returns!)
#else
#define IM_NEWLINE                      "\n"
#endif
#define IM_TABSIZE                      (4)
#define IM_MEMALIGN(_OFF,_ALIGN)        (((_OFF) + (_ALIGN - 1)) & ~(_ALIGN - 1))               // Memory align e.g. IM_ALIGN(0,4)=0, IM_ALIGN(1,4)=4, IM_ALIGN(4,4)=4, IM_ALIGN(5,4)=8
#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255
#define IM_FLOOR(_VAL)                  ((float)(int)(_VAL))                                    // ImFloor() is not inlined in MSVC debug builds
#define IM_ROUND(_VAL)                  ((float)(int)((_VAL) + 0.5f))                           //

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifdef _MSC_VER
#define RAINGUI_CDECL __cdecl
#else
#define RAINGUI_CDECL
#endif

// Warnings
#if defined(_MSC_VER) && !defined(__clang__)
#define IM_MSVC_WARNING_SUPPRESS(XXXX)  __pragma(warning(suppress: XXXX))
#else
#define IM_MSVC_WARNING_SUPPRESS(XXXX)
#endif

// Debug Tools
// Use 'Metrics->Tools->Item Picker' to break into the call-stack of a specific item.
#ifndef IM_DEBUG_BREAK
#if defined(__clang__)
#define IM_DEBUG_BREAK()    __builtin_debugtrap()
#elif defined (_MSC_VER)
#define IM_DEBUG_BREAK()    __debugbreak()
#else
#define IM_DEBUG_BREAK()    IM_ASSERT(0)    // It is expected that you define IM_DEBUG_BREAK() into something that will break nicely in a debugger!
#endif
#endif // #ifndef IM_DEBUG_BREAK

//-----------------------------------------------------------------------------
// [SECTION] Generic helpers
// Note that the ImXXX helpers functions are lower-level than RainGui functions.
// RainGui functions or the RainGui context are never called/used from other ImXXX functions.
//-----------------------------------------------------------------------------
// - Helpers: Hashing
// - Helpers: Sorting
// - Helpers: Bit manipulation
// - Helpers: String, Formatting
// - Helpers: UTF-8 <> wchar conversions
// - Helpers: ImVec2/ImVec4 operators
// - Helpers: Maths
// - Helpers: Geometry
// - Helper: ImVec1
// - Helper: ImVec2ih
// - Helper: ImRect
// - Helper: ImBitArray
// - Helper: ImBitVector
// - Helper: ImSpan<>, ImSpanAllocator<>
// - Helper: ImPool<>
// - Helper: ImChunkStream<>
//-----------------------------------------------------------------------------

// Helpers: Hashing
RAINGUI_API RainGuiID       ImHashData(const void* data, size_t data_size, ImU32 seed = 0);
RAINGUI_API RainGuiID       ImHashStr(const char* data, size_t data_size = 0, ImU32 seed = 0);
#ifndef RAINGUI_DISABLE_OBSOLETE_FUNCTIONS
static inline RainGuiID   ImHash(const void* data, int size, ImU32 seed = 0) { return size ? ImHashData(data, (size_t)size, seed) : ImHashStr((const char*)data, 0, seed); } // [moved to ImHashStr/ImHashData in 1.68]
#endif

// Helpers: Sorting
#define ImQsort         qsort

// Helpers: Color Blending
RAINGUI_API ImU32         ImAlphaBlendColors(ImU32 col_a, ImU32 col_b);

// Helpers: Bit manipulation
static inline bool      ImIsPowerOfTwo(int v)           { return v != 0 && (v & (v - 1)) == 0; }
static inline bool      ImIsPowerOfTwo(ImU64 v)         { return v != 0 && (v & (v - 1)) == 0; }
static inline int       ImUpperPowerOfTwo(int v)        { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }

// Helpers: String, Formatting
RAINGUI_API int           ImStricmp(const char* str1, const char* str2);
RAINGUI_API int           ImStrnicmp(const char* str1, const char* str2, size_t count);
RAINGUI_API void          ImStrncpy(char* dst, const char* src, size_t count);
RAINGUI_API char*         ImStrdup(const char* str);
RAINGUI_API char*         ImStrdupcpy(char* dst, size_t* p_dst_size, const char* str);
RAINGUI_API const char*   ImStrchrRange(const char* str_begin, const char* str_end, char c);
RAINGUI_API int           ImStrlenW(const ImWchar* str);
RAINGUI_API const char*   ImStreolRange(const char* str, const char* str_end);                // End end-of-line
RAINGUI_API const ImWchar*ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin);   // Find beginning-of-line
RAINGUI_API const char*   ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end);
RAINGUI_API void          ImStrTrimBlanks(char* str);
RAINGUI_API const char*   ImStrSkipBlank(const char* str);
RAINGUI_API int           ImFormatString(char* buf, size_t buf_size, const char* fmt, ...) IM_FMTARGS(3);
RAINGUI_API int           ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args) IM_FMTLIST(3);
RAINGUI_API const char*   ImParseFormatFindStart(const char* format);
RAINGUI_API const char*   ImParseFormatFindEnd(const char* format);
RAINGUI_API const char*   ImParseFormatTrimDecorations(const char* format, char* buf, size_t buf_size);
RAINGUI_API int           ImParseFormatPrecision(const char* format, int default_value);
static inline bool      ImCharIsBlankA(char c)          { return c == ' ' || c == '\t'; }
static inline bool      ImCharIsBlankW(unsigned int c)  { return c == ' ' || c == '\t' || c == 0x3000; }

// Helpers: UTF-8 <> wchar conversions
RAINGUI_API int           ImTextStrToUtf8(char* buf, int buf_size, const ImWchar* in_text, const ImWchar* in_text_end);      // return output UTF-8 bytes count
RAINGUI_API int           ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);          // read one character. return input UTF-8 bytes count
RAINGUI_API int           ImTextStrFromUtf8(ImWchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
RAINGUI_API int           ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                            // return number of UTF-8 code-points (NOT bytes count)
RAINGUI_API int           ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end);                        // return number of bytes to express one char in UTF-8
RAINGUI_API int           ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end);                   // return number of bytes to express string in UTF-8

// Helpers: ImVec2/ImVec4 operators
// We are keeping those disabled by default so they don't leak in user space, to allow user enabling implicit cast operators between ImVec2 and their own types (using IM_VEC2_CLASS_EXTRA etc.)
// We unfortunately don't have a unary- operator for ImVec2 because this would needs to be defined inside the class itself.
#ifdef RAINGUI_DEFINE_MATH_OPERATORS
IM_MSVC_RUNTIME_CHECKS_OFF
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)                  { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)                  { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Helpers: File System
#ifdef RAINGUI_DISABLE_FILE_FUNCTIONS
#define RAINGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
typedef void* ImFileHandle;
static inline ImFileHandle  ImFileOpen(const char*, const char*)                    { return NULL; }
static inline bool          ImFileClose(ImFileHandle)                               { return false; }
static inline ImU64         ImFileGetSize(ImFileHandle)                             { return (ImU64)-1; }
static inline ImU64         ImFileRead(void*, ImU64, ImU64, ImFileHandle)           { return 0; }
static inline ImU64         ImFileWrite(const void*, ImU64, ImU64, ImFileHandle)    { return 0; }
#endif
#ifndef RAINGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
typedef FILE* ImFileHandle;
RAINGUI_API ImFileHandle      ImFileOpen(const char* filename, const char* mode);
RAINGUI_API bool              ImFileClose(ImFileHandle file);
RAINGUI_API ImU64             ImFileGetSize(ImFileHandle file);
RAINGUI_API ImU64             ImFileRead(void* data, ImU64 size, ImU64 count, ImFileHandle file);
RAINGUI_API ImU64             ImFileWrite(const void* data, ImU64 size, ImU64 count, ImFileHandle file);
#else
#define RAINGUI_DISABLE_TTY_FUNCTIONS // Can't use stdout, fflush if we are not using default file functions
#endif
RAINGUI_API void*             ImFileLoadToMemory(const char* filename, const char* mode, size_t* out_file_size = NULL, int padding_bytes = 0);

// Helpers: Maths
IM_MSVC_RUNTIME_CHECKS_OFF
// - Wrapper for standard libs functions. (Note that raingui_demo.cpp does _not_ use them to keep the code easy to copy)
#ifndef RAINGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
#define ImFabs(X)           fabsf(X)
#define ImSqrt(X)           sqrtf(X)
#define ImFmod(X, Y)        fmodf((X), (Y))
#define ImCos(X)            cosf(X)
#define ImSin(X)            sinf(X)
#define ImAcos(X)           acosf(X)
#define ImAtan2(Y, X)       atan2f((Y), (X))
#define ImAtof(STR)         atof(STR)
//#define ImFloorStd(X)     floorf(X)           // We use our own, see ImFloor() and ImFloorSigned()
#define ImCeil(X)           ceilf(X)
static inline float  ImPow(float x, float y)    { return powf(x, y); }          // DragBehaviorT/SliderBehaviorT uses ImPow with either float/double and need the precision
static inline double ImPow(double x, double y)  { return pow(x, y); }
static inline float  ImLog(float x)             { return logf(x); }             // DragBehaviorT/SliderBehaviorT uses ImLog with either float/double and need the precision
static inline double ImLog(double x)            { return log(x); }
static inline int    ImAbs(int x)               { return x < 0 ? -x : x; }
static inline float  ImAbs(float x)             { return fabsf(x); }
static inline double ImAbs(double x)            { return fabs(x); }
static inline float  ImSign(float x)            { return (x < 0.0f) ? -1.0f : ((x > 0.0f) ? 1.0f : 0.0f); } // Sign operator - returns -1, 0 or 1 based on sign of argument
static inline double ImSign(double x)           { return (x < 0.0) ? -1.0 : ((x > 0.0) ? 1.0 : 0.0); }
#ifdef RAINGUI_ENABLE_SSE
static inline float  ImRsqrt(float x)           { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static inline float  ImRsqrt(float x)           { return 1.0f / sqrtf(x); }
#endif
static inline double ImRsqrt(double x)          { return 1.0 / sqrt(x); }
#endif
// - ImMin/ImMax/ImClamp/ImLerp/ImSwap are used by widgets which support variety of types: signed/unsigned int/long long float/double
// (Exceptionally using templates here but we could also redefine them for those types)
template<typename T> static inline T ImMin(T lhs, T rhs)                        { return lhs < rhs ? lhs : rhs; }
template<typename T> static inline T ImMax(T lhs, T rhs)                        { return lhs >= rhs ? lhs : rhs; }
template<typename T> static inline T ImClamp(T v, T mn, T mx)                   { return (v < mn) ? mn : (v > mx) ? mx : v; }
template<typename T> static inline T ImLerp(T a, T b, float t)                  { return (T)(a + (b - a) * t); }
template<typename T> static inline void ImSwap(T& a, T& b)                      { T tmp = a; a = b; b = tmp; }
template<typename T> static inline T ImAddClampOverflow(T a, T b, T mn, T mx)   { if (b < 0 && (a < mn - b)) return mn; if (b > 0 && (a > mx - b)) return mx; return a + b; }
template<typename T> static inline T ImSubClampOverflow(T a, T b, T mn, T mx)   { if (b > 0 && (a < mn + b)) return mn; if (b < 0 && (a > mx + b)) return mx; return a - b; }
// - Misc maths helpers
static inline ImVec2 ImMin(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImClamp(const ImVec2& v, const ImVec2& mn, ImVec2 mx)      { return ImVec2((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, float t)          { return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, const ImVec2& t)  { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)          { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImSaturate(float f)                                        { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLengthSqr(const ImVec2& lhs)                             { return (lhs.x * lhs.x) + (lhs.y * lhs.y); }
static inline float  ImLengthSqr(const ImVec4& lhs)                             { return (lhs.x * lhs.x) + (lhs.y * lhs.y) + (lhs.z * lhs.z) + (lhs.w * lhs.w); }
static inline float  ImInvLength(const ImVec2& lhs, float fail_value)           { float d = (lhs.x * lhs.x) + (lhs.y * lhs.y); if (d > 0.0f) return ImRsqrt(d); return fail_value; }
static inline float  ImFloor(float f)                                           { return (float)(int)(f); }
static inline float  ImFloorSigned(float f)                                     { return (float)((f >= 0 || (int)f == f) ? (int)f : (int)f - 1); } // Decent replacement for floorf()
static inline ImVec2 ImFloor(const ImVec2& v)                                   { return ImVec2((float)(int)(v.x), (float)(int)(v.y)); }
static inline int    ImModPositive(int a, int b)                                { return (a + b) % b; }
static inline float  ImDot(const ImVec2& a, const ImVec2& b)                    { return a.x * b.x + a.y * b.y; }
static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)        { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed)    { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline ImVec2 ImMul(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Helpers: Geometry
RAINGUI_API ImVec2     ImBezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t);
RAINGUI_API ImVec2     ImBezierCubicClosestPoint(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, int num_segments);       // For curves with explicit number of segments
RAINGUI_API ImVec2     ImBezierCubicClosestPointCasteljau(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, float tess_tol);// For auto-tessellated curves you can use tess_tol = style.CurveTessellationTol
RAINGUI_API ImVec2     ImBezierQuadraticCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float t);
RAINGUI_API ImVec2     ImLineClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& p);
RAINGUI_API bool       ImTriangleContainsPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
RAINGUI_API ImVec2     ImTriangleClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
RAINGUI_API void       ImTriangleBarycentricCoords(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p, float& out_u, float& out_v, float& out_w);
inline float         ImTriangleArea(const ImVec2& a, const ImVec2& b, const ImVec2& c) { return ImFabs((a.x * (b.y - c.y)) + (b.x * (c.y - a.y)) + (c.x * (a.y - b.y))) * 0.5f; }
RAINGUI_API RainGuiDir   ImGetDirQuadrantFromDelta(float dx, float dy);

// Helper: ImVec1 (1D vector)
// (this odd construct is used to facilitate the transition between 1D and 2D, and the maintenance of some branches/patches)
IM_MSVC_RUNTIME_CHECKS_OFF
struct ImVec1
{
    float   x;
    ImVec1()         { x = 0.0f; }
    ImVec1(float _x) { x = _x; }
};

// Helper: ImVec2ih (2D vector, half-size integer, for long-term packed storage)
struct ImVec2ih
{
    short   x, y;
    ImVec2ih()                           { x = y = 0; }
    ImVec2ih(short _x, short _y)         { x = _x; y = _y; }
    explicit ImVec2ih(const ImVec2& rhs) { x = (short)rhs.x; y = (short)rhs.y; }
};

// Helper: ImRect (2D axis aligned bounding-box)
// NB: we can't rely on ImVec2 math operators being available here!
struct RAINGUI_API ImRect
{
    ImVec2      Min;    // Upper-left
    ImVec2      Max;    // Lower-right

    ImRect()                                        : Min(0.0f, 0.0f), Max(0.0f, 0.0f)  {}
    ImRect(const ImVec2& min, const ImVec2& max)    : Min(min), Max(max)                {}
    ImRect(const ImVec4& v)                         : Min(v.x, v.y), Max(v.z, v.w)      {}
    ImRect(float x1, float y1, float x2, float y2)  : Min(x1, y1), Max(x2, y2)          {}

    ImVec2      GetCenter() const                   { return ImVec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
    ImVec2      GetSize() const                     { return ImVec2(Max.x - Min.x, Max.y - Min.y); }
    float       GetWidth() const                    { return Max.x - Min.x; }
    float       GetHeight() const                   { return Max.y - Min.y; }
    float       GetArea() const                     { return (Max.x - Min.x) * (Max.y - Min.y); }
    ImVec2      GetTL() const                       { return Min; }                   // Top-left
    ImVec2      GetTR() const                       { return ImVec2(Max.x, Min.y); }  // Top-right
    ImVec2      GetBL() const                       { return ImVec2(Min.x, Max.y); }  // Bottom-left
    ImVec2      GetBR() const                       { return Max; }                   // Bottom-right
    bool        Contains(const ImVec2& p) const     { return p.x     >= Min.x && p.y     >= Min.y && p.x     <  Max.x && p.y     <  Max.y; }
    bool        Contains(const ImRect& r) const     { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x <= Max.x && r.Max.y <= Max.y; }
    bool        Overlaps(const ImRect& r) const     { return r.Min.y <  Max.y && r.Max.y >  Min.y && r.Min.x <  Max.x && r.Max.x >  Min.x; }
    void        Add(const ImVec2& p)                { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
    void        Add(const ImRect& r)                { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
    void        Expand(const float amount)          { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
    void        Expand(const ImVec2& amount)        { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
    void        Translate(const ImVec2& d)          { Min.x += d.x; Min.y += d.y; Max.x += d.x; Max.y += d.y; }
    void        TranslateX(float dx)                { Min.x += dx; Max.x += dx; }
    void        TranslateY(float dy)                { Min.y += dy; Max.y += dy; }
    void        ClipWith(const ImRect& r)           { Min = ImMax(Min, r.Min); Max = ImMin(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
    void        ClipWithFull(const ImRect& r)       { Min = ImClamp(Min, r.Min, r.Max); Max = ImClamp(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
    void        Floor()                             { Min.x = IM_FLOOR(Min.x); Min.y = IM_FLOOR(Min.y); Max.x = IM_FLOOR(Max.x); Max.y = IM_FLOOR(Max.y); }
    bool        IsInverted() const                  { return Min.x > Max.x || Min.y > Max.y; }
    ImVec4      ToVec4() const                      { return ImVec4(Min.x, Min.y, Max.x, Max.y); }
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Helper: ImBitArray
inline bool     ImBitArrayTestBit(const ImU32* arr, int n)      { ImU32 mask = (ImU32)1 << (n & 31); return (arr[n >> 5] & mask) != 0; }
inline void     ImBitArrayClearBit(ImU32* arr, int n)           { ImU32 mask = (ImU32)1 << (n & 31); arr[n >> 5] &= ~mask; }
inline void     ImBitArraySetBit(ImU32* arr, int n)             { ImU32 mask = (ImU32)1 << (n & 31); arr[n >> 5] |= mask; }
inline void     ImBitArraySetBitRange(ImU32* arr, int n, int n2) // Works on range [n..n2)
{
    n2--;
    while (n <= n2)
    {
        int a_mod = (n & 31);
        int b_mod = (n2 > (n | 31) ? 31 : (n2 & 31)) + 1;
        ImU32 mask = (ImU32)(((ImU64)1 << b_mod) - 1) & ~(ImU32)(((ImU64)1 << a_mod) - 1);
        arr[n >> 5] |= mask;
        n = (n + 32) & ~31;
    }
}

// Helper: ImBitArray class (wrapper over ImBitArray functions)
// Store 1-bit per value.
template<int BITCOUNT>
struct RAINGUI_API ImBitArray
{
    ImU32           Storage[(BITCOUNT + 31) >> 5];
    ImBitArray()                                { ClearAllBits(); }
    void            ClearAllBits()              { memset(Storage, 0, sizeof(Storage)); }
    void            SetAllBits()                { memset(Storage, 255, sizeof(Storage)); }
    bool            TestBit(int n) const        { IM_ASSERT(n < BITCOUNT); return ImBitArrayTestBit(Storage, n); }
    void            SetBit(int n)               { IM_ASSERT(n < BITCOUNT); ImBitArraySetBit(Storage, n); }
    void            ClearBit(int n)             { IM_ASSERT(n < BITCOUNT); ImBitArrayClearBit(Storage, n); }
    void            SetBitRange(int n, int n2)  { ImBitArraySetBitRange(Storage, n, n2); } // Works on range [n..n2)
};

// Helper: ImBitVector
// Store 1-bit per value.
struct RAINGUI_API ImBitVector
{
    ImVector<ImU32> Storage;
    void            Create(int sz)              { Storage.resize((sz + 31) >> 5); memset(Storage.Data, 0, (size_t)Storage.Size * sizeof(Storage.Data[0])); }
    void            Clear()                     { Storage.clear(); }
    bool            TestBit(int n) const        { IM_ASSERT(n < (Storage.Size << 5)); return ImBitArrayTestBit(Storage.Data, n); }
    void            SetBit(int n)               { IM_ASSERT(n < (Storage.Size << 5)); ImBitArraySetBit(Storage.Data, n); }
    void            ClearBit(int n)             { IM_ASSERT(n < (Storage.Size << 5)); ImBitArrayClearBit(Storage.Data, n); }
};

// Helper: ImSpan<>
// Pointing to a span of data we don't own.
template<typename T>
struct ImSpan
{
    T*                  Data;
    T*                  DataEnd;

    // Constructors, destructor
    inline ImSpan()                                 { Data = DataEnd = NULL; }
    inline ImSpan(T* data, int size)                { Data = data; DataEnd = data + size; }
    inline ImSpan(T* data, T* data_end)             { Data = data; DataEnd = data_end; }

    inline void         set(T* data, int size)      { Data = data; DataEnd = data + size; }
    inline void         set(T* data, T* data_end)   { Data = data; DataEnd = data_end; }
    inline int          size() const                { return (int)(ptrdiff_t)(DataEnd - Data); }
    inline int          size_in_bytes() const       { return (int)(ptrdiff_t)(DataEnd - Data) * (int)sizeof(T); }
    inline T&           operator[](int i)           { T* p = Data + i; IM_ASSERT(p >= Data && p < DataEnd); return *p; }
    inline const T&     operator[](int i) const     { const T* p = Data + i; IM_ASSERT(p >= Data && p < DataEnd); return *p; }

    inline T*           begin()                     { return Data; }
    inline const T*     begin() const               { return Data; }
    inline T*           end()                       { return DataEnd; }
    inline const T*     end() const                 { return DataEnd; }

    // Utilities
    inline int  index_from_ptr(const T* it) const   { IM_ASSERT(it >= Data && it < DataEnd); const ptrdiff_t off = it - Data; return (int)off; }
};

// Helper: ImSpanAllocator<>
// Facilitate storing multiple chunks into a single large block (the "arena")
// - Usage: call Reserve() N times, allocate GetArenaSizeInBytes() worth, pass it to SetArenaBasePtr(), call GetSpan() N times to retrieve the aligned ranges.
template<int CHUNKS>
struct ImSpanAllocator
{
    char*   BasePtr;
    int     CurrOff;
    int     CurrIdx;
    int     Offsets[CHUNKS];
    int     Sizes[CHUNKS];

    ImSpanAllocator()                               { memset(this, 0, sizeof(*this)); }
    inline void  Reserve(int n, size_t sz, int a=4) { IM_ASSERT(n == CurrIdx && n < CHUNKS); CurrOff = IM_MEMALIGN(CurrOff, a); Offsets[n] = CurrOff; Sizes[n] = (int)sz; CurrIdx++; CurrOff += (int)sz; }
    inline int   GetArenaSizeInBytes()              { return CurrOff; }
    inline void  SetArenaBasePtr(void* base_ptr)    { BasePtr = (char*)base_ptr; }
    inline void* GetSpanPtrBegin(int n)             { IM_ASSERT(n >= 0 && n < CHUNKS && CurrIdx == CHUNKS); return (void*)(BasePtr + Offsets[n]); }
    inline void* GetSpanPtrEnd(int n)               { IM_ASSERT(n >= 0 && n < CHUNKS && CurrIdx == CHUNKS); return (void*)(BasePtr + Offsets[n] + Sizes[n]); }
    template<typename T>
    inline void  GetSpan(int n, ImSpan<T>* span)    { span->set((T*)GetSpanPtrBegin(n), (T*)GetSpanPtrEnd(n)); }
};

// Helper: ImPool<>
// Basic keyed storage for contiguous instances, slow/amortized insertion, O(1) indexable, O(Log N) queries by ID over a dense/hot buffer,
// Honor constructor/destructor. Add/remove invalidate all pointers. Indexes have the same lifetime as the associated object.
typedef int ImPoolIdx;
template<typename T>
struct RAINGUI_API ImPool
{
    ImVector<T>     Buf;        // Contiguous data
    RainGuiStorage    Map;        // ID->Index
    ImPoolIdx       FreeIdx;    // Next free idx to use

    ImPool()    { FreeIdx = 0; }
    ~ImPool()   { Clear(); }
    T*          GetByKey(RainGuiID key)               { int idx = Map.GetInt(key, -1); return (idx != -1) ? &Buf[idx] : NULL; }
    T*          GetByIndex(ImPoolIdx n)             { return &Buf[n]; }
    ImPoolIdx   GetIndex(const T* p) const          { IM_ASSERT(p >= Buf.Data && p < Buf.Data + Buf.Size); return (ImPoolIdx)(p - Buf.Data); }
    T*          GetOrAddByKey(RainGuiID key)          { int* p_idx = Map.GetIntRef(key, -1); if (*p_idx != -1) return &Buf[*p_idx]; *p_idx = FreeIdx; return Add(); }
    bool        Contains(const T* p) const          { return (p >= Buf.Data && p < Buf.Data + Buf.Size); }
    void        Clear()                             { for (int n = 0; n < Map.Data.Size; n++) { int idx = Map.Data[n].val_i; if (idx != -1) Buf[idx].~T(); } Map.Clear(); Buf.clear(); FreeIdx = 0; }
    T*          Add()                               { int idx = FreeIdx; if (idx == Buf.Size) { Buf.resize(Buf.Size + 1); FreeIdx++; } else { FreeIdx = *(int*)&Buf[idx]; } IM_PLACEMENT_NEW(&Buf[idx]) T(); return &Buf[idx]; }
    void        Remove(RainGuiID key, const T* p)     { Remove(key, GetIndex(p)); }
    void        Remove(RainGuiID key, ImPoolIdx idx)  { Buf[idx].~T(); *(int*)&Buf[idx] = FreeIdx; FreeIdx = idx; Map.SetInt(key, -1); }
    void        Reserve(int capacity)               { Buf.reserve(capacity); Map.Data.reserve(capacity); }
    int         GetSize() const                     { return Buf.Size; }
};

// Helper: ImChunkStream<>
// Build and iterate a contiguous stream of variable-sized structures.
// This is used by Settings to store persistent data while reducing allocation count.
// We store the chunk size first, and align the final size on 4 bytes boundaries.
// The tedious/zealous amount of casting is to avoid -Wcast-align warnings.
template<typename T>
struct RAINGUI_API ImChunkStream
{
    ImVector<char>  Buf;

    void    clear()                     { Buf.clear(); }
    bool    empty() const               { return Buf.Size == 0; }
    int     size() const                { return Buf.Size; }
    T*      alloc_chunk(size_t sz)      { size_t HDR_SZ = 4; sz = IM_MEMALIGN(HDR_SZ + sz, 4u); int off = Buf.Size; Buf.resize(off + (int)sz); ((int*)(void*)(Buf.Data + off))[0] = (int)sz; return (T*)(void*)(Buf.Data + off + (int)HDR_SZ); }
    T*      begin()                     { size_t HDR_SZ = 4; if (!Buf.Data) return NULL; return (T*)(void*)(Buf.Data + HDR_SZ); }
    T*      next_chunk(T* p)            { size_t HDR_SZ = 4; IM_ASSERT(p >= begin() && p < end()); p = (T*)(void*)((char*)(void*)p + chunk_size(p)); if (p == (T*)(void*)((char*)end() + HDR_SZ)) return (T*)0; IM_ASSERT(p < end()); return p; }
    int     chunk_size(const T* p)      { return ((const int*)p)[-1]; }
    T*      end()                       { return (T*)(void*)(Buf.Data + Buf.Size); }
    int     offset_from_ptr(const T* p) { IM_ASSERT(p >= begin() && p < end()); const ptrdiff_t off = (const char*)p - Buf.Data; return (int)off; }
    T*      ptr_from_offset(int off)    { IM_ASSERT(off >= 4 && off < Buf.Size); return (T*)(void*)(Buf.Data + off); }
    void    swap(ImChunkStream<T>& rhs) { rhs.Buf.swap(Buf); }

};

//-----------------------------------------------------------------------------
// [SECTION] ImDrawList support
//-----------------------------------------------------------------------------

// ImDrawList: Helper function to calculate a circle's segment count given its radius and a "maximum error" value.
// Estimation of number of circle segment based on error is derived using method described in https://stackoverflow.com/a/2244088/15194693
// Number of segments (N) is calculated using equation:
//   N = ceil ( pi / acos(1 - error / r) )     where r > 0, error <= r
// Our equation is significantly simpler that one in the post thanks for choosing segment that is
// perpendicular to X axis. Follow steps in the article from this starting condition and you will
// will get this result.
//
// Rendering circles with an odd number of segments, while mathematically correct will produce
// asymmetrical results on the raster grid. Therefore we're rounding N to next even number (7->8, 8->8, 9->10 etc.)
//
#define IM_ROUNDUP_TO_EVEN(_V)                                  ((((_V) + 1) / 2) * 2)
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MIN                     4
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX                     512
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(_RAD,_MAXERROR)    ImClamp(IM_ROUNDUP_TO_EVEN((int)ImCeil(IM_PI / ImAcos(1 - ImMin((_MAXERROR), (_RAD)) / (_RAD)))), IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MIN, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX)

// Raw equation from IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC rewritten for 'r' and 'error'.
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(_N,_MAXERROR)    ((_MAXERROR) / (1 - ImCos(IM_PI / ImMax((float)(_N), IM_PI))))
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_ERROR(_N,_RAD)     ((1 - ImCos(IM_PI / ImMax((float)(_N), IM_PI))) / (_RAD))

// ImDrawList: Lookup table size for adaptive arc drawing, cover full circle.
#ifndef IM_DRAWLIST_ARCFAST_TABLE_SIZE
#define IM_DRAWLIST_ARCFAST_TABLE_SIZE                          48 // Number of samples in lookup table.
#endif
#define IM_DRAWLIST_ARCFAST_SAMPLE_MAX                          IM_DRAWLIST_ARCFAST_TABLE_SIZE // Sample index _PathArcToFastEx() for 360 angle.

// Data shared between all ImDrawList instances
// You may want to create your own instance of this if you want to use ImDrawList completely without RainGui. In that case, watch out for future changes to this structure.
struct RAINGUI_API ImDrawListSharedData
{
    ImVec2          TexUvWhitePixel;            // UV of white pixel in the atlas
    ImFont*         Font;                       // Current/default font (optional, for simplified AddText overload)
    float           FontSize;                   // Current/default font size (optional, for simplified AddText overload)
    float           CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo()
    float           CircleSegmentMaxError;      // Number of circle segments to use per pixel of radius for AddCircle() etc
    ImVec4          ClipRectFullscreen;         // Value for PushClipRectFullscreen()
    ImDrawListFlags InitialFlags;               // Initial flags at the beginning of the frame (it is possible to alter flags on a per-drawlist basis afterwards)

    // [Internal] Lookup tables
    ImVec2          ArcFastVtx[IM_DRAWLIST_ARCFAST_TABLE_SIZE]; // Sample points on the quarter of the circle.
    float           ArcFastRadiusCutoff;                        // Cutoff radius after which arc drawing will fallback to slower PathArcTo()
    ImU8            CircleSegmentCounts[64];    // Precomputed segment count for given radius before we calculate it dynamically (to avoid calculation overhead)
    const ImVec4*   TexUvLines;                 // UV of anti-aliased lines in the atlas

    ImDrawListSharedData();
    void SetCircleTessellationMaxError(float max_error);
};

struct ImDrawDataBuilder
{
    ImVector<ImDrawList*>   Layers[2];           // Global layers for: regular, tooltip

    void Clear()                    { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].resize(0); }
    void ClearFreeMemory()          { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].clear(); }
    int  GetDrawListCount() const   { int count = 0; for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) count += Layers[n].Size; return count; }
    RAINGUI_API void FlattenIntoSingleLayer();
};

//-----------------------------------------------------------------------------
// [SECTION] Widgets support: flags, enums, data structures
//-----------------------------------------------------------------------------

// Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin().
// This is going to be exposed in raingui.h when stabilized enough.
enum RainGuiItemFlags_
{
    RainGuiItemFlags_None                     = 0,
    RainGuiItemFlags_NoTabStop                = 1 << 0,  // false
    RainGuiItemFlags_ButtonRepeat             = 1 << 1,  // false    // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
    RainGuiItemFlags_Disabled                 = 1 << 2,  // false    // [BETA] Disable interactions but doesn't affect visuals yet. See github.com/ocornut/raingui/issues/211
    RainGuiItemFlags_NoNav                    = 1 << 3,  // false
    RainGuiItemFlags_NoNavDefaultFocus        = 1 << 4,  // false
    RainGuiItemFlags_SelectableDontClosePopup = 1 << 5,  // false    // MenuItem/Selectable() automatically closes current Popup window
    RainGuiItemFlags_MixedValue               = 1 << 6,  // false    // [BETA] Represent a mixed/indeterminate value, generally multi-selection where values differ. Currently only supported by Checkbox() (later should support all sorts of widgets)
    RainGuiItemFlags_ReadOnly                 = 1 << 7   // false    // [ALPHA] Allow hovering interactions but underlying value is not changed.
};

// Flags for ItemAdd()
// FIXME-NAV: _Focusable is _ALMOST_ what you would expect to be called '_TabStop' but because SetKeyboardFocusHere() works on items with no TabStop we distinguish Focusable from TabStop.
enum RainGuiItemAddFlags_
{
    RainGuiItemAddFlags_None                  = 0,
    RainGuiItemAddFlags_Focusable             = 1 << 0    // FIXME-NAV: In current/legacy scheme, Focusable+TabStop support are opt-in by widgets. We will transition it toward being opt-out, so this flag is expected to eventually disappear.
};

// Storage for LastItem data
enum RainGuiItemStatusFlags_
{
    RainGuiItemStatusFlags_None               = 0,
    RainGuiItemStatusFlags_HoveredRect        = 1 << 0,   // Mouse position is within item rectangle (does NOT mean that the window is in correct z-order and can be hovered!, this is only one part of the most-common IsItemHovered test)
    RainGuiItemStatusFlags_HasDisplayRect     = 1 << 1,   // window->DC.LastItemDisplayRect is valid
    RainGuiItemStatusFlags_Edited             = 1 << 2,   // Value exposed by item was edited in the current frame (should match the bool return value of most widgets)
    RainGuiItemStatusFlags_ToggledSelection   = 1 << 3,   // Set when Selectable(), TreeNode() reports toggling a selection. We can't report "Selected", only state changes, in order to easily handle clipping with less issues.
    RainGuiItemStatusFlags_ToggledOpen        = 1 << 4,   // Set when TreeNode() reports toggling their open state.
    RainGuiItemStatusFlags_HasDeactivated     = 1 << 5,   // Set if the widget/group is able to provide data for the RainGuiItemStatusFlags_Deactivated flag.
    RainGuiItemStatusFlags_Deactivated        = 1 << 6,   // Only valid if RainGuiItemStatusFlags_HasDeactivated is set.
    RainGuiItemStatusFlags_HoveredWindow      = 1 << 7,   // Override the HoveredWindow test to allow cross-window hover testing.
    RainGuiItemStatusFlags_FocusedByCode      = 1 << 8,   // Set when the Focusable item just got focused from code.
    RainGuiItemStatusFlags_FocusedByTabbing   = 1 << 9,   // Set when the Focusable item just got focused by Tabbing.
    RainGuiItemStatusFlags_Focused            = RainGuiItemStatusFlags_FocusedByCode | RainGuiItemStatusFlags_FocusedByTabbing

#ifdef RAINGUI_ENABLE_TEST_ENGINE
    , // [raingui_tests only]
    RainGuiItemStatusFlags_Openable           = 1 << 20,  //
    RainGuiItemStatusFlags_Opened             = 1 << 21,  //
    RainGuiItemStatusFlags_Checkable          = 1 << 22,  //
    RainGuiItemStatusFlags_Checked            = 1 << 23   //
#endif
};

// Extend RainGuiInputTextFlags_
enum RainGuiInputTextFlagsPrivate_
{
    // [Internal]
    RainGuiInputTextFlags_Multiline           = 1 << 26,  // For internal use by InputTextMultiline()
    RainGuiInputTextFlags_NoMarkEdited        = 1 << 27,  // For internal use by functions using InputText() before reformatting data
    RainGuiInputTextFlags_MergedItem          = 1 << 28   // For internal use by TempInputText(), will skip calling ItemAdd(). Require bounding-box to strictly match.
};

// Extend RainGuiButtonFlags_
enum RainGuiButtonFlagsPrivate_
{
    RainGuiButtonFlags_PressedOnClick         = 1 << 4,   // return true on click (mouse down event)
    RainGuiButtonFlags_PressedOnClickRelease  = 1 << 5,   // [Default] return true on click + release on same item <-- this is what the majority of Button are using
    RainGuiButtonFlags_PressedOnClickReleaseAnywhere = 1 << 6, // return true on click + release even if the release event is not done while hovering the item
    RainGuiButtonFlags_PressedOnRelease       = 1 << 7,   // return true on release (default requires click+release)
    RainGuiButtonFlags_PressedOnDoubleClick   = 1 << 8,   // return true on double-click (default requires click+release)
    RainGuiButtonFlags_PressedOnDragDropHold  = 1 << 9,   // return true when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
    RainGuiButtonFlags_Repeat                 = 1 << 10,  // hold to repeat
    RainGuiButtonFlags_FlattenChildren        = 1 << 11,  // allow interactions even if a child window is overlapping
    RainGuiButtonFlags_AllowItemOverlap       = 1 << 12,  // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
    RainGuiButtonFlags_DontClosePopups        = 1 << 13,  // disable automatically closing parent popup on press // [UNUSED]
    RainGuiButtonFlags_Disabled               = 1 << 14,  // disable interactions
    RainGuiButtonFlags_AlignTextBaseLine      = 1 << 15,  // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
    RainGuiButtonFlags_NoKeyModifiers         = 1 << 16,  // disable mouse interaction if a key modifier is held
    RainGuiButtonFlags_NoHoldingActiveId      = 1 << 17,  // don't set ActiveId while holding the mouse (RainGuiButtonFlags_PressedOnClick only)
    RainGuiButtonFlags_NoNavFocus             = 1 << 18,  // don't override navigation focus when activated
    RainGuiButtonFlags_NoHoveredOnFocus       = 1 << 19,  // don't report as hovered when nav focus is on this item
    RainGuiButtonFlags_PressedOnMask_         = RainGuiButtonFlags_PressedOnClick | RainGuiButtonFlags_PressedOnClickRelease | RainGuiButtonFlags_PressedOnClickReleaseAnywhere | RainGuiButtonFlags_PressedOnRelease | RainGuiButtonFlags_PressedOnDoubleClick | RainGuiButtonFlags_PressedOnDragDropHold,
    RainGuiButtonFlags_PressedOnDefault_      = RainGuiButtonFlags_PressedOnClickRelease
};

// Extend RainGuiSliderFlags_
enum RainGuiSliderFlagsPrivate_
{
    RainGuiSliderFlags_Vertical               = 1 << 20,  // Should this slider be orientated vertically?
    RainGuiSliderFlags_ReadOnly               = 1 << 21
};

// Extend RainGuiSelectableFlags_
enum RainGuiSelectableFlagsPrivate_
{
    // NB: need to be in sync with last value of RainGuiSelectableFlags_
    RainGuiSelectableFlags_NoHoldingActiveID      = 1 << 20,
    RainGuiSelectableFlags_SelectOnClick          = 1 << 21,  // Override button behavior to react on Click (default is Click+Release)
    RainGuiSelectableFlags_SelectOnRelease        = 1 << 22,  // Override button behavior to react on Release (default is Click+Release)
    RainGuiSelectableFlags_SpanAvailWidth         = 1 << 23,  // Span all avail width even if we declared less for layout purpose. FIXME: We may be able to remove this (added in 6251d379, 2bcafc86 for menus)
    RainGuiSelectableFlags_DrawHoveredWhenHeld    = 1 << 24,  // Always show active when held, even is not hovered. This concept could probably be renamed/formalized somehow.
    RainGuiSelectableFlags_SetNavIdOnHover        = 1 << 25,  // Set Nav/Focus ID on mouse hover (used by MenuItem)
    RainGuiSelectableFlags_NoPadWithHalfSpacing   = 1 << 26   // Disable padding each side with ItemSpacing * 0.5f
};

// Extend RainGuiTreeNodeFlags_
enum RainGuiTreeNodeFlagsPrivate_
{
    RainGuiTreeNodeFlags_ClipLabelForTrailingButton = 1 << 20
};

enum RainGuiSeparatorFlags_
{
    RainGuiSeparatorFlags_None                = 0,
    RainGuiSeparatorFlags_Horizontal          = 1 << 0,   // Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
    RainGuiSeparatorFlags_Vertical            = 1 << 1,
    RainGuiSeparatorFlags_SpanAllColumns      = 1 << 2
};

enum RainGuiTextFlags_
{
    RainGuiTextFlags_None = 0,
    RainGuiTextFlags_NoWidthForLargeClippedText = 1 << 0
};

enum RainGuiTooltipFlags_
{
    RainGuiTooltipFlags_None = 0,
    RainGuiTooltipFlags_OverridePreviousTooltip = 1 << 0      // Override will clear/ignore previously submitted tooltip (defaults to append)
};

// FIXME: this is in development, not exposed/functional as a generic feature yet.
// Horizontal/Vertical enums are fixed to 0/1 so they may be used to index ImVec2
enum RainGuiLayoutType_
{
    RainGuiLayoutType_Horizontal = 0,
    RainGuiLayoutType_Vertical = 1
};

enum RainGuiLogType
{
    RainGuiLogType_None = 0,
    RainGuiLogType_TTY,
    RainGuiLogType_File,
    RainGuiLogType_Buffer,
    RainGuiLogType_Clipboard
};

// X/Y enums are fixed to 0/1 so they may be used to index ImVec2
enum RainGuiAxis
{
    RainGuiAxis_None = -1,
    RainGuiAxis_X = 0,
    RainGuiAxis_Y = 1
};

enum RainGuiPlotType
{
    RainGuiPlotType_Lines,
    RainGuiPlotType_Histogram
};

enum RainGuiInputSource
{
    RainGuiInputSource_None = 0,
    RainGuiInputSource_Mouse,
    RainGuiInputSource_Keyboard,
    RainGuiInputSource_Gamepad,
    RainGuiInputSource_Nav,               // Stored in g.ActiveIdSource only
    RainGuiInputSource_Clipboard,         // Currently only used by InputText()
    RainGuiInputSource_COUNT
};

// FIXME-NAV: Clarify/expose various repeat delay/rate
enum RainGuiInputReadMode
{
    RainGuiInputReadMode_Down,
    RainGuiInputReadMode_Pressed,
    RainGuiInputReadMode_Released,
    RainGuiInputReadMode_Repeat,
    RainGuiInputReadMode_RepeatSlow,
    RainGuiInputReadMode_RepeatFast
};

enum RainGuiNavHighlightFlags_
{
    RainGuiNavHighlightFlags_None         = 0,
    RainGuiNavHighlightFlags_TypeDefault  = 1 << 0,
    RainGuiNavHighlightFlags_TypeThin     = 1 << 1,
    RainGuiNavHighlightFlags_AlwaysDraw   = 1 << 2,       // Draw rectangular highlight if (g.NavId == id) _even_ when using the mouse.
    RainGuiNavHighlightFlags_NoRounding   = 1 << 3
};

enum RainGuiNavDirSourceFlags_
{
    RainGuiNavDirSourceFlags_None         = 0,
    RainGuiNavDirSourceFlags_Keyboard     = 1 << 0,
    RainGuiNavDirSourceFlags_PadDPad      = 1 << 1,
    RainGuiNavDirSourceFlags_PadLStick    = 1 << 2
};

enum RainGuiNavMoveFlags_
{
    RainGuiNavMoveFlags_None                  = 0,
    RainGuiNavMoveFlags_LoopX                 = 1 << 0,   // On failed request, restart from opposite side
    RainGuiNavMoveFlags_LoopY                 = 1 << 1,
    RainGuiNavMoveFlags_WrapX                 = 1 << 2,   // On failed request, request from opposite side one line down (when NavDir==right) or one line up (when NavDir==left)
    RainGuiNavMoveFlags_WrapY                 = 1 << 3,   // This is not super useful for provided for completeness
    RainGuiNavMoveFlags_AllowCurrentNavId     = 1 << 4,   // Allow scoring and considering the current NavId as a move target candidate. This is used when the move source is offset (e.g. pressing PageDown actually needs to send a Up move request, if we are pressing PageDown from the bottom-most item we need to stay in place)
    RainGuiNavMoveFlags_AlsoScoreVisibleSet   = 1 << 5,   // Store alternate result in NavMoveResultLocalVisibleSet that only comprise elements that are already fully visible.
    RainGuiNavMoveFlags_ScrollToEdge          = 1 << 6
};

enum RainGuiNavForward
{
    RainGuiNavForward_None,
    RainGuiNavForward_ForwardQueued,
    RainGuiNavForward_ForwardActive
};

enum RainGuiNavLayer
{
    RainGuiNavLayer_Main  = 0,    // Main scrolling layer
    RainGuiNavLayer_Menu  = 1,    // Menu layer (access with Alt/RainGuiNavInput_Menu)
    RainGuiNavLayer_COUNT
};

enum RainGuiPopupPositionPolicy
{
    RainGuiPopupPositionPolicy_Default,
    RainGuiPopupPositionPolicy_ComboBox,
    RainGuiPopupPositionPolicy_Tooltip
};

struct RainGuiDataTypeTempStorage
{
    ImU8        Data[8];        // Can fit any data up to RainGuiDataType_COUNT
};

// Type information associated to one RainGuiDataType. Retrieve with DataTypeGetInfo().
struct RainGuiDataTypeInfo
{
    size_t      Size;           // Size in bytes
    const char* Name;           // Short descriptive name for the type, for debugging
    const char* PrintFmt;       // Default printf format for the type
    const char* ScanFmt;        // Default scanf format for the type
};

// Extend RainGuiDataType_
enum RainGuiDataTypePrivate_
{
    RainGuiDataType_String = RainGuiDataType_COUNT + 1,
    RainGuiDataType_Pointer,
    RainGuiDataType_ID
};

// Stacked color modifier, backup of modified data so we can restore it
struct RainGuiColorMod
{
    RainGuiCol    Col;
    ImVec4      BackupValue;
};

// Stacked style modifier, backup of modified data so we can restore it. Data type inferred from the variable.
struct RainGuiStyleMod
{
    RainGuiStyleVar   VarIdx;
    union           { int BackupInt[2]; float BackupFloat[2]; };
    RainGuiStyleMod(RainGuiStyleVar idx, int v)     { VarIdx = idx; BackupInt[0] = v; }
    RainGuiStyleMod(RainGuiStyleVar idx, float v)   { VarIdx = idx; BackupFloat[0] = v; }
    RainGuiStyleMod(RainGuiStyleVar idx, ImVec2 v)  { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Stacked storage data for BeginGroup()/EndGroup()
struct RAINGUI_API RainGuiGroupData
{
    RainGuiID     WindowID;
    ImVec2      BackupCursorPos;
    ImVec2      BackupCursorMaxPos;
    ImVec1      BackupIndent;
    ImVec1      BackupGroupOffset;
    ImVec2      BackupCurrLineSize;
    float       BackupCurrLineTextBaseOffset;
    RainGuiID     BackupActiveIdIsAlive;
    bool        BackupActiveIdPreviousFrameIsAlive;
    bool        BackupHoveredIdIsAlive;
    bool        EmitItem;
};

// Simple column measurement, currently used for MenuItem() only.. This is very short-sighted/throw-away code and NOT a generic helper.
struct RAINGUI_API RainGuiMenuColumns
{
    float       Spacing;
    float       Width, NextWidth;
    float       Pos[3], NextWidths[3];

    RainGuiMenuColumns() { memset(this, 0, sizeof(*this)); }
    void        Update(int count, float spacing, bool clear);
    float       DeclColumns(float w0, float w1, float w2);
    float       CalcExtraSpace(float avail_w) const;
};

// Internal state of the currently focused/edited text input box
// For a given item ID, access with RainGui::GetInputTextState()
struct RAINGUI_API RainGuiInputTextState
{
    RainGuiID                 ID;                     // widget id owning the text state
    int                     CurLenW, CurLenA;       // we need to maintain our buffer length in both UTF-8 and wchar format. UTF-8 length is valid even if TextA is not.
    ImVector<ImWchar>       TextW;                  // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>          TextA;                  // temporary UTF8 buffer for callbacks and other operations. this is not updated in every code-path! size=capacity.
    ImVector<char>          InitialTextA;           // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
    bool                    TextAIsValid;           // temporary UTF8 buffer is not initially valid before we make the widget active (until then we pull the data from user argument)
    int                     BufCapacityA;           // end-user buffer capacity
    float                   ScrollX;                // horizontal scrolling/offset
    ImStb::STB_TexteditState Stb;                   // state for stb_textedit.h
    float                   CursorAnim;             // timer for cursor blink, reset on every user action so the cursor reappears immediately
    bool                    CursorFollow;           // set when we want scrolling to follow the current cursor position (not always!)
    bool                    SelectedAllMouseLock;   // after a double-click to select all, we ignore further mouse drags to update selection
    bool                    Edited;                 // edited this frame
    RainGuiInputTextFlags     Flags;                  // copy of InputText() flags
    RainGuiInputTextCallback  UserCallback;           // "
    void*                   UserCallbackData;       // "

    RainGuiInputTextState()                   { memset(this, 0, sizeof(*this)); }
    void        ClearText()                 { CurLenW = CurLenA = 0; TextW[0] = 0; TextA[0] = 0; CursorClamp(); }
    void        ClearFreeMemory()           { TextW.clear(); TextA.clear(); InitialTextA.clear(); }
    int         GetUndoAvailCount() const   { return Stb.undostate.undo_point; }
    int         GetRedoAvailCount() const   { return STB_TEXTEDIT_UNDOSTATECOUNT - Stb.undostate.redo_point; }
    void        OnKeyPressed(int key);      // Cannot be inline because we call in code in stb_textedit.h implementation

    // Cursor & Selection
    void        CursorAnimReset()           { CursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
    void        CursorClamp()               { Stb.cursor = ImMin(Stb.cursor, CurLenW); Stb.select_start = ImMin(Stb.select_start, CurLenW); Stb.select_end = ImMin(Stb.select_end, CurLenW); }
    bool        HasSelection() const        { return Stb.select_start != Stb.select_end; }
    void        ClearSelection()            { Stb.select_start = Stb.select_end = Stb.cursor; }
    void        SelectAll()                 { Stb.select_start = 0; Stb.cursor = Stb.select_end = CurLenW; Stb.has_preferred_x = 0; }
};

// Storage for current popup stack
struct RainGuiPopupData
{
    RainGuiID             PopupId;        // Set on OpenPopup()
    RainGuiWindow*        Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    RainGuiWindow*        SourceWindow;   // Set on OpenPopup() copy of NavWindow at the time of opening the popup
    int                 OpenFrameCount; // Set on OpenPopup()
    RainGuiID             OpenParentId;   // Set on OpenPopup(), we need this to differentiate multiple menu sets from each others (e.g. inside menu bar vs loose menu items)
    ImVec2              OpenPopupPos;   // Set on OpenPopup(), preferred popup position (typically == OpenMousePos when using mouse)
    ImVec2              OpenMousePos;   // Set on OpenPopup(), copy of mouse position at the time of opening popup

    RainGuiPopupData()    { memset(this, 0, sizeof(*this)); OpenFrameCount = -1; }
};

struct RainGuiNavItemData
{
    RainGuiWindow*        Window;         // Init,Move    // Best candidate window (result->ItemWindow->RootWindowForNav == request->Window)
    RainGuiID             ID;             // Init,Move    // Best candidate item ID
    RainGuiID             FocusScopeId;   // Init,Move    // Best candidate focus scope ID
    ImRect              RectRel;        // Init,Move    // Best candidate bounding box in window relative space
    float               DistBox;        //      Move    // Best candidate box distance to current NavId
    float               DistCenter;     //      Move    // Best candidate center distance to current NavId
    float               DistAxial;      //      Move    // Best candidate axial distance to current NavId

    RainGuiNavItemData()  { Clear(); }
    void Clear()        { Window = NULL; ID = FocusScopeId = 0; RectRel = ImRect(); DistBox = DistCenter = DistAxial = FLT_MAX; }
};

enum RainGuiNextWindowDataFlags_
{
    RainGuiNextWindowDataFlags_None               = 0,
    RainGuiNextWindowDataFlags_HasPos             = 1 << 0,
    RainGuiNextWindowDataFlags_HasSize            = 1 << 1,
    RainGuiNextWindowDataFlags_HasContentSize     = 1 << 2,
    RainGuiNextWindowDataFlags_HasCollapsed       = 1 << 3,
    RainGuiNextWindowDataFlags_HasSizeConstraint  = 1 << 4,
    RainGuiNextWindowDataFlags_HasFocus           = 1 << 5,
    RainGuiNextWindowDataFlags_HasBgAlpha         = 1 << 6,
    RainGuiNextWindowDataFlags_HasScroll          = 1 << 7
};

// Storage for SetNexWindow** functions
struct RainGuiNextWindowData
{
    RainGuiNextWindowDataFlags    Flags;
    RainGuiCond                   PosCond;
    RainGuiCond                   SizeCond;
    RainGuiCond                   CollapsedCond;
    ImVec2                      PosVal;
    ImVec2                      PosPivotVal;
    ImVec2                      SizeVal;
    ImVec2                      ContentSizeVal;
    ImVec2                      ScrollVal;
    bool                        CollapsedVal;
    ImRect                      SizeConstraintRect;
    RainGuiSizeCallback           SizeCallback;
    void*                       SizeCallbackUserData;
    float                       BgAlphaVal;             // Override background alpha
    ImVec2                      MenuBarOffsetMinVal;    // *Always on* This is not exposed publicly, so we don't clear it.

    RainGuiNextWindowData()       { memset(this, 0, sizeof(*this)); }
    inline void ClearFlags()    { Flags = RainGuiNextWindowDataFlags_None; }
};

enum RainGuiNextItemDataFlags_
{
    RainGuiNextItemDataFlags_None     = 0,
    RainGuiNextItemDataFlags_HasWidth = 1 << 0,
    RainGuiNextItemDataFlags_HasOpen  = 1 << 1
};

struct RainGuiNextItemData
{
    RainGuiNextItemDataFlags      Flags;
    float                       Width;          // Set by SetNextItemWidth()
    RainGuiID                     FocusScopeId;   // Set by SetNextItemMultiSelectData() (!= 0 signify value has been set, so it's an alternate version of HasSelectionData, we don't use Flags for this because they are cleared too early. This is mostly used for debugging)
    RainGuiCond                   OpenCond;
    bool                        OpenVal;        // Set by SetNextItemOpen()

    RainGuiNextItemData()         { memset(this, 0, sizeof(*this)); }
    inline void ClearFlags()    { Flags = RainGuiNextItemDataFlags_None; } // Also cleared manually by ItemAdd()!
};

struct RainGuiShrinkWidthItem
{
    int         Index;
    float       Width;
};

struct RainGuiPtrOrIndex
{
    void*       Ptr;            // Either field can be set, not both. e.g. Dock node tab bars are loose while BeginTabBar() ones are in a pool.
    int         Index;          // Usually index in a main pool.

    RainGuiPtrOrIndex(void* ptr)  { Ptr = ptr; Index = -1; }
    RainGuiPtrOrIndex(int index)  { Ptr = NULL; Index = index; }
};

//-----------------------------------------------------------------------------
// [SECTION] Columns support
//-----------------------------------------------------------------------------

// Flags for internal's BeginColumns(). Prefix using BeginTable() nowadays!
enum RainGuiOldColumnFlags_
{
    RainGuiOldColumnFlags_None                    = 0,
    RainGuiOldColumnFlags_NoBorder                = 1 << 0,   // Disable column dividers
    RainGuiOldColumnFlags_NoResize                = 1 << 1,   // Disable resizing columns when clicking on the dividers
    RainGuiOldColumnFlags_NoPreserveWidths        = 1 << 2,   // Disable column width preservation when adjusting columns
    RainGuiOldColumnFlags_NoForceWithinWindow     = 1 << 3,   // Disable forcing columns to fit within window
    RainGuiOldColumnFlags_GrowParentContentsSize  = 1 << 4    // (WIP) Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.

    // Obsolete names (will be removed)
#ifndef RAINGUI_DISABLE_OBSOLETE_FUNCTIONS
    , RainGuiColumnsFlags_None                    = RainGuiOldColumnFlags_None,
    RainGuiColumnsFlags_NoBorder                  = RainGuiOldColumnFlags_NoBorder,
    RainGuiColumnsFlags_NoResize                  = RainGuiOldColumnFlags_NoResize,
    RainGuiColumnsFlags_NoPreserveWidths          = RainGuiOldColumnFlags_NoPreserveWidths,
    RainGuiColumnsFlags_NoForceWithinWindow       = RainGuiOldColumnFlags_NoForceWithinWindow,
    RainGuiColumnsFlags_GrowParentContentsSize    = RainGuiOldColumnFlags_GrowParentContentsSize
#endif
};

struct RainGuiOldColumnData
{
    float               OffsetNorm;         // Column start offset, normalized 0.0 (far left) -> 1.0 (far right)
    float               OffsetNormBeforeResize;
    RainGuiOldColumnFlags Flags;              // Not exposed
    ImRect              ClipRect;

    RainGuiOldColumnData() { memset(this, 0, sizeof(*this)); }
};

struct RainGuiOldColumns
{
    RainGuiID             ID;
    RainGuiOldColumnFlags Flags;
    bool                IsFirstFrame;
    bool                IsBeingResized;
    int                 Current;
    int                 Count;
    float               OffMinX, OffMaxX;       // Offsets from HostWorkRect.Min.x
    float               LineMinY, LineMaxY;
    float               HostCursorPosY;         // Backup of CursorPos at the time of BeginColumns()
    float               HostCursorMaxPosX;      // Backup of CursorMaxPos at the time of BeginColumns()
    ImRect              HostInitialClipRect;    // Backup of ClipRect at the time of BeginColumns()
    ImRect              HostBackupClipRect;     // Backup of ClipRect during PushColumnsBackground()/PopColumnsBackground()
    ImRect              HostBackupParentWorkRect;//Backup of WorkRect at the time of BeginColumns()
    ImVector<RainGuiOldColumnData> Columns;
    ImDrawListSplitter  Splitter;

    RainGuiOldColumns()   { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Multi-select support
//-----------------------------------------------------------------------------

#ifdef RAINGUI_HAS_MULTI_SELECT
// <this is filled in 'range_select' branch>
#endif // #ifdef RAINGUI_HAS_MULTI_SELECT

//-----------------------------------------------------------------------------
// [SECTION] Docking support
//-----------------------------------------------------------------------------

#ifdef RAINGUI_HAS_DOCK
// <this is filled in 'docking' branch>
#endif // #ifdef RAINGUI_HAS_DOCK

//-----------------------------------------------------------------------------
// [SECTION] Viewport support
//-----------------------------------------------------------------------------

// RainGuiViewport Private/Internals fields (cardinal sin: we are using inheritance!)
// Every instance of RainGuiViewport is in fact a RainGuiViewportP.
struct RainGuiViewportP : public RainGuiViewport
{
    int                 DrawListsLastFrame[2];  // Last frame number the background (0) and foreground (1) draw lists were used
    ImDrawList*         DrawLists[2];           // Convenience background (0) and foreground (1) draw lists. We use them to draw software mouser cursor when io.MouseDrawCursor is set and to draw most debug overlays.
    ImDrawData          DrawDataP;
    ImDrawDataBuilder   DrawDataBuilder;

    ImVec2              WorkOffsetMin;          // Work Area: Offset from Pos to top-left corner of Work Area. Generally (0,0) or (0,+main_menu_bar_height). Work Area is Full Area but without menu-bars/status-bars (so WorkArea always fit inside Pos/Size!)
    ImVec2              WorkOffsetMax;          // Work Area: Offset from Pos+Size to bottom-right corner of Work Area. Generally (0,0) or (0,-status_bar_height).
    ImVec2              BuildWorkOffsetMin;     // Work Area: Offset being built during current frame. Generally >= 0.0f.
    ImVec2              BuildWorkOffsetMax;     // Work Area: Offset being built during current frame. Generally <= 0.0f.

    RainGuiViewportP()    { DrawListsLastFrame[0] = DrawListsLastFrame[1] = -1; DrawLists[0] = DrawLists[1] = NULL; }
    ~RainGuiViewportP()   { if (DrawLists[0]) IM_DELETE(DrawLists[0]); if (DrawLists[1]) IM_DELETE(DrawLists[1]); }

    // Calculate work rect pos/size given a set of offset (we have 1 pair of offset for rect locked from last frame data, and 1 pair for currently building rect)
    ImVec2  CalcWorkRectPos(const ImVec2& off_min) const                            { return ImVec2(Pos.x + off_min.x, Pos.y + off_min.y); }
    ImVec2  CalcWorkRectSize(const ImVec2& off_min, const ImVec2& off_max) const    { return ImVec2(ImMax(0.0f, Size.x - off_min.x + off_max.x), ImMax(0.0f, Size.y - off_min.y + off_max.y)); }
    void    UpdateWorkRect()            { WorkPos = CalcWorkRectPos(WorkOffsetMin); WorkSize = CalcWorkRectSize(WorkOffsetMin, WorkOffsetMax); } // Update public fields

    // Helpers to retrieve ImRect (we don't need to store BuildWorkRect as every access tend to change it, hence the code asymmetry)
    ImRect  GetMainRect() const         { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    ImRect  GetWorkRect() const         { return ImRect(WorkPos.x, WorkPos.y, WorkPos.x + WorkSize.x, WorkPos.y + WorkSize.y); }
    ImRect  GetBuildWorkRect() const    { ImVec2 pos = CalcWorkRectPos(BuildWorkOffsetMin); ImVec2 size = CalcWorkRectSize(BuildWorkOffsetMin, BuildWorkOffsetMax); return ImRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }
};

//-----------------------------------------------------------------------------
// [SECTION] Settings support
//-----------------------------------------------------------------------------

// Windows data saved in raingui.ini file
// Because we never destroy or rename RainGuiWindowSettings, we can store the names in a separate buffer easily.
// (this is designed to be stored in a ImChunkStream buffer, with the variable-length Name following our structure)
struct RainGuiWindowSettings
{
    RainGuiID     ID;
    ImVec2ih    Pos;
    ImVec2ih    Size;
    bool        Collapsed;
    bool        WantApply;      // Set when loaded from .ini data (to enable merging/loading .ini data into an already running context)

    RainGuiWindowSettings()       { memset(this, 0, sizeof(*this)); }
    char* GetName()             { return (char*)(this + 1); }
};

struct RainGuiSettingsHandler
{
    const char* TypeName;       // Short description stored in .ini file. Disallowed characters: '[' ']'
    RainGuiID     TypeHash;       // == ImHashStr(TypeName)
    void        (*ClearAllFn)(RainGuiContext* ctx, RainGuiSettingsHandler* handler);                                // Clear all settings data
    void        (*ReadInitFn)(RainGuiContext* ctx, RainGuiSettingsHandler* handler);                                // Read: Called before reading (in registration order)
    void*       (*ReadOpenFn)(RainGuiContext* ctx, RainGuiSettingsHandler* handler, const char* name);              // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    void        (*ReadLineFn)(RainGuiContext* ctx, RainGuiSettingsHandler* handler, void* entry, const char* line); // Read: Called for every line of text within an ini entry
    void        (*ApplyAllFn)(RainGuiContext* ctx, RainGuiSettingsHandler* handler);                                // Read: Called after reading (in registration order)
    void        (*WriteAllFn)(RainGuiContext* ctx, RainGuiSettingsHandler* handler, RainGuiTextBuffer* out_buf);      // Write: Output every entries into 'out_buf'
    void*       UserData;

    RainGuiSettingsHandler() { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Metrics, Debug
//-----------------------------------------------------------------------------

struct RainGuiMetricsConfig
{
    bool        ShowWindowsRects;
    bool        ShowWindowsBeginOrder;
    bool        ShowTablesRects;
    bool        ShowDrawCmdMesh;
    bool        ShowDrawCmdBoundingBoxes;
    int         ShowWindowsRectsType;
    int         ShowTablesRectsType;

    RainGuiMetricsConfig()
    {
        ShowWindowsRects = false;
        ShowWindowsBeginOrder = false;
        ShowTablesRects = false;
        ShowDrawCmdMesh = true;
        ShowDrawCmdBoundingBoxes = true;
        ShowWindowsRectsType = -1;
        ShowTablesRectsType = -1;
    }
};

struct RAINGUI_API RainGuiStackSizes
{
    short   SizeOfIDStack;
    short   SizeOfColorStack;
    short   SizeOfStyleVarStack;
    short   SizeOfFontStack;
    short   SizeOfFocusScopeStack;
    short   SizeOfGroupStack;
    short   SizeOfBeginPopupStack;

    RainGuiStackSizes() { memset(this, 0, sizeof(*this)); }
    void SetToCurrentState();
    void CompareWithCurrentState();
};

//-----------------------------------------------------------------------------
// [SECTION] Generic context hooks
//-----------------------------------------------------------------------------

typedef void (*RainGuiContextHookCallback)(RainGuiContext* ctx, RainGuiContextHook* hook);
enum RainGuiContextHookType { RainGuiContextHookType_NewFramePre, RainGuiContextHookType_NewFramePost, RainGuiContextHookType_EndFramePre, RainGuiContextHookType_EndFramePost, RainGuiContextHookType_RenderPre, RainGuiContextHookType_RenderPost, RainGuiContextHookType_Shutdown, RainGuiContextHookType_PendingRemoval_ };

struct RainGuiContextHook
{
    RainGuiID                     HookId;     // A unique ID assigned by AddContextHook()
    RainGuiContextHookType        Type;
    RainGuiID                     Owner;
    RainGuiContextHookCallback    Callback;
    void*                       UserData;

    RainGuiContextHook()          { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] RainGuiContext (main raingui context)
//-----------------------------------------------------------------------------

struct RainGuiContext
{
    bool                    Initialized;
    bool                    FontAtlasOwnedByContext;            // IO.Fonts-> is owned by the RainGuiContext and will be destructed along with it.
    RainGuiIO                 IO;
    RainGuiStyle              Style;
    ImFont*                 Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
    float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize(). Text height for current window.
    float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Base text height.
    ImDrawListSharedData    DrawListSharedData;
    double                  Time;
    int                     FrameCount;
    int                     FrameCountEnded;
    int                     FrameCountRendered;
    bool                    WithinFrameScope;                   // Set by NewFrame(), cleared by EndFrame()
    bool                    WithinFrameScopeWithImplicitWindow; // Set by NewFrame(), cleared by EndFrame() when the implicit debug window has been pushed
    bool                    WithinEndChild;                     // Set within EndChild()
    bool                    GcCompactAll;                       // Request full GC
    bool                    TestEngineHookItems;                // Will call test engine hooks: RainGuiTestEngineHook_ItemAdd(), RainGuiTestEngineHook_ItemInfo(), RainGuiTestEngineHook_Log()
    RainGuiID                 TestEngineHookIdInfo;               // Will call test engine hooks: RainGuiTestEngineHook_IdInfo() from GetID()
    void*                   TestEngine;                         // Test engine user data

    // Windows state
    ImVector<RainGuiWindow*>  Windows;                            // Windows, sorted in display order, back to front
    ImVector<RainGuiWindow*>  WindowsFocusOrder;                  // Root windows, sorted in focus order, back to front.
    ImVector<RainGuiWindow*>  WindowsTempSortBuffer;              // Temporary buffer used in EndFrame() to reorder windows so parents are kept before their child
    ImVector<RainGuiWindow*>  CurrentWindowStack;
    RainGuiStorage            WindowsById;                        // Map window's RainGuiID to RainGuiWindow*
    int                     WindowsActiveCount;                 // Number of unique windows submitted by frame
    ImVec2                  WindowsHoverPadding;                // Padding around resizable windows for which hovering on counts as hovering the window == ImMax(style.TouchExtraPadding, WINDOWS_HOVER_PADDING)
    RainGuiWindow*            CurrentWindow;                      // Window being drawn into
    RainGuiWindow*            HoveredWindow;                      // Window the mouse is hovering. Will typically catch mouse inputs.
    RainGuiWindow*            HoveredWindowUnderMovingWindow;     // Hovered window ignoring MovingWindow. Only set if MovingWindow is set.
    RainGuiWindow*            MovingWindow;                       // Track the window we clicked on (in order to preserve focus). The actual window that is moved is generally MovingWindow->RootWindow.
    RainGuiWindow*            WheelingWindow;                     // Track the window we started mouse-wheeling on. Until a timer elapse or mouse has moved, generally keep scrolling the same window even if during the course of scrolling the mouse ends up hovering a child window.
    ImVec2                  WheelingWindowRefMousePos;
    float                   WheelingWindowTimer;

    // Item/widgets state and tracking information
    RainGuiItemFlags          CurrentItemFlags;                   // == g.ItemFlagsStack.back()
    RainGuiID                 HoveredId;                          // Hovered widget, filled during the frame
    RainGuiID                 HoveredIdPreviousFrame;
    bool                    HoveredIdAllowOverlap;
    bool                    HoveredIdUsingMouseWheel;           // Hovered widget will use mouse wheel. Blocks scrolling the underlying window.
    bool                    HoveredIdPreviousFrameUsingMouseWheel;
    bool                    HoveredIdDisabled;                  // At least one widget passed the rect test, but has been discarded by disabled flag or popup inhibit. May be true even if HoveredId == 0.
    float                   HoveredIdTimer;                     // Measure contiguous hovering time
    float                   HoveredIdNotActiveTimer;            // Measure contiguous hovering time where the item has not been active
    RainGuiID                 ActiveId;                           // Active widget
    RainGuiID                 ActiveIdIsAlive;                    // Active widget has been seen this frame (we can't use a bool as the ActiveId may change within the frame)
    float                   ActiveIdTimer;
    bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
    bool                    ActiveIdAllowOverlap;               // Active widget allows another widget to steal active id (generally for overlapping widgets, but not always)
    bool                    ActiveIdNoClearOnFocusLoss;         // Disable losing active id if the active id window gets unfocused.
    bool                    ActiveIdHasBeenPressedBefore;       // Track whether the active id led to a press (this is to allow changing between PressOnClick and PressOnRelease without pressing twice). Used by range_select branch.
    bool                    ActiveIdHasBeenEditedBefore;        // Was the value associated to the widget Edited over the course of the Active state.
    bool                    ActiveIdHasBeenEditedThisFrame;
    bool                    ActiveIdUsingMouseWheel;            // Active widget will want to read mouse wheel. Blocks scrolling the underlying window.
    ImU32                   ActiveIdUsingNavDirMask;            // Active widget will want to read those nav move requests (e.g. can activate a button and move away from it)
    ImU32                   ActiveIdUsingNavInputMask;          // Active widget will want to read those nav inputs.
    ImU64                   ActiveIdUsingKeyInputMask;          // Active widget will want to read those key inputs. When we grow the RainGuiKey enum we'll need to either to order the enum to make useful keys come first, either redesign this into e.g. a small array.
    ImVec2                  ActiveIdClickOffset;                // Clicked offset from upper-left corner, if applicable (currently only set by ButtonBehavior)
    RainGuiWindow*            ActiveIdWindow;
    RainGuiInputSource        ActiveIdSource;                     // Activating with mouse or nav (gamepad/keyboard)
    int                     ActiveIdMouseButton;
    RainGuiID                 ActiveIdPreviousFrame;
    bool                    ActiveIdPreviousFrameIsAlive;
    bool                    ActiveIdPreviousFrameHasBeenEditedBefore;
    RainGuiWindow*            ActiveIdPreviousFrameWindow;
    RainGuiID                 LastActiveId;                       // Store the last non-zero ActiveId, useful for animation.
    float                   LastActiveIdTimer;                  // Store the last non-zero ActiveId timer since the beginning of activation, useful for animation.

    // Next window/item data
    RainGuiNextWindowData     NextWindowData;                     // Storage for SetNextWindow** functions
    RainGuiNextItemData       NextItemData;                       // Storage for SetNextItem** functions

    // Shared stacks
    ImVector<RainGuiColorMod> ColorStack;                         // Stack for PushStyleColor()/PopStyleColor() - inherited by Begin()
    ImVector<RainGuiStyleMod> StyleVarStack;                      // Stack for PushStyleVar()/PopStyleVar() - inherited by Begin()
    ImVector<ImFont*>       FontStack;                          // Stack for PushFont()/PopFont() - inherited by Begin()
    ImVector<RainGuiID>       FocusScopeStack;                    // Stack for PushFocusScope()/PopFocusScope() - not inherited by Begin(), unless child window
    ImVector<RainGuiItemFlags>ItemFlagsStack;                     // Stack for PushItemFlag()/PopItemFlag() - inherited by Begin()
    ImVector<RainGuiGroupData>GroupStack;                         // Stack for BeginGroup()/EndGroup() - not inherited by Begin()
    ImVector<RainGuiPopupData>OpenPopupStack;                     // Which popups are open (persistent)
    ImVector<RainGuiPopupData>BeginPopupStack;                    // Which level of BeginPopup() we are in (reset every frame)

    // Viewports
    ImVector<RainGuiViewportP*> Viewports;                        // Active viewports (Size==1 in 'master' branch). Each viewports hold their copy of ImDrawData.

    // Gamepad/keyboard Navigation
    RainGuiWindow*            NavWindow;                          // Focused window for navigation. Could be called 'FocusWindow'
    RainGuiID                 NavId;                              // Focused item for navigation
    RainGuiID                 NavFocusScopeId;                    // Identify a selection scope (selection code often wants to "clear other items" when landing on an item of the selection set)
    RainGuiID                 NavActivateId;                      // ~~ (g.ActiveId == 0) && IsNavInputPressed(RainGuiNavInput_Activate) ? NavId : 0, also set when calling ActivateItem()
    RainGuiID                 NavActivateDownId;                  // ~~ IsNavInputDown(RainGuiNavInput_Activate) ? NavId : 0
    RainGuiID                 NavActivatePressedId;               // ~~ IsNavInputPressed(RainGuiNavInput_Activate) ? NavId : 0
    RainGuiID                 NavInputId;                         // ~~ IsNavInputPressed(RainGuiNavInput_Input) ? NavId : 0
    RainGuiID                 NavJustTabbedId;                    // Just tabbed to this id.
    RainGuiID                 NavJustMovedToId;                   // Just navigated to this id (result of a successfully MoveRequest).
    RainGuiID                 NavJustMovedToFocusScopeId;         // Just navigated to this focus scope id (result of a successfully MoveRequest).
    RainGuiKeyModFlags        NavJustMovedToKeyMods;
    RainGuiID                 NavNextActivateId;                  // Set by ActivateItem(), queued until next frame.
    RainGuiInputSource        NavInputSource;                     // Keyboard or Gamepad mode? THIS WILL ONLY BE None or NavGamepad or NavKeyboard.
    ImRect                  NavScoringRect;                     // Rectangle used for scoring, in screen space. Based of window->NavRectRel[], modified for directional navigation scoring.
    int                     NavScoringCount;                    // Metrics for debugging
    RainGuiNavLayer           NavLayer;                           // Layer we are navigating on. For now the system is hard-coded for 0=main contents and 1=menu/title bar, may expose layers later.
    int                     NavIdTabCounter;                    // == NavWindow->DC.FocusIdxTabCounter at time of NavId processing
    bool                    NavIdIsAlive;                       // Nav widget has been seen this frame ~~ NavRectRel is valid
    bool                    NavMousePosDirty;                   // When set we will update mouse position if (io.ConfigFlags & RainGuiConfigFlags_NavEnableSetMousePos) if set (NB: this not enabled by default)
    bool                    NavDisableHighlight;                // When user starts using mouse, we hide gamepad/keyboard highlight (NB: but they are still available, which is why NavDisableHighlight isn't always != NavDisableMouseHover)
    bool                    NavDisableMouseHover;               // When user starts using gamepad/keyboard, we hide mouse hovering highlight until mouse is touched again.
    bool                    NavAnyRequest;                      // ~~ NavMoveRequest || NavInitRequest
    bool                    NavInitRequest;                     // Init request for appearing window to select first item
    bool                    NavInitRequestFromMove;
    RainGuiID                 NavInitResultId;                    // Init request result (first item of the window, or one for which SetItemDefaultFocus() was called)
    ImRect                  NavInitResultRectRel;               // Init request result rectangle (relative to parent window)
    bool                    NavMoveRequest;                     // Move request for this frame
    RainGuiNavMoveFlags       NavMoveRequestFlags;
    RainGuiNavForward         NavMoveRequestForward;              // None / ForwardQueued / ForwardActive (this is used to navigate sibling parent menus from a child menu)
    RainGuiKeyModFlags        NavMoveRequestKeyMods;
    RainGuiDir                NavMoveDir, NavMoveDirLast;         // Direction of the move request (left/right/up/down), direction of the previous move request
    RainGuiDir                NavMoveClipDir;                     // FIXME-NAV: Describe the purpose of this better. Might want to rename?
    RainGuiNavItemData        NavMoveResultLocal;                 // Best move request candidate within NavWindow
    RainGuiNavItemData        NavMoveResultLocalVisibleSet;       // Best move request candidate within NavWindow that are mostly visible (when using RainGuiNavMoveFlags_AlsoScoreVisibleSet flag)
    RainGuiNavItemData        NavMoveResultOther;                 // Best move request candidate within NavWindow's flattened hierarchy (when using RainGuiWindowFlags_NavFlattened flag)
    RainGuiWindow*            NavWrapRequestWindow;               // Window which requested trying nav wrap-around.
    RainGuiNavMoveFlags       NavWrapRequestFlags;                // Wrap-around operation flags.

    // Navigation: Windowing (CTRL+TAB for list, or Menu button + keys or directional pads to move/resize)
    RainGuiWindow*            NavWindowingTarget;                 // Target window when doing CTRL+Tab (or Pad Menu + FocusPrev/Next), this window is temporarily displayed top-most!
    RainGuiWindow*            NavWindowingTargetAnim;             // Record of last valid NavWindowingTarget until DimBgRatio and NavWindowingHighlightAlpha becomes 0.0f, so the fade-out can stay on it.
    RainGuiWindow*            NavWindowingListWindow;             // Internal window actually listing the CTRL+Tab contents
    float                   NavWindowingTimer;
    float                   NavWindowingHighlightAlpha;
    bool                    NavWindowingToggleLayer;

    // Legacy Focus/Tabbing system (older than Nav, active even if Nav is disabled, misnamed. FIXME-NAV: This needs a redesign!)
    RainGuiWindow*            TabFocusRequestCurrWindow;          //
    RainGuiWindow*            TabFocusRequestNextWindow;          //
    int                     TabFocusRequestCurrCounterRegular;  // Any item being requested for focus, stored as an index (we on layout to be stable between the frame pressing TAB and the next frame, semi-ouch)
    int                     TabFocusRequestCurrCounterTabStop;  // Tab item being requested for focus, stored as an index
    int                     TabFocusRequestNextCounterRegular;  // Stored for next frame
    int                     TabFocusRequestNextCounterTabStop;  // "
    bool                    TabFocusPressed;                    // Set in NewFrame() when user pressed Tab

    // Render
    float                   DimBgRatio;                         // 0.0..1.0 animation when fading in a dimming background (for modal window and CTRL+TAB list)
    RainGuiMouseCursor        MouseCursor;

    // Drag and Drop
    bool                    DragDropActive;
    bool                    DragDropWithinSource;               // Set when within a BeginDragDropXXX/EndDragDropXXX block for a drag source.
    bool                    DragDropWithinTarget;               // Set when within a BeginDragDropXXX/EndDragDropXXX block for a drag target.
    RainGuiDragDropFlags      DragDropSourceFlags;
    int                     DragDropSourceFrameCount;
    int                     DragDropMouseButton;
    RainGuiPayload            DragDropPayload;
    ImRect                  DragDropTargetRect;                 // Store rectangle of current target candidate (we favor small targets when overlapping)
    RainGuiID                 DragDropTargetId;
    RainGuiDragDropFlags      DragDropAcceptFlags;
    float                   DragDropAcceptIdCurrRectSurface;    // Target item surface (we resolve overlapping targets by prioritizing the smaller surface)
    RainGuiID                 DragDropAcceptIdCurr;               // Target item id (set at the time of accepting the payload)
    RainGuiID                 DragDropAcceptIdPrev;               // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
    int                     DragDropAcceptFrameCount;           // Last time a target expressed a desire to accept the source
    RainGuiID                 DragDropHoldJustPressedId;          // Set when holding a payload just made ButtonBehavior() return a press.
    ImVector<unsigned char> DragDropPayloadBufHeap;             // We don't expose the ImVector<> directly, RainGuiPayload only holds pointer+size
    unsigned char           DragDropPayloadBufLocal[16];        // Local buffer for small payloads

    // Table
    RainGuiTable*                     CurrentTable;
    int                             CurrentTableStackIdx;
    ImPool<RainGuiTable>              Tables;
    ImVector<RainGuiTableTempData>    TablesTempDataStack;
    ImVector<float>                 TablesLastTimeActive;       // Last used timestamp of each tables (SOA, for efficient GC)
    ImVector<ImDrawChannel>         DrawChannelsTempMergeBuffer;

    // Tab bars
    RainGuiTabBar*                    CurrentTabBar;
    ImPool<RainGuiTabBar>             TabBars;
    ImVector<RainGuiPtrOrIndex>       CurrentTabBarStack;
    ImVector<RainGuiShrinkWidthItem>  ShrinkWidthBuffer;

    // Widget state
    ImVec2                  LastValidMousePos;
    RainGuiInputTextState     InputTextState;
    ImFont                  InputTextPasswordFont;
    RainGuiID                 TempInputId;                        // Temporary text input when CTRL+clicking on a slider, etc.
    RainGuiColorEditFlags     ColorEditOptions;                   // Store user options for color edit widgets
    float                   ColorEditLastHue;                   // Backup of last Hue associated to LastColor[3], so we can restore Hue in lossy RGB<>HSV round trips
    float                   ColorEditLastSat;                   // Backup of last Saturation associated to LastColor[3], so we can restore Saturation in lossy RGB<>HSV round trips
    float                   ColorEditLastColor[3];
    ImVec4                  ColorPickerRef;                     // Initial/reference color at the time of opening the color picker.
    float                   SliderCurrentAccum;                 // Accumulated slider delta when using navigation controls.
    bool                    SliderCurrentAccumDirty;            // Has the accumulated slider delta changed since last time we tried to apply it?
    bool                    DragCurrentAccumDirty;
    float                   DragCurrentAccum;                   // Accumulator for dragging modification. Always high-precision, not rounded by end-user precision settings
    float                   DragSpeedDefaultRatio;              // If speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
    float                   ScrollbarClickDeltaToGrabCenter;    // Distance between mouse and center of grab box, normalized in parent space. Use storage?
    int                     TooltipOverrideCount;
    float                   TooltipSlowDelay;                   // Time before slow tooltips appears (FIXME: This is temporary until we merge in tooltip timer+priority work)
    ImVector<char>          ClipboardHandlerData;               // If no custom clipboard handler is defined
    ImVector<RainGuiID>       MenusIdSubmittedThisFrame;          // A list of menu IDs that were rendered at least once

    // Platform support
    ImVec2                  PlatformImePos;                     // Cursor position request & last passed to the OS Input Method Editor
    ImVec2                  PlatformImeLastPos;
    char                    PlatformLocaleDecimalPoint;         // '.' or *localeconv()->decimal_point

    // Settings
    bool                    SettingsLoaded;
    float                   SettingsDirtyTimer;                 // Save .ini Settings to memory when time reaches zero
    RainGuiTextBuffer         SettingsIniData;                    // In memory .ini settings
    ImVector<RainGuiSettingsHandler>      SettingsHandlers;       // List of .ini settings handlers
    ImChunkStream<RainGuiWindowSettings>  SettingsWindows;        // RainGuiWindow .ini settings entries
    ImChunkStream<RainGuiTableSettings>   SettingsTables;         // RainGuiTable .ini settings entries
    ImVector<RainGuiContextHook>          Hooks;                  // Hooks for extensions (e.g. test engine)
    RainGuiID                             HookIdNext;             // Next available HookId

    // Capture/Logging
    bool                    LogEnabled;                         // Currently capturing
    RainGuiLogType            LogType;                            // Capture target
    ImFileHandle            LogFile;                            // If != NULL log to stdout/ file
    RainGuiTextBuffer         LogBuffer;                          // Accumulation buffer when log to clipboard. This is pointer so our GRainGui static constructor doesn't call heap allocators.
    const char*             LogNextPrefix;
    const char*             LogNextSuffix;
    float                   LogLinePosY;
    bool                    LogLineFirstItem;
    int                     LogDepthRef;
    int                     LogDepthToExpand;
    int                     LogDepthToExpandDefault;            // Default/stored value for LogDepthMaxExpand if not specified in the LogXXX function call.

    // Debug Tools
    bool                    DebugItemPickerActive;              // Item picker is active (started with DebugStartItemPicker())
    RainGuiID                 DebugItemPickerBreakId;             // Will call IM_DEBUG_BREAK() when encountering this id
    RainGuiMetricsConfig      DebugMetricsConfig;

    // Misc
    float                   FramerateSecPerFrame[120];          // Calculate estimate of framerate for user over the last 2 seconds.
    int                     FramerateSecPerFrameIdx;
    int                     FramerateSecPerFrameCount;
    float                   FramerateSecPerFrameAccum;
    int                     WantCaptureMouseNextFrame;          // Explicit capture via CaptureKeyboardFromApp()/CaptureMouseFromApp() sets those flags
    int                     WantCaptureKeyboardNextFrame;
    int                     WantTextInputNextFrame;
    char                    TempBuffer[1024 * 3 + 1];           // Temporary text buffer

    RainGuiContext(ImFontAtlas* shared_font_atlas)
    {
        Initialized = false;
        FontAtlasOwnedByContext = shared_font_atlas ? false : true;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        IO.Fonts = shared_font_atlas ? shared_font_atlas : IM_NEW(ImFontAtlas)();
        Time = 0.0f;
        FrameCount = 0;
        FrameCountEnded = FrameCountRendered = -1;
        WithinFrameScope = WithinFrameScopeWithImplicitWindow = WithinEndChild = false;
        GcCompactAll = false;
        TestEngineHookItems = false;
        TestEngineHookIdInfo = 0;
        TestEngine = NULL;

        WindowsActiveCount = 0;
        CurrentWindow = NULL;
        HoveredWindow = NULL;
        HoveredWindowUnderMovingWindow = NULL;
        MovingWindow = NULL;
        WheelingWindow = NULL;
        WheelingWindowTimer = 0.0f;

        CurrentItemFlags = RainGuiItemFlags_None;
        HoveredId = HoveredIdPreviousFrame = 0;
        HoveredIdAllowOverlap = false;
        HoveredIdUsingMouseWheel = HoveredIdPreviousFrameUsingMouseWheel = false;
        HoveredIdDisabled = false;
        HoveredIdTimer = HoveredIdNotActiveTimer = 0.0f;
        ActiveId = 0;
        ActiveIdIsAlive = 0;
        ActiveIdTimer = 0.0f;
        ActiveIdIsJustActivated = false;
        ActiveIdAllowOverlap = false;
        ActiveIdNoClearOnFocusLoss = false;
        ActiveIdHasBeenPressedBefore = false;
        ActiveIdHasBeenEditedBefore = false;
        ActiveIdHasBeenEditedThisFrame = false;
        ActiveIdUsingMouseWheel = false;
        ActiveIdUsingNavDirMask = 0x00;
        ActiveIdUsingNavInputMask = 0x00;
        ActiveIdUsingKeyInputMask = 0x00;
        ActiveIdClickOffset = ImVec2(-1, -1);
        ActiveIdWindow = NULL;
        ActiveIdSource = RainGuiInputSource_None;
        ActiveIdMouseButton = -1;
        ActiveIdPreviousFrame = 0;
        ActiveIdPreviousFrameIsAlive = false;
        ActiveIdPreviousFrameHasBeenEditedBefore = false;
        ActiveIdPreviousFrameWindow = NULL;
        LastActiveId = 0;
        LastActiveIdTimer = 0.0f;

        NavWindow = NULL;
        NavId = NavFocusScopeId = NavActivateId = NavActivateDownId = NavActivatePressedId = NavInputId = 0;
        NavJustTabbedId = NavJustMovedToId = NavJustMovedToFocusScopeId = NavNextActivateId = 0;
        NavJustMovedToKeyMods = RainGuiKeyModFlags_None;
        NavInputSource = RainGuiInputSource_None;
        NavScoringRect = ImRect();
        NavScoringCount = 0;
        NavLayer = RainGuiNavLayer_Main;
        NavIdTabCounter = INT_MAX;
        NavIdIsAlive = false;
        NavMousePosDirty = false;
        NavDisableHighlight = true;
        NavDisableMouseHover = false;
        NavAnyRequest = false;
        NavInitRequest = false;
        NavInitRequestFromMove = false;
        NavInitResultId = 0;
        NavMoveRequest = false;
        NavMoveRequestFlags = RainGuiNavMoveFlags_None;
        NavMoveRequestForward = RainGuiNavForward_None;
        NavMoveRequestKeyMods = RainGuiKeyModFlags_None;
        NavMoveDir = NavMoveDirLast = NavMoveClipDir = RainGuiDir_None;
        NavWrapRequestWindow = NULL;
        NavWrapRequestFlags = RainGuiNavMoveFlags_None;

        NavWindowingTarget = NavWindowingTargetAnim = NavWindowingListWindow = NULL;
        NavWindowingTimer = NavWindowingHighlightAlpha = 0.0f;
        NavWindowingToggleLayer = false;

        TabFocusRequestCurrWindow = TabFocusRequestNextWindow = NULL;
        TabFocusRequestCurrCounterRegular = TabFocusRequestCurrCounterTabStop = INT_MAX;
        TabFocusRequestNextCounterRegular = TabFocusRequestNextCounterTabStop = INT_MAX;
        TabFocusPressed = false;

        DimBgRatio = 0.0f;
        MouseCursor = RainGuiMouseCursor_Arrow;

        DragDropActive = DragDropWithinSource = DragDropWithinTarget = false;
        DragDropSourceFlags = RainGuiDragDropFlags_None;
        DragDropSourceFrameCount = -1;
        DragDropMouseButton = -1;
        DragDropTargetId = 0;
        DragDropAcceptFlags = RainGuiDragDropFlags_None;
        DragDropAcceptIdCurrRectSurface = 0.0f;
        DragDropAcceptIdPrev = DragDropAcceptIdCurr = 0;
        DragDropAcceptFrameCount = -1;
        DragDropHoldJustPressedId = 0;
        memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

        CurrentTable = NULL;
        CurrentTableStackIdx = -1;
        CurrentTabBar = NULL;

        LastValidMousePos = ImVec2(0.0f, 0.0f);
        TempInputId = 0;
        ColorEditOptions = RainGuiColorEditFlags__OptionsDefault;
        ColorEditLastHue = ColorEditLastSat = 0.0f;
        ColorEditLastColor[0] = ColorEditLastColor[1] = ColorEditLastColor[2] = FLT_MAX;
        SliderCurrentAccum = 0.0f;
        SliderCurrentAccumDirty = false;
        DragCurrentAccumDirty = false;
        DragCurrentAccum = 0.0f;
        DragSpeedDefaultRatio = 1.0f / 100.0f;
        ScrollbarClickDeltaToGrabCenter = 0.0f;
        TooltipOverrideCount = 0;
        TooltipSlowDelay = 0.50f;

        PlatformImePos = PlatformImeLastPos = ImVec2(FLT_MAX, FLT_MAX);
        PlatformLocaleDecimalPoint = '.';

        SettingsLoaded = false;
        SettingsDirtyTimer = 0.0f;
        HookIdNext = 0;

        LogEnabled = false;
        LogType = RainGuiLogType_None;
        LogNextPrefix = LogNextSuffix = NULL;
        LogFile = NULL;
        LogLinePosY = FLT_MAX;
        LogLineFirstItem = false;
        LogDepthRef = 0;
        LogDepthToExpand = LogDepthToExpandDefault = 2;

        DebugItemPickerActive = false;
        DebugItemPickerBreakId = 0;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = FramerateSecPerFrameCount = 0;
        FramerateSecPerFrameAccum = 0.0f;
        WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
        memset(TempBuffer, 0, sizeof(TempBuffer));
    }
};

//-----------------------------------------------------------------------------
// [SECTION] RainGuiWindowTempData, RainGuiWindow
//-----------------------------------------------------------------------------

// Transient per-window data, reset at the beginning of the frame. This used to be called RainGuiDrawContext, hence the DC variable name in RainGuiWindow.
// (That's theory, in practice the delimitation between RainGuiWindow and RainGuiWindowTempData is quite tenuous and could be reconsidered..)
// (This doesn't need a constructor because we zero-clear it as part of RainGuiWindow and all frame-temporary data are setup on Begin)
struct RAINGUI_API RainGuiWindowTempData
{
    // Layout
    ImVec2                  CursorPos;              // Current emitting position, in absolute coordinates.
    ImVec2                  CursorPosPrevLine;
    ImVec2                  CursorStartPos;         // Initial position after Begin(), generally ~ window position + WindowPadding.
    ImVec2                  CursorMaxPos;           // Used to implicitly calculate ContentSize at the beginning of next frame, for scrolling range and auto-resize. Always growing during the frame.
    ImVec2                  IdealMaxPos;            // Used to implicitly calculate ContentSizeIdeal at the beginning of next frame, for auto-resize only. Always growing during the frame.
    ImVec2                  CurrLineSize;
    ImVec2                  PrevLineSize;
    float                   CurrLineTextBaseOffset; // Baseline offset (0.0f by default on a new line, generally == style.FramePadding.y when a framed item has been added).
    float                   PrevLineTextBaseOffset;
    ImVec1                  Indent;                 // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
    ImVec1                  ColumnsOffset;          // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
    ImVec1                  GroupOffset;

    // Last item status
    RainGuiID                 LastItemId;             // ID for last item
    RainGuiItemStatusFlags    LastItemStatusFlags;    // Status flags for last item (see RainGuiItemStatusFlags_)
    ImRect                  LastItemRect;           // Interaction rect for last item
    ImRect                  LastItemDisplayRect;    // End-user display rect for last item (only valid if LastItemStatusFlags & RainGuiItemStatusFlags_HasDisplayRect)

    // Keyboard/Gamepad navigation
    RainGuiNavLayer           NavLayerCurrent;        // Current layer, 0..31 (we currently only use 0..1)
    short                   NavLayersActiveMask;    // Which layers have been written to (result from previous frame)
    short                   NavLayersActiveMaskNext;// Which layers have been written to (accumulator for current frame)
    RainGuiID                 NavFocusScopeIdCurrent; // Current focus scope ID while appending
    bool                    NavHideHighlightOneFrame;
    bool                    NavHasScroll;           // Set when scrolling can be used (ScrollMax > 0.0f)

    // Miscellaneous
    bool                    MenuBarAppending;       // FIXME: Remove this
    ImVec2                  MenuBarOffset;          // MenuBarOffset.x is sort of equivalent of a per-layer CursorPos.x, saved/restored as we switch to the menu bar. The only situation when MenuBarOffset.y is > 0 if when (SafeAreaPadding.y > FramePadding.y), often used on TVs.
    RainGuiMenuColumns        MenuColumns;            // Simplified columns storage for menu items measurement
    int                     TreeDepth;              // Current tree depth.
    ImU32                   TreeJumpToParentOnPopMask; // Store a copy of !g.NavIdIsAlive for TreeDepth 0..31.. Could be turned into a ImU64 if necessary.
    ImVector<RainGuiWindow*>  ChildWindows;
    RainGuiStorage*           StateStorage;           // Current persistent per-window storage (store e.g. tree node open/close state)
    RainGuiOldColumns*        CurrentColumns;         // Current columns set
    int                     CurrentTableIdx;        // Current table index (into g.Tables)
    RainGuiLayoutType         LayoutType;
    RainGuiLayoutType         ParentLayoutType;       // Layout type of parent window at the time of Begin()
    int                     FocusCounterRegular;    // (Legacy Focus/Tabbing system) Sequential counter, start at -1 and increase as assigned via FocusableItemRegister() (FIXME-NAV: Needs redesign)
    int                     FocusCounterTabStop;    // (Legacy Focus/Tabbing system) Same, but only count widgets which you can Tab through.

    // Local parameters stacks
    // We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
    float                   ItemWidth;              // Current item width (>0.0: width in pixels, <0.0: align xx pixels to the right of window).
    float                   TextWrapPos;            // Current text wrap pos.
    ImVector<float>         ItemWidthStack;         // Store item widths to restore (attention: .back() is not == ItemWidth)
    ImVector<float>         TextWrapPosStack;       // Store text wrap pos to restore (attention: .back() is not == TextWrapPos)
    RainGuiStackSizes         StackSizesOnBegin;      // Store size of various stacks for asserting
};

// Storage for one window
struct RAINGUI_API RainGuiWindow
{
    char*                   Name;                               // Window name, owned by the window.
    RainGuiID                 ID;                                 // == ImHashStr(Name)
    RainGuiWindowFlags        Flags;                              // See enum RainGuiWindowFlags_
    ImVec2                  Pos;                                // Position (always rounded-up to nearest pixel)
    ImVec2                  Size;                               // Current size (==SizeFull or collapsed title bar size)
    ImVec2                  SizeFull;                           // Size when non collapsed
    ImVec2                  ContentSize;                        // Size of contents/scrollable client area (calculated from the extents reach of the cursor) from previous frame. Does not include window decoration or window padding.
    ImVec2                  ContentSizeIdeal;
    ImVec2                  ContentSizeExplicit;                // Size of contents/scrollable client area explicitly request by the user via SetNextWindowContentSize().
    ImVec2                  WindowPadding;                      // Window padding at the time of Begin().
    float                   WindowRounding;                     // Window rounding at the time of Begin(). May be clamped lower to avoid rendering artifacts with title bar, menu bar etc.
    float                   WindowBorderSize;                   // Window border size at the time of Begin().
    int                     NameBufLen;                         // Size of buffer storing Name. May be larger than strlen(Name)!
    RainGuiID                 MoveId;                             // == window->GetID("#MOVE")
    RainGuiID                 ChildId;                            // ID of corresponding item in parent window (for navigation to return from child window to parent window)
    ImVec2                  Scroll;
    ImVec2                  ScrollMax;
    ImVec2                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    ImVec2                  ScrollTargetCenterRatio;            // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
    ImVec2                  ScrollTargetEdgeSnapDist;           // 0.0f = no snapping, >0.0f snapping threshold
    ImVec2                  ScrollbarSizes;                     // Size taken by each scrollbars on their smaller axis. Pay attention! ScrollbarSizes.x == width of the vertical scrollbar, ScrollbarSizes.y = height of the horizontal scrollbar.
    bool                    ScrollbarX, ScrollbarY;             // Are scrollbars visible?
    bool                    Active;                             // Set to true on Begin(), unless Collapsed
    bool                    WasActive;
    bool                    WriteAccessed;                      // Set to true when any widget access the current window
    bool                    Collapsed;                          // Set when collapsing window to become only title-bar
    bool                    WantCollapseToggle;
    bool                    SkipItems;                          // Set when items can safely be all clipped (e.g. window not visible or collapsed)
    bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
    bool                    Hidden;                             // Do not display (== HiddenFrames*** > 0)
    bool                    IsFallbackWindow;                   // Set on the "Debug##Default" window.
    bool                    HasCloseButton;                     // Set when the window has a close button (p_open != NULL)
    signed char             ResizeBorderHeld;                   // Current border being held for resize (-1: none, otherwise 0-3)
    short                   BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    short                   BeginOrderWithinParent;             // Begin() order within immediate parent window, if we are a child window. Otherwise 0.
    short                   BeginOrderWithinContext;            // Begin() order within entire raingui context. This is mostly used for debugging submission order related issues.
    short                   FocusOrder;                         // Order within WindowsFocusOrder[], altered when windows are focused.
    RainGuiID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
    ImS8                    AutoFitFramesX, AutoFitFramesY;
    ImS8                    AutoFitChildAxises;
    bool                    AutoFitOnlyGrows;
    RainGuiDir                AutoPosLastDirection;
    ImS8                    HiddenFramesCanSkipItems;           // Hide the window for N frames
    ImS8                    HiddenFramesCannotSkipItems;        // Hide the window for N frames while allowing items to be submitted so we can measure their size
    ImS8                    HiddenFramesForRenderOnly;          // Hide the window until frame N at Render() time only
    ImS8                    DisableInputsFrames;                // Disable window interactions for N frames
    RainGuiCond               SetWindowPosAllowFlags : 8;         // store acceptable condition flags for SetNextWindowPos() use.
    RainGuiCond               SetWindowSizeAllowFlags : 8;        // store acceptable condition flags for SetNextWindowSize() use.
    RainGuiCond               SetWindowCollapsedAllowFlags : 8;   // store acceptable condition flags for SetNextWindowCollapsed() use.
    ImVec2                  SetWindowPosVal;                    // store window position when using a non-zero Pivot (position set needs to be processed when we know the window size)
    ImVec2                  SetWindowPosPivot;                  // store window pivot for positioning. ImVec2(0, 0) when positioning from top-left corner; ImVec2(0.5f, 0.5f) for centering; ImVec2(1, 1) for bottom right.

    ImVector<RainGuiID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack. (In theory this should be in the TempData structure)
    RainGuiWindowTempData     DC;                                 // Temporary per-window data, reset at the beginning of the frame. This used to be called RainGuiDrawContext, hence the "DC" variable name.

    // The best way to understand what those rectangles are is to use the 'Metrics->Tools->Show Windows Rectangles' viewer.
    // The main 'OuterRect', omitted as a field, is window->Rect().
    ImRect                  OuterRectClipped;                   // == Window->Rect() just after setup in Begin(). == window->Rect() for root window.
    ImRect                  InnerRect;                          // Inner rectangle (omit title bar, menu bar, scroll bar)
    ImRect                  InnerClipRect;                      // == InnerRect shrunk by WindowPadding*0.5f on each side, clipped within viewport or parent clip rect.
    ImRect                  WorkRect;                           // Initially covers the whole scrolling region. Reduced by containers e.g columns/tables when active. Shrunk by WindowPadding*1.0f on each side. This is meant to replace ContentRegionRect over time (from 1.71+ onward).
    ImRect                  ParentWorkRect;                     // Backup of WorkRect before entering a container such as columns/tables. Used by e.g. SpanAllColumns functions to easily access. Stacked containers are responsible for maintaining this. // FIXME-WORKRECT: Could be a stack?
    ImRect                  ClipRect;                           // Current clipping/scissoring rectangle, evolve as we are using PushClipRect(), etc. == DrawList->clip_rect_stack.back().
    ImRect                  ContentRegionRect;                  // FIXME: This is currently confusing/misleading. It is essentially WorkRect but not handling of scrolling. We currently rely on it as right/bottom aligned sizing operation need some size to rely on.
    ImVec2ih                HitTestHoleSize;                    // Define an optional rectangular hole where mouse will pass-through the window.
    ImVec2ih                HitTestHoleOffset;

    int                     LastFrameActive;                    // Last frame number the window was Active.
    float                   LastTimeActive;                     // Last timestamp the window was Active (using float as we don't need high precision there)
    float                   ItemWidthDefault;
    RainGuiStorage            StateStorage;
    ImVector<RainGuiOldColumns> ColumnsStorage;
    float                   FontWindowScale;                    // User scale multiplier per-window, via SetWindowFontScale()
    int                     SettingsOffset;                     // Offset into SettingsWindows[] (offsets are always valid as we only grow the array from the back)

    ImDrawList*             DrawList;                           // == &DrawListInst (for backward compatibility reason with code using raingui_internal.h we keep this a pointer)
    ImDrawList              DrawListInst;
    RainGuiWindow*            ParentWindow;                       // If we are a child _or_ popup window, this is pointing to our parent. Otherwise NULL.
    RainGuiWindow*            RootWindow;                         // Point to ourself or first ancestor that is not a child window == Top-level window.
    RainGuiWindow*            RootWindowForTitleBarHighlight;     // Point to ourself or first ancestor which will display TitleBgActive color when this window is active.
    RainGuiWindow*            RootWindowForNav;                   // Point to ourself or first ancestor which doesn't have the NavFlattened flag.

    RainGuiWindow*            NavLastChildNavWindow;              // When going to the menu bar, we remember the child window we came from. (This could probably be made implicit if we kept g.Windows sorted by last focused including child window.)
    RainGuiID                 NavLastIds[RainGuiNavLayer_COUNT];    // Last known NavId for this window, per layer (0/1)
    ImRect                  NavRectRel[RainGuiNavLayer_COUNT];    // Reference rectangle, in window relative space

    int                     MemoryDrawListIdxCapacity;          // Backup of last idx/vtx count, so when waking up the window we can preallocate and avoid iterative alloc/copy
    int                     MemoryDrawListVtxCapacity;
    bool                    MemoryCompacted;                    // Set when window extraneous data have been garbage collected

public:
    RainGuiWindow(RainGuiContext* context, const char* name);
    ~RainGuiWindow();

    RainGuiID     GetID(const char* str, const char* str_end = NULL);
    RainGuiID     GetID(const void* ptr);
    RainGuiID     GetID(int n);
    RainGuiID     GetIDNoKeepAlive(const char* str, const char* str_end = NULL);
    RainGuiID     GetIDNoKeepAlive(const void* ptr);
    RainGuiID     GetIDNoKeepAlive(int n);
    RainGuiID     GetIDFromRectangle(const ImRect& r_abs);

    // We don't use g.FontSize because the window may be != g.CurrentWidow.
    ImRect      Rect() const            { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    float       CalcFontSize() const    { RainGuiContext& g = *GRainGui; float scale = g.FontBaseSize * FontWindowScale; if (ParentWindow) scale *= ParentWindow->FontWindowScale; return scale; }
    float       TitleBarHeight() const  { RainGuiContext& g = *GRainGui; return (Flags & RainGuiWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + g.Style.FramePadding.y * 2.0f; }
    ImRect      TitleBarRect() const    { return ImRect(Pos, ImVec2(Pos.x + SizeFull.x, Pos.y + TitleBarHeight())); }
    float       MenuBarHeight() const   { RainGuiContext& g = *GRainGui; return (Flags & RainGuiWindowFlags_MenuBar) ? DC.MenuBarOffset.y + CalcFontSize() + g.Style.FramePadding.y * 2.0f : 0.0f; }
    ImRect      MenuBarRect() const     { float y1 = Pos.y + TitleBarHeight(); return ImRect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
};

// Backup and restore just enough data to be able to use IsItemHovered() on item A after another B in the same window has overwritten the data.
struct RainGuiLastItemDataBackup
{
    RainGuiID                 LastItemId;
    RainGuiItemStatusFlags    LastItemStatusFlags;
    ImRect                  LastItemRect;
    ImRect                  LastItemDisplayRect;

    RainGuiLastItemDataBackup() { Backup(); }
    void Backup()           { RainGuiWindow* window = GRainGui->CurrentWindow; LastItemId = window->DC.LastItemId; LastItemStatusFlags = window->DC.LastItemStatusFlags; LastItemRect = window->DC.LastItemRect; LastItemDisplayRect = window->DC.LastItemDisplayRect; }
    void Restore() const    { RainGuiWindow* window = GRainGui->CurrentWindow; window->DC.LastItemId = LastItemId; window->DC.LastItemStatusFlags = LastItemStatusFlags; window->DC.LastItemRect = LastItemRect; window->DC.LastItemDisplayRect = LastItemDisplayRect; }
};

//-----------------------------------------------------------------------------
// [SECTION] Tab bar, Tab item support
//-----------------------------------------------------------------------------

// Extend RainGuiTabBarFlags_
enum RainGuiTabBarFlagsPrivate_
{
    RainGuiTabBarFlags_DockNode                   = 1 << 20,  // Part of a dock node [we don't use this in the master branch but it facilitate branch syncing to keep this around]
    RainGuiTabBarFlags_IsFocused                  = 1 << 21,
    RainGuiTabBarFlags_SaveSettings               = 1 << 22   // FIXME: Settings are handled by the docking system, this only request the tab bar to mark settings dirty when reordering tabs
};

// Extend RainGuiTabItemFlags_
enum RainGuiTabItemFlagsPrivate_
{
    RainGuiTabItemFlags_SectionMask_              = RainGuiTabItemFlags_Leading | RainGuiTabItemFlags_Trailing,
    RainGuiTabItemFlags_NoCloseButton             = 1 << 20,  // Track whether p_open was set or not (we'll need this info on the next frame to recompute ContentWidth during layout)
    RainGuiTabItemFlags_Button                    = 1 << 21   // Used by TabItemButton, change the tab item behavior to mimic a button
};

// Storage for one active tab item (sizeof() 28~32 bytes)
struct RainGuiTabItem
{
    RainGuiID             ID;
    RainGuiTabItemFlags   Flags;
    int                 LastFrameVisible;
    int                 LastFrameSelected;      // This allows us to infer an ordered list of the last activated tabs with little maintenance
    float               Offset;                 // Position relative to beginning of tab
    float               Width;                  // Width currently displayed
    float               ContentWidth;           // Width of label, stored during BeginTabItem() call
    ImS16               NameOffset;             // When Window==NULL, offset to name within parent RainGuiTabBar::TabsNames
    ImS16               BeginOrder;             // BeginTabItem() order, used to re-order tabs after toggling RainGuiTabBarFlags_Reorderable
    ImS16               IndexDuringLayout;      // Index only used during TabBarLayout()
    bool                WantClose;              // Marked as closed by SetTabItemClosed()

    RainGuiTabItem()      { memset(this, 0, sizeof(*this)); LastFrameVisible = LastFrameSelected = -1; NameOffset = BeginOrder = IndexDuringLayout = -1; }
};

// Storage for a tab bar (sizeof() 152 bytes)
struct RainGuiTabBar
{
    ImVector<RainGuiTabItem> Tabs;
    RainGuiTabBarFlags    Flags;
    RainGuiID             ID;                     // Zero for tab-bars used by docking
    RainGuiID             SelectedTabId;          // Selected tab/window
    RainGuiID             NextSelectedTabId;      // Next selected tab/window. Will also trigger a scrolling animation
    RainGuiID             VisibleTabId;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
    int                 CurrFrameVisible;
    int                 PrevFrameVisible;
    ImRect              BarRect;
    float               CurrTabsContentsHeight;
    float               PrevTabsContentsHeight; // Record the height of contents submitted below the tab bar
    float               WidthAllTabs;           // Actual width of all tabs (locked during layout)
    float               WidthAllTabsIdeal;      // Ideal width if all tabs were visible and not clipped
    float               ScrollingAnim;
    float               ScrollingTarget;
    float               ScrollingTargetDistToVisibility;
    float               ScrollingSpeed;
    float               ScrollingRectMinX;
    float               ScrollingRectMaxX;
    RainGuiID             ReorderRequestTabId;
    ImS16               ReorderRequestOffset;
    ImS8                BeginCount;
    bool                WantLayout;
    bool                VisibleTabWasSubmitted;
    bool                TabsAddedNew;           // Set to true when a new tab item or button has been added to the tab bar during last frame
    ImS16               TabsActiveCount;        // Number of tabs submitted this frame.
    ImS16               LastTabItemIdx;         // Index of last BeginTabItem() tab for use by EndTabItem()
    float               ItemSpacingY;
    ImVec2              FramePadding;           // style.FramePadding locked at the time of BeginTabBar()
    ImVec2              BackupCursorPos;
    RainGuiTextBuffer     TabsNames;              // For non-docking tab bar we re-append names in a contiguous buffer.

    RainGuiTabBar();
    int                 GetTabOrder(const RainGuiTabItem* tab) const  { return Tabs.index_from_ptr(tab); }
    const char*         GetTabName(const RainGuiTabItem* tab) const
    {
        IM_ASSERT(tab->NameOffset != -1 && (int)tab->NameOffset < TabsNames.Buf.Size);
        return TabsNames.Buf.Data + tab->NameOffset;
    }
};

//-----------------------------------------------------------------------------
// [SECTION] Table support
//-----------------------------------------------------------------------------

#define IM_COL32_DISABLE                IM_COL32(0,0,0,1)   // Special sentinel code which cannot be used as a regular color.
#define RAINGUI_TABLE_MAX_COLUMNS         64                  // sizeof(ImU64) * 8. This is solely because we frequently encode columns set in a ImU64.
#define RAINGUI_TABLE_MAX_DRAW_CHANNELS   (4 + 64 * 2)        // See TableSetupDrawChannels()

// Our current column maximum is 64 but we may raise that in the future.
typedef ImS8 RainGuiTableColumnIdx;
typedef ImU8 RainGuiTableDrawChannelIdx;

// [Internal] sizeof() ~ 104
// We use the terminology "Enabled" to refer to a column that is not Hidden by user/api.
// We use the terminology "Clipped" to refer to a column that is out of sight because of scrolling/clipping.
// This is in contrast with some user-facing api such as IsItemVisible() / IsRectVisible() which use "Visible" to mean "not clipped".
struct RainGuiTableColumn
{
    RainGuiTableColumnFlags   Flags;                          // Flags after some patching (not directly same as provided by user). See RainGuiTableColumnFlags_
    float                   WidthGiven;                     // Final/actual width visible == (MaxX - MinX), locked in TableUpdateLayout(). May be > WidthRequest to honor minimum width, may be < WidthRequest to honor shrinking columns down in tight space.
    float                   MinX;                           // Absolute positions
    float                   MaxX;
    float                   WidthRequest;                   // Master width absolute value when !(Flags & _WidthStretch). When Stretch this is derived every frame from StretchWeight in TableUpdateLayout()
    float                   WidthAuto;                      // Automatic width
    float                   StretchWeight;                  // Master width weight when (Flags & _WidthStretch). Often around ~1.0f initially.
    float                   InitStretchWeightOrWidth;       // Value passed to TableSetupColumn(). For Width it is a content width (_without padding_).
    ImRect                  ClipRect;                       // Clipping rectangle for the column
    RainGuiID                 UserID;                         // Optional, value passed to TableSetupColumn()
    float                   WorkMinX;                       // Contents region min ~(MinX + CellPaddingX + CellSpacingX1) == cursor start position when entering column
    float                   WorkMaxX;                       // Contents region max ~(MaxX - CellPaddingX - CellSpacingX2)
    float                   ItemWidth;                      // Current item width for the column, preserved across rows
    float                   ContentMaxXFrozen;              // Contents maximum position for frozen rows (apart from headers), from which we can infer content width.
    float                   ContentMaxXUnfrozen;
    float                   ContentMaxXHeadersUsed;         // Contents maximum position for headers rows (regardless of freezing). TableHeader() automatically softclip itself + report ideal desired size, to avoid creating extraneous draw calls
    float                   ContentMaxXHeadersIdeal;
    ImS16                   NameOffset;                     // Offset into parent ColumnsNames[]
    RainGuiTableColumnIdx     DisplayOrder;                   // Index within Table's IndexToDisplayOrder[] (column may be reordered by users)
    RainGuiTableColumnIdx     IndexWithinEnabledSet;          // Index within enabled/visible set (<= IndexToDisplayOrder)
    RainGuiTableColumnIdx     PrevEnabledColumn;              // Index of prev enabled/visible column within Columns[], -1 if first enabled/visible column
    RainGuiTableColumnIdx     NextEnabledColumn;              // Index of next enabled/visible column within Columns[], -1 if last enabled/visible column
    RainGuiTableColumnIdx     SortOrder;                      // Index of this column within sort specs, -1 if not sorting on this column, 0 for single-sort, may be >0 on multi-sort
    RainGuiTableDrawChannelIdx DrawChannelCurrent;            // Index within DrawSplitter.Channels[]
    RainGuiTableDrawChannelIdx DrawChannelFrozen;
    RainGuiTableDrawChannelIdx DrawChannelUnfrozen;
    bool                    IsEnabled;                      // Is the column not marked Hidden by the user? (even if off view, e.g. clipped by scrolling).
    bool                    IsEnabledNextFrame;
    bool                    IsVisibleX;                     // Is actually in view (e.g. overlapping the host window clipping rectangle, not scrolled).
    bool                    IsVisibleY;
    bool                    IsRequestOutput;                // Return value for TableSetColumnIndex() / TableNextColumn(): whether we request user to output contents or not.
    bool                    IsSkipItems;                    // Do we want item submissions to this column to be completely ignored (no layout will happen).
    bool                    IsPreserveWidthAuto;
    ImS8                    NavLayerCurrent;                // RainGuiNavLayer in 1 byte
    ImU8                    AutoFitQueue;                   // Queue of 8 values for the next 8 frames to request auto-fit
    ImU8                    CannotSkipItemsQueue;           // Queue of 8 values for the next 8 frames to disable Clipped/SkipItem
    ImU8                    SortDirection : 2;              // RainGuiSortDirection_Ascending or RainGuiSortDirection_Descending
    ImU8                    SortDirectionsAvailCount : 2;   // Number of available sort directions (0 to 3)
    ImU8                    SortDirectionsAvailMask : 4;    // Mask of available sort directions (1-bit each)
    ImU8                    SortDirectionsAvailList;        // Ordered of available sort directions (2-bits each)

    RainGuiTableColumn()
    {
        memset(this, 0, sizeof(*this));
        StretchWeight = WidthRequest = -1.0f;
        NameOffset = -1;
        DisplayOrder = IndexWithinEnabledSet = -1;
        PrevEnabledColumn = NextEnabledColumn = -1;
        SortOrder = -1;
        SortDirection = RainGuiSortDirection_None;
        DrawChannelCurrent = DrawChannelFrozen = DrawChannelUnfrozen = (ImU8)-1;
    }
};

// Transient cell data stored per row.
// sizeof() ~ 6
struct RainGuiTableCellData
{
    ImU32                       BgColor;    // Actual color
    RainGuiTableColumnIdx         Column;     // Column number
};

// FIXME-TABLE: more transient data could be stored in a per-stacked table structure: DrawSplitter, SortSpecs, incoming RowData
struct RainGuiTable
{
    RainGuiID                     ID;
    RainGuiTableFlags             Flags;
    void*                       RawData;                    // Single allocation to hold Columns[], DisplayOrderToIndex[] and RowCellData[]
    RainGuiTableTempData*         TempData;                   // Transient data while table is active. Point within g.CurrentTableStack[]
    ImSpan<RainGuiTableColumn>    Columns;                    // Point within RawData[]
    ImSpan<RainGuiTableColumnIdx> DisplayOrderToIndex;        // Point within RawData[]. Store display order of columns (when not reordered, the values are 0...Count-1)
    ImSpan<RainGuiTableCellData>  RowCellData;                // Point within RawData[]. Store cells background requests for current row.
    ImU64                       EnabledMaskByDisplayOrder;  // Column DisplayOrder -> IsEnabled map
    ImU64                       EnabledMaskByIndex;         // Column Index -> IsEnabled map (== not hidden by user/api) in a format adequate for iterating column without touching cold data
    ImU64                       VisibleMaskByIndex;         // Column Index -> IsVisibleX|IsVisibleY map (== not hidden by user/api && not hidden by scrolling/cliprect)
    ImU64                       RequestOutputMaskByIndex;   // Column Index -> IsVisible || AutoFit (== expect user to submit items)
    RainGuiTableFlags             SettingsLoadedFlags;        // Which data were loaded from the .ini file (e.g. when order is not altered we won't save order)
    int                         SettingsOffset;             // Offset in g.SettingsTables
    int                         LastFrameActive;
    int                         ColumnsCount;               // Number of columns declared in BeginTable()
    int                         CurrentRow;
    int                         CurrentColumn;
    ImS16                       InstanceCurrent;            // Count of BeginTable() calls with same ID in the same frame (generally 0). This is a little bit similar to BeginCount for a window, but multiple table with same ID look are multiple tables, they are just synched.
    ImS16                       InstanceInteracted;         // Mark which instance (generally 0) of the same ID is being interacted with
    float                       RowPosY1;
    float                       RowPosY2;
    float                       RowMinHeight;               // Height submitted to TableNextRow()
    float                       RowTextBaseline;
    float                       RowIndentOffsetX;
    RainGuiTableRowFlags          RowFlags : 16;              // Current row flags, see RainGuiTableRowFlags_
    RainGuiTableRowFlags          LastRowFlags : 16;
    int                         RowBgColorCounter;          // Counter for alternating background colors (can be fast-forwarded by e.g clipper), not same as CurrentRow because header rows typically don't increase this.
    ImU32                       RowBgColor[2];              // Background color override for current row.
    ImU32                       BorderColorStrong;
    ImU32                       BorderColorLight;
    float                       BorderX1;
    float                       BorderX2;
    float                       HostIndentX;
    float                       MinColumnWidth;
    float                       OuterPaddingX;
    float                       CellPaddingX;               // Padding from each borders
    float                       CellPaddingY;
    float                       CellSpacingX1;              // Spacing between non-bordered cells
    float                       CellSpacingX2;
    float                       LastOuterHeight;            // Outer height from last frame
    float                       LastFirstRowHeight;         // Height of first row from last frame
    float                       InnerWidth;                 // User value passed to BeginTable(), see comments at the top of BeginTable() for details.
    float                       ColumnsGivenWidth;          // Sum of current column width
    float                       ColumnsAutoFitWidth;        // Sum of ideal column width in order nothing to be clipped, used for auto-fitting and content width submission in outer window
    float                       ResizedColumnNextWidth;
    float                       ResizeLockMinContentsX2;    // Lock minimum contents width while resizing down in order to not create feedback loops. But we allow growing the table.
    float                       RefScale;                   // Reference scale to be able to rescale columns on font/dpi changes.
    ImRect                      OuterRect;                  // Note: for non-scrolling table, OuterRect.Max.y is often FLT_MAX until EndTable(), unless a height has been specified in BeginTable().
    ImRect                      InnerRect;                  // InnerRect but without decoration. As with OuterRect, for non-scrolling tables, InnerRect.Max.y is
    ImRect                      WorkRect;
    ImRect                      InnerClipRect;
    ImRect                      BgClipRect;                 // We use this to cpu-clip cell background color fill
    ImRect                      Bg0ClipRectForDrawCmd;      // Actual ImDrawCmd clip rect for BG0/1 channel. This tends to be == OuterWindow->ClipRect at BeginTable() because output in BG0/BG1 is cpu-clipped
    ImRect                      Bg2ClipRectForDrawCmd;      // Actual ImDrawCmd clip rect for BG2 channel. This tends to be a correct, tight-fit, because output to BG2 are done by widgets relying on regular ClipRect.
    ImRect                      HostClipRect;               // This is used to check if we can eventually merge our columns draw calls into the current draw call of the current window.
    ImRect                      HostBackupInnerClipRect;    // Backup of InnerWindow->ClipRect during PushTableBackground()/PopTableBackground()
    RainGuiWindow*                OuterWindow;                // Parent window for the table
    RainGuiWindow*                InnerWindow;                // Window holding the table data (== OuterWindow or a child window)
    RainGuiTextBuffer             ColumnsNames;               // Contiguous buffer holding columns names
    ImDrawListSplitter*         DrawSplitter;               // Shortcut to TempData->DrawSplitter while in table. Isolate draw commands per columns to avoid switching clip rect constantly
    RainGuiTableSortSpecs         SortSpecs;                  // Public facing sorts specs, this is what we return in TableGetSortSpecs()
    RainGuiTableColumnIdx         SortSpecsCount;
    RainGuiTableColumnIdx         ColumnsEnabledCount;        // Number of enabled columns (<= ColumnsCount)
    RainGuiTableColumnIdx         ColumnsEnabledFixedCount;   // Number of enabled columns (<= ColumnsCount)
    RainGuiTableColumnIdx         DeclColumnsCount;           // Count calls to TableSetupColumn()
    RainGuiTableColumnIdx         HoveredColumnBody;          // Index of column whose visible region is being hovered. Important: == ColumnsCount when hovering empty region after the right-most column!
    RainGuiTableColumnIdx         HoveredColumnBorder;        // Index of column whose right-border is being hovered (for resizing).
    RainGuiTableColumnIdx         AutoFitSingleColumn;        // Index of single column requesting auto-fit.
    RainGuiTableColumnIdx         ResizedColumn;              // Index of column being resized. Reset when InstanceCurrent==0.
    RainGuiTableColumnIdx         LastResizedColumn;          // Index of column being resized from previous frame.
    RainGuiTableColumnIdx         HeldHeaderColumn;           // Index of column header being held.
    RainGuiTableColumnIdx         ReorderColumn;              // Index of column being reordered. (not cleared)
    RainGuiTableColumnIdx         ReorderColumnDir;           // -1 or +1
    RainGuiTableColumnIdx         LeftMostEnabledColumn;      // Index of left-most non-hidden column.
    RainGuiTableColumnIdx         RightMostEnabledColumn;     // Index of right-most non-hidden column.
    RainGuiTableColumnIdx         LeftMostStretchedColumn;    // Index of left-most stretched column.
    RainGuiTableColumnIdx         RightMostStretchedColumn;   // Index of right-most stretched column.
    RainGuiTableColumnIdx         ContextPopupColumn;         // Column right-clicked on, of -1 if opening context menu from a neutral/empty spot
    RainGuiTableColumnIdx         FreezeRowsRequest;          // Requested frozen rows count
    RainGuiTableColumnIdx         FreezeRowsCount;            // Actual frozen row count (== FreezeRowsRequest, or == 0 when no scrolling offset)
    RainGuiTableColumnIdx         FreezeColumnsRequest;       // Requested frozen columns count
    RainGuiTableColumnIdx         FreezeColumnsCount;         // Actual frozen columns count (== FreezeColumnsRequest, or == 0 when no scrolling offset)
    RainGuiTableColumnIdx         RowCellDataCurrent;         // Index of current RowCellData[] entry in current row
    RainGuiTableDrawChannelIdx    DummyDrawChannel;           // Redirect non-visible columns here.
    RainGuiTableDrawChannelIdx    Bg2DrawChannelCurrent;      // For Selectable() and other widgets drawing across columns after the freezing line. Index within DrawSplitter.Channels[]
    RainGuiTableDrawChannelIdx    Bg2DrawChannelUnfrozen;
    bool                        IsLayoutLocked;             // Set by TableUpdateLayout() which is called when beginning the first row.
    bool                        IsInsideRow;                // Set when inside TableBeginRow()/TableEndRow().
    bool                        IsInitializing;
    bool                        IsSortSpecsDirty;
    bool                        IsUsingHeaders;             // Set when the first row had the RainGuiTableRowFlags_Headers flag.
    bool                        IsContextPopupOpen;         // Set when default context menu is open (also see: ContextPopupColumn, InstanceInteracted).
    bool                        IsSettingsRequestLoad;
    bool                        IsSettingsDirty;            // Set when table settings have changed and needs to be reported into RainGuiTableSetttings data.
    bool                        IsDefaultDisplayOrder;      // Set when display order is unchanged from default (DisplayOrder contains 0...Count-1)
    bool                        IsResetAllRequest;
    bool                        IsResetDisplayOrderRequest;
    bool                        IsUnfrozenRows;             // Set when we got past the frozen row.
    bool                        IsDefaultSizingPolicy;      // Set if user didn't explicitly set a sizing policy in BeginTable()
    bool                        MemoryCompacted;
    bool                        HostSkipItems;              // Backup of InnerWindow->SkipItem at the end of BeginTable(), because we will overwrite InnerWindow->SkipItem on a per-column basis

    RAINGUI_API RainGuiTable()      { memset(this, 0, sizeof(*this)); LastFrameActive = -1; }
    RAINGUI_API ~RainGuiTable()     { IM_FREE(RawData); }
};

// Transient data that are only needed between BeginTable() and EndTable(), those buffers are shared (1 per level of stacked table).
// - Accessing those requires chasing an extra pointer so for very frequently used data we leave them in the main table structure.
// - We also leave out of this structure data that tend to be particularly useful for debugging/metrics.
// FIXME-TABLE: more transient data could be stored here: DrawSplitter, incoming RowData?
struct RainGuiTableTempData
{
    int                         TableIndex;                 // Index in g.Tables.Buf[] pool
    float                       LastTimeActive;             // Last timestamp this structure was used

    ImVec2                      UserOuterSize;              // outer_size.x passed to BeginTable()
    ImDrawListSplitter          DrawSplitter;
    RainGuiTableColumnSortSpecs   SortSpecsSingle;
    ImVector<RainGuiTableColumnSortSpecs> SortSpecsMulti;     // FIXME-OPT: Using a small-vector pattern would be good.

    ImRect                      HostBackupWorkRect;         // Backup of InnerWindow->WorkRect at the end of BeginTable()
    ImRect                      HostBackupParentWorkRect;   // Backup of InnerWindow->ParentWorkRect at the end of BeginTable()
    ImVec2                      HostBackupPrevLineSize;     // Backup of InnerWindow->DC.PrevLineSize at the end of BeginTable()
    ImVec2                      HostBackupCurrLineSize;     // Backup of InnerWindow->DC.CurrLineSize at the end of BeginTable()
    ImVec2                      HostBackupCursorMaxPos;     // Backup of InnerWindow->DC.CursorMaxPos at the end of BeginTable()
    ImVec1                      HostBackupColumnsOffset;    // Backup of OuterWindow->DC.ColumnsOffset at the end of BeginTable()
    float                       HostBackupItemWidth;        // Backup of OuterWindow->DC.ItemWidth at the end of BeginTable()
    int                         HostBackupItemWidthStackSize;//Backup of OuterWindow->DC.ItemWidthStack.Size at the end of BeginTable()

    RAINGUI_API RainGuiTableTempData() { memset(this, 0, sizeof(*this)); LastTimeActive = -1.0f; }
};

// sizeof() ~ 12
struct RainGuiTableColumnSettings
{
    float                   WidthOrWeight;
    RainGuiID                 UserID;
    RainGuiTableColumnIdx     Index;
    RainGuiTableColumnIdx     DisplayOrder;
    RainGuiTableColumnIdx     SortOrder;
    ImU8                    SortDirection : 2;
    ImU8                    IsEnabled : 1; // "Visible" in ini file
    ImU8                    IsStretch : 1;

    RainGuiTableColumnSettings()
    {
        WidthOrWeight = 0.0f;
        UserID = 0;
        Index = -1;
        DisplayOrder = SortOrder = -1;
        SortDirection = RainGuiSortDirection_None;
        IsEnabled = 1;
        IsStretch = 0;
    }
};

// This is designed to be stored in a single ImChunkStream (1 header followed by N RainGuiTableColumnSettings, etc.)
struct RainGuiTableSettings
{
    RainGuiID                     ID;                     // Set to 0 to invalidate/delete the setting
    RainGuiTableFlags             SaveFlags;              // Indicate data we want to save using the Resizable/Reorderable/Sortable/Hideable flags (could be using its own flags..)
    float                       RefScale;               // Reference scale to be able to rescale columns on font/dpi changes.
    RainGuiTableColumnIdx         ColumnsCount;
    RainGuiTableColumnIdx         ColumnsCountMax;        // Maximum number of columns this settings instance can store, we can recycle a settings instance with lower number of columns but not higher
    bool                        WantApply;              // Set when loaded from .ini data (to enable merging/loading .ini data into an already running context)

    RainGuiTableSettings()        { memset(this, 0, sizeof(*this)); }
    RainGuiTableColumnSettings*   GetColumnSettings()     { return (RainGuiTableColumnSettings*)(this + 1); }
};

//-----------------------------------------------------------------------------
// [SECTION] RainGui internal API
// No guarantee of forward compatibility here!
//-----------------------------------------------------------------------------

namespace RainGui
{
    // Windows
    // We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
    // If this ever crash because g.CurrentWindow is NULL it means that either
    // - RainGui::NewFrame() has never been called, which is illegal.
    // - You are calling RainGui functions after RainGui::EndFrame()/RainGui::Render() and before the next RainGui::NewFrame(), which is also illegal.
    inline    RainGuiWindow*  GetCurrentWindowRead()      { RainGuiContext& g = *GRainGui; return g.CurrentWindow; }
    inline    RainGuiWindow*  GetCurrentWindow()          { RainGuiContext& g = *GRainGui; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; }
    RAINGUI_API RainGuiWindow*  FindWindowByID(RainGuiID id);
    RAINGUI_API RainGuiWindow*  FindWindowByName(const char* name);
    RAINGUI_API void          UpdateWindowParentAndRootLinks(RainGuiWindow* window, RainGuiWindowFlags flags, RainGuiWindow* parent_window);
    RAINGUI_API ImVec2        CalcWindowNextAutoFitSize(RainGuiWindow* window);
    RAINGUI_API bool          IsWindowChildOf(RainGuiWindow* window, RainGuiWindow* potential_parent);
    RAINGUI_API bool          IsWindowAbove(RainGuiWindow* potential_above, RainGuiWindow* potential_below);
    RAINGUI_API bool          IsWindowNavFocusable(RainGuiWindow* window);
    RAINGUI_API ImRect        GetWindowAllowedExtentRect(RainGuiWindow* window);
    RAINGUI_API void          SetWindowPos(RainGuiWindow* window, const ImVec2& pos, RainGuiCond cond = 0);
    RAINGUI_API void          SetWindowSize(RainGuiWindow* window, const ImVec2& size, RainGuiCond cond = 0);
    RAINGUI_API void          SetWindowCollapsed(RainGuiWindow* window, bool collapsed, RainGuiCond cond = 0);
    RAINGUI_API void          SetWindowHitTestHole(RainGuiWindow* window, const ImVec2& pos, const ImVec2& size);

    // Windows: Display Order and Focus Order
    RAINGUI_API void          FocusWindow(RainGuiWindow* window);
    RAINGUI_API void          FocusTopMostWindowUnderOne(RainGuiWindow* under_this_window, RainGuiWindow* ignore_window);
    RAINGUI_API void          BringWindowToFocusFront(RainGuiWindow* window);
    RAINGUI_API void          BringWindowToDisplayFront(RainGuiWindow* window);
    RAINGUI_API void          BringWindowToDisplayBack(RainGuiWindow* window);

    // Fonts, drawing
    RAINGUI_API void          SetCurrentFont(ImFont* font);
    inline ImFont*          GetDefaultFont() { RainGuiContext& g = *GRainGui; return g.IO.FontDefault ? g.IO.FontDefault : g.IO.Fonts->Fonts[0]; }
    inline ImDrawList*      GetForegroundDrawList(RainGuiWindow* window) { IM_UNUSED(window); return GetForegroundDrawList(); } // This seemingly unnecessary wrapper simplifies compatibility between the 'master' and 'docking' branches.
    RAINGUI_API ImDrawList*   GetBackgroundDrawList(RainGuiViewport* viewport);                     // get background draw list for the given viewport. this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear raingui contents.
    RAINGUI_API ImDrawList*   GetForegroundDrawList(RainGuiViewport* viewport);                     // get foreground draw list for the given viewport. this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear raingui contents.

    // Init
    RAINGUI_API void          Initialize(RainGuiContext* context);
    RAINGUI_API void          Shutdown(RainGuiContext* context);    // Since 1.60 this is a _private_ function. You can call DestroyContext() to destroy the context created by CreateContext().

    // NewFrame
    RAINGUI_API void          UpdateHoveredWindowAndCaptureFlags();
    RAINGUI_API void          StartMouseMovingWindow(RainGuiWindow* window);
    RAINGUI_API void          UpdateMouseMovingWindowNewFrame();
    RAINGUI_API void          UpdateMouseMovingWindowEndFrame();

    // Generic context hooks
    RAINGUI_API RainGuiID       AddContextHook(RainGuiContext* context, const RainGuiContextHook* hook);
    RAINGUI_API void          RemoveContextHook(RainGuiContext* context, RainGuiID hook_to_remove);
    RAINGUI_API void          CallContextHooks(RainGuiContext* context, RainGuiContextHookType type);

    // Settings
    RAINGUI_API void                  MarkIniSettingsDirty();
    RAINGUI_API void                  MarkIniSettingsDirty(RainGuiWindow* window);
    RAINGUI_API void                  ClearIniSettings();
    RAINGUI_API RainGuiWindowSettings*  CreateNewWindowSettings(const char* name);
    RAINGUI_API RainGuiWindowSettings*  FindWindowSettings(RainGuiID id);
    RAINGUI_API RainGuiWindowSettings*  FindOrCreateWindowSettings(const char* name);
    RAINGUI_API RainGuiSettingsHandler* FindSettingsHandler(const char* type_name);

    // Scrolling
    RAINGUI_API void          SetNextWindowScroll(const ImVec2& scroll); // Use -1.0f on one axis to leave as-is
    RAINGUI_API void          SetScrollX(RainGuiWindow* window, float scroll_x);
    RAINGUI_API void          SetScrollY(RainGuiWindow* window, float scroll_y);
    RAINGUI_API void          SetScrollFromPosX(RainGuiWindow* window, float local_x, float center_x_ratio);
    RAINGUI_API void          SetScrollFromPosY(RainGuiWindow* window, float local_y, float center_y_ratio);
    RAINGUI_API ImVec2        ScrollToBringRectIntoView(RainGuiWindow* window, const ImRect& item_rect);

    // Basic Accessors
    inline RainGuiID          GetItemID()     { RainGuiContext& g = *GRainGui; return g.CurrentWindow->DC.LastItemId; }   // Get ID of last item (~~ often same RainGui::GetID(label) beforehand)
    inline RainGuiItemStatusFlags GetItemStatusFlags() { RainGuiContext& g = *GRainGui; return g.CurrentWindow->DC.LastItemStatusFlags; }
    inline RainGuiID          GetActiveID()   { RainGuiContext& g = *GRainGui; return g.ActiveId; }
    inline RainGuiID          GetFocusID()    { RainGuiContext& g = *GRainGui; return g.NavId; }
    inline RainGuiItemFlags   GetItemFlags()  { RainGuiContext& g = *GRainGui; return g.CurrentItemFlags; }
    RAINGUI_API void          SetActiveID(RainGuiID id, RainGuiWindow* window);
    RAINGUI_API void          SetFocusID(RainGuiID id, RainGuiWindow* window);
    RAINGUI_API void          ClearActiveID();
    RAINGUI_API RainGuiID       GetHoveredID();
    RAINGUI_API void          SetHoveredID(RainGuiID id);
    RAINGUI_API void          KeepAliveID(RainGuiID id);
    RAINGUI_API void          MarkItemEdited(RainGuiID id);     // Mark data associated to given item as "edited", used by IsItemDeactivatedAfterEdit() function.
    RAINGUI_API void          PushOverrideID(RainGuiID id);     // Push given value as-is at the top of the ID stack (whereas PushID combines old and new hashes)
    RAINGUI_API RainGuiID       GetIDWithSeed(const char* str_id_begin, const char* str_id_end, RainGuiID seed);

    // Basic Helpers for widget code
    RAINGUI_API void          ItemSize(const ImVec2& size, float text_baseline_y = -1.0f);
    RAINGUI_API void          ItemSize(const ImRect& bb, float text_baseline_y = -1.0f);
    RAINGUI_API bool          ItemAdd(const ImRect& bb, RainGuiID id, const ImRect* nav_bb = NULL, RainGuiItemAddFlags flags = 0);
    RAINGUI_API bool          ItemHoverable(const ImRect& bb, RainGuiID id);
    RAINGUI_API void          ItemFocusable(RainGuiWindow* window, RainGuiID id);
    RAINGUI_API bool          IsClippedEx(const ImRect& bb, RainGuiID id, bool clip_even_when_logged);
    RAINGUI_API void          SetLastItemData(RainGuiWindow* window, RainGuiID item_id, RainGuiItemStatusFlags status_flags, const ImRect& item_rect);
    RAINGUI_API ImVec2        CalcItemSize(ImVec2 size, float default_w, float default_h);
    RAINGUI_API float         CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x);
    RAINGUI_API void          PushMultiItemsWidths(int components, float width_full);
    RAINGUI_API void          PushItemFlag(RainGuiItemFlags option, bool enabled);
    RAINGUI_API void          PopItemFlag();
    RAINGUI_API bool          IsItemToggledSelection();                                   // Was the last item selection toggled? (after Selectable(), TreeNode() etc. We only returns toggle _event_ in order to handle clipping correctly)
    RAINGUI_API ImVec2        GetContentRegionMaxAbs();
    RAINGUI_API void          ShrinkWidths(RainGuiShrinkWidthItem* items, int count, float width_excess);

#ifndef RAINGUI_DISABLE_OBSOLETE_FUNCTIONS
    // If you have old/custom copy-and-pasted widgets that used FocusableItemRegister():
    //  (Old) RAINGUI_VERSION_NUM  < 18209: using 'ItemAdd(....)'                              and 'bool focused = FocusableItemRegister(...)'
    //  (New) RAINGUI_VERSION_NUM >= 18209: using 'ItemAdd(..., RainGuiItemAddFlags_Focusable)'  and 'bool focused = (GetItemStatusFlags() & RainGuiItemStatusFlags_Focused) != 0'
    // Widget code are simplified as there's no need to call FocusableItemUnregister() while managing the transition from regular widget to TempInputText()
    inline bool FocusableItemRegister(RainGuiWindow* window, RainGuiID id)  { IM_ASSERT(0); IM_UNUSED(window); IM_UNUSED(id); return false; } // -> pass RainGuiItemAddFlags_Focusable flag to ItemAdd()
    inline void FocusableItemUnregister(RainGuiWindow* window)            { IM_ASSERT(0); IM_UNUSED(window); }                              // -> unnecessary: TempInputText() uses RainGuiInputTextFlags_MergedItem
#endif

    // Logging/Capture
    RAINGUI_API void          LogBegin(RainGuiLogType type, int auto_open_depth);           // -> BeginCapture() when we design v2 api, for now stay under the radar by using the old name.
    RAINGUI_API void          LogToBuffer(int auto_open_depth = -1);                      // Start logging/capturing to internal buffer
    RAINGUI_API void          LogRenderedText(const ImVec2* ref_pos, const char* text, const char* text_end = NULL);
    RAINGUI_API void          LogSetNextTextDecoration(const char* prefix, const char* suffix);

    // Popups, Modals, Tooltips
    RAINGUI_API bool          BeginChildEx(const char* name, RainGuiID id, const ImVec2& size_arg, bool border, RainGuiWindowFlags flags);
    RAINGUI_API void          OpenPopupEx(RainGuiID id, RainGuiPopupFlags popup_flags = RainGuiPopupFlags_None);
    RAINGUI_API void          ClosePopupToLevel(int remaining, bool restore_focus_to_window_under_popup);
    RAINGUI_API void          ClosePopupsOverWindow(RainGuiWindow* ref_window, bool restore_focus_to_window_under_popup);
    RAINGUI_API bool          IsPopupOpen(RainGuiID id, RainGuiPopupFlags popup_flags);
    RAINGUI_API bool          BeginPopupEx(RainGuiID id, RainGuiWindowFlags extra_flags);
    RAINGUI_API void          BeginTooltipEx(RainGuiWindowFlags extra_flags, RainGuiTooltipFlags tooltip_flags);
    RAINGUI_API RainGuiWindow*  GetTopMostPopupModal();
    RAINGUI_API ImVec2        FindBestWindowPosForPopup(RainGuiWindow* window);
    RAINGUI_API ImVec2        FindBestWindowPosForPopupEx(const ImVec2& ref_pos, const ImVec2& size, RainGuiDir* last_dir, const ImRect& r_outer, const ImRect& r_avoid, RainGuiPopupPositionPolicy policy);
    RAINGUI_API bool          BeginViewportSideBar(const char* name, RainGuiViewport* viewport, RainGuiDir dir, float size, RainGuiWindowFlags window_flags);

    // Gamepad/Keyboard Navigation
    RAINGUI_API void          NavInitWindow(RainGuiWindow* window, bool force_reinit);
    RAINGUI_API bool          NavMoveRequestButNoResultYet();
    RAINGUI_API void          NavMoveRequestCancel();
    RAINGUI_API void          NavMoveRequestForward(RainGuiDir move_dir, RainGuiDir clip_dir, const ImRect& bb_rel, RainGuiNavMoveFlags move_flags);
    RAINGUI_API void          NavMoveRequestTryWrapping(RainGuiWindow* window, RainGuiNavMoveFlags move_flags);
    RAINGUI_API float         GetNavInputAmount(RainGuiNavInput n, RainGuiInputReadMode mode);
    RAINGUI_API ImVec2        GetNavInputAmount2d(RainGuiNavDirSourceFlags dir_sources, RainGuiInputReadMode mode, float slow_factor = 0.0f, float fast_factor = 0.0f);
    RAINGUI_API int           CalcTypematicRepeatAmount(float t0, float t1, float repeat_delay, float repeat_rate);
    RAINGUI_API void          ActivateItem(RainGuiID id);   // Remotely activate a button, checkbox, tree node etc. given its unique ID. activation is queued and processed on the next frame when the item is encountered again.
    RAINGUI_API void          SetNavID(RainGuiID id, RainGuiNavLayer nav_layer, RainGuiID focus_scope_id, const ImRect& rect_rel);

    // Focus Scope (WIP)
    // This is generally used to identify a selection set (multiple of which may be in the same window), as selection
    // patterns generally need to react (e.g. clear selection) when landing on an item of the set.
    RAINGUI_API void          PushFocusScope(RainGuiID id);
    RAINGUI_API void          PopFocusScope();
    inline RainGuiID          GetFocusedFocusScope()          { RainGuiContext& g = *GRainGui; return g.NavFocusScopeId; }                            // Focus scope which is actually active
    inline RainGuiID          GetFocusScope()                 { RainGuiContext& g = *GRainGui; return g.CurrentWindow->DC.NavFocusScopeIdCurrent; }   // Focus scope we are outputting into, set by PushFocusScope()

    // Inputs
    // FIXME: Eventually we should aim to move e.g. IsActiveIdUsingKey() into IsKeyXXX functions.
    RAINGUI_API void          SetItemUsingMouseWheel();
    inline bool             IsActiveIdUsingNavDir(RainGuiDir dir)                         { RainGuiContext& g = *GRainGui; return (g.ActiveIdUsingNavDirMask & (1 << dir)) != 0; }
    inline bool             IsActiveIdUsingNavInput(RainGuiNavInput input)                { RainGuiContext& g = *GRainGui; return (g.ActiveIdUsingNavInputMask & (1 << input)) != 0; }
    inline bool             IsActiveIdUsingKey(RainGuiKey key)                            { RainGuiContext& g = *GRainGui; IM_ASSERT(key < 64); return (g.ActiveIdUsingKeyInputMask & ((ImU64)1 << key)) != 0; }
    RAINGUI_API bool          IsMouseDragPastThreshold(RainGuiMouseButton button, float lock_threshold = -1.0f);
    inline bool             IsKeyPressedMap(RainGuiKey key, bool repeat = true)           { RainGuiContext& g = *GRainGui; const int key_index = g.IO.KeyMap[key]; return (key_index >= 0) ? IsKeyPressed(key_index, repeat) : false; }
    inline bool             IsNavInputDown(RainGuiNavInput n)                             { RainGuiContext& g = *GRainGui; return g.IO.NavInputs[n] > 0.0f; }
    inline bool             IsNavInputTest(RainGuiNavInput n, RainGuiInputReadMode rm)      { return (GetNavInputAmount(n, rm) > 0.0f); }
    RAINGUI_API RainGuiKeyModFlags GetMergedKeyModFlags();

    // Drag and Drop
    RAINGUI_API bool          BeginDragDropTargetCustom(const ImRect& bb, RainGuiID id);
    RAINGUI_API void          ClearDragDrop();
    RAINGUI_API bool          IsDragDropPayloadBeingAccepted();

    // Internal Columns API (this is not exposed because we will encourage transitioning to the Tables API)
    RAINGUI_API void          SetWindowClipRectBeforeSetChannel(RainGuiWindow* window, const ImRect& clip_rect);
    RAINGUI_API void          BeginColumns(const char* str_id, int count, RainGuiOldColumnFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
    RAINGUI_API void          EndColumns();                                                               // close columns
    RAINGUI_API void          PushColumnClipRect(int column_index);
    RAINGUI_API void          PushColumnsBackground();
    RAINGUI_API void          PopColumnsBackground();
    RAINGUI_API RainGuiID       GetColumnsID(const char* str_id, int count);
    RAINGUI_API RainGuiOldColumns* FindOrCreateColumns(RainGuiWindow* window, RainGuiID id);
    RAINGUI_API float         GetColumnOffsetFromNorm(const RainGuiOldColumns* columns, float offset_norm);
    RAINGUI_API float         GetColumnNormFromOffset(const RainGuiOldColumns* columns, float offset);

    // Tables: Candidates for public API
    RAINGUI_API void          TableOpenContextMenu(int column_n = -1);
    RAINGUI_API void          TableSetColumnWidth(int column_n, float width);
    RAINGUI_API void          TableSetColumnSortDirection(int column_n, RainGuiSortDirection sort_direction, bool append_to_sort_specs);
    RAINGUI_API int           TableGetHoveredColumn(); // May use (TableGetColumnFlags() & RainGuiTableColumnFlags_IsHovered) instead. Return hovered column. return -1 when table is not hovered. return columns_count if the unused space at the right of visible columns is hovered.
    RAINGUI_API float         TableGetHeaderRowHeight();
    RAINGUI_API void          TablePushBackgroundChannel();
    RAINGUI_API void          TablePopBackgroundChannel();

    // Tables: Internals
    inline    RainGuiTable*   GetCurrentTable() { RainGuiContext& g = *GRainGui; return g.CurrentTable; }
    RAINGUI_API RainGuiTable*   TableFindByID(RainGuiID id);
    RAINGUI_API bool          BeginTableEx(const char* name, RainGuiID id, int columns_count, RainGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0, 0), float inner_width = 0.0f);
    RAINGUI_API void          TableBeginInitMemory(RainGuiTable* table, int columns_count);
    RAINGUI_API void          TableBeginApplyRequests(RainGuiTable* table);
    RAINGUI_API void          TableSetupDrawChannels(RainGuiTable* table);
    RAINGUI_API void          TableUpdateLayout(RainGuiTable* table);
    RAINGUI_API void          TableUpdateBorders(RainGuiTable* table);
    RAINGUI_API void          TableUpdateColumnsWeightFromWidth(RainGuiTable* table);
    RAINGUI_API void          TableDrawBorders(RainGuiTable* table);
    RAINGUI_API void          TableDrawContextMenu(RainGuiTable* table);
    RAINGUI_API void          TableMergeDrawChannels(RainGuiTable* table);
    RAINGUI_API void          TableSortSpecsSanitize(RainGuiTable* table);
    RAINGUI_API void          TableSortSpecsBuild(RainGuiTable* table);
    RAINGUI_API RainGuiSortDirection TableGetColumnNextSortDirection(RainGuiTableColumn* column);
    RAINGUI_API void          TableFixColumnSortDirection(RainGuiTable* table, RainGuiTableColumn* column);
    RAINGUI_API float         TableGetColumnWidthAuto(RainGuiTable* table, RainGuiTableColumn* column);
    RAINGUI_API void          TableBeginRow(RainGuiTable* table);
    RAINGUI_API void          TableEndRow(RainGuiTable* table);
    RAINGUI_API void          TableBeginCell(RainGuiTable* table, int column_n);
    RAINGUI_API void          TableEndCell(RainGuiTable* table);
    RAINGUI_API ImRect        TableGetCellBgRect(const RainGuiTable* table, int column_n);
    RAINGUI_API const char*   TableGetColumnName(const RainGuiTable* table, int column_n);
    RAINGUI_API RainGuiID       TableGetColumnResizeID(const RainGuiTable* table, int column_n, int instance_no = 0);
    RAINGUI_API float         TableGetMaxColumnWidth(const RainGuiTable* table, int column_n);
    RAINGUI_API void          TableSetColumnWidthAutoSingle(RainGuiTable* table, int column_n);
    RAINGUI_API void          TableSetColumnWidthAutoAll(RainGuiTable* table);
    RAINGUI_API void          TableRemove(RainGuiTable* table);
    RAINGUI_API void          TableGcCompactTransientBuffers(RainGuiTable* table);
    RAINGUI_API void          TableGcCompactTransientBuffers(RainGuiTableTempData* table);
    RAINGUI_API void          TableGcCompactSettings();

    // Tables: Settings
    RAINGUI_API void                  TableLoadSettings(RainGuiTable* table);
    RAINGUI_API void                  TableSaveSettings(RainGuiTable* table);
    RAINGUI_API void                  TableResetSettings(RainGuiTable* table);
    RAINGUI_API RainGuiTableSettings*   TableGetBoundSettings(RainGuiTable* table);
    RAINGUI_API void                  TableSettingsInstallHandler(RainGuiContext* context);
    RAINGUI_API RainGuiTableSettings*   TableSettingsCreate(RainGuiID id, int columns_count);
    RAINGUI_API RainGuiTableSettings*   TableSettingsFindByID(RainGuiID id);

    // Tab Bars
    RAINGUI_API bool          BeginTabBarEx(RainGuiTabBar* tab_bar, const ImRect& bb, RainGuiTabBarFlags flags);
    RAINGUI_API RainGuiTabItem* TabBarFindTabByID(RainGuiTabBar* tab_bar, RainGuiID tab_id);
    RAINGUI_API void          TabBarRemoveTab(RainGuiTabBar* tab_bar, RainGuiID tab_id);
    RAINGUI_API void          TabBarCloseTab(RainGuiTabBar* tab_bar, RainGuiTabItem* tab);
    RAINGUI_API void          TabBarQueueReorder(RainGuiTabBar* tab_bar, const RainGuiTabItem* tab, int offset);
    RAINGUI_API void          TabBarQueueReorderFromMousePos(RainGuiTabBar* tab_bar, const RainGuiTabItem* tab, ImVec2 mouse_pos);
    RAINGUI_API bool          TabBarProcessReorder(RainGuiTabBar* tab_bar);
    RAINGUI_API bool          TabItemEx(RainGuiTabBar* tab_bar, const char* label, bool* p_open, RainGuiTabItemFlags flags);
    RAINGUI_API ImVec2        TabItemCalcSize(const char* label, bool has_close_button);
    RAINGUI_API void          TabItemBackground(ImDrawList* draw_list, const ImRect& bb, RainGuiTabItemFlags flags, ImU32 col);
    RAINGUI_API void          TabItemLabelAndCloseButton(ImDrawList* draw_list, const ImRect& bb, RainGuiTabItemFlags flags, ImVec2 frame_padding, const char* label, RainGuiID tab_id, RainGuiID close_button_id, bool is_contents_visible, bool* out_just_closed, bool* out_text_clipped);

    // Render helpers
    // AVOID USING OUTSIDE OF RAINGUI.CPP! NOT FOR PUBLIC CONSUMPTION. THOSE FUNCTIONS ARE A MESS. THEIR SIGNATURE AND BEHAVIOR WILL CHANGE, THEY NEED TO BE REFACTORED INTO SOMETHING DECENT.
    // NB: All position are in absolute pixels coordinates (we are never using window coordinates internally)
    RAINGUI_API void          RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
    RAINGUI_API void          RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
    RAINGUI_API void          RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    RAINGUI_API void          RenderTextClippedEx(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    RAINGUI_API void          RenderTextEllipsis(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, float clip_max_x, float ellipsis_max_x, const char* text, const char* text_end, const ImVec2* text_size_if_known);
    RAINGUI_API void          RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
    RAINGUI_API void          RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding = 0.0f);
    RAINGUI_API void          RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float grid_step, ImVec2 grid_off, float rounding = 0.0f, ImDrawFlags flags = 0);
    RAINGUI_API void          RenderNavHighlight(const ImRect& bb, RainGuiID id, RainGuiNavHighlightFlags flags = RainGuiNavHighlightFlags_TypeDefault); // Navigation highlight
    RAINGUI_API const char*   FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.

    // Render helpers (those functions don't access any RainGui state!)
    RAINGUI_API void          RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, RainGuiDir dir, float scale = 1.0f);
    RAINGUI_API void          RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col);
    RAINGUI_API void          RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz);
    RAINGUI_API void          RenderMouseCursor(ImDrawList* draw_list, ImVec2 pos, float scale, RainGuiMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow);
    RAINGUI_API void          RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, RainGuiDir direction, ImU32 col);
    RAINGUI_API void          RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
    RAINGUI_API void          RenderRectFilledWithHole(ImDrawList* draw_list, ImRect outer, ImRect inner, ImU32 col, float rounding);

#ifndef RAINGUI_DISABLE_OBSOLETE_FUNCTIONS
    // [1.71: 2019/06/07: Updating prototypes of some of the internal functions. Leaving those for reference for a short while]
    inline void RenderArrow(ImVec2 pos, RainGuiDir dir, float scale=1.0f) { RainGuiWindow* window = GetCurrentWindow(); RenderArrow(window->DrawList, pos, GetColorU32(RainGuiCol_Text), dir, scale); }
    inline void RenderBullet(ImVec2 pos)                                { RainGuiWindow* window = GetCurrentWindow(); RenderBullet(window->DrawList, pos, GetColorU32(RainGuiCol_Text)); }
#endif

    // Widgets
    RAINGUI_API void          TextEx(const char* text, const char* text_end = NULL, RainGuiTextFlags flags = 0);
    RAINGUI_API bool          ButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), RainGuiButtonFlags flags = 0);
    RAINGUI_API bool          CloseButton(RainGuiID id, const ImVec2& pos);
    RAINGUI_API bool          CollapseButton(RainGuiID id, const ImVec2& pos);
    RAINGUI_API bool          ArrowButtonEx(const char* str_id, RainGuiDir dir, ImVec2 size_arg, RainGuiButtonFlags flags = 0);
    RAINGUI_API void          Scrollbar(RainGuiAxis axis);
    RAINGUI_API bool          ScrollbarEx(const ImRect& bb, RainGuiID id, RainGuiAxis axis, float* p_scroll_v, float avail_v, float contents_v, ImDrawFlags flags);
    RAINGUI_API bool          ImageButtonEx(RainGuiID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col);
    RAINGUI_API ImRect        GetWindowScrollbarRect(RainGuiWindow* window, RainGuiAxis axis);
    RAINGUI_API RainGuiID       GetWindowScrollbarID(RainGuiWindow* window, RainGuiAxis axis);
    RAINGUI_API RainGuiID       GetWindowResizeCornerID(RainGuiWindow* window, int n); // 0..3: corners
    RAINGUI_API RainGuiID       GetWindowResizeBorderID(RainGuiWindow* window, RainGuiDir dir);
    RAINGUI_API void          SeparatorEx(RainGuiSeparatorFlags flags);
    RAINGUI_API bool          CheckboxFlags(const char* label, ImS64* flags, ImS64 flags_value);
    RAINGUI_API bool          CheckboxFlags(const char* label, ImU64* flags, ImU64 flags_value);

    // Widgets low-level behaviors
    RAINGUI_API bool          ButtonBehavior(const ImRect& bb, RainGuiID id, bool* out_hovered, bool* out_held, RainGuiButtonFlags flags = 0);
    RAINGUI_API bool          DragBehavior(RainGuiID id, RainGuiDataType data_type, void* p_v, float v_speed, const void* p_min, const void* p_max, const char* format, RainGuiSliderFlags flags);
    RAINGUI_API bool          SliderBehavior(const ImRect& bb, RainGuiID id, RainGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, RainGuiSliderFlags flags, ImRect* out_grab_bb);
    RAINGUI_API bool          SplitterBehavior(const ImRect& bb, RainGuiID id, RainGuiAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend = 0.0f, float hover_visibility_delay = 0.0f);
    RAINGUI_API bool          TreeNodeBehavior(RainGuiID id, RainGuiTreeNodeFlags flags, const char* label, const char* label_end = NULL);
    RAINGUI_API bool          TreeNodeBehaviorIsOpen(RainGuiID id, RainGuiTreeNodeFlags flags = 0);                     // Consume previous SetNextItemOpen() data, if any. May return true when logging
    RAINGUI_API void          TreePushOverrideID(RainGuiID id);

    // Template functions are instantiated in raingui_widgets.cpp for a finite number of types.
    // To use them externally (for custom widget) you may need an "extern template" statement in your code in order to link to existing instances and silence Clang warnings (see #2036).
    // e.g. " extern template RAINGUI_API float RoundScalarWithFormatT<float, float>(const char* format, RainGuiDataType data_type, float v); "
    template<typename T, typename SIGNED_T, typename FLOAT_T>   RAINGUI_API float ScaleRatioFromValueT(RainGuiDataType data_type, T v, T v_min, T v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_size);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   RAINGUI_API T     ScaleValueFromRatioT(RainGuiDataType data_type, float t, T v_min, T v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_size);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   RAINGUI_API bool  DragBehaviorT(RainGuiDataType data_type, T* v, float v_speed, T v_min, T v_max, const char* format, RainGuiSliderFlags flags);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   RAINGUI_API bool  SliderBehaviorT(const ImRect& bb, RainGuiID id, RainGuiDataType data_type, T* v, T v_min, T v_max, const char* format, RainGuiSliderFlags flags, ImRect* out_grab_bb);
    template<typename T, typename SIGNED_T>                     RAINGUI_API T     RoundScalarWithFormatT(const char* format, RainGuiDataType data_type, T v);
    template<typename T>                                        RAINGUI_API bool  CheckboxFlagsT(const char* label, T* flags, T flags_value);

    // Data type helpers
    RAINGUI_API const RainGuiDataTypeInfo*  DataTypeGetInfo(RainGuiDataType data_type);
    RAINGUI_API int           DataTypeFormatString(char* buf, int buf_size, RainGuiDataType data_type, const void* p_data, const char* format);
    RAINGUI_API void          DataTypeApplyOp(RainGuiDataType data_type, int op, void* output, const void* arg_1, const void* arg_2);
    RAINGUI_API bool          DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, RainGuiDataType data_type, void* p_data, const char* format);
    RAINGUI_API int           DataTypeCompare(RainGuiDataType data_type, const void* arg_1, const void* arg_2);
    RAINGUI_API bool          DataTypeClamp(RainGuiDataType data_type, void* p_data, const void* p_min, const void* p_max);

    // InputText
    RAINGUI_API bool          InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, RainGuiInputTextFlags flags, RainGuiInputTextCallback callback = NULL, void* user_data = NULL);
    RAINGUI_API bool          TempInputText(const ImRect& bb, RainGuiID id, const char* label, char* buf, int buf_size, RainGuiInputTextFlags flags);
    RAINGUI_API bool          TempInputScalar(const ImRect& bb, RainGuiID id, const char* label, RainGuiDataType data_type, void* p_data, const char* format, const void* p_clamp_min = NULL, const void* p_clamp_max = NULL);
    inline bool             TempInputIsActive(RainGuiID id)       { RainGuiContext& g = *GRainGui; return (g.ActiveId == id && g.TempInputId == id); }
    inline RainGuiInputTextState* GetInputTextState(RainGuiID id)   { RainGuiContext& g = *GRainGui; return (g.InputTextState.ID == id) ? &g.InputTextState : NULL; } // Get input text state if active

    // Color
    RAINGUI_API void          ColorTooltip(const char* text, const float* col, RainGuiColorEditFlags flags);
    RAINGUI_API void          ColorEditOptionsPopup(const float* col, RainGuiColorEditFlags flags);
    RAINGUI_API void          ColorPickerOptionsPopup(const float* ref_col, RainGuiColorEditFlags flags);

    // Plot
    RAINGUI_API int           PlotEx(RainGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size);

    // Shade functions (write over already created vertices)
    RAINGUI_API void          ShadeVertsLinearColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
    RAINGUI_API void          ShadeVertsLinearUV(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp);

    // Garbage collection
    RAINGUI_API void          GcCompactTransientMiscBuffers();
    RAINGUI_API void          GcCompactTransientWindowBuffers(RainGuiWindow* window);
    RAINGUI_API void          GcAwakeTransientWindowBuffers(RainGuiWindow* window);

    // Debug Tools
    RAINGUI_API void          ErrorCheckEndFrameRecover(RainGuiErrorLogCallback log_callback, void* user_data = NULL);
    inline void             DebugDrawItemRect(ImU32 col = IM_COL32(255,0,0,255))    { RainGuiContext& g = *GRainGui; RainGuiWindow* window = g.CurrentWindow; GetForegroundDrawList(window)->AddRect(window->DC.LastItemRect.Min, window->DC.LastItemRect.Max, col); }
    inline void             DebugStartItemPicker()                                  { RainGuiContext& g = *GRainGui; g.DebugItemPickerActive = true; }

    RAINGUI_API void          DebugNodeColumns(RainGuiOldColumns* columns);
    RAINGUI_API void          DebugNodeDrawList(RainGuiWindow* window, const ImDrawList* draw_list, const char* label);
    RAINGUI_API void          DebugNodeDrawCmdShowMeshAndBoundingBox(ImDrawList* out_draw_list, const ImDrawList* draw_list, const ImDrawCmd* draw_cmd, bool show_mesh, bool show_aabb);
    RAINGUI_API void          DebugNodeStorage(RainGuiStorage* storage, const char* label);
    RAINGUI_API void          DebugNodeTabBar(RainGuiTabBar* tab_bar, const char* label);
    RAINGUI_API void          DebugNodeTable(RainGuiTable* table);
    RAINGUI_API void          DebugNodeTableSettings(RainGuiTableSettings* settings);
    RAINGUI_API void          DebugNodeWindow(RainGuiWindow* window, const char* label);
    RAINGUI_API void          DebugNodeWindowSettings(RainGuiWindowSettings* settings);
    RAINGUI_API void          DebugNodeWindowsList(ImVector<RainGuiWindow*>* windows, const char* label);
    RAINGUI_API void          DebugNodeViewport(RainGuiViewportP* viewport);
    RAINGUI_API void          DebugRenderViewportThumbnail(ImDrawList* draw_list, RainGuiViewportP* viewport, const ImRect& bb);

} // namespace RainGui


//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas internal API
//-----------------------------------------------------------------------------

// This structure is likely to evolve as we add support for incremental atlas updates
struct ImFontBuilderIO
{
    bool    (*FontBuilder_Build)(ImFontAtlas* atlas);
};

// Helper for font builder
RAINGUI_API const ImFontBuilderIO* ImFontAtlasGetBuilderForStbTruetype();
RAINGUI_API void      ImFontAtlasBuildInit(ImFontAtlas* atlas);
RAINGUI_API void      ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent);
RAINGUI_API void      ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* stbrp_context_opaque);
RAINGUI_API void      ImFontAtlasBuildFinish(ImFontAtlas* atlas);
RAINGUI_API void      ImFontAtlasBuildRender8bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned char in_marker_pixel_value);
RAINGUI_API void      ImFontAtlasBuildRender32bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned int in_marker_pixel_value);
RAINGUI_API void      ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_multiply_factor);
RAINGUI_API void      ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride);

//-----------------------------------------------------------------------------
// [SECTION] Test Engine specific hooks (raingui_test_engine)
//-----------------------------------------------------------------------------

#ifdef RAINGUI_ENABLE_TEST_ENGINE
extern void         RainGuiTestEngineHook_ItemAdd(RainGuiContext* ctx, const ImRect& bb, RainGuiID id);
extern void         RainGuiTestEngineHook_ItemInfo(RainGuiContext* ctx, RainGuiID id, const char* label, RainGuiItemStatusFlags flags);
extern void         RainGuiTestEngineHook_IdInfo(RainGuiContext* ctx, RainGuiDataType data_type, RainGuiID id, const void* data_id);
extern void         RainGuiTestEngineHook_IdInfo(RainGuiContext* ctx, RainGuiDataType data_type, RainGuiID id, const void* data_id, const void* data_id_end);
extern void         RainGuiTestEngineHook_Log(RainGuiContext* ctx, const char* fmt, ...);
#define RAINGUI_TEST_ENGINE_ITEM_ADD(_BB,_ID)                 if (g.TestEngineHookItems) RainGuiTestEngineHook_ItemAdd(&g, _BB, _ID)               // Register item bounding box
#define RAINGUI_TEST_ENGINE_ITEM_INFO(_ID,_LABEL,_FLAGS)      if (g.TestEngineHookItems) RainGuiTestEngineHook_ItemInfo(&g, _ID, _LABEL, _FLAGS)   // Register item label and status flags (optional)
#define RAINGUI_TEST_ENGINE_LOG(_FMT,...)                     if (g.TestEngineHookItems) RainGuiTestEngineHook_Log(&g, _FMT, __VA_ARGS__)          // Custom log entry from user land into test log
#define RAINGUI_TEST_ENGINE_ID_INFO(_ID,_TYPE,_DATA)          if (g.TestEngineHookIdInfo == id) RainGuiTestEngineHook_IdInfo(&g, _TYPE, _ID, (const void*)(_DATA));
#define RAINGUI_TEST_ENGINE_ID_INFO2(_ID,_TYPE,_DATA,_DATA2)  if (g.TestEngineHookIdInfo == id) RainGuiTestEngineHook_IdInfo(&g, _TYPE, _ID, (const void*)(_DATA), (const void*)(_DATA2));
#else
#define RAINGUI_TEST_ENGINE_ITEM_ADD(_BB,_ID)                 do { } while (0)
#define RAINGUI_TEST_ENGINE_ITEM_INFO(_ID,_LABEL,_FLAGS)      do { } while (0)
#define RAINGUI_TEST_ENGINE_LOG(_FMT,...)                     do { } while (0)
#define RAINGUI_TEST_ENGINE_ID_INFO(_ID,_TYPE,_DATA)          do { } while (0)
#define RAINGUI_TEST_ENGINE_ID_INFO2(_ID,_TYPE,_DATA,_DATA2)  do { } while (0)
#endif

//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // #ifndef RAINGUI_DISABLE
