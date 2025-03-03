# Exercise 12-02

Write a program that draws a tree showing the hierarchical parent-child
relationships of all processes on the system, going all the way back to
*init*. For each process, the program should display the process ID and
the command being executed. The output of the program should be similar
to that produced by *pstree(1)*, although it does not need to be as
sophisticated. The parent of each process on the system can be found by
inspecting the *PPid:* line of all the `/proc/PID/status` files on the
system. Be careful to handle the possibility that a process's parent
(and thus its `/proc/PID` directory) disappears during the scan of all
`/proc/PID` directories.

## Solution

I created a binary search tree abstract data type to store the processes.
I decided to have the BST key be the PID of each process, and its
value be a `struct pinfo_type` type that I defined:

```c
struct pinfo_type {
    pid_t pid;
    pid_t ppid;
    char *cmd;
    struct pinfo_type *children;
    struct pinfo_type *next;
};
```

`pid` is the process ID, `ppid` is the parent process ID, `cmd` is
the name of the command used when the process was created, `children`
is a linked list of children of this process. Since the process
can be in a list of another process's children, the `next` field
points to the next process in said list. In other words, `next`
points to a sibling process if this process is not the last one
in the parent's `children` list.

I iterated through the PID directories under `/proc`, parsed the
`/proc/PID/status` file corresponding to each process, and added
the node with this information to the binary search tree.
After the tree was built, I walked the tree, calling a function
I defined called `populateChildren`. Given a process PID,
the function searches for its parent in the BST by using the `ppid`
field of `pinfo_type`, and then it adds the process to the parent's
list of children.

Once all children have been populated, I call `printProcessTree`
to display (as neatly as possible) the hierarchy of all the processes.

I did not do the condensing that `pstree` seems to do. For example, if a lead process
named `postgres` creates 5 follower processes (say, named `postgres-child`),
then my implementation will display them all, as opposed to `pstree` which may display
something along the lines of `5*{postgres-child}`.

### Compiling and Running

Below is sample when compiling and running my implementation:

```c
make
./process_tree
```

```
init(0)->kthreadd(2)->kworker/3:2-events(23869)
                    ->kworker/u32:2(23867)
                    ->kworker/u32:0-events_power_efficient(23866)
                    ->kworker/5:3(23857)
                    ->kworker/5:2-events(23856)
                    ->kworker/4:1-events(23853)
                    ->kworker/u33:0(23852)
                    ->kworker/6:2-events(23845)
                    ->kworker/0:1-i915-unordered(23835)
                    ->kworker/7:1-events(23834)
                    ->kworker/2:2-events(23833)
                    ->kworker/1:2-i915-unordered(23801)
                    ->kworker/3:1-events(23800)
                    ->kworker/5:1-events(23791)
                    ->kworker/6:1-i915-unordered(23694)
                    ->kworker/7:0-i915-unordered(23691)
                    ->kworker/0:0-events(23690)
                    ->kworker/2:1-events(23689)
                    ->kworker/4:0-events(23685)
                    ->kworker/u32:4-events_unbound(23600)
                    ->kworker/1:1-cgroup_destroy(23569)
                    ->kworker/0:2-i915-unordered(23544)
                    ->kworker/u32:3-flush-259:0(23516)
                    ->kworker/7:2-events(23431)
                    ->kworker/3:0-events(23198)
                    ->kworker/2:0-events(23146)
                    ->kworker/u33:1+i915_flip(23020)
                    ->kworker/4:2-mm_percpu_wq(22750)
                    ->kworker/u32:1-events_unbound(22610)
                    ->kworker/5:0-i915-unordered(21055)
                    ->kworker/u33:2-rb_allocator(20911)
                    ->kworker/6:0-i915-unordered(20861)
                    ->krfcommd(1737)
                    ->psimon(1608)
                    ->UVM Tools Event Queue(1502)
                    ->UVM deferred release queue(1501)
                    ->UVM global queue(1500)
                    ->irq/148-AudioDSP(1059)
                    ->nv_queue(866)
                    ->nvidia(865)
                    ->irq/147-nvidia(864)
                    ->nvidia-modeset/deferred_close_kthread_q(857)
                    ->nvidia-modeset/kthread_q(856)
                    ->card1-crtc2(841)
                    ->card1-crtc1(840)
                    ->card1-crtc0(839)
                    ->nv_open_q(838)
                    ->nv_queue(837)
                    ->nv_queue(836)
                    ->kworker/R-ttm(822)
                    ->kworker/R-led_workqueue(741)
                    ->irq/145-iwlwifi(610)
                    ->kworker/R-cryptd(609)
                    ->kworker/R-cfg80211(593)
                    ->irq/144-mei_me(583)
                    ->kworker/R-asus_wireless_workqueue(580)
                    ->psimon(488)
                    ->kworker/R-ext4-rsv-conversion(309)
                    ->jbd2/nvme0n1p5-8(308)
                    ->irq/30-ELAN1200:00(248)
                    ->kworker/R-nvme-auth-wq(228)
                    ->kworker/R-nvme-delete-wq(227)
                    ->kworker/R-nvme-reset-wq(226)
                    ->kworker/R-nvme-wq(225)
                    ->kworker/R-scsi_tmf_4(220)
                    ->scsi_eh_4(219)
                    ->kworker/R-scsi_tmf_3(210)
                    ->scsi_eh_3(209)
                    ->kworker/R-scsi_tmf_2(208)
                    ->scsi_eh_2(207)
                    ->kworker/R-scsi_tmf_1(205)
                    ->scsi_eh_1(204)
                    ->kworker/R-scsi_tmf_0(200)
                    ->scsi_eh_0(199)
                    ->kworker/4:1H(195)
                    ->kworker/6:1H-events_highpri(194)
                    ->kworker/7:1H-events_highpri(179)
                    ->kworker/3:1H-events_highpri(159)
                    ->kworker/2:1H-events_highpri(156)
                    ->kworker/1:1H-kblockd(132)
                    ->kworker/R-charger_manager(131)
                    ->kworker/R-kstrp(113)
                    ->kworker/R-ipv6_addrconf(106)
                    ->kworker/0:1H-events_highpri(105)
                    ->kworker/R-mld(104)
                    ->hwrng(103)
                    ->kworker/R-acpi_thermal_pm(101)
                    ->irq/124-pcie-dpc(100)
                    ->irq/124-aerdrv(99)
                    ->irq/123-pcie-dpc(98)
                    ->irq/123-aerdrv(97)
                    ->kworker/R-kthrotld(96)
                    ->ecryptfs-kthread(95)
                    ->kswapd0(94)
                    ->kworker/5:1H-kblockd(93)
                    ->watchdogd(91)
                    ->kworker/R-devfreq_wq(89)
                    ->kworker/R-edac-poller(88)
                    ->kworker/R-md_bitmap(87)
                    ->kworker/R-md(86)
                    ->kworker/R-ata_sff(85)
                    ->kworker/R-tpm_dev_wq(84)
                    ->irq/9-acpi(80)
                    ->kworker/R-blkcg_punt_bio(78)
                    ->kworker/R-kblockd(77)
                    ->kworker/R-kintegrityd(76)
                    ->khugepaged(75)
                    ->ksmd(74)
                    ->kcompactd0(73)
                    ->kworker/R-writeback(72)
                    ->oom_reaper(70)
                    ->khungtaskd(69)
                    ->kauditd(68)
                    ->kworker/R-inet_frag_wq(66)
                    ->kdevtmpfs(65)
                    ->kworker/7:0H-events_highpri(64)
                    ->ksoftirqd/7(62)
                    ->migration/7(61)
                    ->idle_inject/7(60)
                    ->cpuhp/7(59)
                    ->kworker/6:0H-events_highpri(58)
                    ->ksoftirqd/6(56)
                    ->migration/6(55)
                    ->idle_inject/6(54)
                    ->cpuhp/6(53)
                    ->kworker/5:0H-events_highpri(52)
                    ->ksoftirqd/5(50)
                    ->migration/5(49)
                    ->idle_inject/5(48)
                    ->cpuhp/5(47)
                    ->kworker/4:0H-events_highpri(46)
                    ->ksoftirqd/4(44)
                    ->migration/4(43)
                    ->idle_inject/4(42)
                    ->cpuhp/4(41)
                    ->kworker/3:0H-events_highpri(40)
                    ->ksoftirqd/3(38)
                    ->migration/3(37)
                    ->idle_inject/3(36)
                    ->cpuhp/3(35)
                    ->kworker/2:0H-events_highpri(34)
                    ->ksoftirqd/2(32)
                    ->migration/2(31)
                    ->idle_inject/2(30)
                    ->cpuhp/2(29)
                    ->kworker/1:0H-events_highpri(28)
                    ->ksoftirqd/1(26)
                    ->migration/1(25)
                    ->idle_inject/1(24)
                    ->cpuhp/1(23)
                    ->cpuhp/0(22)
                    ->idle_inject/0(21)
                    ->migration/0(20)
                    ->rcu_exp_gp_kthread_worker(19)
                    ->rcu_exp_par_gp_kthread_worker/0(18)
                    ->rcu_preempt(17)
                    ->ksoftirqd/0(16)
                    ->rcu_tasks_trace_kthread(15)
                    ->rcu_tasks_rude_kthread(14)
                    ->rcu_tasks_kthread(13)
                    ->kworker/R-mm_percpu_wq(12)
                    ->kworker/0:0H-events_highpri(9)
                    ->kworker/R-netns(7)
                    ->kworker/R-slub_flushwq(6)
                    ->kworker/R-sync_wq(5)
                    ->kworker/R-rcu_gp(4)
                    ->pool_workqueue_release(3)
       ->systemd(1)->fwupd(8521)
                   ->systemd(2268)->gnome-terminal-(5760)->bash(8392)->process_tree(23875)
                                  ->speech-dispatch(5111)->sd_openjtalk(5132)
                                                         ->sd_dummy(5129)
                                                         ->sd_espeak-ng(5126)
                                                         ->sd_espeak-ng-mb(5123)
                                  ->snap(4369)
                                  ->chrome_crashpad(3672)
                                  ->gvfsd-metadata(3397)
                                  ->xdg-desktop-por(3253)
                                  ->dconf-service(3238)
                                  ->xdg-desktop-por(3185)
                                  ->tracker-miner-f(3182)
                                  ->gjs(3151)
                                  ->evolution-addre(3146)
                                  ->gvfs-gphoto2-vo(3126)
                                  ->gvfs-goa-volume(3116)
                                  ->gvfs-afc-volume(3110)
                                  ->evolution-calen(3104)
                                  ->goa-identity-se(3097)
                                  ->gvfs-mtp-volume(3089)
                                  ->goa-daemon(3079)
                                  ->ibus-portal(3062)
                                  ->ibus-x11(3060)
                                  ->gvfs-udisks2-vo(3012)
                                  ->gsd-printer(3007)
                                  ->gsd-xsettings(2932)
                                  ->gsd-wacom(2929)
                                  ->gsd-sound(2926)
                                  ->gsd-smartcard(2923)
                                  ->gsd-sharing(2915)
                                  ->gsd-screensaver(2909)
                                  ->gsd-rfkill(2904)
                                  ->gsd-print-notif(2901)
                                  ->gsd-power(2891)
                                  ->gsd-media-keys(2885)
                                  ->gsd-keyboard(2882)
                                  ->gsd-housekeepin(2879)
                                  ->gsd-datetime(2875)
                                  ->gsd-color(2874)
                                  ->gsd-a11y-settin(2869)
                                  ->ibus-daemon(2865)->ibus-engine-sim(3194)
                                                     ->ibus-extension-(3049)
                                                     ->ibus-memconf(3047)
                                  ->gjs(2846)
                                  ->evolution-sourc(2832)
                                  ->gnome-shell-cal(2825)
                                  ->at-spi2-registr(2794)
                                  ->gnome-shell(2756)->gjs(20323)
                                                     ->firefox(14922)->Web Content(15835)
                                                                     ->Web Content(15744)
                                                                     ->Web Content(15742)
                                                                     ->Isolated Web Co(15698)
                                                                     ->Inference(15696)
                                                                     ->Utility Process(15688)
                                                                     ->WebExtensions(15481)
                                                                     ->RDD Process(15137)
                                                                     ->Privileged Cont(15133)
                                                                     ->Socket Process(15092)
                                                     ->spotify(11115)->spotify(11269)
                                                                     ->spotify(11222)->spotify(11357)
                                                                                     ->spotify(11270)
                                                                     ->spotify(11221)->spotify(11267)
                                                     ->zoom(9885)->zoom(9892)->ZoomWebviewHost(9932)->ZoomWebviewHost(10007)
                                                                                                    ->chrome-sandbox(9962)->ZoomWebviewHost(9963)->ZoomWebviewHost(9965)->ZoomWebviewHost(10334)
                                                                                                                                                                        ->ZoomWebviewHost(10274)
                                                                                                                                                                        ->ZoomWebviewHost(10223)
                                                                                                                                                                        ->ZoomWebviewHost(10198)
                                                                                                                                                                        ->ZoomWebviewHost(10175)
                                                                                                                                                                        ->ZoomWebviewHost(10039)
                                                                                                    ->ZoomWebviewHost(9961)->ZoomWebviewHost(10001)
                                                     ->Discord(3645)->Discord(3822)
                                                                    ->Discord(3774)
                                                                    ->Discord(3711)
                                                                    ->Discord(3656)->Discord(3658)
                                                                    ->Discord(3655)->Discord(3707)
                                                     ->mutter-x11-fram(2791)
                                  ->gnome-session-b(2724)->update-notifier(5278)
                                                         ->evolution-alarm(2924)
                                                         ->gsd-disk-utilit(2902)
                                  ->gvfsd-fuse(2721)
                                  ->gvfsd(2709)->gvfsd-trash(3259)
                                  ->xdg-desktop-por(2698)
                                  ->gnome-session-c(2694)
                                  ->gcr-ssh-agent(2693)
                                  ->at-spi-bus-laun(2658)->dbus-daemon(2665)
                                  ->snapd-desktop-i(2512)->snapd-desktop-i(2573)
                                  ->xdg-permission-(2359)
                                  ->xdg-document-po(2351)->fusermount3(2366)
                                  ->dbus-daemon(2310)
                                  ->gnome-keyring-d(2292)->ssh-agent(16960)
                                  ->pipewire-pulse(2290)
                                  ->wireplumber(2288)
                                  ->pipewire(2285)
                                  ->pipewire(2284)
                                  ->(sd-pam)(2272)
                   ->kerneloops(2233)
                   ->kerneloops(2230)
                   ->cups-browsed(2227)
                   ->colord(1889)
                   ->rtkit-daemon(1682)
                   ->gdm3(1582)->gdm-session-wor(2243)->gdm-x-session(2401)->gnome-session-b(2575)
                                                                           ->Xorg(2414)
                   ->unattended-upgr(1566)
                   ->cupsd(1563)
                   ->upowerd(1517)
                   ->nvidia-persiste(1507)
                   ->ModemManager(1269)
                   ->rsyslogd(1190)
                   ->wpa_supplicant(1163)
                   ->NetworkManager(1162)
                   ->udisksd(1142)
                   ->thermald(1140)
                   ->systemd-logind(1139)
                   ->switcheroo-cont(1137)
                   ->cron(1136)
                   ->accounts-daemon(1133)
                   ->snapd(1130)
                   ->power-profiles-(1120)
                   ->polkitd(1119)
                   ->gnome-remote-de(1112)
                   ->dbus-daemon(1109)
                   ->bluetoothd(1108)
                   ->avahi-daemon(1107)->avahi-daemon(1153)
                   ->systemd-timesyn(833)
                   ->systemd-resolve(832)
                   ->systemd-oomd(831)
                   ->systemd-udevd(449)
                   ->systemd-journal(358)
```

Below is the `pstree` command that I was aspiring to (for comparison):

```
systemd─┬─ModemManager───3*[{ModemManager}]
        ├─NetworkManager───3*[{NetworkManager}]
        ├─accounts-daemon───3*[{accounts-daemon}]
        ├─avahi-daemon───avahi-daemon
        ├─bluetoothd
        ├─colord───3*[{colord}]
        ├─cron
        ├─cups-browsed───3*[{cups-browsed}]
        ├─cupsd
        ├─dbus-daemon
        ├─fwupd───5*[{fwupd}]
        ├─gdm3─┬─gdm-session-wor─┬─gdm-x-session─┬─Xorg───5*[{Xorg}]
        │      │                 │               ├─gnome-session-b───3*[{gnome-session-b}]
        │      │                 │               └─3*[{gdm-x-session}]
        │      │                 └─3*[{gdm-session-wor}]
        │      └─3*[{gdm3}]
        ├─gnome-remote-de───3*[{gnome-remote-de}]
        ├─2*[kerneloops]
        ├─nvidia-persiste
        ├─polkitd───3*[{polkitd}]
        ├─power-profiles-───3*[{power-profiles-}]
        ├─rsyslogd───3*[{rsyslogd}]
        ├─rtkit-daemon───2*[{rtkit-daemon}]
        ├─snapd───14*[{snapd}]
        ├─switcheroo-cont───3*[{switcheroo-cont}]
        ├─systemd─┬─(sd-pam)
        │         ├─at-spi-bus-laun─┬─dbus-daemon
        │         │                 └─4*[{at-spi-bus-laun}]
        │         ├─at-spi2-registr───3*[{at-spi2-registr}]
        │         ├─chrome_crashpad───2*[{chrome_crashpad}]
        │         ├─dbus-daemon
        │         ├─dconf-service───3*[{dconf-service}]
        │         ├─evolution-addre───6*[{evolution-addre}]
        │         ├─evolution-calen───9*[{evolution-calen}]
        │         ├─evolution-sourc───4*[{evolution-sourc}]
        │         ├─gcr-ssh-agent───2*[{gcr-ssh-agent}]
        │         ├─2*[gjs───11*[{gjs}]]
        │         ├─gnome-keyring-d─┬─ssh-agent
        │         │                 └─4*[{gnome-keyring-d}]
        │         ├─gnome-session-b─┬─evolution-alarm───7*[{evolution-alarm}]
        │         │                 ├─gsd-disk-utilit───3*[{gsd-disk-utilit}]
        │         │                 ├─update-notifier───5*[{update-notifier}]
        │         │                 └─4*[{gnome-session-b}]
        │         ├─gnome-session-c───{gnome-session-c}
        │         ├─gnome-shell─┬─Discord─┬─Discord───Discord───19*[{Discord}]
        │         │             │         ├─Discord───Discord
        │         │             │         ├─Discord───10*[{Discord}]
        │         │             │         ├─Discord───35*[{Discord}]
        │         │             │         ├─Discord───5*[{Discord}]
        │         │             │         └─36*[{Discord}]
        │         │             ├─firefox─┬─Inference───30*[{Inference}]
        │         │             │         ├─Isolated Web Co───27*[{Isolated Web Co}]
        │         │             │         ├─Privileged Cont───27*[{Privileged Cont}]
        │         │             │         ├─RDD Process───4*[{RDD Process}]
        │         │             │         ├─Socket Process───5*[{Socket Process}]
        │         │             │         ├─Utility Process───4*[{Utility Process}]
        │         │             │         ├─3*[Web Content───17*[{Web Content}]]
        │         │             │         ├─WebExtensions───26*[{WebExtensions}]
        │         │             │         └─106*[{firefox}]
        │         │             ├─gjs───13*[{gjs}]
        │         │             ├─mutter-x11-fram───9*[{mutter-x11-fram}]
        │         │             ├─spotify─┬─spotify───spotify───12*[{spotify}]
        │         │             │         ├─spotify─┬─spotify───7*[{spotify}]
        │         │             │         │         └─spotify───18*[{spotify}]
        │         │             │         ├─spotify───12*[{spotify}]
        │         │             │         └─70*[{spotify}]
        │         │             ├─zoom───zoom─┬─ZoomWebviewHost─┬─ZoomWebviewHost───ZoomWebviewHost───20*[{ZoomWebviewHost}]
        │         │             │             │                 ├─ZoomWebviewHost───12*[{ZoomWebviewHost}]
        │         │             │             │                 ├─chrome-sandbox───ZoomWebviewHost───ZoomWebviewHost─┬─2*[ZoomWebvi+
        │         │             │             │                 │                                                    └─4*[ZoomWebvi+
        │         │             │             │                 └─39*[{ZoomWebviewHost}]
        │         │             │             └─75*[{zoom}]
        │         │             └─19*[{gnome-shell}]
        │         ├─gnome-shell-cal───6*[{gnome-shell-cal}]
        │         ├─gnome-terminal-─┬─bash───pstree
        │         │                 └─6*[{gnome-terminal-}]
        │         ├─goa-daemon───4*[{goa-daemon}]
        │         ├─goa-identity-se───3*[{goa-identity-se}]
        │         ├─gsd-a11y-settin───4*[{gsd-a11y-settin}]
        │         ├─gsd-color───4*[{gsd-color}]
        │         ├─gsd-datetime───4*[{gsd-datetime}]
        │         ├─gsd-housekeepin───4*[{gsd-housekeepin}]
        │         ├─gsd-keyboard───4*[{gsd-keyboard}]
        │         ├─gsd-media-keys───4*[{gsd-media-keys}]
        │         ├─gsd-power───4*[{gsd-power}]
        │         ├─gsd-print-notif───3*[{gsd-print-notif}]
        │         ├─gsd-printer───3*[{gsd-printer}]
        │         ├─gsd-rfkill───3*[{gsd-rfkill}]
        │         ├─gsd-screensaver───3*[{gsd-screensaver}]
        │         ├─gsd-sharing───4*[{gsd-sharing}]
        │         ├─gsd-smartcard───4*[{gsd-smartcard}]
        │         ├─gsd-sound───4*[{gsd-sound}]
        │         ├─gsd-wacom───4*[{gsd-wacom}]
        │         ├─gsd-xsettings───4*[{gsd-xsettings}]
        │         ├─gvfs-afc-volume───4*[{gvfs-afc-volume}]
        │         ├─gvfs-goa-volume───3*[{gvfs-goa-volume}]
        │         ├─gvfs-gphoto2-vo───3*[{gvfs-gphoto2-vo}]
        │         ├─gvfs-mtp-volume───3*[{gvfs-mtp-volume}]
        │         ├─gvfs-udisks2-vo───4*[{gvfs-udisks2-vo}]
        │         ├─gvfsd─┬─gvfsd-trash───4*[{gvfsd-trash}]
        │         │       └─3*[{gvfsd}]
        │         ├─gvfsd-fuse───6*[{gvfsd-fuse}]
        │         ├─gvfsd-metadata───3*[{gvfsd-metadata}]
        │         ├─ibus-daemon─┬─ibus-engine-sim───3*[{ibus-engine-sim}]
        │         │             ├─ibus-extension-───4*[{ibus-extension-}]
        │         │             ├─ibus-memconf───3*[{ibus-memconf}]
        │         │             └─3*[{ibus-daemon}]
        │         ├─ibus-portal───3*[{ibus-portal}]
        │         ├─ibus-x11───3*[{ibus-x11}]
        │         ├─2*[pipewire───2*[{pipewire}]]
        │         ├─pipewire-pulse───2*[{pipewire-pulse}]
        │         ├─snap───13*[{snap}]
        │         ├─snapd-desktop-i───snapd-desktop-i───12*[{snapd-desktop-i}]
        │         ├─speech-dispatch─┬─sd_dummy───2*[{sd_dummy}]
        │         │                 ├─sd_espeak-ng───{sd_espeak-ng}
        │         │                 ├─sd_espeak-ng-mb
        │         │                 ├─sd_openjtalk
        │         │                 └─4*[{speech-dispatch}]
        │         ├─tracker-miner-f───7*[{tracker-miner-f}]
        │         ├─wireplumber───5*[{wireplumber}]
        │         ├─xdg-desktop-por───6*[{xdg-desktop-por}]
        │         ├─xdg-desktop-por───9*[{xdg-desktop-por}]
        │         ├─xdg-desktop-por───4*[{xdg-desktop-por}]
        │         ├─xdg-document-po─┬─fusermount3
        │         │                 └─7*[{xdg-document-po}]
        │         └─xdg-permission-───3*[{xdg-permission-}]
        ├─systemd-journal
        ├─systemd-logind
        ├─systemd-oomd
        ├─systemd-resolve
        ├─systemd-timesyn───{systemd-timesyn}
        ├─systemd-udevd
        ├─thermald───4*[{thermald}]
        ├─udisksd───5*[{udisksd}]
        ├─unattended-upgr───{unattended-upgr}
        ├─upowerd───3*[{upowerd}]
        └─wpa_supplicant
```
