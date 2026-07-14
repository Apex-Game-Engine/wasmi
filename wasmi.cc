#include <cstdint>
#include <intrin.h>

namespace wasmi {

template <typename... _Types>
struct WaTypePack {
	using Types = _Types...;
};

namespace TypePack {

/// MaxSize

template <typename... _Ts>
struct MaxSizeHelper;

template <typename _T0, typename... _Ts>
struct MaxSizeHelper<_T0, _Ts...> {
	static constexpr size_t value = sizeof(_T0) > MaxSizeHelper<_Ts...>::value ? sizeof(_T0) : MaxSizeHelper<_Ts...>::value;
};

template <>
struct MaxSizeHelper<> {
	static constexpr size_t value = 0;
};

template <typename _TypePack>
struct MaxSize;

template <typename... _Types>
struct MaxSize<WaTypePack<_Types...>> {
	static constexpr size_t value = MaxSizeHelper<_Types...>::value;
};

static_assert(MaxSize<WaTypePack<int32_t, int64_t, float, double>>::value == sizeof(double), "MaxSize calculation is incorrect");

/// MaxAlign

template <typename... _Types>
struct MaxAlignHelper;

template <typename _T0, typename... _Ts>
struct MaxAlignHelper<_T0, _Ts...> {
	static constexpr size_t value = alignof(_T0) > MaxAlignHelper<_Ts...>::template value ? alignof(_T0) : MaxAlignHelper<_Ts...>::template value;
};

template <>
struct MaxAlignHelper<> {
	static constexpr size_t value = 0;
};

template <typename _TypePack>
struct MaxAlign;

template <typename... _Types>
struct MaxAlign<WaTypePack<_Types...>> {
	static constexpr size_t value = MaxAlignHelper<_Types...>::template value;
};

/// TypeIndex

template <typename _Type, typename... _Types>
struct TypeIndexHelper;

template <typename _Type, typename _T0, typename... _Ts>
struct TypeIndexHelper<_Type, _T0, _Ts...> {
	static constexpr size_t value = std::is_same_v<_Type, _T0> ? 0 : 1 + TypeIndexHelper<_Type, _Ts...>::template value;
};

template <typename _Type>
struct TypeIndexHelper<_Type> {
	static constexpr size_t value = 0;
};

template <typename _Type, typename _TypePack>
struct TypeIndex;

}

template <typename _TypePack>
class WaUnionStorage {
public:
	WaUnionStorage() = default;

	template <typename _Type>
	WaUnionStorage()
private:
	alignas(TypePack::MaxAlign<_TypePack>::value) char m_storage[TypePack::MaxSize<_TypePack>::value];
};

template <typename... _Types>
class WaUnion : public WaUnion<WaTypePack<_Types...>> {
public:
};

template <typename... _Types>
class WaUnion<WaTypePack<_Types...>> {
	using TypePack = WaTypePack<_Types...>;
public:
protected:
	
};

template <typename _Type>
class WaUnique;

template <typename _Type>
class WaSharedRef;

template <typename _Type>
class WaWeakRef;


enum WaNumberType {
	ntI32, 		// = 0x7F
	ntI64, 		// = 0x7E
	ntF32, 		// = 0x7D
	ntF64, 		// = 0x7C
};

enum WaVectorType {
	vtV128,		// = 0x7B
};

enum WaHeapType {
	htArray, 	// = 0x6A,
	htStruct, 	// = 0x6B,
	htI31, 		// = 0x6C,
	htEq, 		// = 0x6D,
	htAny, 		// = 0x6E,
	htExtern, 	// = 0x6F,
	htFunc,		// = 0x70,
	htNone, 	// = 0x71,
	htNoextern, // = 0x72,
	htNofunc, 	// = 0x73,
	htNoexn, 	// = 0x74,
};

enum WaRefType {
	rtArray, 	// = 0x6A,
	rtStruct, 	// = 0x6B,
	rtI31, 		// = 0x6C,
	rtEq, 		// = 0x6D,
	rtAny, 		// = 0x6E,
	rtExtern, 	// = 0x6F,
	rtFunc,		// = 0x70,
	rtNone, 	// = 0x71,
	rtNoextern, // = 0x72,
	rtNofunc, 	// = 0x73,
	rtNoexn, 	// = 0x74,
};

template <uint32_t _Tag, typename _UnderlyingType>
struct WaPrimitiveType {
	using UnderlyingType = _UnderlyingType;
	static constexpr uint32_t Tag = _Tag;
	_UnderlyingType value;
};

#define WA_PRIMITIVE_TYPES(MACRO) \
	MACRO(WaI32, ntI32, int32_t) \
	MACRO(WaI64, ntI64, int64_t) \
	MACRO(WaF32, ntF32, float) \
	MACRO(WaF64, ntF64, double) \
	MACRO(WaV128, vtV128, __m128) \
	MACRO(WaAnyRef, rtAny, int32_t) \
	MACRO(WaEqRef, rtEq, int32_t) \
	MACRO(WaI32Ref, rtI31, int32_t) \
	MACRO(WaStructRef, rtStruct, int32_t) \
	MACRO(WaArrayRef, rtArray, int32_t) \
	MACRO(WaNullRef, rtNone, int32_t) \
	MACRO(WaFuncRef, rtFunc, int32_t) \
	MACRO(WaNullFuncRef, rtNofunc, int32_t) \
	MACRO(WaExnRef, rtExn, int32_t) \
	MACRO(WaNullExnRef, rtNoexn, int32_t) \
	MACRO(WaExternRef, rtExtern, int32_t) \
	MACRO(WaNullExternRef, rtNoextern, int32_t)

#define WA_DECLARE_PRIMITIVE_TYPE_ALIAS(_Name, _Tag, _UnderlyingType) \
	using _Name = WaPrimitiveType<_Tag, _UnderlyingType>;

WA_PRIMITIVE_TYPES(WA_DECLARE_PRIMITIVE_TYPE_ALIAS)

#define WA_PRIMITIVE_TYPE_ALIAS_LIST(_Name, _Tag, _UnderlyingType) \
	_Name,

using WaValueType = WaUnion<WA_PRIMITIVE_TYPES(WA_PRIMITIVE_TYPE_ALIAS_LIST)>;

class WaParam {
private:
	WaValueType m_value;
};

class WaType {
public:
	WaType(WaRange<WaParam> params, WaRange<WaReturn> returns, WaRange<WaLocal> locals);
private:
	WaRange<WaParam> m_params;
};

class WaFunc {
public:
private:
	WaWeakRef<WaType> m_type;
};

class WaTable;
class WaMemory;
class WaImport;
class WaModule;

}

int main(int argc, char *argv[]) {
	return 0;
}
