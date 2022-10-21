#define option(type) struct { bool isSome; type value; }
#define setOption(opt, val) opt.value = val; opt.isSome = true

