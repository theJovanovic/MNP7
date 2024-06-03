**MNP7**

This project implements MNP7 compression/decompression as it was described in the book "Data Compression: The Complete Reference" by David Salomon (3rd Edition).
This includes:
 1. Stage 1 (RLE compression)
 2. Stage 2 (Two-dimensional variant of adaptive Huffman coding)

Sidenotes:
 - Algorithm assumes that some form of natural language is being transmitted.
   That being said, the code tables prioritize letters by assigning them at the start of each table.
   This ensures that the letters get compressed as much as possible.
 - In the book, table is being selected by the current character value (the character that is currently being processed), 
   while the row is being selected by the previous character (character that is preceding current character).
   In this implementation, these values are swapped (previous character selects the table and the current character selects the row inside that table).
