<div align="center">
	<h3>WBTP — Web Binary Transfer Protocol </h3>
</div>

**WBTP** is a drop-in replacement for **HTTP** (but not **HTTPS**). However, readable text was swapped out for packed binary! This allows for quicker transfer speeds and serialization/deserialization, at the loss of direct readability.

This project is built on top of [*lib*WBTP](https://github.com/gh-wbtp/wbtp) to provide a quick client CLI. You can find documentation for the commands either by not passing any args or visiting [this section](#cli-documentation).

Each release package provides native binaries/executables for one of [5 platforms](#supported-platforms-5), along with licensing and this very same README!

---

### Table of Contents
- [Installation](#installation)
- [CLI Documentation](#cli-documentation)
- [Supported Platforms (5)](#supported-platforms-5)

---

### Installation

> **GUI installer coming to Win(slop|dows) soon!** For now, extract the release ZIP and place both the **EXE** and **DLL** next to each other in some location on your **PATH**. This installation section is meant only for Unix-like systems.

We have a remote sh/bash script you can execute directly! It automatically picks a package for your OS/arch, downloads it, extracts it, and places the binaries where they need to go. It's that easy!

```bash
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/cli/main/installers/install.sh | sh              # User-local
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/cli/main/installers/install.sh | sh -s -- --sys  # System-wide
```

**Suppose you get tired of us...** you're cooked? Nope! Actually you are, 'cus you do in fact need it to survive. But we also provided an uninstallation script for the weirdos :)

```bash
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/cli/main/installers/uninstall.sh | sh              # User-local
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/cli/main/installers/uninstall.sh | sh -s -- --sys  # System-wide
```

---

### CLI Documentation

Quick notes:
- Passing no args shows a minimal help menu.
- Only the server URL is required to be passed by default.
- The server URL may be passed anywhere outside the value of a flag.
- Raw mode (`--raw`) is best for non-textual **response** payloads such as image data.

Usage:
```xml
wbtp [--type|-t <get|put|request>] [--params|-p <params>] [--payload|-pl <payload>] [--raw|-r] [--verbose|-v] <url>
```

|Flag|Shorthand|Optional|Description|
|---|---|---|---|
|`--type`|`-t`|✅|Request type (string with strict values)|
|`--params`|`-p`|✅|Request params (string)|
|`--payload`|`-pl`|✅|Request payload (string)|
|`--raw`|`-r`|When passed, prints the response payload directly to stdout (no text constraints or newlines)|
|`--verbose`|`-v`|✅|When passed, always prints out stringified response data|

**Correct usages:**
```bash
$ wbtp 192.168.1.251               # LAN address, default port (6969), default path (/)
$ wbtp 192.168.1.251:6970          # LAN address, explicit port (6970), default path (/)
$ wbtp 192.168.1.251/about         # LAN address, default port (6969), explicit path (/about)
$ wbtp 192.168.1.251:6971/contact  # LAN address, explicit port (6971), explicit path (/contact)

$ wbtp 38.45.229.157               # IPv4 address, default port (6969), default path (/)
$ wbtp 38.45.229.157:6970          # IPv4 address, explicit port (6970), default path (/)
$ wbtp 38.45.229.157/about         # IPv4 address, default port (6969), explicit path (/about)
$ wbtp 38.45.229.157:6971/contact  # IPv4 address, explicit port (6971), explicit path (/contact)

$ wbtp example.com               # Domain, default port (6969), default path (/)
$ wbtp example.com:6970          # Domain, explicit port (6970), default path (/)
$ wbtp example.com/about         # Domain, default port (6969), explicit path (/about)
$ wbtp example.com:6971/contact  # Domain, explicit port (6971), explicit path (/contact)

$ wbtp -v 192.168.1.251                                     # Verbose mode
$ wbtp --type request example.com:6970                      # Generic request type
$ wbtp -t get -v example.com/about                          # Explicit GET and verbose mode
$ wbtp --type put -pl "Hey guys!" example.com:6971/contact  # PUT with payload

...
```

**Incorrect usages:**
```bash
$ wbtp wbtp://example.com
$ wbtp -t get -v
$ wbtp -v
$ wbtp  # Will show minimal help menu
...
```

---

### Supported Platforms (5)

- Windows (x86_64 + arm64)
- Linux (x86_64 + arm64)
- macOS (arm64)
