#!/bin/sh
OWNER="gh-wbtp"
REPO="cli"
BINARY="wbtp"
USER_DIRECTORY="$HOME/.local/bin"
SYSTEM_DIRECTORY="/usr/local/bin"
case "$(uname -s)-$(uname -m)" in
	Linux-x86_64)  FILE="wbtp-cli-linux-x86_64.tar.gz" ;;
	Linux-aarch64) FILE="wbtp-cli-linux-arm64.tar.gz" ;;
	Linux-arm64)   FILE="wbtp-cli-linux-arm64.tar.gz" ;;
	Darwin-arm64)  FILE="wbtp-cli-macos-arm64.tar.gz" ;;
	*) echo "Unsupported platform." >&2; exit 1 ;;
esac
DIRECTORY="$USER_DIRECTORY"
case "$1" in
	--system|--sys) DIRECTORY="$SYSTEM_DIRECTORY" ;;
	-h|--help)
		echo "Usage: $0 [--system|--sys]"
		exit 0
		;;
	"") ;;
	*) echo "Unknown option! $1" >&2; exit 1 ;;
esac
TMP="${TMPDIR:-/tmp}/$BINARY.$$"
mkdir -p "$TMP" "$DIRECTORY"
trap 'rm -rf "$TMP"' EXIT
URL="https://github.com/$OWNER/$REPO/releases/latest/download/$FILE"
if command -v curl >/dev/null 2>&1; then
	curl -fsSL "$URL" | tar -xz -C "$TMP"
elif command -v wget >/dev/null 2>&1; then
	wget -qO- "$URL" | tar -xz -C "$TMP"
else
	echo "curl or wget is required." >&2
	exit 1
fi
if [ "$DIRECTORY" = "$SYSTEM_DIRECTORY" ] && [ ! -w "$DIRECTORY" ]; then
	sudo install -m 755 "$TMP/$BINARY" "$DIRECTORY/$BINARY"
else
	install -m 755 "$TMP/$BINARY" "$DIRECTORY/$BINARY"
fi
echo "Installed $BINARY -> $DIRECTORY/$BINARY"
case ":$PATH:" in
	*":$DIRECTORY:"*) ;;
	*) echo "Please add $DIRECTORY to PATH: export PATH=\"$DIRECTORY:\$PATH\"" ;;
esac