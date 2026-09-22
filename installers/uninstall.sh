#!/bin/sh
BINARY="wbtp"
USER_DIR="$HOME/.local/bin"
SYSTEM_DIR="/usr/local/bin"
DIR="$USER_DIR"
case "$1" in
	--system|--sys) DIR="$SYSTEM_DIR" ;;
	-h|--help)
		echo "Usage: $0 [--system|--sys]"
		exit 0
		;;
	"") ;;
	*) echo "Unknown option! $1" >&2; exit 1 ;;
esac
FILE="$DIR/$BINARY"
if [ ! -e "$FILE" ]; then
	echo "$BINARY is not installed in $DIR"
	exit 0
fi
if [ "$DIR" = "$SYSTEM_DIR" ] && [ ! -w "$FILE" ]; then
	sudo rm "$FILE"
else
	rm "$FILE"
fi
echo "Uninstalled $BINARY from $DIR"