# XML_Lib Security & OASIS Catalog Resolution Guide

Hardening XML processing against XXE attacks, mitigating XML bombs, and resolving offline schemas using OASIS XML Catalogs 1.1.

---

## 1. Threat Model & XML Vulnerabilities

XML parsers are susceptible to several classes of severe vulnerabilities when processing untrusted input:

### 1.1 XML External Entity (XXE) Injection
In standard XML 1.0, documents can declare external general entities pointing to external URIs:
```xml
<!DOCTYPE root [
    <!ENTITY secret SYSTEM "file:///etc/passwd">
]>
<root>&secret;</root>
```
If external entities are resolved naively, attackers can read sensitive local files, perform Server-Side Request Forgery (SSRF) against internal networks, or trigger denial-of-service conditions.

### 1.2 Billion Laughs (Exponential Entity Expansion)
An attacker defines nested entities where each entity expands exponentially into multiple copies of another:
```xml
<!DOCTYPE lolz [
 <!ENTITY lol "lol">
 <!ENTITY lol1 "&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;&lol;">
 <!ENTITY lol2 "&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;&lol1;">
 ...
 <!ENTITY lol9 "&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;&lol8;">
]>
<lolz>&lol9;</lolz>
```
A tiny 1 KB document can expand into gigabytes of data, causing process memory exhaustion and crashing the host.

### 1.3 Quadratic Blowup & Resource Exhaustion
Even without deep recursion, defining a massive single entity or thousands of nested tags/attributes can overwhelm parsers lacking resource quotas.

---

## 2. Defense-in-Depth in XML_Lib

`XML_Lib` is engineered with **secure-by-default** principles.

### 2.1 Default XXE Protection
In `XML_Lib`, `ParseOptions::allowExternalEntities` defaults to **`false`**. Any attempt by an untrusted XML document to load an external entity without an explicit resolver immediately throws a `SyntaxError` and aborts parsing.

### 2.2 Granular Resource Quotas (`ParseOptions`)

```cpp
#include <XML_Lib/XML.hpp>

using namespace XML_Lib;

ParseOptions secureOptions;
// 1. Cap maximum document size in bytes
secureOptions.maxXmlSize = 10 * 1024 * 1024; // 10 MiB limit

// 2. Limit entity expansion recursion depth (mitigates Billion Laughs)
secureOptions.maxEntityExpansionDepth = 50;

// 3. Limit element nesting depth (prevents stack exhaustion)
secureOptions.maxNestingDepth = 100;

// 4. Limit element and attribute counts (prevents quadratic blowup)
secureOptions.maxElementCount = 100000;
secureOptions.maxAttributeCount = 200;
secureOptions.maxTotalAttributeCount = 50000;

// 5. Strictly enforce XML Namespaces 1.0
secureOptions.strictNamespaces = true;

XML xml;
xml.parse(source, secureOptions);
```

---

## 3. OASIS XML Catalogs 1.1 Resolution

In production systems—particularly air-gapped networks, containerized microservices, and secure enclaves—parsers must validate documents against standard schemas (e.g. W3C XSD schemas or industry DTDs) **without making outbound network connections**.

`XML_Lib` provides native, full-featured compliance with the **OASIS XML Catalogs V1.1 Standard** via the `OASIS_Catalog` class.

### 3.1 Supported OASIS Catalog Rules

- **`system`**: Maps an exact `systemId` URI to a local filesystem path or URI.
- **`public`**: Maps an exact `publicId` identifier to a local path or URI.
- **`rewriteSystem`**: Rewrites a system ID prefix matching a starting string to a local directory prefix.
- **`rewriteURI`**: Rewrites a general URI prefix to a local directory prefix.
- **In-Memory Overrides**: Directly supplies string content for an identifier without disk I/O.

---

## 4. Configuring `OASIS_Catalog`

### 4.1 Programmatic Configuration

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/OASIS_Catalog.hpp>

using namespace XML_Lib;

void setupSecureParser() {
    OASIS_Catalog catalog;

    // 1. Exact system ID mapping
    catalog.addSystemMapping(
        "http://www.w3.org/2001/XMLSchema.dtd",
        "/etc/xml/schemas/XMLSchema.dtd"
    );

    // 2. Exact public ID mapping
    catalog.addPublicMapping(
        "-//W3C//DTD XMLSCHEMA 200102//EN",
        "/etc/xml/schemas/XMLSchema.dtd"
    );

    // 3. Prefix rewriting for entire schema directories
    catalog.addRewriteSystem(
        "https://schemas.company.internal/v1/",
        "/opt/local_schemas/v1/"
    );

    // 4. In-memory content injection (no disk access required)
    catalog.addMemoryContent(
        "urn:company:entities",
        "<!ENTITY companyName 'Acme Corp'>"
    );

    // Bind catalog as the active entity resolver
    ParseOptions options;
    options.entityResolver = &catalog;

    XML xml;
    xml.parse(FileSource{"untrusted_input.xml"}, options);
}
```

### 4.2 Loading from an OASIS XML Catalog File (`catalog.xml`)

Standard `catalog.xml` files can be parsed and loaded directly:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<catalog xmlns="urn:oasis:names:tc:entity:xmlns:xml:catalog">
    <system 
        systemId="http://example.com/dtd/invoice.dtd" 
        uri="local/invoice.dtd"/>
        
    <public 
        publicId="-//EXAMPLE//DTD INVOICE 1.0//EN" 
        uri="local/invoice.dtd"/>
        
    <rewriteSystem 
        systemIdStartString="http://example.com/schemas/" 
        rewritePrefix="file:///var/cache/schemas/"/>
</catalog>
```

```cpp
// Load and initialize catalog from disk file
auto catalog = OASIS_Catalog::fromFile("/etc/xml/catalog.xml");

ParseOptions options;
options.entityResolver = &catalog;

XML xml;
xml.parse(FileSource{"incoming_invoice.xml"}, options);
```

---

## 5. Air-Gapped Deployment Cookbook

When deploying `XML_Lib` in restricted or air-gapped environments:

1. **Pre-populate the local schema cache**: Bundle all required standard DTDs and XSD files in the application container image (e.g. `/opt/app/schemas/`).
2. **Author an OASIS `catalog.xml`**: Map all external HTTP/HTTPS schema references to their `/opt/app/schemas/` counterparts.
3. **Instantiate `OASIS_Catalog::fromFile()`** at application startup as a singleton or shared instance.
4. **Set `options.entityResolver = &catalog`** across all parser invocations.
5. **Verify network isolation**: Any schema or entity not registered in the catalog will fail safely with a resolution error without attempting network sockets.
