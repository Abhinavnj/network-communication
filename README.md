# network-communication

*Completed in collaboration between Abhinav Sirohi (as3249) & Srinandini Marpaka (sm2237) for Systems Programming (01:198:214)*

## Testing
### Test Cases
- running the server without any arguments
- running the server with more than two arguments
- incorrect payload lengths
- incorrect number of newline characters
- long key/value lengths
- short key/value lengths (including single character)
- inserting the same key (updates the key with the new value)
- inserting different keys
- getting the values of keys
- getting the values of deleted keys
- deleting keys
- deleting nonexistent keys
- inserting keys and getting them from different clients
- deleting keys and attempting to get them from different clients
- deleting all inserted keys
- multiple messages from the same client
- correctly formatted input followed by incorrectly formatted input from the same client
- incorrect message code (not SET, GET, or DEL)
- nondecimal payload length
- negative payload length
- checking return code of correct input (OKG, OKS, OKD)
- connecting many clients

### Strategy
#### Correctness
We ensured the correctness of our program by connecting to the server via Netcat and screening the results. We also wrote a client program to test predefined test cases from input files and compared the output with the output in our expected output files. We also connected to the server through multiple clients and ensured that our key-value pairs were updating synchronously.

#### Robustness
We tested our server by sending it improperly formatted inputs and ensured that it did not crash the program and returned the expected error codes. We tested the program with keys and values of different sizes (long and short). We also connected to the server through a large number of clients and ensured that the server did not crash and everything updated as it was supposed to.

All use of this code must be consistent with the [DCS Academic Integrity Policy](https://www.cs.rutgers.edu/academics/undergraduate/academic-integrity-policy/programming-assignments) and the [Rutgers University Code of Student Conduct](http://studentconduct.rutgers.edu/student-conduct-processes/university-code-of-student-conduct/).