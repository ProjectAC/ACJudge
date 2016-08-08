#pragma once

#include "../Definations/types.h"
#include "../Definations/enums.h"

// [Interface] set_limits
// Set time and space limits for current process
// Always called immediately after forking
Return set_limits(Limit time, Limit space);

// [Interface] set_rules
// Set rules (security) for current process
// Called when <restricted> is true
// [Thanks] QingDaoU Judger
// link: https://github.com/QingdaoU/Judger
Return set_rules(const char *path);

// [Interface] set_gid
// Set group of current process to guest for safety
Return set_gid();

// [Interface] redirection
// Redirect file input/output/error target
Return redirection(const char *in, const char *out, const char *err);

// [Interface] run
// Run the 
// Limit time and space
// And (might) restrict syscalls if <restricted> set to true
// Redirect I/O to file <fin> <fout> and <ferr>
// If these pointers are set to NULL, then stdin/out will be remained 
Result run(const char *path, const char *name, Limit time, Limit space, bool restricted, const char *fin, const char *fout, const char *ferr);