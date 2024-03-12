# Exercise 3-1

When using the Linux-specific `reboot()` system call to reboot the system, the second
arugment, `magic2`, must be specified as one of a set of magic numbers (e.g., `LINUX_REBOOT_MAGIC2`).
What is the significance of these numbers? (Converting them to hexadecimal provides a clue).

## Solution

Section 2 of the man pages provides information about system calls.

```bash
# Man pages for the man pages. Among other things, it shows the section.
man man

# Display man pages for reboot system call (section 2) (by default, without the 2, it shows section 8)
man 2 reboot
```

The following excerpt can be found in the manual pages mentioned above:

```
# man 2 reboot excerpt
...
This system call fails (with the error EINVAL) unless magic equals LINUX_REBOOT_MAGIC1 (that is, 0xfee1dead) and magic2  equals
       LINUX_REBOOT_MAGIC2 (that is, 672274793).  However, since 2.1.17 also LINUX_REBOOT_MAGIC2A (that is, 85072278) and since 2.1.97
       also LINUX_REBOOT_MAGIC2B (that is, 369367448) and since 2.5.71 also LINUX_REBOOT_MAGIC2C (that is, 537993216) are permitted as
       values for magic2.  (The hexadecimal values of these constants are meaningful.)
...
```

The hexadecimal values are as follows:

| Label                  | Decimal   | Hex        |
| ---------------------  | --------- | ---------- |
| LINUX\_REBOOT\_MAGIC2  | 672274793 | 0x28121969 |
| LINUX\_REBOOT\_MAGIC2A |  85072278 |  0x5121996 |
| LINUX\_REBOOT\_MAGIC2B | 369367448 | 0x16041998 |
| LINUX\_REBOOT\_MAGIC2C | 537993216 | 0x20112000 |

Each hexadecimal value represents a date:

- `LINUX\_REBOOT\_MAGIC2`: Its hex value corresponds to the date December 28th, 1969, the day when Linus Torvalds was born
  [see Wikipedia](https://en.wikipedia.org/wiki/Linus_Torvalds#Torvalds).

The man pages mention also that the other three HEX values represent the birth of each of Linus' daughters. See the
Personal Life section of the Wikipedia page.
