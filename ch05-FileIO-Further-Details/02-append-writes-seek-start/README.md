# Exercise 05-02

Write a program tha topens n existing file for writing with the `O_APPEND` flag, and
then seeks to the beginning of the file before writing some data. Where does the data
appear in the file? Why?

## Solution

Based on the implementation in this directory, I found that the content is written
to the end of the file, and not the beginning. This is because the `O_APPEND` access
mode ensures that writes to the file happen at the end. The call atomically seeks
to end and writes the content. This renders any previous `lseek` redundant.
