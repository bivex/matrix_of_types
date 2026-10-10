import sys
import os

def make_table(headers, rows, custom_widths=None):
    col_widths = [len(h) for h in headers]
    for row in rows:
        if row == ["---"]:
            continue
        for i, cell in enumerate(row):
            if i < len(col_widths):
                col_widths[i] = max(col_widths[i], len(str(cell)))
            else:
                col_widths.append(len(str(cell)))
                
    if custom_widths:
        for i, w in enumerate(custom_widths):
            if i < len(col_widths):
                col_widths[i] = max(col_widths[i], w)

    sep = "+" + "+".join("-" * (w + 2) for w in col_widths) + "+"
    double_sep = "+" + "+".join("=" * (w + 2) for w in col_widths) + "+"
    
    lines = []
    lines.append(sep)
    header_line = "|" + "|".join(f" {headers[i]:<{col_widths[i]}} " for i in range(len(headers))) + "|"
    lines.append(header_line)
    lines.append(double_sep)
    
    for row in rows:
        if row == ["---"]:
            lines.append(sep)
            continue
        row_line = "|" + "|".join(f" {str(row[i]):<{col_widths[i]}} " for i in range(len(row))) + "|"
        lines.append(row_line)
        
    lines.append(sep)
    return "\n".join(lines)

headers_main = ["Category / Concept", "Bits / Range", "C (C17/C23)", "C++ (C++20/23)", "Rust", "Go", "Java", "C#", "Python", "TypeScript"]

# 1. Signed Integers
rows_signed_int = [
    ["8-bit Signed", "8b [-128..127]", "int8_t / signed char", "std::int8_t", "i8", "int8", "byte", "sbyte", "int", "number"],
    ["16-bit Signed", "16b [-32K..32K]", "int16_t / short", "std::int16_t", "i16", "int16", "short", "short", "int", "number"],
    ["32-bit Signed", "32b [-2.1B..2.1B]", "int32_t / int", "std::int32_t", "i32", "int32 / rune", "int", "int", "int", "number"],
    ["64-bit Signed", "64b [-9.2E18..9.2E18]", "int64_t / long long", "std::int64_t", "i64", "int64", "long", "long", "int", "number / bigint"],
    ["128-bit Signed", "128b [~-1.7E38..]", "__int128_t (ext)", "__int128_t (ext)", "i128", "math/big.Int", "BigInteger", "Int128", "int", "bigint"],
    ["Arch-sized Signed", "32/64b (Ptr Size)", "intptr_t / ptrdiff_t", "std::intptr_t", "isize", "int", "N/A (long)", "nint", "int", "number"],
    ["Arbitrary Precision", "Unbounded", "mpz_t (GMP)", "boost::multiprecision", "num_bigint::BigInt", "big.Int (math/big)", "BigInteger", "BigInteger", "int (native)", "bigint (native)"]
]

# 2. Unsigned Integers
rows_unsigned_int = [
    ["8-bit Unsigned", "8b [0..255]", "uint8_t / uchar", "std::uint8_t", "u8", "uint8 / byte", "N/A (short/int)", "byte", "int", "number"],
    ["16-bit Unsigned", "16b [0..65535]", "uint16_t / ushort", "std::uint16_t", "u16", "uint16", "char (16b unsigned)", "ushort", "int", "number"],
    ["32-bit Unsigned", "32b [0..4.29B]", "uint32_t / uint", "std::uint32_t", "u32", "uint32", "Integer.toUnsigned*", "uint", "int", "number"],
    ["64-bit Unsigned", "64b [0..1.84E19]", "uint64_t / ulonglong", "std::uint64_t", "u64", "uint64", "Long.toUnsigned*", "ulong", "int", "bigint"],
    ["128-bit Unsigned", "128b [0..3.4E38]", "__uint128_t (ext)", "__uint128_t (ext)", "u128", "math/big.Int", "BigInteger", "UInt128", "int", "bigint"],
    ["Arch-sized Unsigned", "32/64b (Size/Ptr)", "size_t / uintptr_t", "std::size_t", "usize", "uint / uintptr", "N/A (int/long)", "nuint", "int", "number"]
]

# 3. Floating-Point & Numeric
rows_float = [
    ["16-bit Float (Half)", "16b IEEE 754", "_Float16 (C23)", "std::float16_t", "f16 (nightly/crate)", "N/A", "Float.floatToFloat16", "Half", "float / numpy.f16", "number"],
    ["16-bit Brain Float", "16b (8e, 7m)", "__bf16 (ext)", "std::bfloat16_t", "bfloat16 (crate)", "N/A", "N/A (DL libs)", "N/A (Tensors)", "torch.bfloat16", "N/A"],
    ["32-bit Float (Single)", "32b IEEE 754", "float", "float", "f32", "float32", "float", "float", "float (64b)", "number (64b)"],
    ["64-bit Float (Double)", "64b IEEE 754", "double", "double", "f64", "float64", "double", "double", "float (native)", "number (native)"],
    ["Extended Float (80/128b)", "80/128b IEEE", "long double / _Float128", "long double / float128", "f128 (nightly)", "big.Float", "BigDecimal", "N/A (decimal)", "decimal.Decimal", "N/A (library)"],
    ["Decimal (Fixed-Point)", "Exact decimal base-10", "N/A (custom/decNumber)", "N/A (boost::decimal)", "rust_decimal::Decimal", "shopspring/decimal", "BigDecimal", "decimal (128b)", "decimal.Decimal", "decimal.js"],
    ["Rational / Fraction", "Exact ratio (num/den)", "N/A (lib)", "boost::rational<T>", "num_rational::Ratio<T>", "big.Rat (math/big)", "Commons Math Fraction", "BigRational (NuGet)", "fractions.Fraction", "fraction.js"],
    ["Complex Numbers", "Real + Imag pairs", "double _Complex", "std::complex<T>", "num_complex::Complex", "complex64/128", "Commons Math Complex", "Complex", "complex (1+2j)", "N/A (library)"]
]

# 4. Boolean & Textual Characters
rows_bool_char = [
    ["Boolean", "1b / 1B (true/false)", "bool (_Bool)", "bool", "bool", "bool", "boolean (Boolean)", "bool (Boolean)", "bool", "boolean"],
    ["ASCII / Byte Char", "8-bit char/byte", "char", "char", "u8 (b'a')", "byte (uint8)", "byte", "byte", "bytes (b'a')", "string"],
    ["Unicode Code Point", "21/32b Unicode scalar", "char32_t (<uchar.h>)", "char32_t", "char (32-bit)", "rune (int32)", "int / char (16b)", "Rune / char (16b)", "str (len 1)", "string"],
    ["16-bit Unicode Unit", "UTF-16 code unit", "char16_t", "char16_t", "u16", "uint16", "char", "char", "str", "string"]
]

# 5. Strings & Byte Sequences
rows_string_bytes = [
    ["Owned Dynamic String", "UTF-8 / UTF-16 heap", "char* / char[] (manual)", "std::string", "String (UTF-8)", "string (immutable)", "String (UTF-16)", "string (UTF-16)", "str (immutable)", "string (immutable)"],
    ["Borrowed String View", "Ptr + Len slice", "const char*", "std::string_view", "&str (borrowed UTF-8)", "string", "CharSequence", "ReadOnlySpan<char>", "memoryview / str", "string"],
    ["Mutable String Buffer", "Capacity-growing", "open_memstream / buf", "std::string", "String", "strings.Builder", "StringBuilder", "StringBuilder", "io.StringIO", "string[] / Array"],
    ["Raw Byte Array / Buffer", "Contiguous binary", "uint8_t[] / void*", "std::vector<uint8_t>", "Vec<u8> / &[u8]", "[]byte", "byte[]", "byte[] / Span<byte>", "bytes / bytearray", "Uint8Array / Buffer"],
    ["C-Compatible String", "Null-terminated '\\0'", "char*", "const char*", "std::ffi::CString/CStr", "*C.char (cgo)", "byte[] + '\\0'", "sbyte* / IntPtr", "ctypes.c_char_p", "N/A (Buffer)"]
]

# 6. Collections & Sequence Types
rows_sequences = [
    ["Fixed-Size Array", "Stack/Inline N items", "T[N]", "std::array<T, N> / T[N]", "[T; N]", "[N]T", "T[]", "T[] / T[N] inline", "tuple / list", "[T, ...N] (tuple)"],
    ["Dynamic Array / Vector", "Heap resizable list", "struct { T* d; sz n; }", "std::vector<T>", "Vec<T>", "[]T (slice)", "ArrayList<T>", "List<T>", "list", "Array<T> / T[]"],
    ["Array Slice / View", "Non-owning fat pointer", "struct { T* p; sz len; }", "std::span<T>", "&[T] / &mut [T]", "[]T", "List.subList", "Span<T> / Memory<T>", "memoryview / slice", "TypedArray.subarray"],
    ["Heterogeneous Tuple", "Multi-type product", "struct { ... }", "std::tuple<T1, T2>", "(T1, T2)", "struct { A T1; B T2 }", "record / Pair", "(T1, T2) ValueTuple", "tuple", "[T1, T2]"],
    ["Double-Ended Queue", "FIFO / LIFO dequeue", "N/A (linked list)", "std::deque<T>", "VecDeque<T>", "container/list", "ArrayDeque<T>", "Queue<T> / Deque", "collections.deque", "Array (push/shift)"],
    ["Linked List", "Node-pointer chain", "struct Node* next", "std::list<T>", "LinkedList<T>", "container/list", "LinkedList<T>", "LinkedList<T>", "collections.deque", "custom class"]
]

# 7. Associative & Set Collections
rows_associative = [
    ["Hash Map / Dictionary", "O(1) key-value table", "N/A (uthash / lib)", "std::unordered_map<K,V>", "HashMap<K, V>", "map[K]V", "HashMap<K, V>", "Dictionary<TKey, TVal>", "dict", "Map<K, V> / Record"],
    ["Sorted Map (Tree)", "O(log N) ordered key", "N/A (red-black tree)", "std::map<K, V>", "BTreeMap<K, V>", "N/A (third-party)", "TreeMap<K, V>", "SortedDictionary<K, V>", "dict (insertion-ord)", "N/A (third-party)"],
    ["Hash Set", "O(1) unique elements", "N/A (hash table keys)", "std::unordered_set<T>", "HashSet<T>", "map[T]struct{}", "HashSet<T>", "HashSet<T>", "set", "Set<T>"],
    ["Sorted Set (Tree)", "O(log N) unique order", "N/A (tree nodes)", "std::set<T>", "BTreeSet<T>", "N/A (third-party)", "TreeSet<T>", "SortedSet<T>", "N/A (sorted list)", "N/A (third-party)"],
    ["Bit Set / Bit Array", "Dense bit flags", "uint32_t flags / bitfields", "std::bitset<N>", "bitvec::BitVec", "big.Int / math/bits", "BitSet", "BitArray", "int (bitwise)", "Uint8Array / bigint"]
]

# 8. Composite, Custom & Object Types
rows_composites = [
    ["Plain Data Struct", "Layout of fields", "struct Foo { ... }", "struct Foo { ... }", "struct Foo { ... }", "type Foo struct { ... }", "record Foo(...) (Java16)", "struct / record struct", "@dataclass / NamedTuple", "interface Foo / type Foo"],
    ["OOP Class", "Fields + methods + VMT", "N/A (struct + vtable)", "class Foo { ... }", "struct + impl Trait", "struct + methods", "class Foo { ... }", "class / record class", "class Foo: ...", "class Foo { ... }"],
    ["Interface / Trait", "Contract / Polymorphism", "struct of func ptrs", "abstract class / concept", "trait Foo { ... }", "type Foo interface", "interface Foo { ... }", "interface IFoo { ... }", "abc.ABC / Protocol", "interface Foo"],
    ["Anonymous Object", "Inline untyped fields", "struct { ... }", "struct { ... }", "N/A (tuples/structs)", "struct { ... }", "record (Java 16+)", "new { Foo = 1 }", "dict / SimpleNamespace", "{ foo: 1 } (object lit)"]
]

# 9. Algebraic Data Types, Enums & Variants
rows_enums_variants = [
    ["C-Style Enum", "Named integer constants", "enum Color { RED, ... }", "enum class Color", "enum Color (fieldless)", "const (iota)", "enum Color", "enum Color", "Enum (enum module)", "enum Color / Union"],
    ["Tagged Union / Variant", "Sum type (Type A | B)", "tagged union (manual)", "std::variant<Ts...>", "enum Color { R(u8)... }", "any + type switch", "sealed interface + rec", "discriminated union", "Union[A, B] / A | B", "A | B (discriminated)"],
    ["Untagged Union", "Overlapping raw memory", "union Data { int i; }", "union Data { int i; }", "union Data (unsafe)", "unsafe.Pointer", "N/A", "[StructLayout(Explicit)]", "ctypes.Union", "N/A"]
]

# 10. Memory, Pointers & Smart References
rows_memory = [
    ["Raw Untyped Pointer", "Direct memory address", "void*", "void*", "*const () / *mut ()", "unsafe.Pointer", "N/A", "void* (unsafe)", "ctypes.c_void_p", "N/A"],
    ["Raw Typed Pointer", "Address to typed data", "T*", "T*", "*const T / *mut T", "*T", "N/A", "T* (unsafe)", "ctypes.POINTER(T)", "N/A"],
    ["Safe Reference", "Guaranteed alias/borrow", "N/A (uses T*)", "T& / const T&", "&T / &mut T (borrow)", "*T (GC safe)", "Object ref (GC)", "ref T / in T / out T", "Object ref (GC)", "Object ref (GC)"],
    ["Unique Ownership", "RAII Single-owner heap", "manual malloc/free", "std::unique_ptr<T>", "Box<T>", "N/A (GC managed)", "N/A (GC managed)", "N/A (GC managed)", "N/A (GC managed)", "N/A (GC managed)"],
    ["Shared Ref-Counted", "Atomic/Shared refcount", "manual atomic refcnt", "std::shared_ptr<T>", "Rc<T> / Arc<T>", "N/A (GC managed)", "N/A (GC managed)", "N/A (GC managed)", "N/A (GC managed)", "N/A (GC managed)"],
    ["Weak Non-Owning Ref", "Ref without retain cycle", "N/A", "std::weak_ptr<T>", "std::rc::Weak / Arc::Weak", "N/A (GC)", "WeakReference<T>", "WeakReference<T>", "weakref.ref", "WeakRef<T>"]
]

# 11. Optionality, Result & Special Types
rows_special = [
    ["Null / Nil Value", "Absence of pointer/ref", "NULL ((void*)0)", "nullptr", "N/A (None in Option)", "nil", "null", "null", "None", "null / undefined"],
    ["Optional / Maybe Type", "Value or Nothing", "pointer / sentinel", "std::optional<T>", "Option<T>", "*T (or custom generic)", "Optional<T>", "Nullable<T> / T?", "Optional[T] (T | None)", "T | undefined / T?"],
    ["Result / Error Type", "Value or Error object", "int status / errno", "std::expected<T, E>", "Result<T, E>", "(T, error) multi-val", "Exception (Result lib)", "Exception / Result lib", "Exception (try/except)", "throw Error / Result"],
    ["Unit / Void Type", "No value / Empty tuple", "void", "void", "() (unit)", "struct{}", "void (Void boxed)", "void", "None (NoneType)", "void / undefined"],
    ["Bottom / Never Type", "Does not return", "_Noreturn (C11)", "[[noreturn]] / unreachable", "! (never)", "panic / infinite loop", "throw Exception", "void [DoesNotReturn]", "typing.Never / NoReturn", "never"],
    ["Top / Universal Type", "Accepts any value", "void*", "std::any", "Box<dyn Any>", "any (interface{})", "Object", "object", "object / Any", "unknown / any"]
]

# 12. Functions, Closures & Callables
rows_functions = [
    ["Function Pointer", "Code memory address", "R (*)(Args...)", "R (*)(Args...)", "fn(Args...) -> R", "func(Args...) R", "Method reference", "delegate*<Args..., R>", "Callable[[...], R]", "(args) => R"],
    ["Closure / Callable", "Env captures + code", "context ptr + fn ptr", "std::function<R(Args)>", "impl Fn/FnMut/FnOnce", "func(Args...) R", "Function<T, R>", "Func<T, R> / Action<T>", "Callable (lambda/func)", "(args) => R / Function"]
]

# 13. Concurrency, Async & Threading Primitives
rows_concurrency = [
    ["Future / Promise / Task", "Deferred computation", "thrd_t / callback", "std::future<T>", "impl Future<Output=T>", "chan T / goroutine", "CompletableFuture<T>", "Task<T> / ValueTask<T>", "asyncio.Task[T]", "Promise<T>"],
    ["Concurrency Channel", "Message passing queue", "ringbuf + mutex", "boost::fibers::channel", "tokio::sync::mpsc", "chan T (native)", "BlockingQueue<T>", "Channel<T>", "asyncio.Queue[T]", "AsyncQueue / emitter"],
    ["Mutual Exclusion (Mutex)", "Lock for exclusive access", "mtx_t (threads.h)", "std::mutex", "std::sync::Mutex<T>", "sync.Mutex", "ReentrantLock", "lock(obj) / Monitor", "threading.Lock", "N/A (event loop)"],
    ["Read-Write Lock", "Multi-read single-write", "pthread_rwlock_t", "std::shared_mutex", "std::sync::RwLock<T>", "sync.RWMutex", "ReentrantReadWriteLock", "ReaderWriterLockSlim", "threading.RLock", "N/A"],
    ["Atomic Integer / Ptr", "Lock-free hardware CAS", "_Atomic(int32_t)", "std::atomic<T>", "AtomicI32 / AtomicUsize", "sync/atomic.Int32", "AtomicInteger / VarHandle", "Interlocked / Volatile", "N/A (GIL protected)", "Atomics (SharedArrayBuf)"],
    ["OS Thread Handle", "OS schedulable thread", "thrd_t", "std::jthread / thread", "std::thread::JoinHandle", "N/A (M:N runtime)", "Thread (OS / Virtual)", "System.Threading.Thread", "threading.Thread", "Worker (Node / Web)"]
]

# 14. SIMD & Hardware Vector Types
rows_simd = [
    ["128-bit Vector (4xf32)", "128-bit SIMD lane", "__m128 (SSE)", "__m128 / std::simd", "core::simd::f32x4", "internal asm", "FloatVector (SPECIES_128)", "Vector128<float>", "numpy.ndarray (C)", "Wasm v128 / WebGL"],
    ["256-bit Vector (8xf32)", "256-bit SIMD lane", "__m256 (AVX)", "__m256 / std::simd", "core::simd::f32x8", "internal asm", "FloatVector (SPECIES_256)", "Vector256<float>", "numpy.ndarray (C)", "Wasm SIMD"],
    ["512-bit Vector (16xf32)", "512-bit SIMD lane", "__m512 (AVX-512)", "__m512 / std::simd", "core::simd::f32x16", "internal asm", "FloatVector (SPECIES_512)", "Vector512<float>", "numpy.ndarray (C)", "N/A"],
    ["Portable SIMD Vector", "Hardware agnostic SIMD", "vector extensions", "std::experimental::simd", "std::simd::Simd<T, N>", "N/A", "jdk.incubator.vector", "System.Numerics.Vector<T>", "numpy vectorized", "N/A"]
]

# 15. Temporal Types: Dates, Times & Durations
rows_temporal = [
    ["Monotonic Instant", "Elapsed / clock timing", "struct timespec", "chrono::steady_clock", "std::time::Instant", "time.Time (monotonic)", "java.time.Instant", "Stopwatch.GetTimestamp", "time.monotonic()", "performance.now()"],
    ["UTC Wall Timestamp", "Posix epoch timestamp", "time_t", "chrono::system_clock", "chrono::DateTime<Utc>", "time.Time", "java.time.Instant", "DateTimeOffset.UtcNow", "datetime.datetime.utcnow", "Date.now() / Date"],
    ["Civil / Local Date-Time", "Calendar Y-M-D H:M:S", "struct tm", "chrono::year_month_day", "chrono::NaiveDateTime", "time.Time", "java.time.LocalDateTime", "DateTime", "datetime.datetime", "Temporal.PlainDateTime"],
    ["Timezone-Aware Time", "Timestamp + TZ offset", "struct tm + tzset", "chrono::zoned_time", "chrono::DateTime<Tz>", "time.Time (Location)", "java.time.ZonedDateTime", "DateTimeOffset", "datetime (zoneinfo)", "Temporal.ZonedDateTime"],
    ["Time Span / Duration", "Delta between instants", "time_t / double secs", "chrono::duration", "std::time::Duration", "time.Duration (int64 ns)", "java.time.Duration", "TimeSpan", "datetime.timedelta", "Temporal.Duration / ms"]
]

# 16. SQL Database to Programming Language Type Mappings
headers_sql = ["SQL Type Standard", "PostgreSQL / MySQL", "C / C++", "Rust", "Go", "Java (JDBC)", "C# (.NET)", "Python", "TypeScript"]
rows_sql = [
    ["BOOLEAN", "BOOLEAN / TINYINT(1)", "bool", "bool", "bool", "boolean (Types.BOOLEAN)", "bool (DbType.Boolean)", "bool", "boolean"],
    ["SMALLINT", "INT2 / SMALLINT (16b)", "int16_t", "i16", "int16", "short (Types.SMALLINT)", "short (DbType.Int16)", "int", "number"],
    ["INTEGER", "INT4 / INT (32b)", "int32_t", "i32", "int32", "int (Types.INTEGER)", "int (DbType.Int32)", "int", "number"],
    ["BIGINT", "INT8 / BIGINT (64b)", "int64_t", "i64", "int64", "long (Types.BIGINT)", "long (DbType.Int64)", "int", "bigint / number"],
    ["NUMERIC / DECIMAL", "NUMERIC(p,s) / DECIMAL", "char* / custom dec", "rust_decimal::Decimal", "shopspring/decimal", "BigDecimal (Types.NUMERIC)", "decimal (DbType.Decimal)", "decimal.Decimal", "string / Decimal.js"],
    ["REAL / FLOAT4", "REAL / FLOAT (32b)", "float", "f32", "float32", "float (Types.REAL)", "float (DbType.Single)", "float", "number"],
    ["DOUBLE PRECISION", "FLOAT8 / DOUBLE (64b)", "double", "f64", "float64", "double (Types.DOUBLE)", "double (DbType.Double)", "float", "number"],
    ["VARCHAR / TEXT", "VARCHAR(n) / TEXT", "const char* / string", "String", "string", "String (Types.VARCHAR)", "string (DbType.String)", "str", "string"],
    ["BYTEA / BLOB", "BYTEA / BLOB / VARBINARY", "uint8_t* / vector<u8>", "Vec<u8>", "[]byte", "byte[] / Blob", "byte[] (DbType.Binary)", "bytes", "Uint8Array / Buffer"],
    ["TIMESTAMP (TZ)", "TIMESTAMPTZ / TIMESTAMP", "time_t / chrono", "chrono::DateTime<Utc>", "time.Time", "OffsetDateTime (JDBC 4.2)", "DateTimeOffset", "datetime.datetime", "Date / string (ISO)"],
    ["UUID", "UUID / BINARY(16)", "uuid_t / uint8_t[16]", "uuid::Uuid", "google/uuid.UUID", "java.util.UUID", "Guid (DbType.Guid)", "uuid.UUID", "string"],
    ["JSON / JSONB", "JSONB / JSON", "nlohmann::json", "serde_json::Value", "json.RawMessage / any", "String / JsonNode", "JsonDocument / string", "dict / list", "any / Record<string,any>"]
]

# 17. Extended Systems, Functional & Scripting Languages Matrix
headers_extended = ["Category / Concept", "Zig (0.13+)", "Haskell (GHC 9+)", "OCaml (5.x)", "Swift (5/6)", "Kotlin (2.0)", "PHP (8.2+)", "Ruby (3.2+)"]
rows_extended = [
    ["8-bit Signed Int", "i8", "Int8", "int (tagged)", "Int8", "Byte", "int", "Integer"],
    ["16-bit Signed Int", "i16", "Int16", "int", "Int16", "Short", "int", "Integer"],
    ["32-bit Signed Int", "i32", "Int32", "int32", "Int32", "Int", "int", "Integer"],
    ["64-bit Signed Int", "i64", "Int64", "int64", "Int64 / Int (64b)", "Long", "int", "Integer"],
    ["Unsigned Integers", "u8, u16, u32, u64", "Word8, Word16...", "N/A (uint lib)", "UInt8, UInt16...", "UByte, UShort...", "N/A (signed only)", "N/A (signed only)"],
    ["Arbitrary Precision", "std.math.big.int", "Integer (native)", "Z.t (zarith)", "BigInt (lib)", "BigInteger", "GMP / bcmath", "Integer (auto-upcast)"],
    ["32-bit Float", "f32", "Float", "float (64b)", "Float", "Float", "float (64b)", "Float (64b)"],
    ["64-bit Float", "f64", "Double", "float", "Double", "Double", "float", "Float"],
    ["Boolean", "bool", "Bool", "bool", "Bool", "Boolean", "bool", "TrueClass / FalseClass"],
    ["Unicode Character", "u21 / u8", "Char", "char (8-bit)", "Character", "Char (16-bit)", "string (len 1)", "String (len 1)"],
    ["Dynamic String", "[]const u8", "String / Text", "string", "String (UTF-8)", "String (UTF-16)", "string", "String"],
    ["Dynamic Array / List", "std.ArrayList(T)", "[a] (list) / Vector", "list / array", "Array<T> / [T]", "List<T> / MutableList<T>", "array", "Array"],
    ["Associative Map", "std.AutoHashMap(K, V)", "Map.Map k v", "Map.Make / Hashtbl", "Dictionary<K, V>", "Map<K, V>", "array (assoc)", "Hash"],
    ["Unique Set", "AutoHashMap(T, void)", "Set.Set a", "Set.Make", "Set<T>", "Set<T>", "array (keys)", "Set"],
    ["Tuple", "struct { T1, T2 }", "(a, b)", "a * b", "(T1, T2)", "Pair<A, B>", "array", "[a, b]"],
    ["Optional / Nullable", "?T (optional pointer)", "Maybe a", "'a option", "Optional<T> (T?)", "T? (Nullable ref)", "?Type / null", "nil (dynamic)"],
    ["Result / Error", "!T (error union)", "Either e a", "('a, 'e) result", "Result<T, E>", "Result<T>", "throw Exception", "rescue Exception"],
    ["Tagged Union / ADT", "union(enum) { ... }", "data Type = A | B", "type t = A | B", "enum T { case a(Int) }", "sealed class/interface", "enum / match", "dynamic class hierarchy"],
    ["Closure / Function", "*const fn(...) ...", "a -> b", "'a -> 'b", "(Args) -> Ret", "(Args) -> Ret", "Closure / callable", "Proc / lambda"],
    ["Unit / Void", "void", "()", "unit (())", "Void / ()", "Unit", "void", "nil"],
    ["Bottom / Never", "noreturn", "Void (empty)", "empty", "Never", "Nothing", "never", "N/A"]
]

# 18. Type System Characteristics & Taxonomy (15 Languages!)
headers_taxonomy = ["Language", "Typing Paradigm", "Type Enforcement", "Inference Level", "Typing Style", "Null Safety Model", "Generics / Polymorphism", "Memory Management"]
rows_taxonomy = [
    ["C", "Static", "Weak (implicit conversions)", "None (C23 auto basic)", "Nominal", "Unsafe (NULL pointer)", "None (Macros / _Generic)", "Manual (malloc / free)"],
    ["C++", "Static", "Medium-Strong", "Local (auto, decltype)", "Nominal + Concepts", "Unsafe (nullptr / optional)", "Monomorphization (Templates)", "RAII / Smart Pointers / Manual"],
    ["Rust", "Static", "Strong (no implicit cast)", "Local + Bidirectional (let)", "Nominal (Traits)", "Safe (Option<T>, no null)", "Monomorphization (Generics/Traits)", "Ownership / Borrow Checker"],
    ["Zig", "Static", "Strong (explicit casts)", "Compile-time (comptime)", "Nominal + Comptime Duck", "Safe (?T, error unions !T)", "Comptime (Types as values)", "Manual (Explicit Allocators)"],
    ["Go", "Static", "Strong (explicit casts)", "Local (:= syntax)", "Structural (Interfaces)", "Semi-safe (nilable pointers/slices)", "Monomorphization (GC dictionary)", "Garbage Collected (Concurrent)"],
    ["Java", "Static", "Strong", "Local (var keyword)", "Nominal", "Unsafe (NullPointerException / Optional)", "Type Erasure (<T> erased to Object)", "Garbage Collected (Generational)"],
    ["C#", "Static (+ dynamic opt)", "Strong", "Local (var keyword)", "Nominal", "Compiler-enforced (Nullable Ref Types)", "Reified Generics (Native CLR)", "Garbage Collected (Generational)"],
    ["Python", "Dynamic (Type hints)", "Strong (1 + '2' is TypeError)", "Gradual (mypy / pyright)", "Duck Typing / Protocols", "None (NoneType, Optional[T] hints)", "Type Erasure (typing module)", "Reference Counting + Cyclic GC"],
    ["TypeScript", "Static (compiles to JS)", "Medium (JS semantics at runtime)", "Local + Full Contextual", "Structural (Duck typing)", "Strict (strictNullChecks / union undefined)", "Type Erasure (erased at compile)", "Host Garbage Collector (V8/JSC)"],
    ["Haskell", "Static", "Strong (pure type safety)", "Global (Hindley-Milner)", "Nominal + Typeclasses", "Safe (Maybe a, no null)", "Parametric (Typeclasses / Higher-rank)", "Garbage Collected (Lazy evaluation)"],
    ["OCaml", "Static", "Strong (sound type system)", "Global (Hindley-Milner)", "Nominal + Structural (Modules/Objects)", "Safe ('a option, no null)", "Parametric (Functors / Polymorphic)", "Garbage Collected (Generational GC)"],
    ["Swift", "Static", "Strong", "Full Local (let / var)", "Nominal + Protocols", "Safe (Optional<T> / T?, no raw null)", "Monomorphization + Witness Tables", "Automatic Reference Counting (ARC)"],
    ["Kotlin", "Static", "Strong", "Full Local (val / var)", "Nominal", "Safe (T vs T? in type system)", "Type Erasure (Reified in inline fun)", "Garbage Collected (JVM GC)"],
    ["PHP", "Dynamic (+ strict_types)", "Gradual (weak by default, strict opt)", "Local (none / dynamic)", "Nominal + Duck", "Safe (?Type in type hints)", "Type Erasure (Generics via DocBlocks)", "Reference Counting + Cycle Collector"],
    ["Ruby", "Dynamic", "Strong (no implicit '1' + 2)", "None (dynamic runtime)", "Pure Duck Typing", "None (nil is NilClass object)", "Duck Typing (no static generics)", "Garbage Collected (Mark-and-Sweep)"]
]

# 19. N x N Type Conversion & Coercion Matrix
headers_casts = ["Conversion Pair", "C / C++ Behavior", "Rust Behavior", "Go Behavior", "Java Behavior", "C# Behavior", "Python Behavior", "TypeScript Behavior"]
rows_casts = [
    ["bool -> integer", "Implicit promotion (0 or 1)", "val as i32 (explicit)", "Explicit if/else needed", "Explicit (val ? 1 : 0)", "Convert.ToInt32(val)", "int(val) (True==1, False==0)", "Number(val) (true->1, false->0)"],
    ["int8 -> int32", "Implicit lossless widening", "val as i32 (or i32::from)", "int32(val) (explicit)", "Implicit widening", "Implicit widening", "Automatic (all ints same)", "No-op (all are number)"],
    ["int32 -> int8", "Implicit truncation (UB on overflow in C)", "val as i8 (truncates)", "int8(val) (truncates)", "(byte) val (truncates)", "(sbyte) val (checked opt)", "N/A (single int type)", "val & 0xFF (bitwise mask)"],
    ["int32 -> int64", "Implicit widening", "val as i64 (or i64::from)", "int64(val) (explicit)", "Implicit widening", "Implicit widening", "Automatic", "BigInt(val) (if bigint)"],
    ["int64 -> int32", "Implicit narrowing (data loss)", "val as i32 (or TryFrom)", "int32(val) (truncates)", "(int) val (truncates)", "(int) val (checked opt)", "Automatic", "Number(val) (clamp/round)"],
    ["int32 -> float32", "Implicit (may lose low bits)", "val as f32 (lossy cast)", "float32(val) (explicit)", "Implicit (may lose precision)", "Implicit", "float(val)", "No-op (number is f64)"],
    ["float32 -> int32", "Implicit truncation to zero", "val as i32 (saturates)", "int32(val) (truncates)", "(int) val (truncates)", "(int) val (truncates)", "int(val) (truncates)", "Math.trunc(val) | 0"],
    ["float32 -> float64", "Implicit lossless promotion", "val as f64 (lossless)", "float64(val) (explicit)", "Implicit promotion", "Implicit promotion", "No-op (native 64b)", "No-op (native 64b)"],
    ["float64 -> float32", "Implicit rounding / overflow", "val as f32 (rounds/inf)", "float32(val) (rounds)", "(float) val (rounds)", "(float) val (rounds)", "numpy.float32(val)", "Math.fround(val)"],
    ["string -> integer", "atoi(s) / strtol(s, &end, 10)", "s.parse::<i32>() (Result)", "strconv.Atoi(s) (error)", "Integer.parseInt(s) (throws)", "int.Parse / int.TryParse", "int(s) (ValueError)", "parseInt(s, 10) / Number(s)"],
    ["integer -> string", "sprintf(buf, \"%d\", val)", "val.to_string()", "strconv.Itoa(val)", "String.valueOf(val)", "val.ToString()", "str(val)", "String(val) / `${val}`"],
    ["pointer -> integer", "(uintptr_t) ptr", "ptr as usize", "uintptr(ptr)", "N/A (managed refs)", "(IntPtr) ptr / (nint)", "ctypes.addressof", "N/A (managed)"],
    ["pointer -> pointer", "reinterpret_cast<T*> / (T*)", "ptr as *const U / transmute", "(*U)(unsafe.Pointer(p))", "N/A (type-safe)", "Unsafe.As<T, U>", "ctypes.cast", "N/A (structural)"]
]

# ----------------- DEDICATED DEEP-DIVE C & C++ SECTIONS -----------------

# 21. C & C++ Networking & Protocol Types
headers_cpp_net = ["Category / Concept", "Semantic Meaning", "C Standard / POSIX", "Modern C++ (C++20/23 / Asio)", "Memory Layout & Size"]
rows_cpp_net = [
    ["IPv4 Address", "32-bit internet host address", "struct in_addr (s_addr)", "boost::asio::ip::address_v4", "4 bytes, Big-Endian (Network Order)"],
    ["IPv6 Address", "128-bit internet host address", "struct in6_addr (s6_addr[16])", "boost::asio::ip::address_v6", "16 bytes (128 bits contiguous)"],
    ["Generic Socket Address", "Polymorphic socket base struct", "struct sockaddr (sa_family, data)", "sockaddr_storage / asio endpoint", "16 bytes minimum base struct"],
    ["IPv4 Socket Endpoint", "IPv4 address + 16b port pair", "struct sockaddr_in", "boost::asio::ip::tcp::endpoint", "16 bytes (family, port, in_addr, pad)"],
    ["IPv6 Socket Endpoint", "IPv6 address + port + scope ID", "struct sockaddr_in6", "boost::asio::ip::tcp::endpoint", "28 bytes (family, port, flow, in6, scope)"],
    ["Socket Storage Buffer", "Opaque buffer for any sock type", "struct sockaddr_storage", "std::aligned_storage_t / asio buffer", "128 bytes (guaranteed 64-bit aligned)"],
    ["Socket Handle", "OS network socket descriptor", "int (POSIX socket descriptor)", "boost::asio::ip::tcp::socket", "4 bytes (POSIX) / 8 bytes (Win64 SOCKET)"],
    ["Ethernet / MAC Address", "48-bit hardware layer 2 address", "struct ether_addr / uint8_t[6]", "std::array<uint8_t, 6>", "6 bytes contiguous octets"],
    ["Port Number", "16-bit network service port", "in_port_t / uint16_t (htons/ntohs)", "uint16_t (host vs network order)", "2 bytes (Big-Endian on wire)"],
    ["Scatter-Gather IO Buffer", "Vectorized I/O memory chunk", "struct iovec (iov_base, iov_len)", "std::span<std::byte> / asio::buffer", "Pointer + size_t (16 bytes on 64-bit)"]
]

# 22. C & C++ Cryptography & Secure Memory Types
headers_cpp_crypto = ["Category / Concept", "Security Purpose", "C Standard / POSIX / OpenSSL", "Modern C++ (Botan / Libsodium)", "Security & Execution Guarantee"]
rows_cpp_crypto = [
    ["SHA-256 Digest", "256-bit cryptographic digest", "uint8_t hash[32] / SHA256_CTX", "std::array<std::byte, 32>", "32 bytes fixed, non-reversible hash"],
    ["SHA-512 Digest", "512-bit cryptographic digest", "uint8_t hash[64] / SHA512_CTX", "std::array<std::byte, 64>", "64 bytes fixed, high collision resistance"],
    ["HMAC Authentication Tag", "Keyed hash message auth code", "EVP_MAC* / uint8_t mac[32]", "botan::MAC / std::array<uint8_t, 32>", "32/64 bytes, constant-time compare"],
    ["Asymmetric Private Key", "RSA/ECC signing and decrypt", "EVP_PKEY* (OpenSSL opaque struct)", "botan::Private_Key / std::unique_ptr", "Heap opaque key, protected memory"],
    ["Asymmetric Public Key", "RSA/ECC signature verification", "EVP_PKEY* (OpenSSL opaque struct)", "botan::Public_Key / std::unique_ptr", "Heap opaque key structure"],
    ["Secure Zeroization", "Wipe sensitive RAM on cleanup", "explicit_bzero / OPENSSL_cleanse", "volatile wipe wrapper / secure_vector", "Compiler cannot optimize away memset"],
    ["Constant-Time Equality", "Prevent timing side-channel leaks", "CRYPTO_memcmp (OpenSSL)", "botan::constant_time_compare", "Execution time independent of data match"],
    ["Hardware Entropy Seed", "CSPRNG cryptographically secure", "getentropy() / /dev/urandom", "std::random_device (hardware)", "OS kernel non-blocking entropy pool"]
]

# 23. C & C++ Computer Graphics, Physics & Geometry Types
headers_cpp_gfx = ["Category / Concept", "Mathematical Semantics", "Plain C Structure", "Modern C++ (GLM / DirectXMath)", "Memory Layout & Alignment"]
rows_cpp_gfx = [
    ["2D Vector / Point", "(x, y) 2D Cartesian coords", "struct { float x, y; }", "glm::vec2 / DirectX::XMFLOAT2", "8 bytes (2x 32-bit float)"],
    ["3D Vector / Point", "(x, y, z) 3D world coords", "struct { float x, y, z; }", "glm::vec3 / DirectX::XMFLOAT3", "12 bytes packed / 16 bytes padded"],
    ["4D Vector / Homogeneous", "(x, y, z, w) homogeneous coords", "struct { float x, y, z, w; }", "glm::vec4 / DirectX::XMVECTOR", "16 bytes (SIMD aligned, alignas(16))"],
    ["Quaternion", "3D rotation (x, y, z, w)", "struct { float x, y, z, w; }", "glm::quat / DirectX::XMVECTOR", "16 bytes (w is scalar, xyz vector)"],
    ["3x3 Matrix", "Linear 2D affine / 3D rotation", "float m[3][3] (column-major)", "glm::mat3 / DirectX::XMFLOAT3X3", "36 bytes (9x 32-bit floats)"],
    ["4x4 Transform Matrix", "Model-View-Projection matrix", "float m[4][4] (column-major)", "glm::mat4 / DirectX::XMMATRIX", "64 bytes (16x floats, alignas(16/64))"],
    ["Packed RGBA8888 Color", "32-bit true color with alpha", "uint32_t (0xAARRGGBB)", "struct Color { uint8_t r,g,b,a; }", "4 bytes (1 byte per channel)"],
    ["HDR Float4 Color", "Linear high dynamic range color", "struct { float r, g, b, a; }", "glm::vec4 / DirectX::XMFLOAT4", "16 bytes (floating-point color)"],
    ["Axis-Aligned Bounding Box", "Min / Max 3D spatial extents", "struct { float min[3], max[3]; }", "struct AABB { glm::vec3 min, max; }", "24 bytes (or 32 bytes SIMD padded)"],
    ["3D Ray", "Origin point + unit direction", "struct { float orig[3], dir[3]; }", "struct Ray { glm::vec3 orig, dir; }", "24 bytes (2x 3D vectors)"]
]

# 24. C & C++ OS, POSIX & Windows System Types
headers_cpp_os = ["Category / Concept", "OS Resource Abstraction", "POSIX / Unix (C / C++)", "Win32 / Windows API", "C++ Standard Abstraction"]
rows_cpp_os = [
    ["File Descriptor / Handle", "Open kernel stream descriptor", "int (0=stdin, 1=stdout, 2=stderr)", "HANDLE (INVALID_HANDLE_VALUE)", "std::fstream / std::osyncstream"],
    ["Filesystem Path", "OS directory and file path", "const char* (UTF-8 POSIX path)", "const wchar_t* (UTF-16 WCHAR path)", "std::filesystem::path (C++17)"],
    ["Process Identifier (PID)", "Unique running process ID", "pid_t (signed 32-bit integer)", "DWORD (unsigned 32-bit integer)", "N/A (Platform OS API)"],
    ["User / Group Identifier", "POSIX security credentials", "uid_t, gid_t (unsigned 32-bit)", "PSID (Security Identifier pointer)", "N/A (Platform OS API)"],
    ["Virtual Memory Map", "Direct page-table memory mapping", "void* (mmap / munmap / mprotect)", "LPVOID (VirtualAlloc / MapViewOfFile)", "boost::iostreams::mapped_file"],
    ["File Offset / Size", "64-bit file position and size", "off_t / off64_t (signed 64-bit)", "LARGE_INTEGER (QuadPart 64-bit)", "std::filesystem::file_size (uintmax_t)"],
    ["File Metadata / Stat", "Permissions, inode, size, time", "struct stat (stat, fstat, lstat)", "BY_HANDLE_FILE_INFORMATION", "std::filesystem::file_status"],
    ["Signal Handler Function", "Hardware/OS interrupt callback", "sighandler_t / void (*)(int)", "PHANDLER_ROUTINE (Console Handler)", "std::signal (global signal hook)"],
    ["Atomic Signal Flag", "Async-signal-safe flag variable", "volatile sig_atomic_t", "volatile sig_atomic_t", "std::atomic_flag (lock-free)"],
    ["Shared Library Module", "Dynamic linked library handle", "void* (dlopen, dlsym, dlclose)", "HMODULE / HINSTANCE (LoadLibrary)", "boost::dll::shared_library"]
]

# 25. C & C++ Hardware, Embedded & Low-Level Memory Types
headers_cpp_hw = ["Category / Concept", "Hardware / Embedded Role", "C Language Construct", "C++ Modern Construct", "Hardware & Silicon Impact"]
rows_cpp_hw = [
    ["MMIO Hardware Register", "Direct memory-mapped I/O port", "volatile uint32_t* const REG", "volatile uint32_t* const REG", "Prevents compiler register cache / reordering"],
    ["Hardware Bitfield", "Packed hardware control bits", "struct { unsigned int en:1; }", "struct Reg { uint32_t en:1; }", "Direct bit-level mapping to HW control words"],
    ["Cache-Line Aligned Struct", "Prevent CPU false sharing", "_Alignas(64) struct CacheLine", "alignas(destructive_interference_size)", "Aligns memory to 64/128B L1/L2 cache boundary"],
    ["Packed Struct (No Pad)", "Zero padding between members", "struct __attribute__((packed))", "#pragma pack(push, 1)", "Exact byte layout for EEPROM / wire packets"],
    ["Fixed-Point (Q16.16)", "Integer with fractional bits", "int32_t (16 integer, 16 fractional)", "fixed_point<int32_t, -16>", "High-speed DSP / MCU math without FPU silicon"],
    ["Little-Endian Integer", "Explicit little-endian word", "uint32_t + le32toh / htole32", "boost::endian::little_uint32_t", "Guarantees little-endian storage in RAM/flash"],
    ["Big-Endian Integer", "Explicit big-endian word", "uint32_t + be32toh / htobe32", "boost::endian::big_uint32_t", "Guarantees big-endian storage in RAM/flash"],
    ["DMA Contiguous Buffer", "Hardware Direct Memory Access", "void* (coherent uncached pages)", "alignas(4096) std::byte[PAGE_SZ]", "Bypasses CPU cache; bus master hardware reads"],
    ["Aliasing-Safe Raw Byte", "Type aliasing exception byte", "unsigned char", "std::byte (C++17 distinct enum type)", "Permitted to alias any type under strict aliasing"]
]

# 26. C++ Metaprogramming, Templates & Advanced Type Traits
headers_cpp_meta = ["Metaprogramming Concept", "Purpose in C++ Type System", "C++20 / C++23 Modern Construct", "Pre-C++20 Classical Construct", "Evaluation & Runtime Cost"]
rows_cpp_meta = [
    ["Concept / Constraint", "Constrain template arguments", "template<std::integral T> / requires", "SFINAE (std::enable_if_t<...>) ", "Zero runtime cost (compile-time rejection)"],
    ["Type Identity Barrier", "Block template argument deduction", "std::type_identity_t<T>", "template<class T> struct id { using type=T; }", "Zero runtime cost (pure type transformation)"],
    ["Type Predicate Trait", "Query compile-time type property", "std::is_integral_v<T> / is_pod_v<T>", "std::is_integral<T>::value", "Zero runtime cost (constexpr bool constant)"],
    ["Type Transformation Trait", "Strip qualifiers and references", "std::remove_cvref_t<T> / decay_t<T>", "typename std::decay<T>::type", "Zero runtime cost (type substitution)"],
    ["CRTP (Static Interface)", "Compile-time static polymorphism", "template<class D> struct Base { ... }", "CRTP pattern with static_cast<D*>", "Zero runtime cost (no vtable, directly inlined)"],
    ["Tag Dispatching", "Function overload by type tag", "void f(T, std::true_type)", "std::true_type / std::false_type tags", "Zero runtime cost (inlined by optimizer)"],
    ["Strong / Opaque Newtype", "Disallow accidental type swap", "struct UserId { uint64_t v; };", "typedef uint64_t UserId (weak alias)", "Zero runtime cost (scalar ABI compatible)"],
    ["Compile-Time Constant", "Value computed during build", "constexpr auto x / consteval fn()", "enum { Val = 42 } / template constants", "Stored in binary read-only text/rodata"],
    ["Runtime Type Info (RTTI)", "Dynamic type query / inspection", "typeid(expr) / std::type_info", "typeid(expr) / dynamic_cast", "Requires 8-byte pointer in virtual table (vptr)"]
]

# 27. C & C++ Memory Model, Atomics & Memory Ordering
headers_cpp_atomic = ["Memory Order / Atomic Concept", "C Standard (<stdatomic.h>)", "C++ Standard (<atomic>)", "CPU Barrier / Bus Operation", "Intended Use Case & Invariants"]
rows_cpp_atomic = [
    ["Relaxed Ordering", "memory_order_relaxed", "std::memory_order_relaxed", "Plain atomic instruction (no fences)", "Atomic counters, metrics, no synchronization"],
    ["Acquire Ordering", "memory_order_acquire", "std::memory_order_acquire", "Load-Load & Load-Store barrier", "Acquiring mutex, reading published shared data"],
    ["Release Ordering", "memory_order_release", "std::memory_order_release", "Load-Store & Store-Store barrier", "Releasing mutex, publishing initialized data"],
    ["Acquire-Release", "memory_order_acq_rel", "std::memory_order_acq_rel", "Combined Acquire and Release barrier", "Read-modify-write operations, CAS loops"],
    ["Sequential Consistency", "memory_order_seq_cst", "std::memory_order_seq_cst", "Full memory fence (mfence / dmb ish)", "Default safe multi-threaded memory access"],
    ["Atomic Flag Primitive", "atomic_flag (ATOMIC_FLAG_INIT)", "std::atomic_flag", "Single-bit hardware test-and-set", "Guaranteed lock-free spinlocks, minimal mutex"],
    ["Atomic Smart Pointer", "N/A (atomic_is_lock_free check)", "std::atomic<std::shared_ptr<T>>", "Double-word CAS or internal spinlock", "Lock-free concurrent graphs and data structures"]
]

out = []

out.append("=" * 175)
out.append("                                            DEFINITIVE MASTER MATRIX OF PROGRAMMING DATA TYPES                                            ")
out.append("=" * 175)
out.append("")
out.append("A COMPREHENSIVE, EXHAUSTIVE MULTI-LANGUAGE REFERENCE ACROSS PRIMITIVE, COMPOSITE, CONCURRENCY, HARDWARE, DATABASE, AND META-TYPE SYSTEMS")
out.append("Primary Languages Covered: C (C17/C23) | C++ (C++20/23) | Rust | Go | Java (17/21) | C# (.NET 8/9) | Python (3.10+) | TypeScript (5.x)")
out.append("Extended Languages Covered: Zig (0.13+) | Haskell (GHC 9+) | OCaml (5.x) | Swift (5/6) | Kotlin (2.0) | PHP (8.2+) | Ruby (3.2+) | SQL (ISO/PG)")
out.append("Deep-Dive Low-Level Sections: C & C++ Systems, POSIX/Windows OS, Networking, Cryptography, Graphics/Math, Hardware/MMIO, Metaprogramming & Atomics")
out.append("")
out.append("TABLE OF CONTENTS:")
out.append("  [PART I: CORE MULTI-LANGUAGE COMPARISON]")
out.append("  SECTION  1: SIGNED INTEGER TYPES")
out.append("  SECTION  2: UNSIGNED INTEGER & ARCHITECTURE-SIZED TYPES")
out.append("  SECTION  3: FLOATING-POINT, FIXED-POINT, RATIONAL & NUMERIC TYPES")
out.append("  SECTION  4: BOOLEAN & TEXTUAL CHARACTER TYPES")
out.append("  SECTION  5: STRING & CONTIGUOUS BYTE BUFFER TYPES")
out.append("  SECTION  6: SEQUENCE & COLLECTION TYPES (ARRAYS, SLICES, TUPLES, QUEUES)")
out.append("  SECTION  7: ASSOCIATIVE & SET COLLECTIONS (HASH MAPS, TREES, SETS, BITSETS)")
out.append("  SECTION  8: USER-DEFINED, COMPOSITE & OBJECT TYPES (STRUCTS, CLASSES, INTERFACES)")
out.append("  SECTION  9: ALGEBRAIC DATA TYPES, ENUMS & VARIANTS")
out.append("  SECTION 10: POINTERS, REFERENCES & MEMORY MANAGEMENT TYPES")
out.append("  SECTION 11: OPTIONALITY, ERROR HANDLING, BOTTOM & TOP TYPES")
out.append("  SECTION 12: FUNCTIONAL, CALLABLE & CLOSURE TYPES")
out.append("  SECTION 13: CONCURRENCY, ASYNC & THREADING PRIMITIVES")
out.append("  SECTION 14: SIMD & HARDWARE VECTOR TYPES")
out.append("  SECTION 15: TEMPORAL TYPES: DATES, TIMES & DURATIONS")
out.append("  SECTION 16: RELATIONAL DATABASE (SQL) TO PROGRAMMING LANGUAGE TYPE MAPPINGS")
out.append("  SECTION 17: EXTENDED SYSTEMS, FUNCTIONAL & SCRIPTING LANGUAGES MATRIX (ZIG, HASKELL, OCAML, SWIFT, KOTLIN, PHP, RUBY)")
out.append("  SECTION 18: COMPREHENSIVE TYPE SYSTEM CHARACTERISTICS & TAXONOMY (15 LANGUAGES)")
out.append("  SECTION 19: N x N TYPE CONVERSION, CASTING & COERCION MATRIX")
out.append("  SECTION 20: TYPE THEORY TAXONOMY & DEFINITIVE GLOSSARY")
out.append("")
out.append("  [PART II: DEEP-DIVE C & C++ SPECIALIZED SYSTEMS & DOMAIN MATRICES]")
out.append("  SECTION 21: C & C++ NETWORKING & PROTOCOL TYPES (IPV4, IPV6, SOCKETS, BUFFERS)")
out.append("  SECTION 22: C & C++ CRYPTOGRAPHY & SECURE MEMORY TYPES (HASHES, ZEROIZATION, KEYS)")
out.append("  SECTION 23: C & C++ COMPUTER GRAPHICS, PHYSICS & GEOMETRY TYPES (VECTORS, MATRICES, QUATERNIONS)")
out.append("  SECTION 24: C & C++ OS, POSIX & WINDOWS SYSTEM TYPES (HANDLES, PATHS, MMAP, SIGNALS)")
out.append("  SECTION 25: C & C++ HARDWARE, EMBEDDED & LOW-LEVEL MEMORY TYPES (MMIO, BITFIELDS, Q-FORMAT, ENDIAN)")
out.append("  SECTION 26: C++ METAPROGRAMMING, TEMPLATES & ADVANCED TYPE TRAITS (CONCEPTS, SFINAE, CRTP, TRAITS)")
out.append("  SECTION 27: C & C++ MEMORY MODEL, ATOMICS & MEMORY ORDERING (ACQUIRE, RELEASE, SEQ_CST, BARRIERS)")
out.append("")

def add_section(title, headers, rows):
    out.append("-" * 175)
    out.append(f"{title}")
    out.append("-" * 175)
    out.append(make_table(headers, rows))
    out.append("")

# Part I
add_section("SECTION 1: SIGNED INTEGER TYPES", headers_main, rows_signed_int)
add_section("SECTION 2: UNSIGNED INTEGER & ARCHITECTURE-SIZED TYPES", headers_main, rows_unsigned_int)
add_section("SECTION 3: FLOATING-POINT, FIXED-POINT, RATIONAL & NUMERIC TYPES", headers_main, rows_float)
add_section("SECTION 4: BOOLEAN & TEXTUAL CHARACTER TYPES", headers_main, rows_bool_char)
add_section("SECTION 5: STRING & CONTIGUOUS BYTE BUFFER TYPES", headers_main, rows_string_bytes)
add_section("SECTION 6: SEQUENCE & COLLECTION TYPES (ARRAYS, SLICES, TUPLES, QUEUES)", headers_main, rows_sequences)
add_section("SECTION 7: ASSOCIATIVE & SET COLLECTIONS (HASH MAPS, TREES, SETS, BITSETS)", headers_main, rows_associative)
add_section("SECTION 8: USER-DEFINED, COMPOSITE & OBJECT TYPES (STRUCTS, CLASSES, INTERFACES)", headers_main, rows_composites)
add_section("SECTION 9: ALGEBRAIC DATA TYPES, ENUMS & VARIANTS", headers_main, rows_enums_variants)
add_section("SECTION 10: POINTERS, REFERENCES & MEMORY MANAGEMENT TYPES", headers_main, rows_memory)
add_section("SECTION 11: OPTIONALITY, ERROR HANDLING, BOTTOM & TOP TYPES", headers_main, rows_special)
add_section("SECTION 12: FUNCTIONAL, CALLABLE & CLOSURE TYPES", headers_main, rows_functions)
add_section("SECTION 13: CONCURRENCY, ASYNC & THREADING PRIMITIVES", headers_main, rows_concurrency)
add_section("SECTION 14: SIMD & HARDWARE VECTOR TYPES", headers_main, rows_simd)
add_section("SECTION 15: TEMPORAL TYPES: DATES, TIMES & DURATIONS", headers_main, rows_temporal)
add_section("SECTION 16: RELATIONAL DATABASE (SQL) TO PROGRAMMING LANGUAGE TYPE MAPPINGS", headers_sql, rows_sql)
add_section("SECTION 17: EXTENDED SYSTEMS, FUNCTIONAL & SCRIPTING LANGUAGES MATRIX (ZIG, HASKELL, OCAML, SWIFT, KOTLIN, PHP, RUBY)", headers_extended, rows_extended)
add_section("SECTION 18: COMPREHENSIVE TYPE SYSTEM CHARACTERISTICS & TAXONOMY (15 LANGUAGES)", headers_taxonomy, rows_taxonomy)
add_section("SECTION 19: N x N TYPE CONVERSION, CASTING & COERCION MATRIX", headers_casts, rows_casts)

# Section 20 Glossary
out.append("-" * 175)
out.append("SECTION 20: TYPE THEORY TAXONOMY & DEFINITIVE GLOSSARY")
out.append("-" * 175)
glossary_text = """
1. SCALAR / PRIMITIVE TYPES
   - An indivisible value that holds a single data unit (e.g., integer, floating point, boolean, character).
   - In low-level systems (C, Rust, Go), these map directly to CPU registers and machine words.

2. PRODUCT TYPES (Records, Tuples, Structs)
   - In type theory, a Product Type A x B is a composite type where a value contains BOTH an instance of A AND an instance of B.
   - Examples: `(int, string)` tuple, `struct Point { int x; int y; }`. The cardinality of possible values is |A| * |B|.

3. SUM TYPES / COPRODUCTS (Tagged Unions, Discriminated Unions, Algebraic Data Types)
   - A type A + B where a value contains EITHER an instance of A OR an instance of B, with a runtime or compiler tag distinguishing them.
   - Examples: Rust `enum`, Haskell `data`, Swift `enum with associated values`, C++ `std::variant`. The cardinality is |A| + |B|.

4. UNIT TYPE ( () / void ) vs BOTTOM TYPE ( ! / never / Nothing )
   - Unit Type (1): A type with exactly ONE value. It carries no information. Used when a function finishes normally but returns nothing meaningful.
     Examples: Rust `()`, OCaml/Haskell `()`, Go `struct{}`, Kotlin `Unit`, Swift `Void`.
   - Bottom Type (0 / Empty): A type with ZERO values. It can never be instantiated. Used to represent computations that NEVER return (infinite loops,
     panics, fatal exits, exceptions).
     Examples: Rust `!`, TypeScript `never`, Kotlin `Nothing`, Python `typing.Never`, Haskell `Void`.

5. TOP TYPE ( Universal Supertype / Any )
   - The type that is a supertype of all other types. Any value in the system can be assigned to a variable of this type.
   - Examples: TypeScript `unknown` / `any`, Java `java.lang.Object`, C# `object`, Go `any` (`interface{}`), Python `object` / `typing.Any`.

6. NOMINAL vs STRUCTURAL vs DUCK TYPING
   - Nominal Typing: Type compatibility and equivalence are determined solely by explicit declaration and name.
     Even if two structs have identical fields, they are distinct and incompatible unless explicitly related by inheritance. (Java, C++, Rust, C#).
   - Structural Typing: Type compatibility is determined by the shape and layout of the type (its members and method signatures).
     If type A has all properties of type B, A is assignable to B without declaring an explicit relationship. (TypeScript, Go interfaces, OCaml objects).
   - Duck Typing: Runtime structural typing ("If it walks like a duck and quacks like a duck, it's a duck"). No compile-time checks; method dispatch
     fails at runtime if the member does not exist. (Python, Ruby, JavaScript).

7. TYPE SAFETY: SOUNDNESS vs COMPLETENESS
   - Soundness: A type system is sound if well-typed programs are guaranteed to never cause undefined behavior or type errors at runtime ("Well-typed
     programs cannot go wrong"). Rust, Haskell, and Standard ML prioritize soundness.
   - Completeness: A type system is complete if every program that would not fail at runtime is accepted by the type checker. In practice, undecidability
     (Rice's theorem / Halting problem) makes simultaneously sound and complete static analysis impossible.
   - TypeScript intentionally trades strict soundness for developer expressiveness and JS compatibility (`any` escapes, bivariant method parameters).

8. GENERICS & POLYMORPHISM MECHANISMS
   - Monomorphization: The compiler generates a distinct copy of machine code for every concrete type used with a generic function or template.
     Pros: Zero runtime overhead, inlineable, optimal CPU cache. Cons: Code bloat, longer compilation times. (C++ templates, Rust generics, Zig comptime).
   - Type Erasure: Generic type parameters are checked at compile time and then erased; a single compiled function operates on universal pointers (`Object`).
     Pros: Tiny binary footprint, rapid compilation. Cons: Boxing/unboxing overhead for primitives, reflection lacks runtime type parameters. (Java, TypeScript).
   - Reified Generics: The runtime preserves exact type parameters in bytecode/metadata, enabling `typeof(T)`, zero-overhead value-type specialization,
     and runtime type queries. (C# CLR generics).
"""
out.append(glossary_text.strip())
out.append("")

# Part II: C & C++ Deep-Dive Sections
add_section("SECTION 21: C & C++ NETWORKING & PROTOCOL TYPES (IPV4, IPV6, SOCKETS, BUFFERS)", headers_cpp_net, rows_cpp_net)
add_section("SECTION 22: C & C++ CRYPTOGRAPHY & SECURE MEMORY TYPES (HASHES, ZEROIZATION, KEYS)", headers_cpp_crypto, rows_cpp_crypto)
add_section("SECTION 23: C & C++ COMPUTER GRAPHICS, PHYSICS & GEOMETRY TYPES (VECTORS, MATRICES, QUATERNIONS)", headers_cpp_gfx, rows_cpp_gfx)
add_section("SECTION 24: C & C++ OS, POSIX & WINDOWS SYSTEM TYPES (HANDLES, PATHS, MMAP, SIGNALS)", headers_cpp_os, rows_cpp_os)
add_section("SECTION 25: C & C++ HARDWARE, EMBEDDED & LOW-LEVEL MEMORY TYPES (MMIO, BITFIELDS, Q-FORMAT, ENDIAN)", headers_cpp_hw, rows_cpp_hw)
add_section("SECTION 26: C++ METAPROGRAMMING, TEMPLATES & ADVANCED TYPE TRAITS (CONCEPTS, SFINAE, CRTP, TRAITS)", headers_cpp_meta, rows_cpp_meta)
add_section("SECTION 27: C & C++ MEMORY MODEL, ATOMICS & MEMORY ORDERING (ACQUIRE, RELEASE, SEQ_CST, BARRIERS)", headers_cpp_atomic, rows_cpp_atomic)

out.append("=" * 175)
out.append("                                                       END OF DEFINITIVE MATRIX OF TYPES                                                      ")
out.append("=" * 175)

content = "\n".join(out)
target_path = "/Volumes/External/Code/matrix_of_types/matrix_of_types.txt"
with open(target_path, "w", encoding="utf-8") as f:
    f.write(content)

print(f"Generated {target_path} successfully. Total lines: {len(out)}, File size: {len(content)} bytes.")
