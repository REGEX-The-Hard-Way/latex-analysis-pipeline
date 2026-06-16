# Memgraph — Comprehensive Architecture & Implementation Analysis

> Analysis date: 2026-06-16
> Git HEAD: `9e3f5b720` — `fix: use the correct conan profile when generating the SBOM (#4272)`
> Total files: 7,772 | Total directories: 1,773
> Language: C/C++ (primary), Python, Rust | Build: CMake + Conan

---

## Table of Contents

1. [Working Functionality](#1-working-functionality)
2. [Supported Cypher Syntax Patterns](#2-supported-cypher-syntax-patterns)
3. [Storage Design](#3-storage-design)
4. [Per-Subsystem File Analysis](#4-per-subsystem-file-analysis)
5. [Pros and Cons of Implementations](#5-pros-and-cons-of-implementations)

---

## 1. Working Functionality

### 1.1 Core Database Engine

| Feature | Status | Description |
|---------|--------|-------------|
| **In-Memory Graph Storage** | Working | SkipList-based vertex/edge storage with MVCC |
| **On-Disk Graph Storage** | Working | RocksDB-backed persistent storage |
| **Analytical Mode** | Working | Direct-mutation, no-MVCC, no-durability mode |
| **Cypher Query Language** | Working | Neo4j-compatible parser (ANTLR4), planner, executor |
| **ACID Transactions** | Working | Snapshot isolation, READ_COMMITTED, READ_UNCOMMITTED |
| **Multi-Version Concurrency Control** | Working | Delta-chain MVCC with commit timestamps |
| **Write-Ahead Logging** | Working | Periodic snapshots + continuous WAL (v35 format) |
| **Crash Recovery** | Working | Snapshot load + WAL replay |
| **Garbage Collection** | Working | Periodic GC (unlink → index cleanup → delete phases) |
| **Property Store Compression** | Working | Zstd compression, configurable float precision |

### 1.2 Indexes

| Index Type | Status | Backend |
|-----------|--------|---------|
| **Label Index** | Working | SkipList (in-memory) / RocksDB (on-disk) |
| **Label+Property Index** | Working | SkipList / RocksDB |
| **Edge Type Index** | Working | SkipList / RocksDB |
| **Edge Type+Property Index** | Working | SkipList / RocksDB |
| **Global Edge Property Index** | Working | SkipList / RocksDB |
| **Text Index (Full-Text Search)** | Working | Tantivy (vertex + edge variants) |
| **Vector Index (Similarity Search)** | Working | USearch (vertex + edge variants) |
| **Point Index (Spatial)** | Working | R-tree (distance + bounding-box queries) |

### 1.3 Constraints

| Constraint Type | Status |
|----------------|--------|
| **Existence (NOT NULL)** | Working |
| **Unique** | Working (single + composite properties) |
| **Type** | Working (property data type enforcement) |

### 1.4 High Availability

| Feature | Status | Details |
|---------|--------|---------|
| **Raft-based Coordination** | Enterprise | NuRaft, odd-numbered coordinator cluster |
| **Automatic Failover** | Enterprise | Lag-aware, most-up-to-date instance selection |
| **SYNC Replication** | Working | Blocking delta streaming |
| **ASYNC Replication** | Working | Non-blocking delta streaming |
| **STRICT_SYNC Replication** | Enterprise | Synchronous with strict guarantees |
| **Manual Failover** | Enterprise | CLI-driven promote/demote |

### 1.5 Authentication & Authorization

| Feature | Status | Details |
|---------|--------|---------|
| **RBAC (Role-Based Access Control)** | Working | 32 coarse-grained permissions |
| **Label-Based Access Control** | Enterprise | Fine-grained permissions on labels/edge types |
| **Multi-Tenant Database Access** | Enterprise | Per-DB access grants/denies |
| **Password Hashing** | Working | bcrypt (workfactor 12), SHA256, SHA256-multi |
| **SSO (SAML, OIDC, Kerberos, LDAP)** | Working | External Python modules via Unix pipes |
| **User Impersonation** | Enterprise | UUID-based identity resolution |
| **User Profiles (Resource Limits)** | Enterprise | Per-user session/memory limits |
| **TLS/SSL** | Working | In-transit encryption, cluster-wide cert reload |

### 1.6 Streaming & Data Import

| Feature | Status | Details |
|---------|--------|---------|
| **Kafka Consumer** | Working | librdkafka-based streaming ingestion |
| **Pulsar/RedPanda Consumer** | Working | pulsar-client-cpp-based streaming |
| **Stream Transformations** | Working | User-defined MAGE module per batch |
| **Parquet Loading** | Working | Apache Arrow (local, S3, HTTP/HTTPS/FTP) |
| **JSONL Loading** | Working | simdjson (local, S3, HTTP/HTTPS/FTP) |
| **CSV Loading** | Working | LOAD CSV Cypher clause |
| **Neo4j Import** | Working | n2mg_cypherl.sh conversion scripts |

### 1.7 Graph Algorithms (MAGE Library)

| Algorithm Type | Count | Languages |
|---------------|-------|-----------|
| **Community Detection** | Multiple | C++, Python |
| **Centrality** | Multiple | C++ |
| **Shortest Path** | Multiple | C++ |
| **PageRank** | Working | C++ |
| **Node Embeddings (Node2Vec)** | Working | Python |
| **GNN (Link Prediction, Node Classification)** | Working | Python |
| **Temporal Graph Networks** | Working | Python |
| **Graph Coloring** | Working | Python (genetic algorithm) |
| **K-Means Clustering** | Working | Python |
| **TSP / VRP** | Working | Python |
| **LLM Utilities** | Working | Python |

### 1.8 Administrative & Operational

| Feature | Status |
|---------|--------|
| **Bolt Protocol v1** | Working (Neo4j-compatible) |
| **Bolt Protocol v5.x** | Working (v2 async server) |
| **WebSocket Logging** | Working |
| **HTTP API** | Working (metrics, health, monitoring) |
| **Prometheus Metrics** | Working (60+ per-DB metrics, 50+ HA metrics) |
| **Audit Logging** | Enterprise (CSV format, ring-buffer buffered) |
| **Telemetry** | Working (anonymous usage stats, KV-store persistence) |
| **Query Profiling (EXPLAIN/PROFILE)** | Working |
| **Plan Caching** | Working (AST + plan LRU caches) |
| **Parallel Query Execution** | Enterprise (thread-pool parallelism) |
| **Multi-Tenancy** | Enterprise (isolated databases per tenant) |
| **TTL (Time-to-Live)** | Working (automatic vertex/edge expiry) |
| **Enum Types** | Working |
| **Schema Introspection** | Working (SHOW SCHEMA INFO) |
| **Server-Side Parameters** | Working |
| **Server-Side Descriptions** | Working |
| **Database Dump** | Working |
| **Free Memory** | Working |
| **Instance Configuration** | Working (SET/SHOW SETTING) |

### 1.9 Client Drivers & Tooling

| Component | Status |
|-----------|--------|
| **C/C++ Client (mgcxx)** | Working |
| **mgconsole** | Working (CLI client) |
| **Memgraph Lab** | Working (GUI visualization) |
| **Python Driver** | Working |
| **C#/Go/Java/JS/Node.js Drivers** | Working |
| **Helm Charts** | Working (Kubernetes deployment) |
| **Docker Images** | Working |
| **DEB/RPM Packages** | Working |
| **AI Toolkit** | External (MCP server, agent frameworks) |

---

## 2. Supported Cypher Syntax Patterns

### 2.1 Reading Clauses

```cypher
-- Standard match with optional direction
MATCH (n:Label {prop: value}) RETURN n
MATCH (n)-[r:EDGE_TYPE]->(m) RETURN n, r, m
MATCH p = (n)-[*1..3]->(m) RETURN p               -- variable-length path
MATCH (n)-[*]->(m) RETURN n, m                     -- unbounded path
MATCH (n) WHERE n.prop =~ "regex.*" RETURN n       -- regex match

-- Optional match (left outer join semantics)
OPTIONAL MATCH (n)-[r]->(m) RETURN n, r, m

-- RETURN clause
RETURN *                                             -- all variables
RETURN DISTINCT n.prop                              -- deduplication
RETURN n.prop AS renamed                           -- aliasing
RETURN n {.prop1, .prop2}                          -- map projection

-- WHERE clause (can appear inline in MATCH or after)
WHERE n.prop = value AND n.age > 30
WHERE n.name STARTS WITH "A"
WHERE n.name ENDS WITH "Z"
WHERE n.name CONTAINS "foo"
WHERE n.prop IN [1, 2, 3]
WHERE n.prop IS NULL / IS NOT NULL

-- WITH clause (pipeline boundary)
MATCH (n) WITH n, count(*) AS cnt WHERE cnt > 1 RETURN n, cnt

-- UNWIND (list expansion)
UNWIND [1, 2, 3] AS x RETURN x
UNWIND range(0, 10) AS i RETURN i

-- CALL procedure
CALL procedure.name(arg1, arg2) YIELD field1, field2 WHERE field1 > 0

-- CALL subquery (Cypher 4.x+)
CALL { MATCH (n) RETURN n } RETURN n
CALL (*) { RETURN 42 AS x }                         -- import all outer variables
CALL (a, b) { RETURN a.prop + b.prop AS sum }       -- explicit scoped variables
```

### 2.2 Writing Clauses

```cypher
-- CREATE
CREATE (n:Label {prop: 'value'})                   -- single node
CREATE (n)-[:EDGE_TYPE {prop: val}]->(m)           -- with edge
CREATE (n {key1: 'val', key2: 42})                  -- map literal properties

-- DELETE
DELETE n                                            -- delete node (fails if edges exist)
DETACH DELETE n                                     -- delete node + all edges

-- SET
SET n.prop = 'value'                                -- scalar assignment
SET n = {prop: 'val', prop2: 42}                    -- map override (replaces all)
SET n += {newProp: 'val'}                           -- map update (merges)
SET n:NewLabel                                      -- add label
SET n.prop.sub.nested = 'val'                       -- nested property

-- REMOVE
REMOVE n.prop                                       -- remove property
REMOVE n:Label                                      -- remove label
REMOVE n.prop.sub.nested                            -- remove nested property

-- MERGE (upsert)
MERGE (n:Label {id: 42})
  ON CREATE SET n.created = timestamp()
  ON MATCH SET n.updated = timestamp()

-- FOREACH (iteration with updates)
FOREACH (x IN list | CREATE (:Node {val: x}))
FOREACH (x IN list | SET n.prop = x | MERGE ...)
```

### 2.3 Set Operations

```cypher
MATCH (n) RETURN n
UNION                                               -- deduplicated merge
MATCH (m) RETURN m

MATCH (n) RETURN n
UNION ALL                                           -- non-deduplicated merge
MATCH (m) RETURN m
```

### 2.4 Modifiers

```cypher
ORDER BY n.prop ASC, n.prop2 DESC                   -- ascending/descending
SKIP 10                                              -- offset
LIMIT 20                                             -- row limit
```

### 2.5 Query Hints (Pragmas)

```cypher
USING INDEX n:Label(property)                        -- force label-property index
USING EDGE INDEX :EDGE_TYPE(property)               -- force edge-type property index
USING PERIODIC COMMIT 1000                           -- batch transaction commits
PARALLEL EXECUTION                                   -- request parallel execution
QUERY MEMORY LIMIT 100 MB                            -- per-query memory cap
```

### 2.6 Expressions

```cypher
-- Logical operators (precedence: OR < XOR < AND < NOT)
n.prop = 1 OR n.prop2 > 5
n.prop = 1 XOR n.prop2 > 5
n.prop = 1 AND n.prop2 > 5
NOT n.prop = 1

-- Comparison operators
= , <> , < , > , <= , >=
IS NULL , IS NOT NULL
IN [val1, val2, ...]

-- Arithmetic operators
+ , - , * , / , % , ^                               -- add, sub, mul, div, mod, exponent

-- String operators
=~ "regex.*pattern"                                  -- regex match
STARTS WITH "prefix"
ENDS WITH "suffix"
CONTAINS "substring"

-- List operations
list[0]                                              -- subscript
list[1..3]                                           -- slicing

-- CASE expressions
CASE n.prop
  WHEN 1 THEN 'one'
  WHEN 2 THEN 'two'
  ELSE 'other'
END

CASE
  WHEN n.prop > 10 THEN 'high'
  WHEN n.prop > 5 THEN 'medium'
  ELSE 'low'
END
```

### 2.7 Literals

```cypher
-- Typed literals
true, false                                          -- boolean
42, -5                                               -- integer
3.14, -2.5e10                                        -- double
'string', "string"                                   -- string (single or double quoted)
null                                                  -- null

-- Collection literals
[1, 2, 3]                                            -- list
[1, 'two', [3, 4]]                                   -- heterogeneous list
{key: 'value', num: 42}                              -- map
n {.*, .prop1, inner: n.inner {.*}}                  -- map projection

-- Parameters (prepared statements)
$param_name
$0                                                    -- positional parameter
```

### 2.8 List Comprehensions

```cypher
-- Basic form
[x IN list WHERE x > 5 | x * 2]                     -- filter + map

-- Filter-only form
[x IN list WHERE x > 5]                              -- equivalent to [x IN list WHERE x > 5 | x]

-- Nested comprehension
[y IN n.list WHERE y.val > 0 | y.name]
```

### 2.9 Pattern Comprehensions

```cypher
[(n)-[r]->(m:Label) WHERE r.weight > 0.5 | m.name]  -- list of matched values
[(n)-[r]->(m:Label) | r]                             -- edges
[(n)-[r]->(m:Label) WHERE r.val | {r: r, m: m}]     -- structured results
```

### 2.10 Existential Subqueries

```cypher
-- Pattern-based existence
MATCH (n) WHERE EXISTS { (n)-[:KNOWS]->(:Person) } RETURN n

-- Query-based existence
MATCH (n) WHERE EXISTS { MATCH (n)-[r]->(m) WHERE r.weight > 0.5 } RETURN n
```

### 2.11 Predicate Functions

```cypher
all(x IN list WHERE x > 0)                           -- all satisfy
any(x IN list WHERE x > 5)                           -- any one satisfies
none(x IN list WHERE x < 0)                          -- none satisfy
single(x IN list WHERE x > 5)                        -- exactly one satisfies
exists(n.prop)                                        -- property existence
```

### 2.12 Scalar Functions (Complete List)

```cypher
-- Type conversion
toBoolean(expr)         toInteger(expr)         toFloat(expr)         toString(expr)
toBooleanOrNull(expr)   toIntegerOrNull(expr)   toFloatOrNull(expr)   toStringOrNull(expr)
toBooleanList(list)     toIntegerList(list)     toFloatList(list)     toStringList(list)

-- String functions
toStringOrNull(expr)    toLower(str)            toUpper(str)
trim(str)               lTrim(str)              rTrim(str)
replace(str, old, new)  substring(str, s, l)    left(str, n)
right(str, n)           split(str, delim)        reverse(str)
size(str)

-- Mathematical functions
abs(n)     ceil(n)     floor(n)     round(n)     sign(n)
sqrt(n)    log(n)      log10(n)     exp(n)
e()         pi()
sin(n)     cos(n)      tan(n)       asin(n)      acos(n)
atan(n)    atan2(y, x)  haversin(n)
degrees(r)  radians(d)

-- Temporal types & functions
date()           date('2024-01-15')         date({year: 2024, month: 1, day: 15})
localTime()      localTime('14:30:00')
localDateTime()  localDateTime('2024-01-15T14:30:00')
duration()       duration('P1DT2H30M')
date.truncate('year', d)                   -- truncation granularity
duration.between(d1, d2)                   -- compute duration
duration.inDays(d, dur)                     -- arithmetic
date(d)                                     -- cast to date
datetime()       datetime('2024-01-15T14:30:00+01:00')

-- Spatial functions
point({x: 1.0, y: 2.0})                    -- 2D point
point({x: 1, y: 2, z: 3})                  -- 3D point
point({longitude: 1, latitude: 2, crs: 'WGS-84'})
point.distance(p1, p2)                     -- point distance
point.withinBBox(p, p_min, p_max)          -- bounding box check

-- UUID & system
randomUUID()                                -- random UUIDv4
timestamp()                                 -- current epoch microseconds

-- Coalesce
coalesce(expr1, expr2, ..., default)        -- first non-null
```

### 2.13 Aggregation Functions

```cypher
count(*)       count(n.prop)     count(DISTINCT n.prop)
min(n.prop)    max(n.prop)       avg(n.prop)
sum(n.prop)    collect(n.prop)
```

### 2.14 List Functions

```cypher
range(start, end [, step])                      -- integer range: [start, end]
size(list)                                       -- list length
head(list)                                       -- first element
last(list)                                       -- last element
tail(list)                                       -- all but first
reverse(list)                                     -- reversed copy
reduce(total = 0, x IN list | total + x)        -- fold/reduce
keys(map)                                        -- map keys as list
values(map)                                      -- map values as list
nodes(path)                                       -- nodes in path
relationships(path)                               -- edges in path
labels(node)                                     -- labels of node
type(edge)                                        -- edge type name
properties(obj)                                   -- properties as map
id(obj)                                           -- internal integer ID
elementId(obj)                                    -- string element ID
startNode(edge)                                   -- source of edge
endNode(edge)                                     -- dest of edge
counter()                                         -- row counter (Memgraph-specific)
hops()                                            -- path hop count (Memgraph-specific)
```

### 2.15 Memgraph-Specific Vector/Search Functions

```cypher
-- Vector similarity search (via vector index)
CALL vector_distance_search(:Label, 'property', embedding, num_results)

-- Point spatial search
CALL point_distance_search(:Label, 'property', point, distance)
CALL withinbbox_distance_search(:Label, 'property', p_min, p_max)
```

### 2.16 Administrative Commands (Complete List)

```cypher
-- Index management
CREATE INDEX ON :Label(property)
CREATE INDEX ON :Label(prop1, prop2)               -- composite
CREATE POINT INDEX ON :Label(property)
CREATE TEXT INDEX ON :Label(property)
CREATE VECTOR INDEX ON :Label(property)
  WITH CONFIG {"dimension": 768, "capacity": 1000, "metric": "cosine"}
CREATE EDGE INDEX ON :EDGE_TYPE(property)
CREATE EDGE TEXT INDEX ON :EDGE_TYPE(property)
CREATE EDGE POINT INDEX ON :EDGE_TYPE(property)
CREATE EDGE VECTOR INDEX ON :EDGE_TYPE(property)
  WITH CONFIG {"dimension": 768, "capacity": 1000}
CREATE GLOBAL EDGE PROPERTY INDEX ON property
DROP INDEX ON :Label(property)
DROP ALL INDEXES

-- Constraint management
CREATE CONSTRAINT ON (n:Label) ASSERT EXISTS (n.prop)
CREATE CONSTRAINT ON (n:Label) ASSERT n.prop IS UNIQUE
CREATE CONSTRAINT ON (n:Label) ASSERT n.prop IS TYPED INTEGER
DROP CONSTRAINT ON (n:Label) ASSERT EXISTS (n.prop)
DROP ALL CONSTRAINTS

-- Auth management (30+ commands)
CREATE USER username [IDENTIFIED BY 'password']
CREATE USER username [PASSWORD 'prehashed_value']
DROP USER username
SET PASSWORD FOR username TO 'new_password'
CHANGE PASSWORD TO 'new_password'

CREATE ROLE rolename
DROP ROLE rolename

GRANT PRIVILEGE privilege TO user_or_role
REVOKE PRIVILEGE privilege FROM user_or_role
DENY PRIVILEGE privilege TO user_or_role
SET ROLE FOR username TO role

GRANT ROLE rolename TO username
REVOKE ROLE rolename FROM username

GRANT DATABASE dbname TO user_or_role
DENY DATABASE dbname TO user_or_role
REVOKE DATABASE dbname FROM user_or_role
SET MAIN DATABASE dbname FOR user_or_role

-- Impersonation (Enterprise)
GRANT IMPERSONATE USER user1 TO user2
DENY IMPERSONATE USER user1 TO user2
REVOKE IMPERSONATE USER user1 FROM user2

-- User profiles (Enterprise)
CREATE USER PROFILE profilename
  WITH CONFIG {"limits": {"sessions": 10, "transactions_memory": "100MB"}}
DROP USER PROFILE profilename
SET USER PROFILE profilename FOR user

-- Database management (Enterprise)
CREATE DATABASE dbname
DROP DATABASE dbname
RENAME DATABASE oldname TO newname
USE DATABASE dbname
SHOW DATABASE(S)

-- Multi-tenancy
SHOW TENANT PROFILES
SHOW TENANT PROFILE profilename

-- Streaming (Kafka / Pulsar)
CREATE KAFKA STREAM streamname
  TOPICS topic1,topic2
  TRANSFORM transformation_name
  BOOTSTRAP_SERVERS 'host:9092'
  [CONFIGS {key: 'value'}]
  [CREDENTIALS {sasl.username: 'user', sasl.password: 'pass'}]

CREATE PULSAR STREAM streamname
  TOPICS topic1,topic2
  TRANSFORM transformation_name
  SERVICE_URL 'pulsar://host:6650'
  [BATCH_INTERVAL 100]
  [BATCH_SIZE 1000]

START/STOP STREAM streamname
START/STOP ALL STREAMS
DROP STREAM streamname
SHOW STREAM(S)
CHECK STREAM streamname [BATCH_LIMIT 10 [TIMEOUT 5000]]

-- Triggers
CREATE TRIGGER triggername ON CREATE
  EXECUTE 'cypher query'
DROP TRIGGER triggername
SHOW TRIGGERS

-- Replication
SET REPLICATION ROLE TO (MAIN|REPLICA)
  WITH PORT 10000
SHOW REPLICAS
REGISTER REPLICA name (SYNC|ASYNC|STRICT_SYNC) TO 'host:port'
DROP REPLICA name

-- Coordinator (Enterprise)
REGISTER INSTANCE instance_name WITH CONFIG {
  'bolt_server': 'host:7687',
  'management_server': 'host:10000',
  'replication_server': 'host:20000'}
UNREGISTER INSTANCE instance_name
SET INSTANCE instance_name TO MAIN
DEMOTE INSTANCE instance_name
ADD COORDINATOR host1 host2 ...
REMOVE COORDINATOR host1 host2 ...
FORCE RESET CLUSTER STATE
SHOW INSTANCE(S)
YIELD LEADERSHIP

-- Configuration
SHOW CONFIG
SHOW VERSION
SHOW SETTING(S) setting_name
SET setting_name TO value

-- Parameters
SET parameter_name TO value
UNSET parameter_name
SHOW PARAMETERS
DELETE ALL PARAMETERS

-- Snapshots
CREATE SNAPSHOT
RECOVER SNAPSHOT
SHOW SNAPSHOTS
SHOW SNAPSHOT 'filename'

-- Enum management
CREATE ENUM enum_name VALUES ('v1', 'v2', 'v3')
SHOW ENUMS
ALTER ENUM enum_name ADD VALUES ('v4')
ALTER ENUM enum_name UPDATE 'v1' TO 'v1_updated'
DROP ENUM enum_name

-- Descriptions (schema metadata)
SET DESCRIPTION ON :Label TO 'description text'
SET DESCRIPTION ON :Label.property TO 'property description'
DELETE DESCRIPTION ON :Label
SHOW DESCRIPTIONS

-- TTL management
SET TTL CONFIG FOR :Label {property: 'created_at', period: 86400}
SET TTL CONFIG FOR :EDGE_TYPE {property: 'expires_at', period: 3600}
SHOW TTL CONFIG FOR :Label
STOP TTL FOR :Label

-- Other administrative
LOCK DATA DIRECTORY
FREE MEMORY
ANALYZE GRAPH (DELETE|STOP|)
DUMP DATABASE
SHOW TRANSACTIONS
TERMINATE TRANSACTIONS 'tx_id1', 'tx_id2', ...
SET SESSION TRACE (ON|OFF)
SHOW SCHEMA INFO                                   -- full graph ontology
SHOW MEMORY INFO
SHOW MEMORY LIMIT
RELOAD TLS
DROP GRAPH
SET STORAGE MODE TO (IN_MEMORY_TRANSACTIONAL|IN_MEMORY_ANALYTICAL|ON_DISK_TRANSACTIONAL)
SET ISOLATION LEVEL TO (SNAPSHOT ISOLATION|READ COMMITTED|READ UNCOMMITTED)
```

---

## 3. Storage Design

### 3.1 High-Level Architecture

The storage subsystem implements a **three-mode** graph storage engine:

```
┌──────────────────────────────────────────────────────────────────────┐
│                        Storage Abstraction                            │
│                                                                       │
│  ┌───────────────────────┐  ┌───────────────────────┐  ┌───────────┐ │
│  │ InMemoryTransactional │  │ InMemoryAnalytical    │  │ Disk      │ │
│  │ (SkipList + MVCC)     │  │ (SkipList, no MVCC)   │  │ (RocksDB) │ │
│  └───────────┬───────────┘  └───────────────────────┘  └─────┬─────┘ │
│              │                                                │       │
│  ┌───────────▼───────────────────────────────────────────────────┐   │
│  │                    Indices                                     │   │
│  │  Label │ Label+Property │ EdgeType │ Text │ Vector │ Point   │   │
│  └────────────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────┐                                         │
│  │        Constraints        │                                         │
│  │  Existence │ Unique │ Type │                                        │
│  └──────────────────────────┘                                         │
│  ┌─────────────────────────────────────────┐                          │
│  │            Durability                     │                          │
│  │  Snapshots + Write-Ahead Log (WAL)       │                          │
│  └─────────────────────────────────────────┘                          │
└──────────────────────────────────────────────────────────────────────┘
```

### 3.2 Storage Modes

| Mode | Enum | MVCC | GC | Durability | Replication | Indices |
|------|------|------|----|-----------|-------------|---------|
| `IN_MEMORY_TRANSACTIONAL` | Production default | Full delta-chain | Periodic | WAL+snapshot | Full | All types |
| `IN_MEMORY_ANALYTICAL` | Direct mutation | None | None | None | None | All types |
| `ON_DISK_TRANSACTIONAL` | RocksDB TS-based | RocksDB TS | N/A | RocksDB WAL | Limited | Label/property/edge |

### 3.3 Core Data Structures

#### Vertex (80 bytes)
```
┌─────────────────────────────────────────────┐
│ Gid gid                 (8 bytes, uint64)   │
│ small_vector<LabelId>   (labels)            │
│ Edges in_edges          (tuple<ETid, V*, E>) │
│ Edges out_edges         (tuple<ETid, V*, E>) │
│ PropertyStore           (12 bytes, compressed)│
│ RWSpinLock lock         (mutable)            │
│ PointerPack<Delta,2>    (delta chain + flags)│
└─────────────────────────────────────────────┘
```

#### Edge (lightweight)
```
┌───────────────────────────────────────┐
│ Gid gid              (8 bytes)        │
│ PropertyStore        (12 bytes, opt)   │  -- only when properties_on_edges is true
│ RWSpinLock lock                        │
│ PointerPack<Delta,1>                   │
└───────────────────────────────────────┘

When properties_on_edges is false, edges are stored as EdgeRef
(union of Gid + pointer) inline in vertex edge lists.
```

#### PropertyStore (12 bytes, compressed)
```
┌──────────────────────────────────────────────────┐
│ Internal buffer (12 bytes):                      │
│   size   (4 bytes) -- total storage size         │
│   buffer (8 bytes) -- inline data or heap ptr    │
│                                                  │
│ Encoding:                                        │
│   Property ID: 2-bit size prefix                 │
│     (7-bit, 15-bit, or 31-bit ID value)          │
│   Type tag:   1 byte                             │
│     EMPTY, NONE, BOOL, INT, DOUBLE, STRING,      │
│     LIST, MAP, TEMPORAL_DATA, ZONED_TEMPORAL,    │
│     OFFSET_ZONED_TEMPORAL, ENUM, POINT, VECTOR   │
│   Value: inline (<=8 bytes) or offset (var-size)  │
│                                                  │
│ Float precision: configurable (16/32/64 bits)    │
│ Compression: Zstd (optional, configurable level) │
└──────────────────────────────────────────────────┘
```

#### Delta (<=56 bytes)
```
┌──────────────────────────────────────────────┐
│ CommitInfo* commit_info    (8 bytes)         │
│ uint64_t command_id        (8 bytes)         │
│ PreviousPtr prev           (8 bytes)         │  -- tagged: Delta|Vertex|Edge
│ atomic<Delta*> next        (8 bytes)         │
│ union {                                      │
│   Action action                            │
│   struct { Action; opt_str value; }         │
│   struct { Action; LabelId value; }         │
│   struct { Action; PropertyId; Value*; V*; }│
│   struct { Action; ETid; TaggedVertexPtr; EdgeRef; }│
│ };                                           │
│                                              │
│ Action types:                                │
│   DELETE_DESERIALIZED_OBJECT                 │
│   DELETE_OBJECT, RECREATE_OBJECT             │
│   SET_PROPERTY, ADD_LABEL, REMOVE_LABEL      │
│   ADD_IN_EDGE, ADD_OUT_EDGE                  │
│   REMOVE_IN_EDGE, REMOVE_OUT_EDGE            │
└──────────────────────────────────────────────┘
```

#### Delta Container (16 KiB slabs)
```
┌─────────────────────────────────────────────┐
│ PageSlabMemoryResource                      │
│   └─ 4-page (16 KiB) slabs                  │
│       └─ static_vector<Delta, 292> per slab │
│           └─ 292 * 56 = 16,352 bytes/slab   │
│                                              │
│ std::forward_list links slabs               │
│ DbArenaMemoryResource for allocation         │
│ Prometheus gauge tracks unreleased deltas    │
└─────────────────────────────────────────────┘
```

### 3.4 MVCC (Multi-Version Concurrency Control)

```
Timeline:
───────────────────────────────────────────────────────────►

Transaction IDs:  2^63 ... 2^63+N  (uncommitted)
Commit timestamps: 0 ... 2^63-1     (committed)

Snapshot Isolation logic:

  ApplyDeltasForRead():
    1. Walk delta chain (newest → oldest)
    2. SNAPSHOT_ISOLATION:
       Apply deltas with timestamp < transaction.start_timestamp
    3. READ_COMMITTED:
       Apply deltas with timestamp < kTransactionInitialId (=2^63)
       (i.e., only committed deltas)
    4. READ_UNCOMMITTED:
       No deltas applied (raw object state)
    5. View (NEW/OLD):
       Controls visibility of own transaction's recent changes

  PrepareForWrite():
    Fails if another uncommitted transaction modified the object
    (write-write conflict detection)

  Non-Sequential Deltas:
    Edge creation deltas can be prepended out-of-order
    to improve concurrent edge-write performance,
    but increase delta chain traversal cost.
```

### 3.5 Index Implementations

```
┌────────────────────────────────────────────────────────┐
│                     Index Types                         │
├────────────────────────────────────────────────────────┤
│ In-Memory Backend (SkipList):                          │
│   • LabelIndex            -- label → vertex set        │
│   • LabelPropertyIndex    -- (label, prop) → vertex set│
│   • EdgeTypeIndex         -- edge type → edge set      │
│   • EdgeTypePropertyIndex -- (etype, prop) → edge set  │
│   • EdgePropertyIndex     -- prop → edge set (global)  │
│   • PointIndex            -- R-tree spatial             │
│   • TextIndex             -- Tantivy FTS               │
│   • VectorIndex           -- USearch ANN               │
│                                                         │
│ On-Disk Backend (RocksDB):                              │
│   • Same conceptual types, implemented as RocksDB CFs   │
│   • Timestamp-aware comparator for MVCC-like behavior   │
└────────────────────────────────────────────────────────┘
```

### 3.6 Constraints Enforcement

- **Existence**: Traversed during `SetProperty` — fails if property is null and constraint exists
- **Unique**: Lock-based check during commit — searches index for duplicates
- **Type**: Validates property values against declared types during `SetProperty`

### 3.7 Durability & Recovery

```
┌────────────────────────────────────────────────────────────────────┐
│                      Durability Architecture                        │
│                                                                     │
│  Modes:                                                             │
│  ┌ DISABLED ──────────────────────────────────────────────────┐    │
│  │ PERIODIC_SNAPSHOT ── periodic full snapshots, no WAL ─────┤    │
│  │ PERIODIC_SNAPSHOT_WITH_WAL ── snapshots + continuous WAL ─┤    │
│  └──────────────────────────────────────────────────────────┘    │
│                                                                     │
│  Snapshot Format (v35):                                             │
│  ┌────────────┬──────────────────────────────────────────────┐     │
│  │ Magic "MGsn"│ Version │ SnapshotInfo │ Sections...         │     │
│  └────────────┴──────────────────────────────────────────────┘     │
│                                                                     │
│  Sections (in order):                                               │
│  1. Edges         2. Vertices       3. Indices         4. EdgeIdx  │
│  5. Constraints   6. Mapper         7. Enums           8. Epochs   │
│  9. Metadata     10. Edge Batches  11. Vertex Batches 12. TTL      │
│ 13. Descriptions                                                    │
│                                                                     │
│  WAL Format (v35):                                                  │
│  ┌────────────┬──────────────────────────────────────────────┐     │
│  │ Magic "MGwl"│ Version │ WalInfo │ Delta stream...          │     │
│  └────────────┴──────────────────────────────────────────────┘     │
│                                                                     │
│  Each delta is: [Marker byte][SLK-encoded payload]                  │
│  Transactions: [START marker][deltas...][END marker]                │
│  50+ marker types for all operations                                │
│                                                                     │
│  Recovery Process:                                                  │
│  1. Find latest snapshot file (YYYYmmddHHMMSSffffff_timestamp)     │
│  2. Load snapshot → restore all objects, indices, constraints      │
│  3. Find WAL files with seq >= snapshot durable timestamp          │
│  4. Replay WAL deltas in order                                    │
│  5. Rebuild derived state (indices + constraints)                  │
└────────────────────────────────────────────────────────────────────┘
```

### 3.8 Garbage Collection

```
GC Phases (when oldest_active >= a committed timestamp):
  1. UNLINK: Unlink committed deltas from delta chains
  2. INDEX_CLEANUP: Remove deleted vertices/edges from indices
  3. DELETE: Remove deleted objects from main storage, deallocate

CommitLog (bitmap-based active transaction tracking):
  • Blocks of 8192 fields × 64 bits each
  • O(1) amortized OldestActive query
  • Used to determine which committed deltas are safe to GC
```

### 3.9 Memory Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                     Memory Management Hierarchy                       │
│                                                                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ Global Memory Tracker (system-wide limit)                     │   │
│  │  ┌──────────────────────────────────────────────────────┐    │   │
│  │  │ Graph Memory Tracker (all graph allocations)          │    │   │
│  │  │  ┌───────────────────────────────────────────────┐   │    │   │
│  │  │  │ Database Memory Tracker (per-tenant limit)     │   │    │   │
│  │  │  │  ┌─────────────────────────────────────────┐  │   │    │   │
│  │  │  │  │ Query Memory Trackers (per-query limit)  │  │   │    │   │
│  │  │  │  └─────────────────────────────────────────┘  │   │    │   │
│  │  │  └───────────────────────────────────────────────┘   │    │   │
│  │  └──────────────────────────────────────────────────────┘    │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                      │
│  Allocation Interception (jemalloc):                                  │
│    malloc/free/new/delete → TrackAllocOnCurrentThread()               │
│    Thread-local storage (initial-exec TLS model, minimal overhead)    │
│    ThreadTrackingBlocker prevents recursive tracking                  │
│                                                                      │
│  DB Arena System:                                                     │
│    Per-database jemalloc arenas for isolation                         │
│    GlobalArenaPool recycles arenas across DB lifetimes                │
│    DbAwareThread inherits arena context across thread boundaries      │
│    Custom extent hooks (db_arena_alloc/dalloc/commit/decommit/...)    │
└─────────────────────────────────────────────────────────────────────┘
```

### 3.10 On-Disk Storage Layout (RocksDB)

```
Storage directory:
  data_directory/
    memgraph.lock                      -- file lock
    databases/
      <db_name>/
        rocksdb/                       -- RocksDB data directory
        wal/                           -- WAL files
        snapshots/                     -- snapshot files
        audit.log                      -- audit log (Enterprise)

RocksDB Column Families:
  vertex_chandle      -- Vertex objects with timestamp-keyed entries
  edge_chandle        -- Edge objects with timestamp-keyed entries
  default_chandle     -- Default / misc data
  out_edges_chandle   -- Outbound edge adjacency
  in_edges_chandle    -- Inbound edge adjacency

Key encoding: [original_key][timestamp] using ComparatorWithU64TsImpl
Edge import mode: Vertex cache accelerates bulk edge creation
```

---

## 4. Per-Subsystem File Analysis

### 4.1 `src/storage/v2/` — Storage Engine (~150 files)

**Core MVCC & Data Structures (root)**

| File | Lines | Purpose |
|------|-------|---------|
| `storage.hpp/.cpp` | ~2000 | Base Storage class: accessors, ID mapping, global state, indices, constraints, TTL |
| `storage_mode.hpp/.cpp` | ~50 | IN_MEMORY_TRANSACTIONAL, IN_MEMORY_ANALYTICAL, ON_DISK_TRANSACTIONAL |
| `config.hpp/.cpp` | ~300 | GC, durability, transaction, disk, salient configuration |
| `mvcc.hpp` | ~200 | ApplyDeltasForRead, PrepareForWrite, NonSequential write, delta linking |
| `delta.hpp/.cpp` | ~150 | Delta struct (10 action types), CommitInfo, PreviousPtr |
| `delta_action.hpp` | ~20 | DeltaAction enum |
| `delta_container.hpp` | ~100 | Slab-based (16 KiB) delta allocator via DB arena |
| `commit_log.hpp/.cpp` | ~300 | Bitmap-based active transaction tracking |
| `transaction.hpp/.cpp` | ~500 | Transaction struct: isolation, deltas, metadata deltas, caches |
| `property_store.hpp/.cpp` | ~800 | Compressed binary property buffer with Zstd support |
| `property_value.hpp/.cppm` | ~400 | Typed property value module |
| `vertex.hpp` | ~80 | Vertex: gid, labels, edges, properties, lock, delta ptr |
| `edge.hpp` | ~60 | Edge: gid, properties, lock, delta ptr; EdgeMetadata for index |
| `vertex_accessor.hpp` | ~200 | Vertex CRUD facade with MVCC visibility |
| `edge_accessor.hpp` | ~200 | Edge CRUD facade with MVCC visibility |
| `edge_ref.hpp` | ~30 | EdgeRef: Gid or Edge* union |
| `id_types.hpp` | ~80 | Gid, LabelId, PropertyId, EdgeTypeId (strong wrappers) |
| `name_id_mapper.hpp` | ~200 | Bidirectional string↔ID mapping via SkipListDb |
| `enum_store.hpp` | ~100 | Enum type/value registration and lookup |
| `schema_info.hpp/.cpp` | ~200 | Schema tracking data structures |
| `metadata_delta.hpp` | ~150 | Tagged union for DDL operations (index/constraint/enum/TTL) |
| `commit_args.hpp` | ~50 | Main/ReplicaWrite/ReplicaRead commit context variant |
| `commit_ts_info.hpp` | ~20 | Last durable timestamp + committed count |
| `transaction_constants.hpp` | ~20 | kTimestampInitialId, kTransactionInitialId |
| `isolation_level.hpp` | ~20 | SNAPSHOT_ISOLATION, READ_COMMITTED, READ_UNCOMMITTED |
| `access_type.hpp` | ~15 | Storage access types |
| `view.hpp` | ~15 | NEW vs OLD view for within-transaction visibility |
| `edge_direction.hpp` | ~15 | OUT vs IN |
| `modified_edge.hpp` | ~60 | ModifiedEdgesMap for disk storage edge tracking |
| `gc_status.hpp` | ~40 | GcPhase enum, GcProgress for SHOW TRANSACTIONS |
| `vertex_info_cache.hpp` | ~50 | Speedup for long delta chain vertex info |
| `async_indexer.hpp` | ~50 | Background index creation for auto-indexing |
| `ttl.hpp/.cpp` | ~300 | Time-to-live system (enable/stop/configure/disable) |
| `snapshot_progress.hpp/.cpp` | ~100 | Progress tracking during snapshot creation |
| `edge_import_mode.hpp` | ~20 | EdgeImportMode enum for disk bulk loading |
| `common_function_signatures.hpp` | ~50 | Shared function signatures |
| `description_store.hpp` | ~80 | Metadata descriptions for labels/edges/properties/patterns |
| `indexed_property_decoder.hpp` | ~30 | Template for external property decoding |
| `fmt.hpp` | ~30 | fmt formatting for storage types |
| `database_protector.hpp` | ~30 | Interface for async operation DB access |
| `point.hpp` | ~40 | Point2d, Point3d spatial types |
| `point_functions.hpp` | ~40 | Distance/bounding-box computation |
| `temporal.hpp/.cpp` | ~300 | Date, Time, DateTime, Duration types |
| `schema_info_glue.hpp` | ~50 | Glue between schema info and storage |
| `population_status.hpp` | ~20 | Index population status flags |
| `edge_info_helpers.hpp` | ~40 | Edge info extraction |
| `vertex_info_helpers.hpp` | ~40 | Vertex info extraction |
| `constraint_verification_info.hpp` | ~30 | Constraint verification during commit |
| `result.hpp` | ~30 | Result<T> generic outcome type |

**`disk/` (RocksDB-backed on-disk storage)**

| File | Purpose |
|------|---------|
| `storage.hpp/.cpp` | DiskStorage with DiskAccessor, edge import mode, vertex/edge caches |
| `rocksdb_storage.hpp/.cpp` | RocksDB TransactionDB wrapper, 5 CFs, timestamp-aware comparator |
| `rocksdb_utils.hpp/.cpp` | Utility functions for RocksDB operations |
| `durable_metadata.hpp/.cpp` | Persisted vertex count, edge count, last timestamp |
| `edge_import_mode_cache.hpp/.cpp` | Vertex cache for bulk edge import |
| `delta_utils.hpp/.cpp` | Delta handling in RocksDB context |
| `name_id_mapper.hpp` | RocksDB-backed name↔ID mapping |
| `label_index.hpp/.cpp` | RocksDB label index |
| `label_property_index.hpp/.cpp` | RocksDB label+property index |
| `edge_type_index.hpp/.cpp` | RocksDB edge type index |
| `edge_type_property_index.hpp/.cpp` | RocksDB edge type+property index |
| `edge_property_index.hpp/.cpp` | RocksDB global edge property index |
| `unique_constraints.hpp/.cpp` | RocksDB unique constraint enforcement |

**`inmemory/` (SkipList-backed in-memory storage)**

| File | Purpose |
|------|---------|
| `storage.hpp/.cpp` | InMemoryStorage with full MVCC, GC, WAL, snapshot, replication |
| `label_index.hpp/.cpp` | SkipList label index |
| `label_property_index.hpp/.cpp` | SkipList label+property composite index |
| `edge_type_index.hpp/.cpp` | SkipList edge type index |
| `edge_type_property_index.hpp/.cpp` | SkipList edge type+property index |
| `edge_property_index.hpp/.cpp` | SkipList global edge property index |
| `unique_constraints.hpp/.cpp` | SkipList unique constraint enforcement |
| `indices_mvcc.hpp` | MVCC-aware index lookups |
| `snapshot_info.hpp` | Snapshot-related info |
| `storagefwd.hpp` | Forward declarations |
| `replication/recovery.hpp/.cpp` | Replica recovery from WAL/snapshot |

**`durability/` (Snapshot + WAL)**

| File | Purpose |
|------|---------|
| `durability.hpp/.cpp` | Recovery orchestrator, file discovery, RecoverDerivedState |
| `wal.hpp/.cpp` | WalFile class, WalDeltaData variant, encoding/decoding/loading |
| `snapshot.hpp/.cpp` | SnapshotInfo, CreateSnapshot, LoadSnapshot, retention management |
| `serialization.hpp/.cpp` | BaseEncoder/Decoder, binary serialization primitives |
| `version.hpp` | Format versions (v14–v35), magic bytes "MGwl"/"MGsn" |
| `marker.hpp` | Re-exports wire_format::Marker, lists all valid marker values |
| `metadata.hpp` | RecoveryInfo, RecoveredIndicesAndConstraints |
| `paths.hpp` | File/directory naming conventions |
| `exceptions.hpp` | RecoveryFailure exception |
| `recovery_type.hpp` | ParallelizedSchemaCreationInfo |
| `storage_global_operation.hpp` | StorageMetadataOperation enum (DDL operations) |
| `ttl_operation_type.hpp` | TtlOperationType enum |

**`indices/` (Index implementations)**

| File | Purpose |
|------|---------|
| `indices.hpp/.cpp` | Indices struct: owns all index implementations |
| `active_indices.hpp` | Concurrent-safe snapshot of active indices |
| `active_indices_updater.hpp/.cpp` | Thread-safe index addition/removal |
| `label_index.hpp` | LabelIndex interface |
| `label_index_stats.hpp/.cpp` | Cardinality estimation for label indices |
| `label_property_index.hpp` | LabelPropertyIndex interface |
| `label_property_index_stats.hpp/.cpp` | Cardinality estimation for LP indices |
| `label_property_index_entry.hpp` | Entry type for LP index values |
| `label_properties_indices_info.hpp` | Relevant LP indices for planning |
| `edge_type_index.hpp` | EdgeTypeIndex interface |
| `edge_type_property_index.hpp` | EdgeTypePropertyIndex interface |
| `edge_property_index.hpp` | EdgePropertyIndex interface |
| `point_index.hpp/.cpp` | R-tree based PointIndexStorage |
| `point_index_change_collector.hpp/.cpp` | Batched point index changes |
| `point_iterator.hpp/.cpp` | Point index result iterators |
| `text_index.hpp/.cpp` | Tantivy-backed TextIndex |
| `text_edge_index.hpp/.cpp` | Tantivy-backed TextEdgeIndex |
| `text_index_utils.hpp/.cpp` | Text index spec, search results |
| `vector_index.hpp/.cpp` | USearch-backed VectorIndex |
| `vector_edge_index.hpp/.cpp` | USearch-backed VectorEdgeIndex |
| `vector_index_utils.hpp` | Vector index spec, match modes |
| `vector_match_mode.hpp` | SINGLE, WILDCARD, ANY_OF, ALL_OF |
| `property_path.hpp` | Ordered property ID sequence |
| `index_order.hpp` | ASC/DESC ordering |
| `tracked_vector_allocator.hpp` | Memory-tracking allocator |
| `errors.hpp` | Index-related errors |
| `indices_utils.hpp` | Utility functions |

**`constraints/` (Constraint enforcement)**

| File | Purpose |
|------|---------|
| `constraints.hpp/.cpp` | Constraints struct |
| `active_constraints.hpp/.cpp` | Concurrent-safe snapshot |
| `active_constraints_updater.hpp` | Thread-safe add/remove |
| `existence_constraints.hpp/.cpp` | NOT NULL enforcement |
| `unique_constraints.hpp/.cpp` | Uniqueness enforcement |
| `type_constraints.hpp/.cpp` | Type enforcement |
| `type_constraints_kind.hpp` | TypeConstraintKind enum |
| `type_constraints_validator.hpp/.cpp` | Property type validation |
| `constraint_violation.hpp` | ConstraintViolation errors |
| `constraints_mvcc.hpp` | MVCC-aware constraint checking |
| `utils.hpp` | Utility functions |

**`replication/` (Storage-level replication)**

| File | Purpose |
|------|---------|
| `replication_client.hpp/.cpp` | Client for syncing from replication master |
| `replication_storage_state.hpp/.cpp` | Per-database replication roles |
| `replication_transaction.hpp/.cpp` | Transaction data replication |
| `rpc.hpp/.cpp` | RPC protocol for replication |
| `serialization.hpp/.cpp` | Delta serialization for network |
| `slk.hpp/.cpp` | SLK integration |
| `recovery.hpp` | Recovery structures |
| `global.hpp` | Global replication config |
| `enums.hpp` | Replication enumerations |

---

### 4.2 `src/query/` — Query Engine (~80 files)

**Core Pipeline**

| File | Purpose |
|------|---------|
| `cypher_query_interpreter.hpp/.cpp` | ParseQuery, MakeLogicalPlan, CypherQueryToPlan, AST/plan caching |
| `interpreter.hpp/.cpp` | Per-session query lifecycle (Parse/Prepare/Pull), transactions, auth |
| `interpreter_context.hpp/.cpp` | Global singleton: AST cache, replication, auth, DBMS, worker pool |
| `context.hpp/.cpp` | Per-query ExecutionContext: frame, evaluator, DB accessor, profiling |
| `typed_value.hpp/.cpp` | Universal value type (Null, Bool, Int, Double, String, List, Map, Vertex, Edge, Path, Date, DateTime, Duration, Point, Enum, Graph) |
| `common.hpp/.cpp` | QueryTimestamp, AbortReason, ExecutionStats, Notification |
| `parameters.hpp` | Token-position-keyed parameters |
| `db_accessor.hpp/.cpp` | High-level storage accessor wrapping Name→ID lookups |
| `config.hpp` | InterpreterConfig |
| `metadata.hpp/.cpp` | Query metadata |
| `graph.hpp/.cpp` | In-memory graph for RETURN results |
| `trigger.hpp/.cpp` | Trigger infrastructure |
| `auth_checker.hpp` | Authorization checking |

**`frontend/` (Parsing + AST)**

| File | Purpose |
|------|---------|
| `parsing.hpp/.cpp` | ParseIntegerLiteral, ParseStringLiteral (Unicode), ParseDoubleLiteral |
| `stripped.hpp/.cpp` | StrippedQuery: literal-stripping for cacheable hashes |
| `opencypher/grammar/Cypher.g4` | Base openCypher grammar (standard clauses) |
| `opencypher/grammar/MemgraphCypher.g4` | ~1029 lines: Memgraph extended grammar |
| `opencypher/grammar/MemgraphCypherLexer.g4` | Memgraph lexer tokens |
| `opencypher/parser.hpp` | ANTLR4 Parser wrapper |
| `ast/ast.hpp` | All AST node types (~60 node types) |
| `ast/ast_storage.hpp` | Arena allocator with string interning |
| `ast/ast_visitor.hpp` | Visitor base classes |
| `ast/cypher_main_visitor.hpp/.cpp` | ~150 visit* methods: parse tree → AST |
| `ast/pretty_print.hpp/.cpp` | AST pretty-printer |
| `semantic/symbol.hpp/.cpp` | Symbol (named variable + frame position) |
| `semantic/symbol_table.hpp` | SymbolTable |
| `semantic/symbol_generator.hpp/.cpp` | MakeSymbolTable: scan AST → symbol table |
| `semantic/rw_checker.hpp/.cpp` | Read-only vs write query detection |
| `semantic/required_privileges.hpp/.cpp` | Extract required auth privileges from AST |

**`plan/` (Query Planning + Optimization)**

| File | Purpose |
|------|---------|
| `planner.hpp` | MakeLogicalPlan, PostProcessor, plan cache validation |
| `preprocess.hpp/.cpp` | CollectQueryParts: AST → QueryParts |
| `operator.hpp/.cpp` | ~60 logical operators + their Cursor implementations (~3200+ lines) |
| `rule_based_planner.hpp/.cpp` | RuleBasedPlanner: QueryParts → operator tree |
| `variable_start_planner.hpp/.cpp` | Multiple-plan generation for cost-based selection |
| `cost_estimator.hpp` | CostEstimator: cardinality × per-op cost |
| `cost_constants.hpp` | Estimation parameters |
| `read_write_type_checker.hpp/.cpp` | Plan read/write type inference |
| `profile.hpp/.cpp` | ProfilingStats: EXPLAIN/PROFILE output (table/JSON) |
| `scoped_profile.hpp` | RAII per-operator TSC cycle counter |
| `used_index_checker.hpp/.cpp` | Validate cached plan's indices are still ready |
| `parallel_checker.hpp/.cpp` | Verify plan supports parallel execution |
| `vertex_count_cache.hpp` | Cached DB stats for planning |
| `pretty_print.hpp/.cpp` | Operator tree formatting |
| `hint_provider.hpp/.cpp` | Index hint extraction from pragmas |
| `rewrite/index_lookup.hpp/.cpp` | Filter+ScanAll → indexed scan |
| `rewrite/edge_index_lookup.hpp` | Edge index rewriting |
| `rewrite/join.hpp` | Cartesian+filter → HashJoin |
| `rewrite/general.hpp/.cpp` | RemoveExpressions from AND-trees |
| `rewrite/range.hpp/.cpp` | Filter compaction into RangeOperator |
| `rewrite/balanced_union.hpp` | BalancedDisjunctionUnion |
| `rewrite/enum.hpp` | Enum access rewriting |
| `rewrite/order_by_elimination.hpp` | ORDER BY elimination when index order matches |
| `rewrite/plan_validator.hpp` | Post-rewrite validation |
| `rewrite/periodic_delete.hpp` | Periodic commit DELETE rewriting |
| `rewrite/parallel_rewrite.hpp` | Parallel execution injection (Enterprise) |

**`interpret/` (Expression Evaluation)**

| File | Purpose |
|------|---------|
| `eval.hpp/.cpp` | ExpressionEvaluator: TypedValue visitor for all AST nodes |
| `frame.hpp/.cpp` | Frame: dense TypedValue vector indexed by symbol position |
| `awesome_memgraph_functions.hpp/.cpp` | NameToFunction registry, FunctionContext, IsFunctionPure |

**`stream/` (Stream Processing)**

| File | Purpose |
|------|---------|
| `streams.hpp/.cpp` | Streams manager: create/start/stop/check, KV persistence, restoration |
| `sources.hpp/.cpp` | KafkaStream, PulsarStream wrappers |
| `common.hpp/.cpp` | CommonStreamInfo, StreamSourceType |

---

### 4.3 `src/auth/` — Authentication (~20 files)

| File | Purpose |
|------|---------|
| `auth.hpp/.cpp` | Core Auth class: user/role CRUD, authentication, SSO, database access |
| `models.hpp/.cpp` | User, Role models with permissions, fine-grained access, impersonation |
| `module.hpp/.cpp` | External SSO module management (clone+seccomp sandbox, Unix pipe IPC) |
| `crypto.hpp/.cpp` | Password hashing (bcrypt, SHA256, SHA256-multi), hash upgrade |
| `exceptions.hpp` | AuthException |
| `rpc.hpp/.cpp` | UpdateAuthDataRpc, DropAuthDataRpc for replication |
| `profiles/user_profiles.hpp/.cpp` | UserProfiles: resource limiting profiles |
| `reference_modules/common.py` | Shared module helpers |
| `reference_modules/ldap.py` | LDAP authentication module |
| `reference_modules/saml.py` | SAML SSO module |
| `reference_modules/oidc.py` | OpenID Connect SSO module |
| `reference_modules/kerberos.py` | Kerberos SSO module |
| `reference_modules/example.py` | Template module |

---

### 4.4 `src/coordination/` — HA Coordination (~40 files, Enterprise)

| File | Purpose |
|------|---------|
| `coordinator_instance.hpp/.cpp` | CoordinatorInstance: manages Raft state, leadership, failover |
| `raft_state.hpp/.cpp` | RaftState: wraps NuRaft server, callbacks, snapshots |
| `coordinator_state.hpp/.cpp` | Coordinator cluster logic: leader/follower transitions |
| `coordinator_state_machine.hpp/.cpp` | Extends nuraft::state_machine: commit, snapshot, migration |
| `coordinator_state_manager.hpp/.cpp` | Extends nuraft::state_mgr: config, state, log store |
| `coordinator_cluster_state.hpp/.cpp` | Application state: data instances, MAIN UUID, cluster settings |
| `coordinator_log_store.hpp/.cpp` | Raft log persistence |
| `replication_instance_connector.hpp/.cpp` | Coordinator→data instance connection management |
| `replication_instance_client.hpp/.cpp` | RPC client for data instances |
| `coordinator_instance_connector.hpp/.cpp` | Inter-coordinator connection management |
| `coordinator_instance_client.hpp/.cpp` | Inter-coordinator RPC client |
| `coordinator_instance_management_server.hpp/.cpp` | Management RPC server |
| `data_instance_management_server.hpp/.cpp` | Data instance management server |
| `coordinator_rpc.hpp/.cpp` | 40+ RPC message types with versioned SLK serialization |
| `coordinator_communication_config.hpp/.cpp` | Coordinator communication config |
| `constants.hpp` | Cluster constants |
| `coordinator_ops_status.hpp` | Operation status enums |
| `instance_status.hpp` | Instance status tracking |
| `instance_state.hpp` | Instance state types |
| `coordination_observer.hpp/.cpp` | Observation hooks |
| `utils.hpp/.cpp` | Coordinator utilities |
| `logger.hpp/.cpp` | Coordinator-specific logging |

---

### 4.5 `src/replication/` — Data Plane Replication (~15 files)

| File | Purpose |
|------|---------|
| `state.hpp/.cpp` | Global ReplicationState: MAIN/REPLICA roles, durability |
| `config.hpp/.cpp` | ReplicationServerConfig, ReplicationClientConfig |
| `replication_server.hpp/.cpp` | RPC server for replica data reception |
| `replication_client.hpp/.cpp` | Outbound client: heartbeats, delta/WAL streaming |
| `epoch.hpp/.cpp` | ReplicationEpoch for MAIN UUID discrimination |
| `status.hpp/.cpp` | Durability schema, versioning up to V6 |

---

### 4.6 `src/replication_handler/` — Replication Glue (~6 files)

| File | Purpose |
|------|---------|
| `replication_handler.hpp/.cpp` | ReplicationHandler: implements ReplicationQueryHandler, MAIN↔REPLICA transitions |
| `system_replication.hpp/.cpp` | SystemRecoveryHandler, FinalizeSystemTxHandler |
| `system_rpc.hpp/.cpp` | SystemRecoveryReq/Res V1/V2 |
| `auth_replication_handlers.hpp/.cpp` | Auth data replication (users, roles, profiles) |

---

### 4.7 `src/dbms/` — Database Management (~10 files)

| File | Purpose |
|------|---------|
| `database.hpp` | Database aggregate: storage, triggers, streams, plan cache, TTL, memory |
| `dbms_handler.hpp` | DbmsHandler: multi-tenant database management, stats aggregation |
| `handler.hpp` | Handler<T>: generic multi-tenant Gatekeeper management |
| `database_handler.hpp` | DatabaseHandler (Enterprise): factory for new databases |
| `replication_handlers.hpp/.cpp` | RPC handlers for DB create/drop/rename replication |
| `rpc.hpp` | SLK-serializable DB replication messages |
| `coordinator_handler.hpp` | CoordinatorHandler: cluster coordination operations |
| `tenant_profiles.hpp` | TenantProfiles: resource profiles (Enterprise) |
| `global.hpp` | Error enums, exceptions |
| `constants.hpp` | Default DB name, system DB alias, multi-tenant dir name |
| `inmemory/replication_handlers.hpp/.cpp` | In-memory replication state |
| `inmemory/storage_helper.hpp` | Storage helper |

---

### 4.8 `src/communication/` — Protocol Layer (~30 files)

| File | Purpose |
|------|---------|
| `server.hpp` | Server template (epoll edge-triggered, thread pool) |
| `listener.hpp` | Epoll-based connection listener |
| `session.hpp` | Session: socket, SSL, buffering |
| `client.hpp` | Client: blocking socket with SSL |
| `context.hpp` | ServerContext/ClientContext: SSL/TLS config |
| `buffer.hpp` | 64KB grow-only byte buffer |
| `metrics.hpp` | Bolt driver connection metrics |
| `exceptions.hpp` | SessionClosedException |
| `bolt/v1/` | Bolt protocol v1 (decoder, encoder, state machine, session) |
| `bolt/v1/bolt/client.hpp/.cpp` | High-level Bolt client |
| `v2/server.hpp` | Async server using Boost.Asio |
| `v2/pool.hpp` | IOContextThreadPool |
| `v2/session.hpp` | Async session: TCP, SSL, WebSocket variant |
| `http/server.hpp` | HTTP server (Boost.Beast) |
| `websocket/server.hpp` | WebSocket logging server |

---

### 4.9 `src/integrations/` — Streaming Integration (~15 files)

| File | Purpose |
|------|---------|
| `kafka/consumer.hpp/.cpp` | librdkafka wrapper: Start, Stop, Check, offset management |
| `kafka/exceptions.hpp` | 9 Kafka exception types |
| `pulsar/consumer.hpp/.cpp` | Pulsar C++ client wrapper: Start, Stop, Check, dedup |
| `pulsar/exceptions.hpp` | 8 Pulsar exception types |

---

### 4.10 `src/planner/` — E-Graph Plan Optimizer (~30 files)

| File | Purpose |
|------|---------|
| `eids.cppm` | EClassId, ENodeId strong types |
| `concepts.cppm` | ENodeSymbol concept |
| `union_find.cppm` | Disjoint-set: path halving, union by rank |
| `enode.cppm` | ENode: symbol + children, hash-consed |
| `eclass.cppm` | EClass: equivalence class with parent tracking |
| `egraph.cppm` | EGraph: De Moura/Bjorner rebuild algorithm |
| `typed_egraph.cppm` | Type-safe wrapper with symbol traits |
| `pattern/pattern.hpp` | Declarative pattern definition DSL |
| `pattern/match.hpp` | Match + MatchBindings |
| `pattern/match_index.hpp` | MatcherIndex |
| `rewrite/rule.hpp` | RewriteRule: pattern + apply function |
| `rewrite/rewriter.hpp` | RewriteEngine: saturation loop |
| `extract/extract.hpp` | Cost-based extraction from e-graph |
| `extract/pareto_frontier.hpp` | Multi-objective Pareto frontier |

---

### 4.11 `src/io/`, `src/slk/`, `src/wire_format/` — I/O & Serialization (~10 files)

| File | Purpose |
|------|---------|
| `io/network/endpoint.hpp` | Network endpoint with DNS resolution |
| `io/network/socket.hpp` | POSIX socket RAII wrapper |
| `io/network/epoll.hpp` | Linux epoll RAII wrapper |
| `slk/streams.hpp/.cpp` | Segmented streaming protocol (256 KiB chunks) |
| `slk/serialization.hpp` | Template-based type serialization (654 lines) |
| `wire_format/marker.hpp` | Marker enum: 50+ type/delta markers |
| `wire_format/serialization.hpp` | Marker + SLK convenience functions |

---

### 4.12 `src/memory/` — Memory Management (~10 files)

| File | Purpose |
|------|---------|
| `query_memory_control.hpp/.cpp` | Per-thread query memory tracking |
| `global_memory_control.hpp/.cpp` | Global jemalloc hooks, purge, background threads |
| `db_arena.hpp/.cpp` | Per-DB jemalloc arena pools, extent hooks |
| `db_arena_fwd.hpp` | ArenaMemoryResource, DbArenaScope, DbAwareThread |
| `malloc_free.cpp` | malloc/free/calloc/realloc interceptors |
| `new_delete.cpp` | operator new/delete interceptors |

---

### 4.13 `src/utils/` — Utilities (~100+ files)

Key subsystems: concurrency (spin_lock, rw_lock, barrier, data_queue, gatekeeper), threading (thread_pool, priority_thread_pool, scheduler), memory (memory_tracker, small_vector, static_vector, PMR types), data structures (lru_cache, skip_list, ring_buffer, event_map), serialization (base64, endian, compressor, uuid), system (signals, stat, file, sysinfo), strings (string, safe_string, fixed_string), flags (flag_validation, settings), and more.

---

### 4.14 `src/metrics/`, `src/audit/`, `src/telemetry/`, `src/license/` — Operational (~15 files)

| Subsystem | Purpose |
|-----------|---------|
| `metrics/prometheus_metrics.hpp/.cpp` | ~60 per-DB gauges/counters/histograms, ~50 HA metrics |
| `audit/log.hpp/.cpp` | Enterprise CSV audit logging with ring buffer |
| `telemetry/telemetry.hpp/.cpp` | Anonymous usage telemetry with KV persistence |
| `license/license.hpp/.cpp` | Enterprise license validation (CLI > ENV > DB priority) |

---

### 4.15 `mage/` — Algorithm Library (~50 files)

Built-in algorithms: community detection, centrality, shortest paths, PageRank, bridging, node2vec, k-means, TSP, VRP, graph coloring, set cover, GNN link prediction, GNN node classification, temporal graph networks, LLM utilities, XML support, JSON utilities, Elasticsearch integration.

---

## 5. Pros and Cons of Implementations

### 5.1 Pros

#### Architecture & Design

| Pro | Detail |
|-----|--------|
| **Three-mode storage** | Unified API over IN_MEMORY_TRANSACTIONAL, ANALYTICAL, and ON_DISK modes lets users trade performance vs durability |
| **Clean abstraction layers** | storage → dbms → glue → communication layers are well-separated, enabling independent evolution |
| **Template-based protocol polymorphism** | Server<TSession, TSessionContext> instantiated for Bolt, HTTP, WebSocket without code duplication |
| **E-graph planner V2** | State-of-the-art equality saturation approach for query optimization (experimental but architecturally sound) |
| **Pluggable index architecture** | New index types (text, vector, point, edge-property) added without modifying core storage |
| **Dual V1/V2 communication** | Maintains backward-compatible Bolt v1 while evolving to async v2 |

#### Performance

| Pro | Detail |
|-----|--------|
| **Sub-millisecond traversals** | In-memory SkipList-based storage with pointer-chasing friendly layout |
| **PropertyStore compression** | 12-byte compact structure with Zstd for space efficiency |
| **Non-sequential deltas** | Edge creation deltas prepended out-of-order for concurrent write throughput |
| **Cursor-based pull execution** | Streaming results without materializing intermediate sets |
| **Plan caching** | Two-level cache (AST + plan) keyed by normalized query hash |
| **Cost-based planning** | VariableStartPlanner generates multiple plans, selects cheapest using DB statistics |
| **Parallel execution** | Thread-pool operators for scan, aggregate, sort (Enterprise) |
| **Edge import mode** | Specialized bulk-load path avoids repeated RocksDB lookups |
| **CommitLog bitmap** | O(1) amortized oldest-active-transaction lookup for efficient GC scheduling |

#### Reliability

| Pro | Detail |
|-----|--------|
| **MVCC snapshot isolation** | Full ACID semantics for concurrent transactions |
| **Write-ahead logging** | Crash recovery via snapshot + WAL replay |
| **Raft consensus** | Battle-tested (NuRaft library) for HA coordination |
| **System transactions** | ISystemAction abstraction for replicated metadata operations |
| **Exponential backoff leader reconciliation** | Robust coordinator recovery on leader restart |
| **Epoch-based cache invalidation** | Auth and other caches stay consistent across updates |

#### Security

| Pro | Detail |
|-----|--------|
| **SSO via external modules** | seccomp-sandboxed Python processes for SAML/OIDC/Kerberos/LDAP |
| **Custom hash algorithms** | bcrypt (workfactor 12), SHA256-multi with salt, auto-upgrade unsalted hashes |
| **Three-value access control** | GRANT/DENY/NEUTRAL with DENY priority (most-secure model) |
| **Label-based fine-grained ACLs** | ANY vs EXACTLY matching modes for edge types and labels |
| **UUID-based impersonation** | Survives user rename/recreation |

#### Observability

| Pro | Detail |
|-----|--------|
| **Prometheus metrics** | 60+ per-database metrics, 50+ HA metrics, 6 histograms |
| **Query profiling** | EXPLAIN (plan tree) and PROFILE (TSC-annotated operator costs) |
| **Audit logging** | Non-blocking ring-buffer, CSV format, SQL-injection-safe escaping |
| **Telemetry** | Offline-resilient KV-stored persistent telemetry |

### 5.2 Cons

#### Complexity & Maintainability

| Con | Detail |
|-----|--------|
| **Excessive code generation** | ANTLR4 generates ~40MB of C++ from grammars; slow compile times |
| **Three storage modes, three index backends** | In-memory SkipList, RocksDB, and Tantivy/USearch each require separate index implementations — combinatorial explosion (8 index types × 3 backends = 24 implementations) |
| **Enterprise/Community ifdefs** | `#ifdef MG_ENTERPRISE` scattered throughout ~150 files; significant cognitive overhead |
| **Versioned serialization everywhere** | WAL v35, snapshot v35, durability schema V6, coordinator RPC V1→V3, system recovery V1→V2 — migration complexity is high and error-prone |
| **Two planners** | RuleBasedPlanner + experimental PlannerV2 (e-graph) means two code paths to maintain; PlannerV2 is not yet production |
| **SLK is bespoke** | Custom binary serialization protocol (not Protobuf/Cap'n'Proto/FlatBuffers) — adds learning curve and maintenance burden |
| **Module-based C++20** | `.cppm` C++20 modules used for some subsystems (property_value, planner) but not others — inconsistent module/header usage |
| **~7,772 files in the repo** | Massive codebase; onboarding difficulty is high |

#### Performance Concerns

| Con | Detail |
|-----|--------|
| **Delta chain walk cost** | Long-running transactions or non-sequential deltas can produce very long delta chains, slowing reads |
| **GC stutter** | Periodic GC can block mutation-heavy workloads during unlink/index-cleanup/delete phases |
| **Full snapshot cost** | Periodic full snapshots duplicate the entire database; for large DBs this is expensive I/O |
| **WAL replay on recovery** | For large WAL chains (many WAL files after snapshot), recovery time can be significant |
| **PropertyStore 12-byte limit** | Small property buffers overflow to heap allocation, losing the compact benefit |
| **Edge import mode is disk-only** | No equivalent bulk-load optimization for in-memory mode |
| **Spin-lock contention** | Delta chain locks use spin locks — under high write contention, CPU spinning wastes cycles |

#### Architecture Gaps

| Con | Detail |
|-----|--------|
| **No sharding/partitioning** | Single-node storage model; no automatic graph partitioning for horizontal scale-out |
| **Coordinator is separate process** | HA requires a separate coordinator cluster (odd number of nodes) — adds operational complexity |
| **Unified main for all databases** | During failover, a single instance becomes MAIN for ALL databases; can't have per-DB mains |
| **Limited on-disk storage features** | No replication, no text/vector/point indices in ON_DISK_TRANSACTIONAL mode |
| **RocksDB dependency** | Only on-disk backend option; no pluggable storage engines |
| **No optimistic concurrency** | Write-write conflicts abort transactions rather than resolving them |
| **Session memory is thread-local** | Can't easily move query state between threads |
| **Bolt v1 backward compatibility** | Maintaining two Bolt protocol versions (v1 epoll + v2 asio) duplicates protocol logic |

#### Operational Issues

| Con | Detail |
|-----|--------|
| **WAL file cleanup is snapshot-gated** | WAL files accumulate until the next snapshot is created; long snapshot intervals = large disk usage |
| **No incremental backups** | Only full snapshots; no differential or incremental backup capability |
| **SSO modules are single-threaded** | `clone()` + seccomp per module means process-per-auth-call overhead |
| **Prometheus histogram buckets are hardcoded** | 15 fixed buckets (10us–60s for latency, 1KB/s–1GB/s for throughput); not configurable |
| **Telemetry can't be user-disabled cleanly** | Shipped with default-on telemetry that some enterprise deployments must disable manually |
| **ARM64 is secondary** | Optimized primarily for x86_64 (intrinsic-specific code paths) |

#### Testing Gaps

| Con | Detail |
|-----|--------|
| **No formal verification** | MVCC correctness relies on testing, not TLA+/proof |
| **Fuzz testing is limited** | Only PropertyStore has fuzz tests; no fuzz testing on delta chains, WAL, or replication |
| **Jepsen tests are unmaintained** | Jepsen tests exist but are not run in CI |
| **Driver tests are manual** | Client driver tests (C#, Go, Java, JS, Node, Python) are not automated in CI |

---

## Summary Statistics

| Metric | Count |
|--------|-------|
| Total source files | 7,772 |
| Total directories | 1,773 |
| C++ source files | ~3,500+ |
| Test files | ~400+ |
| Core storage files | ~150 |
| Query engine files | ~80 |
| Auth files | ~20 |
| Replication files | ~25 |
| Coordinator files | ~40 |
| Communication files | ~30 |
| MAGE algorithm library files | ~50 |
| Public API headers | 14 |
| Supported Cypher clauses | 20+ |
| Supported Cypher functions | 80+ |
| Administrative commands | 50+ |
| Index types | 8 |
| Constraint types | 3 |
| Permission types | 32 coarse + 8 fine-grained |
| Prometheus metrics families | 110+ |
| Build dependencies (Conan) | 37 |
| CMake build definition | 557 lines |
| Latest WAL/snapshot version | v35 |
| Reference SSO modules | 5 (LDAP, SAML, OIDC, Kerberos, Basic) |
