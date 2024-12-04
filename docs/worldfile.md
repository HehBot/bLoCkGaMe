# World file format

## Info
For a chunk size of 32x32x32 (which makes 32768 bytes for a chunk), the following byte offsets are used

| Bytes             | Purpose       |
|-------------------|---------------|
| 0-4               | # of chunks   |
| 4-32768           | free-bitmap   |
| 32768-32768\*2    | first table   |
| 32768\*2-\*       | remaining data|

Each table consists of 1638 entries of the form
0-4 x
0-4 y
0-4 z
0-8 file byte offset
followed by 8 bytes containing file byte offset of the next table
