#!/usr/bin/env python3

KNOWN_RCS = set([
    'EFI_SUCCESS', 'EFI_WARN_UNKNOWN_GLYPH',
    'EFI_WARN_DELETE_FAILURE', 'EFI_WARN_WRITE_FAILURE',
    'EFI_WARN_BUFFER_TOO_SMALL', 'EFI_WARN_STALE_DATA', 'EFI_WARN_FILE_SYSTEM',
    'EFI_WARN_RESET_REQUIRED', 'EFI_LOAD_ERROR', 'EFI_INVALID_PARAMETER',
    'EFI_UNSUPPORTED', 'EFI_BAD_BUFFER_SIZE', 'EFI_BUFFER_TOO_SMALL',
    'EFI_NOT_READY', 'EFI_DEVICE_ERROR', 'EFI_WRITE_PROTECTED',
    'EFI_OUT_OF_RESOURCES', 'EFI_VOLUME_CORRUPTED', 'EFI_VOLUME_FULL',
    'EFI_NO_MEDIA', 'EFI_MEDIA_CHANGED', 'EFI_NOT_FOUND', 'EFI_ACCESS_DENIED',
    'EFI_NO_RESPONSE', 'EFI_NO_MAPPING', 'EFI_TIMEOUT', 'EFI_NOT_STARTED',
    'EFI_ALREADY_STARTED', 'EFI_ABORTED', 'EFI_ICMP_ERROR', 'EFI_TFTP_ERROR',
    'EFI_PROTOCOL_ERROR', 'EFI_INCOMPATIBLE_VERSION', 'EFI_SECURITY_VIOLATION',
    'EFI_CRC_ERROR', 'EFI_END_OF_MEDIA', 'EFI_END_OF_FILE',
    'EFI_INVALID_LANGUAGE', 'EFI_COMPROMISED_DATA', 'EFI_IP_ADDRESS_CONFLICT',
    'EFI_HTTP_ERROR'])


def load_blacklist() -> set[str]:
    r = set()

    with open('blacklist.txt') as f:
        for line in f:
            line = line.rstrip()

            if line in r:
                print(f"Duplicate {line} in blacklist")
            else:
                r.add(line)

    print(f"{len(r)} blacklisted")
    return r


def load_desired(filename: str) -> dict[str, set[str]]:
    r: dict[str, set[str]] = {}
    title = False

    with open(filename) as f:
        for line in f:
            line = line.rstrip()

            if line == 'EFI functions return codes':
                title = True
                continue

            if not title or '#' in line or ':' not in line:
                continue

            func, rcs = line.split(sep=': ')

            if func in r:
                print(f"Duplicate {func} in {filename}")

            else:
                r[func] = set()

            for rc in rcs.split(sep=', '):
                if rc not in KNOWN_RCS:
                    print(f"Unknown EFI return code {rc} in {filename}")

                if rc in r[func]:
                    print(
                        f"Duplicate return code {rc} for {func} in {filename}")

                else:
                    r[func].add(rc)

    print(f"{len(r)} functions in {filename}")
    return r


def load_record(bl: set[str]) -> dict[str, set[int]]:
    r: dict[str, set[int]] = {}

    with open('record.log') as f:
        for line in f:
            line = line.rstrip()
            assert ' -> ' in line
            func, rc = line.split(sep=' -> ')

            if func in bl:
                continue

            if func not in r:
                r[func] = set()

            r[func].add(int(rc, base=0))

    print(f"{len(r)} unique records")
    return r


def rc_to_efi(x: int) -> str:
    if x == 0:
        return 'EFI_SUCCESS'
    elif x == 1:
        return 'EFI_WARN_UNKNOWN_GLYPH'
    elif x == 2:
        return 'EFI_WARN_DELETE_FAILURE'
    elif x == 3:
        return 'EFI_WARN_WRITE_FAILURE'
    elif x == 4:
        return 'EFI_WARN_BUFFER_TOO_SMALL'
    elif x == 5:
        return 'EFI_WARN_STALE_DATA'
    elif x == 6:
        return 'EFI_WARN_FILE_SYSTEM'
    elif x == 7:
        return 'EFI_WARN_RESET_REQUIRED'
    elif x == 0x8000000000000000 + 1:
        return 'EFI_LOAD_ERROR'
    elif x == 0x8000000000000000 + 2:
        return 'EFI_INVALID_PARAMETER'
    elif x == 0x8000000000000000 + 3:
        return 'EFI_UNSUPPORTED'
    elif x == 0x8000000000000000 + 4:
        return 'EFI_BAD_BUFFER_SIZE'
    elif x == 0x8000000000000000 + 5:
        return 'EFI_BUFFER_TOO_SMALL'
    elif x == 0x8000000000000000 + 6:
        return 'EFI_NOT_READY'
    elif x == 0x8000000000000000 + 7:
        return 'EFI_DEVICE_ERROR'
    elif x == 0x8000000000000000 + 8:
        return 'EFI_WRITE_PROTECTED'
    elif x == 0x8000000000000000 + 9:
        return 'EFI_OUT_OF_RESOURCES'
    elif x == 0x8000000000000000 + 10:
        return 'EFI_VOLUME_CORRUPTED'
    elif x == 0x8000000000000000 + 11:
        return 'EFI_VOLUME_FULL'
    elif x == 0x8000000000000000 + 12:
        return 'EFI_NO_MEDIA'
    elif x == 0x8000000000000000 + 13:
        return 'EFI_MEDIA_CHANGED'
    elif x == 0x8000000000000000 + 14:
        return 'EFI_NOT_FOUND'
    elif x == 0x8000000000000000 + 15:
        return 'EFI_ACCESS_DENIED'
    elif x == 0x8000000000000000 + 16:
        return 'EFI_NO_RESPONSE'
    elif x == 0x8000000000000000 + 17:
        return 'EFI_NO_MAPPING'
    elif x == 0x8000000000000000 + 18:
        return 'EFI_TIMEOUT'
    elif x == 0x8000000000000000 + 19:
        return 'EFI_NOT_STARTED'
    elif x == 0x8000000000000000 + 20:
        return 'EFI_ALREADY_STARTED'
    elif x == 0x8000000000000000 + 21:
        return 'EFI_ABORTED'
    elif x == 0x8000000000000000 + 22:
        return 'EFI_ICMP_ERROR'
    elif x == 0x8000000000000000 + 23:
        return 'EFI_TFTP_ERROR'
    elif x == 0x8000000000000000 + 24:
        return 'EFI_PROTOCOL_ERROR'
    elif x == 0x8000000000000000 + 25:
        return 'EFI_INCOMPATIBLE_VERSION'
    elif x == 0x8000000000000000 + 26:
        return 'EFI_SECURITY_VIOLATION'
    elif x == 0x8000000000000000 + 27:
        return 'EFI_CRC_ERROR'
    elif x == 0x8000000000000000 + 28:
        return 'EFI_END_OF_MEDIA'
    elif x == 0x8000000000000000 + 31:
        return 'EFI_END_OF_FILE'
    elif x == 0x8000000000000000 + 32:
        return 'EFI_INVALID_LANGUAGE'
    elif x == 0x8000000000000000 + 33:
        return 'EFI_COMPROMISED_DATA'
    elif x == 0x8000000000000000 + 34:
        return 'EFI_IP_ADDRESS_CONFLICT'
    elif x == 0x8000000000000000 + 35:
        return 'EFI_HTTP_ERROR'
    else:
        print(f"Unknown EFI return code {x}")
        return 'UNKNOWN'


def translate_record(r: dict[str, set[int]]) -> dict[str, set[str]]:
    rt: dict[str, set[str]] = {}

    for func in r:
        rt[func] = set()

        for rc in r[func]:
            efi = rc_to_efi(rc)
            rt[func].add(efi)

    return rt


def check_record(d: dict[str, set[str]], r: dict[str, set[str]]) -> None:
    for func in sorted(r):
        if func not in d:
            print(f"Recorded function {func} is unknown, not in desired")
            continue

        for rc in sorted(r[func]):
            if rc not in d[func]:
                print(f"{rc} is not an allowed record return code for {func}")


# Verify that what coccinelle found is a subset of the desired
def check_cocci(d: dict[str, set[str]], c: dict[str, set[str]]) -> None:
    for func in sorted(c):
        if func not in d:
            print(f"Cocci function {func} is unknown, not in desired")
            continue

        for rc in sorted(c[func]):
            if rc not in d[func]:
                print(f"{rc} is not an allowed cocci return code for {func}")


if __name__ == '__main__':
    d = load_desired('desired.txt')
    bl = load_blacklist()
    r = load_record(bl)
    rt = translate_record(r)
    check_record(d, rt)
    c = load_desired('cocci.log')
    check_cocci(d, c)
