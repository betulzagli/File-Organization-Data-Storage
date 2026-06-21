# File Organization & Data Storage Benchmarking

This project is a high-performance C application designed to clean a large-scale password dataset (10 million entries) and evaluate the engineering trade-offs of storing it using three distinct data representation methodologies: **Raw (Plain Text)**, **Hashed (MD5)**, and **Compressed (Huffman Coding)**.

The system empirically benchmarks search execution times, CPU/RAM utilization, and disk footprint to demonstrate the fundamental principles of selecting the right storage architecture.

## 🚀 Features & Architecture

* **Memory-Efficient Preprocessing:** Ingests the 10 million entry `Pwdb_top-10000000.txt` dataset and utilizes the `qsort` algorithm to deduplicate and sanitize the data down to 500,000 unique records without memory leaks.
* **Native Cryptography:** Implements secure 128-bit MD5 hashing directly via the Windows Native Cryptography API (`wincrypt.h`), avoiding external dependencies like OpenSSL.
* **Lossless Bitwise Compression:** Features a complete, custom-built Huffman encoder/decoder that performs frequency analysis, builds a binary tree, and packs prefix-free binary codes using bitwise shift operations.
* **Zero Third-Party Libraries:** Built entirely with Standard C and native Windows Application Programming Interfaces (APIs).

## 📊 Operational Data Flow

```text
    [RawData] (10M SecLists Passwords)
         │
         ▼
    [Preprocessing Module] (qsort & Deduplication)
         │
         ▼
    [Processed Dataset] (cleaned.txt - 500k unique)
         │
    ┌────┼──────────────────────────┐
    ▼    ▼                          ▼
  [Raw] [Hashed] (MD5)            [Compressed] (Huffman)
    │    │                          │
    │    │   ┌──────────────────────┘
    │    │   ▼
    │    │ [Decompression] (temp_unpacked.txt)
    │    │   │
    └────┼───┘
         ▼
    [Benchmarking Engine] (search.c)
         │
         ▼
    [Results] (report.txt)
```

## ⏱️ Complexity Analysis

The theoretical time complexities of the core operations are evaluated using Big O notation:
* **Sorting and Preprocessing:** The internal `qsort` algorithm operates with an average time complexity of **O(N log N)**, where N is the number of raw passwords.
* **Raw Search:** The linear search mechanism reads line-by-line, resulting in a worst-case time complexity of **O(N)**.
* **Hashed Search:** Generating the hash takes **O(L)** time (where L is string length), followed by a linear lookup. The overall search complexity remains **O(N)**, but with a higher constant factor due to cryptographic overhead.
* **Compressed Search:** The decompression phase requires **O(V)** time, where V is the total number of bits in the compressed file, prior to the **O(N)** linear search.

## 📈 Performance Benchmark Results

Results are aggregated over 100 randomized search iterations to ensure statistical reliability, filtering out OS-level background interference.

| Storage Method | Avg Search Time | Max Search Time | Std. Deviation | Avg CPU | Avg RAM | Disk Size |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **RAW** | 4.8400 ms | 10.0000 ms | 2.6598 ms | 6.26 % | 5.97 MB | 4.55 MB |
| **HASHED** | 14.5400 ms | 35.0000 ms | 8.2054 ms | 6.16 % | 5.97 MB | 16.21 MB |
| **COMPRESSED** | 5.6000 ms | 72.0000 ms | 7.2305 ms | 5.37 % | 5.98 MB | 2.81 MB |

* **Compression Ratio:** `0.6176` (Approx. 38% reduction in physical disk space)
* **Decompression Time (Overhead):** `778.0000 ms`

## ⚙️ How to Run Locally

1. Clone the repository:
   ```bash
   git clone [https://github.com/betulzagli/File-Organization-Data-Storage.git](https://github.com/betulzagli/File-Organization-Data-Storage.git)
   ```
2. Download the `Pwdb_top-10000000.txt` file from the [SecLists Repository](https://github.com/danielmiessler/SecLists/tree/master/Passwords/Common-Credentials) and place it inside the `RawData/` directory.
3. Open the `.sln` file in **Microsoft Visual Studio** (MSVC compiler required).
4. Build and Run the project in `Release` or `Debug` mode. The system will automatically execute all phases and print the benchmark table to the terminal.
