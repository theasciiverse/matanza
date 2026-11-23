#!/bin/sh
# Records the catalog preview, asciiverse/matanza.cast: 100x30, a battle
# on this machine against three computer ships, flown by keys sent through
# tmux (asciinema records the pane). Needs asciinema 3, tmux and a build in
# build/linux (sh asciiverse/build.sh). A throwaway HOME and state folder,
# and a neutral player, keep personal details out of the file.
set -eu
cd "$(dirname "$0")/.."
root=$(pwd)
work=$(mktemp -d)
trap 'tmux kill-session -t matanza-rec 2>/dev/null || true; rm -rf "$work"' EXIT
mkdir -p "$work/home" "$work/bin"
rec=$(command -v asciinema)
cp build/linux/matanza build/linux/matanza-server "$work/bin/"
out="$root/asciiverse/matanza.cast"
rm -f "$out"

tmux new-session -d -s matanza-rec -x 100 -y 30 \
  "cd '$work' && env -i PATH='$work/bin:/usr/bin:/bin' HOME='$work/home' XDG_STATE_HOME='$work/home/state' \
   USER=player LOGNAME=player TERM=xterm-256color COLORTERM=truecolor LANG=C.UTF-8 \
   '$rec' rec --window-size 100x30 --idle-time-limit 2 --capture-env TERM,COLORTERM,LANG \
   --title Matanza --command 'matanza --play --ships 3' '$out'"

key() { tmux send-keys -t matanza-rec "$@"; }
hold() { # key, times, pause
  h=0; while [ "$h" -lt "$2" ]; do key "$1"; sleep "$3"; h=$((h + 1)); done
}

sleep 4          # Freaks Unidos
key Enter; sleep 2
key Enter; sleep 1
key Enter; sleep 3
# fly: turn, thrust, turn towards the others and shoot
hold Left 4 0.1; hold Up 6 0.3; sleep 2
n=0
while [ "$n" -lt 9 ]; do
  hold Right 2 0.1; hold Space 12 0.12; hold Up 2 0.3
  hold Left 3 0.1; hold Space 10 0.12
  n=$((n + 1))
done
key - ; sleep 3  # zoom out for the wide view
hold Space 15 0.12
key q; sleep 1; key y; sleep 3
tmux kill-session -t matanza-rec 2>/dev/null || true
grep -l "/home/\|$(whoami)\|$(hostname)" "$out" && { echo "personal details in the cast" >&2; exit 1; }
ls -l "$out"
