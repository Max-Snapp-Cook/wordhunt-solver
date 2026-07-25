# Trie-Based Word Hunt Solver in C

This is a CLI tool to efficiently solve a Word Hunt or Boggle board of any square dimensions. The Trie data structure allows words to be found in O(L) time, where L is the length of the word.

## How to use:
- Clone the repository and build solver.c, then run the executable.
- Type in the letters on the board from left to right. As long as the board is a perfect square, a list of possible words sorted by length will be returned.
