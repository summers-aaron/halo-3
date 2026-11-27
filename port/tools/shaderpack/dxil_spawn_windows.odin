#+build windows
package main

// dxil_spawn for Windows hosts: runs the DXC command line of one module (dxil.odin dxil_compile_module) and waits for it.

import "core:fmt"
import "core:os"

// Runs cmd and blocks until it ends: whether it exited normally and its exit code (CreateProcess: no fork, so none of
// the POSIX version's page-table cost).
dxil_spawn :: proc(cmd: []string) -> (exited: bool, code: int, err: string) {
	process, perr := os.process_start(os.Process_Desc{command = cmd})
	if perr != nil {return false, 0, fmt.tprintf("cannot run %s: %v", cmd[0], perr)}
	state, werr := os.process_wait(process)
	if werr != nil {return false, 0, fmt.tprintf("waiting for %s: %v", cmd[0], werr)}
	return state.exited, state.exit_code, ""
}
