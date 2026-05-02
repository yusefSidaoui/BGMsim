# BGP Simulator

## Overview

This project implements a simplified **BGP (Border Gateway Protocol) simulator** in C++. It builds an AS graph from CAIDA data, propagates routing announcements using BGP rules, applies route selection policies, and outputs the final routing state as a CSV.

---

## Files

### `main.cpp`

* Entry point
* Parses CLI arguments
* Builds graph, loads data, runs simulation, writes output

### `ASGraph.cpp`

* Stores the AS graph
* Adds provider/customer and peer relationships
* Detects cycles
* Computes propagation ranks

### `Parser.cpp`

* Reads input files:

  * AS relationships (CAIDA)
  * Announcements (CSV)
  * ROV-enabled ASNs
* Seeds initial announcements into the graph

### `BGP.cpp`

* Core BGP logic:

  * `better()` → route selection (relationship > path length > next hop)
  * `sendAnnouncement()` → sends routes between ASes
  * `processNode()` → updates local RIB
* Implements propagation:

  * Up (customers → providers)
  * Across (peers, one hop)
  * Down (providers → customers)
* Outputs final results (`dumpCSV`)

---

## How to Run

### Compile

```bash
g++ -std=c++17 src/*.cpp -Iinclude -o simulator
```

### Run

```bash
./simulator <as-rel> <announcements> <rov> <output>
``` 