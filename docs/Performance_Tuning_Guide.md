# XML_Lib Performance & Optimization Guide

A technical guide on maximizing throughput, eliminating heap allocations, minimizing memory footprints, and configuring embedded builds in **XML_Lib**.

---

## 1. High-Performance Memory Model: PMR Monotonic Arenas

### 1.1 The Allocation Bottleneck in Traditional DOM Parsers
Parsing a 50 MB XML document using standard DOM allocators typically triggers hundreds of thousands of heap allocations for node vectors, tag name strings, and attribute lists. This leads to heap fragmentation, CPU cache thrashing, and high deallocation latency.

### 1.2 Monotonic Arenas in XML_Lib
`XML_Lib` solves this bottleneck by allocating all DOM child nodes and attribute containers from thread-local `std::pmr::monotonic_buffer_resource` arenas:
- **Zero Heap Allocations**: As long as the document fits within the configured arena buffer, child node allocations require zero calls to the system allocator (`malloc`/`new`).
- **Cache Locality**: Contiguous arena memory ensures node structures reside sequentially in CPU L1/L2 caches.
- **Instant Deallocation**: When the arena is destroyed or reset, the entire memory block is reclaimed in a single $O(1)$ operation without walking individual nodes.

### 1.3 Tuning Arena Size
The arena buffer size can be configured at build time:

```bash
# Configure 1 MB arena buffer (default is 256 KB)
cmake -B build -S . -DXML_LIB_ARENA_SIZE_KB=1024
```

> [!TIP]
> If a document exceeds the initial arena buffer, `monotonic_buffer_resource` gracefully falls back to the upstream system allocator. Correctness is strictly preserved; only the zero-allocation guarantee is lost for the overflow portion.

---

## 2. Zero-Copy File Ingestion (`MMapSource`)

When reading files from disk, standard C++ streams (`std::ifstream`) copy data from the kernel page cache into an intermediate userspace buffer, and then into string allocations.

`MMapSource` eliminates these copies by mapping the file directly into process address space using OS-level virtual memory primitives (`mmap` on Linux/macOS, `CreateFileMappingA` on Windows):

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XML_Sources.hpp>

using namespace XML_Lib;

void parseLargeDataset(const std::string &path) {
    // Maps the file directly into virtual memory (zero userspace buffer copies)
    MMapSource source{path};

    XML xml;
    xml.parse(source);
}
```

### Ingestion Throughput Comparison (1 GB XML File)
| I/O Method | Throughput | Kernel-to-User Copies | Memory Footprint |
| :--- | :--- | :--- | :--- |
| `FileSource` (Buffered Stream) | ~210 MB/s | 2 copies | High |
| `BufferSource` (Loaded String) | ~340 MB/s | 1 copy | $2 \times$ File Size |
| **`MMapSource` (Memory Mapped)** | **~620 MB/s** | **0 copies** | **Kernel Page Cache** |

---

## 3. Schema & Query Pre-Compilation

In web services, message brokers, and database engines, documents frequently share identical schemas and XPath queries.

### 3.1 Pre-Compiled XSD Schemas (`XSD_Schema`)
Parsing an XSD schema constructs a complex type hierarchy, facet validators, and state machine transitions. Constructing this schema model on every request creates an unnecessary bottleneck.

```cpp
#include <XML_Lib/XSD_Schema.hpp>

// Compile schema once during service initialization
const XSD_Schema g_invoiceSchema = XSD_Schema::fromFile("/etc/schemas/invoice.xsd");

void handleIncomingRequest(const std::string &xmlPayload) {
    XML doc{xmlPayload};
    
    // Validate concurrently across multiple threads without locks
    doc.validate(g_invoiceSchema);
}
```

### 3.2 Pre-Compiled XPath Expressions (`XPathExpression`)
Similarly, parsing XPath 1.0 expressions involves lexing tokens and constructing abstract syntax trees (ASTs). `XPathExpression` caches the AST once:

```cpp
#include <XML_Lib/XPath.hpp>

// Compile expression once
const XPathExpression g_query{"//transaction[@status='PENDING' and @amount > 500]"};

void auditDocuments(const std::vector<std::string> &files) {
    for (const auto &file : files) {
        XML doc{file};
        auto matches = doc.xpath(g_query);
        // Process matches...
    }
}
```

---

## 4. Architectural Selection Matrix

```
                      Do you need random access / XPath / XSD?
                                   /            \
                                 YES             NO
                                 /                \
                    Is file size > 500 MB?        Use XMLReader (Pull Parser)
                        /            \            O(1) memory, ~380 MB/s
                      YES             NO
                      /                \
        Use MMapSource + XML       Use BufferSource / FileSource
        Zero-copy page cache       Standard PMR DOM parsing
```

---

## 5. Embedded & Resource-Constrained Targets

`XML_Lib` provides dedicated build options for embedded microcontrollers, IoT gateways, and size-constrained binaries:

```bash
# Configure embedded preset
cmake -B build -S . -DXML_LIB_EMBEDDED=ON
```

### Specific Feature Flags

| CMake Option | Default | Effect |
| :--- | :--- | :--- |
| `XML_LIB_EMBEDDED` | `OFF` | Enables size optimization (`MinSizeRel`), disables exceptions/RTTI |
| `XML_LIB_NO_EXCEPTIONS` | `OFF` | Compiles with `-fno-exceptions` (forces non-throwing APIs) |
| `XML_LIB_NO_RTTI` | `OFF` | Compiles with `-fno-rtti` |
| `XML_LIB_ENABLE_LTO` | `ON` | Link-Time Optimization (interprocedural optimization) |
| `XML_LIB_BUILD_SIZE_OPTIMIZED` | `OFF` | Enables `-Os` size optimization |
| `XML_LIB_ENABLE_XPATH` | `ON` | Can be set to `OFF` to strip the entire XPath engine |
| `XML_LIB_ENABLE_XSD` | `ON` | Can be set to `OFF` to strip the XSD validation subsystem |
| `XML_LIB_ENABLE_DTD` | `ON` | Can be set to `OFF` to strip the DTD validation subsystem |
| `XML_LIB_ENABLE_STRINGIFY` | `ON` | Can be set to `OFF` for read-only parsers |

Stripping unused subsystems and compiling with `-Os` reduces the compiled library footprint to **< 180 KB**.

---

## 6. Running the Performance Benchmark Suite

`XML_Lib` includes a dedicated micro-benchmark suite built on Catch2:

```bash
# Build the benchmark executable
cmake --build build --target XML_Lib_Performance_Tests

# Run benchmarks
./build/tests/XML_Lib_Performance_Tests
```
