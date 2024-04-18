# Exercise 17-1

Write a program that displays the permissions from the ACL entry that corresponds to
a particular user or group. The program should take two command-line arguments. The
first argument is either of the letter `u` or `g`, indicating whether the second
argument identifies a user or group. (The function defined in Listing 8-1, on page
159, can be used to allow the second commaand-line argument to be specified numerically
or as a name.) If the ACL entry that corresponds to the given user or group falls into
the group class, then the program should additionally display the permissions that would
apply after the ACL entry has been modified by the ACL mask entry.
