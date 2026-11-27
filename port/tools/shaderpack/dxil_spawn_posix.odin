#+build !windows
package main

// dxil_spawn for POSIX hosts: runs the DXC command line of one module (dxil.odin dxil_compile_module) and waits for it.

import "core:c"
import "core:fmt"
import "core:os"
import "core:strings"
import "core:sys/posix"

// Runs cmd (cmd[0] searched in PATH, this process's environment) and blocks until it ends: whether it exited normally
// and its exit code. posix_spawnp, not os.process_start: glibc's posix_spawn is clone(CLONE_VM | CLONE_VFORK) + exec,
// a cost independent of the size of this process, where os.process_start forks and so copies the page tables of every
// map the tool has read (gigabytes) once per compile, with the workers serialised on the address-space lock: 3,430
// compiles took 5x as long as DXC needs for them.
dxil_spawn :: proc(cmd: []string) -> (exited: bool, code: int, err: string) {
	argv := make([]cstring, len(cmd) + 1, context.temp_allocator)
	for a, i in cmd {argv[i] = strings.clone_to_cstring(a, context.temp_allocator)}
	env, eerr := os.environ(context.temp_allocator)
	if eerr != nil {return false, 0, fmt.tprintf("cannot read the environment: %v", eerr)}
	envp := make([]cstring, len(env) + 1, context.temp_allocator)
	for e, i in env {envp[i] = strings.clone_to_cstring(e, context.temp_allocator)}
	pid: posix.pid_t
	if e := posix.posix_spawnp(&pid, argv[0], nil, nil, raw_data(argv), raw_data(envp)); e != .NONE {
		return false, 0, fmt.tprintf("cannot run %s: %v", cmd[0], e)
	}
	status: c.int
	for posix.waitpid(pid, &status, {}) != pid {
		if e := posix.errno(); e != .EINTR {return false, 0, fmt.tprintf("waiting for %s: %v", cmd[0], e)}
	}
	if posix.WIFEXITED(status) {return true, int(posix.WEXITSTATUS(status)), ""}
	return false, 0, ""
}
