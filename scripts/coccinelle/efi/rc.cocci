// Options: --no-includes --include-headers

virtual patch
virtual org
virtual report
virtual context

@initialize:python@
@@
blacklist = set(['r', 'ret', 'exit_status', 'tmp', 'status', 'efi_ret',
		 'EFI_EXIT'])

# Dict of set of return codes / callee functions, by caller function
d: dict[str, set[str]] = {}

# Add return code (or callee)
# p: position
# f: caller function
# i: return code / callee function
def add_rc(p, f: str, i: str) -> None:
	if i in blacklist:
		return

	coccilib.report.print_report(p[0], f"{f} -> {i}")

	if f not in d:
		d[f] = set()

	d[f].add(i)

# set of efi functions
ef: set[str] = set()

# Add function with efi entry
# p: position
# f: function
def add_efi(p, f: str) -> None:
	coccilib.report.print_report(p[0], f">{f}")
	ef.add(f)

@finalize:python@
@@
# Possible return codes for function.
# f: function
# lev: callee level
def possible_rc(f: str, lev: int = 0) -> set[str]:
	rc = set()

	# First pass: only return codes.
	for i in d[f]:
		if 'EFI_' in i:
			rc.add(i)

	pre = '  ' * lev
	print(f"{pre}{f}:", ', '.join(sorted(rc)))

	# Second pass: the rest.
	for i in d[f]:
		if 'EFI_' in i:
			continue
		elif i in d:
			rc |= possible_rc(i, lev + 1)
		else:
			print(f"{pre}??? {i}")

	return rc

syn = {}

for f in d:
	if f not in ef:
		continue

	print()
	syn[f] = possible_rc(f)

print()
print('EFI functions return codes')
print('--------------------------')
print()

for f in sorted(syn):
	print(f"{f}:", ', '.join(sorted(syn[f])))

// Extract caller -> callee and efi return codes

@r1 exists@
identifier f, i, m, ret;
expression e;
position p;
typedef efi_status_t;
@@
  efi_status_t f(...)
  {
    ...
(
*   ret = EFI_CALL((*e->m)(...));
    ...
    return ret;
|
*   ret = EFI_CALL((e->m)(...));
    ...
    return ret;
|
*   ret = EFI_CALL(e->m(...));
    ...
    return ret;
|
    ret = EFI_CALL(i@p(...));
    ...
    return ret;
|
*   ret = EFI_CALL(e);
    ...
    return ret;
|
*   ret = e->m(...);
    ...
    return ret;
|
    ret = i@p(...);
    ...
    return ret;
|
    ret = e ? i@p : ret;
    ...
    return ret;
|
    ret = i@p;
    ...
    return ret;
|
*   ret = e;
    ...
    return ret;
|
*   return EFI_CALL(e);
|
    return EFI_EXIT(i@p);
|
    return i@p(...);
|
    return i@p;
|
*   return e;
)
  }

@script:python@
p << r1.p;
f << r1.f;
i << r1.i;
@@
add_rc(p, f, i)

// Extract functions with EFI_ENTRY and a return code

@r2 exists@
identifier f;
position p;
typedef efi_status_t;
@@
  efi_status_t f(...)
  {
  ...
  EFI_ENTRY@p
  ...
  }

@script:python@
p << r2.p;
f << r2.f;
@@
add_efi(p, f)
