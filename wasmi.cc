#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <immintrin.h>

#ifndef waAssertf
#include <cassert>
#define waAssertf(cond, msg) assert((cond) && (msg))
#endif

#ifdef WASMI_USE_EXCEPTIONS
#include <stdexcept>
#define waThrow(msg) throw std::runtime_error(msg)
#define waTry try
#define waCatch catch (...)
#else
#define waThrow(msg) waAssertf(false, msg)
#define waTry if (true)
#define waCatch else
#endif

namespace wasmi {

template <typename... _Types>
struct WaTypePack {};

namespace typepack_helpers {

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
	static constexpr size_t value = alignof(_T0) > MaxAlignHelper<_Ts...>::value ? alignof(_T0) : MaxAlignHelper<_Ts...>::value;
};

template <>
struct MaxAlignHelper<> {
	static constexpr size_t value = 0;
};

template <typename _TypePack>
struct MaxAlign;

template <typename... _Types>
struct MaxAlign<WaTypePack<_Types...>> {
	static constexpr size_t value = MaxAlignHelper<_Types...>::value;
};

/// TypeIndex

template <typename _Type, typename... _Types>
struct TypeIndexHelper;

template <typename _Type, typename _T0, typename... _Ts>
struct TypeIndexHelper<_Type, _T0, _Ts...> {
	static constexpr size_t value = std::is_same_v<_Type, _T0> ? 0 : 1 + TypeIndexHelper<_Type, _Ts...>::value;
};

template <typename _Type>
struct TypeIndexHelper<_Type>; // Incomplete specialization for the base case when _Type is not found in the pack.

template <typename _Type, typename _TypePack>
struct TypeIndex;

template <typename _Type, typename... _Types>
struct TypeIndex<_Type, WaTypePack<_Types...>> {
	static constexpr size_t value = TypeIndexHelper<_Type, _Types...>::value;
};

}

template <typename _IndexType, typename... _Types>
class WaUnion;

template <typename _IndexType, typename... _Types>
class WaUnion<_IndexType, WaTypePack<_Types...>> {
	using TypePack = WaTypePack<_Types...>;
	using IndexType = _IndexType;
public:
	WaUnion() : m_typeIndex(0) {}
	~WaUnion() = default;

	template <typename _Type, typename... _Args>
	void Emplace(_Args&&... args) {
		static_assert(typepack_helpers::TypeIndex<_Type, TypePack>::value < sizeof...(_Types), "Type not found in TypePack");
		m_typeIndex = typepack_helpers::TypeIndex<_Type, TypePack>::value;
		new (m_storage) _Type(std::forward<_Args>(args)...);
	}

	template <typename _Type>
	bool Is() const {
		static_assert(typepack_helpers::TypeIndex<_Type, TypePack>::value < sizeof...(_Types), "Type not found in TypePack");
		return m_typeIndex == typepack_helpers::TypeIndex<_Type, TypePack>::value;
	}

	template <typename _Type>
	_Type& Get() {
		if (!Is<_Type>()) {
			waThrow("Type mismatch in WaUnion::Get");
		}
		GetUnsafe<_Type>(); // Ensure the type is valid before returning
	}

	template <typename _Type>
	_Type& GetUnsafe() {
		static_assert(typepack_helpers::TypeIndex<_Type, TypePack>::value < sizeof...(_Types), "Type not found in TypePack");
		return *reinterpret_cast<_Type*>(m_storage);
	}

	template <typename _Type>
	const _Type& GetUnsafe() const {
		static_assert(typepack_helpers::TypeIndex<_Type, TypePack>::value < sizeof...(_Types), "Type not found in TypePack");
		return *reinterpret_cast<const _Type*>(m_storage);
	}

protected:
	void* GetStorage() { return m_storage; }
	const void* GetStorage() const { return m_storage; }
private:
	IndexType m_typeIndex;
	alignas(typepack_helpers::MaxAlign<TypePack>::value) char m_storage[typepack_helpers::MaxSize<TypePack>::value];
};

template <typename _IndexType, typename... _Types>
class WaUnion : public WaUnion<_IndexType, WaTypePack<_Types...>> {
	using Base = WaUnion<_IndexType, WaTypePack<_Types...>>;
public:
	WaUnion() : Base() {}
	~WaUnion() = default;

	using Base::Emplace;
	using Base::Is;
	using Base::Get;
	using Base::GetUnsafe;

private:
};

template <typename _Type>
class WaUnique {
private:
	_Type* m_ptr;
};

template <typename _Type>
class WaSharedRef {

};

template <typename _Type>
class WaWeakRef {

private:
	_Type* m_ptr;
};

template <typename _Iterator>
class WaRange {
public:
	WaRange(_Iterator begin, _Iterator end) : m_begin(begin), m_end(end) {}
	_Iterator begin() const { return m_begin; }
	_Iterator end() const { return m_end; }
private:
	_Iterator m_begin;
	_Iterator m_end;
};

/// Wasm Types

enum WaNumberType {
	ntF64, 		// = 0x7C
	ntF32, 		// = 0x7D
	ntI64, 		// = 0x7E
	ntI32, 		// = 0x7F
};

enum WaVectorType {
	vtV128,		// = 0x7B
};

enum WaHeapType {
	htExn, 		// = 0x69,
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
	rtExn, 		// = 0x69,
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


#define WASMI_CONCAT_EX(A, B) A ## B
#define WASMI_CONCAT(A, B) WASMI_CONCAT_EX(A, B)

#define WASMI_PRIMITIVE_MACRO_SELECTOR(_1, _2, _3, _4, ...) _4
#define WASMI_DISPATCH(DISPATCHER, ...) WASMI_CONCAT(DISPATCHER, WASMI_PRIMITIVE_MACRO_SELECTOR(__VA_ARGS__, _3, _2, _1))

#define COMMA ,

#define WASMI_PRIMITIVE_TYPES(M) \
	M(WaI32, ntI32, int32_t) \
	M(",") M(WaI64, ntI64, int64_t) \
	M(",") M(WaF32, ntF32, float) \
	M(",") M(WaF64, ntF64, double) \
	M(",") M(WaV128, vtV128, __m128) \
	M(",") M(WaAnyRef, rtAny, int32_t) \
	M(",") M(WaEqRef, rtEq, int32_t) \
	M(",") M(WaI32Ref, rtI31, int32_t) \
	M(",") M(WaStructRef, rtStruct, int32_t) \
	M(",") M(WaArrayRef, rtArray, int32_t) \
	M(",") M(WaNullRef, rtNone, int32_t) \
	M(",") M(WaFuncRef, rtFunc, int32_t) \
	M(",") M(WaNullFuncRef, rtNofunc, int32_t) \
	M(",") M(WaExnRef, rtExn, int32_t) \
	M(",") M(WaNullExnRef, rtNoexn, int32_t) \
	M(",") M(WaExternRef, rtExtern, int32_t) \
	M(",") M(WaNullExternRef, rtNoextern, int32_t)

#define WASMI_DECLARE_PRIMITIVE_TYPE_ALIAS(_Alias, _Tag, _Type) \
	using _Alias = WaPrimitiveType<_Tag, _Type>;

#define ALIAS_DISPATCH_1(_Comma)
#define ALIAS_DISPATCH_3(_Alias, _Tag, _Type) WASMI_DECLARE_PRIMITIVE_TYPE_ALIAS(_Alias, _Tag, _Type)
#define ALIAS_GEN(...) WASMI_DISPATCH(ALIAS_DISPATCH, __VA_ARGS__)(__VA_ARGS__)

WASMI_PRIMITIVE_TYPES(ALIAS_GEN)

#define TYPEPACK_DISPATCH_1(_Comma) COMMA
#define TYPEPACK_DISPATCH_3(_Alias, _Tag, _Type) _Alias
#define TYPEPACK_GEN(...) WASMI_DISPATCH(TYPEPACK_DISPATCH, __VA_ARGS__)(__VA_ARGS__)

using WaValueType = WaUnion<WASMI_PRIMITIVE_TYPES(TYPEPACK_GEN)>;

class WaParam {
public:
private:
	WaValueType m_value;
};

class WaReturn {
public:
private:
	WaValueType m_value;
};

class WaLocal {
public:
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
	WaFunc(WaWeakRef<WaType> type);
private:
	WaWeakRef<WaType> m_type;
};

class WaTable;
class WaMemory;
class WaImport;
class WaModule;

}

int main(int argc, char *argv[]) {
	(void)argc; (void)argv;
	return 0;
}
