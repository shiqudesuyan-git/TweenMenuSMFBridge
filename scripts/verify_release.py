"""Check the delivered Windows DLL and, optionally, its installable archive."""
import argparse
import ctypes
import hashlib
import json
from pathlib import Path
import struct
import zipfile


def verify_dll(path: Path) -> str:
    data = path.read_bytes()
    assert data[:2] == b"MZ", "Not a Windows executable"
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    assert data[pe:pe + 4] == b"PE\0\0", "Missing PE signature"
    assert struct.unpack_from("<H", data, pe + 4)[0] == 0x8664, "Not an x64 binary"
    assert struct.unpack_from("<H", data, pe + 22)[0] & 0x2000, "Not a DLL"

    class PluginInfo(ctypes.Structure):
        _fields_ = [("infoVersion", ctypes.c_uint32),
                    ("name", ctypes.c_char_p), ("version", ctypes.c_uint32)]

    # Loading into this isolated process does not call SKSEPlugin_Load.
    dll = ctypes.WinDLL(str(path.resolve()))
    getattr(dll, "SKSEPlugin_Load")
    query = dll.SKSEPlugin_Query
    query.argtypes = [ctypes.c_void_p, ctypes.POINTER(PluginInfo)]
    query.restype = ctypes.c_bool
    info = PluginInfo()
    assert query(None, ctypes.byref(info)), "SKSEPlugin_Query failed"
    assert info.name == b"TweenMenuSMFBridge", "Wrong plugin identity"
    assert info.infoVersion == 1, "Wrong SKSE plugin info ABI"
    assert info.version == 0x01000000, "Wrong plugin version"
    assert ctypes.c_uint32.in_dll(dll, "SKSEPlugin_Version").value == 1, "Missing modern SKSE metadata"
    print("PASS: Windows x64 DLL loads, SKSE Load/Query/Version exports, plugin identity and version")
    return hashlib.sha256(data).hexdigest()


def verify_zip(path: Path, dll_hash: str) -> None:
    with zipfile.ZipFile(path) as archive:
        assert archive.testzip() is None, "Damaged archive"
        names = archive.namelist()
        allowed_roots = {"SKSE", "Interface", "licenses", "README.md", "README.zh-CN.md",
                         "VALIDATION.md", "LICENSE", "THIRD_PARTY_NOTICES.md"}
        assert all(n.split("/")[0] in allowed_roots for n in names), "Unexpected development files in release"
        plugin_name = "SKSE/Plugins/TweenMenuSMFBridge.dll"
        config_name = "Interface/tweenoptions/TweenMenuSMFBridge.json"
        assert names.count(plugin_name) == names.count(config_name) == 1, "Wrong Data-root layout"
        assert [n for n in names if n.lower().endswith(".dll")] == [plugin_name], "Unexpected DLL in release"
        assert not any("fixtures" in n.lower() or "bridgechecks" in n.lower() for n in names), "Test fixtures in release"
        assert hashlib.sha256(archive.read(plugin_name)).hexdigest() == dll_hash, "Archive DLL differs from verified DLL"
        config = json.loads(archive.read(config_name).decode("utf-8-sig"))
        assert config["papyrus_event"] == "TweenMenuSMFBridge_Open", "Wrong callback event"
        assert "skseName" not in config, "skseName would override the callback event"
        assert isinstance(config["priority"], int), "Priority is not an integer"
        assert all(config.get(key) for key in ("menuName", "icon", "category")), "Required Tween fields are missing"
        assert "README.zh-CN.md" in names and "VALIDATION.md" in names, "Release documentation is missing"
    print("PASS: Archive integrity, install layout, verified DLL, JSON contract, documentation, and fixture exclusion")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--dll", required=True, type=Path)
    parser.add_argument("--zip", type=Path)
    args = parser.parse_args()
    digest = verify_dll(args.dll)
    print(f"DLL SHA256: {digest}")
    if args.zip:
        verify_zip(args.zip, digest)
