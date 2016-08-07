#include "../Definations/types.h"
#include "../Definations/enums.h"

// [Interface] set_limits
// Set time and space limits for current process
// Always called immediately after forking
Return set_limits(Limit time, Limit space);

// [Interface] set_rules
// Set rules (security) for current process
// Called when <restricted> is true
Return set_rules();

// [Interface] redirection
// Redirect file input/output/error target
Return redirection(const char *in, const char *out, const char *err);

// [Interface] run
// Run the 
// Limit time and space
// And (might) restrict syscalls if <restricted> set to true
Return run(const char name[], Limit time, Limit space, bool restricted);