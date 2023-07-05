# Usage Guide
- Change the test vectors by uncommenting in cc20_tb.cpp
- Change the length of the test vector in cc20.h (header file)


# ChaCha-20 Performance Report

This report provides a summary of the exhaustive synthesis reports (given in this directory) for the ChaCha-20 encryption algorithm.

## Non-Optimized Code

### Latency
The non-optimized code has a latency of 3774 cycles.

| Latency   |
| --------- | 
|3774|

### Resource Utilization
Comparatively less resource utilization.

| Name           | BRAM_18K | DSP48E | FF     | LUT   | URAM |
| -------------- | -------- | ------ | ------ | ----- | ---- |
| Total          | 7        | 0      | 2452   | 3928  | 0    |
| Available      | 280      | 220    | 106400 | 53200 | 0    |
| Utilization (%)| 2        | 0      | 2      | 7     | 0    |


## Optimized Code

### Latency
The optimized code has a significantly lower latency of 1455 cycles, a reduction of over 60% compared to the non-optimized code.

|  Latency  |
| --------- | 
|    1455   |

### Resource Utilization
The optimized code has a higher resource utilization.

| Name           | BRAM_18K | DSP48E | FF     | LUT   | URAM |
| -------------- | -------- | ------ | ------ | ----- | ---- |
| Total          | 3        | 0      | 23974  | 27031 | 0    |
| Available      | 280      | 220    | 106400 | 53200 | 0    |
| Utilization (%)| 1        | 0      | 22     | 50    | 0    |
