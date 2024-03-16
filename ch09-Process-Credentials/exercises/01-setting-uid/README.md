# Exercise 09-01

Assume in each of the following cases that the initial set of process user IDs is
*real=1000 effective=0 saved=0 file-system=0*. What would be the state of the user
IDs after the following calls?

<ol type="a">
	<li> setuid(2000); </li>
	<li> setreuid(-1, 2000);
	<li> seteuid(2000);
	<li> setfsuid(2000);
	<li> setresuid(-1, 2000, 3000);
</ol>

## Solution

Because the effective user ID is 0, the process is privileged, which influences the
outcome of each of the following calls.

<ol type ="a">
	<li> 
		The `setuid(2000);` for a privileged process changes the real, effective,
		and saved set user IDs of the calling process to whatever value was specified.
		Moreover, since the effective user is changed, the file-system user ID changes
		to the same value. Hence, the state of the IDs after this call is:
		real=2000 effective=2000 saved=2000 file-system=2000.
	</li>
	<li>
		The `setreuid(-1,2000);` for a privileged process leaves the real ID unchanged, but
		changes the effective used ID to 2000, which in turn changes the file-system user ID.
		Since the effective used ID has a value that is distinct from the real user ID prior
		to the call (that is, 2000 is different from 1000), the saved set user ID is also
		changed. The resulting state is: real=1000 effective=2000 saved set=2000 file-system=2000.
	</li>
	<li>
		The `seteuid(2000);` for a privileged process changes the effective user ID. The file-system
		ID changes to the same value as the effective used ID, as usual. The resulting state becomes:
		real=1000 effective=2000 saved set=0 file-system=2000.
	</li>
	<li>
		The Linux-specific `setfsuid(2000)`; changes the file-system ID to the value 2000. The result is:
		real=1000 effective=0 saved=0 file-system=2000
	</li>
	<li>
	The non-SUSv3 (and seldom available) `setresuid(-1,2000,3000);` independently changes the real,
	effective, and saved set user IDs. Since the effective user ID is changing, so does the
	file-system user ID. The result is: real=1000 effective=2000 saved=3000 file-system=2000.
	</li>
</ol>
