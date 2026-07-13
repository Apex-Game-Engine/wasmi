#include <cstdint>

namespace wasmi {

enum WaPrimitive {
	eI32,
	eI64,
	eF32,
	eF64
};

using wa_byte = std::uint8_t:
using wa_i32 = std::int32_t;
using wa_i64 = std::int64_t;
using wa_f32 = std::float_t;
using wa_f64 = std::double_t;

template <typename... _Types>
struct WaTypePack;

template <typename _Tag, typename... _Types>
class WaTaggedUnion : public WaTaggedUnion<_Tag, WaTypePack<_Types...>> {
	
};

template <typename... _Types>
class WaUnion : public WaUnion<WaTypePack<_Types...> {
};

template <typename _Type>
class WaUnique;

template <typename _Type>
class WaSharedRef;

template <typename _Type>
class WaWeakRef;

using WaPrimTypePack = WaTypePack<wa_i32, wa_i64, wa_f32, wa_f64>;

class WaParam {
private:
	WaUnion<WaPrimitive, WaPrimTypePack> m_prim;
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
