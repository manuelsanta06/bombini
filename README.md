# bombini

An extremely fast, C-based CLI tool and daemon designed as a backend for Linux app launchers (like Quickshell, AGS, Eww, or custom Rofi setups). 

Bombini reads your system's `.desktop` files, caches them in RAM, and uses a UNIX domain socket to provide instant fuzzy-search capabilities. It outputs clean JSON, completely decoupling the heavy lifting of file parsing and searching from your frontend UI.

Features
--------

* **Zero-Latency Searches:** The application list is kept in RAM. Searches take microseconds.
* **Client-Server Architecture:** Runs as a background daemon using UNIX Sockets (`/tmp/bombini.sock`), preventing the CPU overhead of spawning new processes for every keystroke.
* **Fuzzy Matching:** Built-in smart string matching to find apps even with typos or partial names.
* **JSON Output:** Ready to be parsed natively by JavaScript, QML, or any modern frontend.
* **Hot Reloading:** Rebuild the application cache on the fly without killing the daemon.
* **Fallback Mode:** Can act as a standard standalone CLI if the daemon is not running.
* **Terminal App Support:** Intelligently handles CLI apps (`Terminal=true`) by either passing a boolean to your frontend or dynamically prepending a terminal emulator command.

Installing
------------

### From source
To compile the project, simply clone the repository and run `make` :

```bash
git clone https://github.com/manuelsanta06/bombini.git
cd bombini
make
```

The compiled binary will be located at `build/bombini`.
or use `sudo make install` to add it directly to '/usr/local/bin/'

### Package managers

#### Arch Linux (AUR)
The easiest way to install bombini is via the AUR:
```bash
    yay -S bombini
```

Usage
-----

Bombini operates in a few different modes depending on the flags provided.

### 1. Start the Daemon
Bombini is designed to run in the background. If you installed the package, you can use the provided systemd user service
```bash
systemctl --user enable --now bombini
```

Alternatively, start it manually:
```bash
bombini --daemon
```

Not using the daemon will make the CLI create the app list on each search.

### 2. Search for an App

With the daemon running, pass a string to Bombini. It will act as a client, ask the daemon, and print the JSON response:

```bash
bombini "fire"

```

### 3. Hot Reload

While Bombini features automatic reloading via inotify, you can force a cache rebuild at any time with:

```bash
bombini --reload
```

### 4. Handling Terminal Apps

Some `.desktop` files (like `htop` or `nvim`) are marked to run in a terminal. Bombini gives you two ways to handle this:

* **Frontend Routing (Default):** The JSON output includes `"terminal": true`. You can use this boolean in your frontend (like Quickshell) to launch your terminal emulator dynamically.
* **Backend Wrapping:** Pass a terminal wrapper to Bombini using the `-T` flag. Bombini will automatically prepend it to the `exec` string for terminal apps.

```bash
bombini -T "kitty -e" "htop"

```

## Options

| Flag | Long Flag | Description |
| --- | --- | --- |
| `-d` | `--daemon` | Start the background server. |
| `-S` | `--standAlone` | Force local search (bypass daemon). |
| `-R` | `--reload` | Force the running daemon to rebuild its cache. |
| `-P` | `--plain` | Output as tab-separated plain text. |
| `-T` | `--term` | wrapper command for terminal applications |
| `-p` | `--setPath` | Override `.desktop` files path (use `:` as separator). |
| `-a` | `--addPath` | Append a path to the search list. |
| `-h` | `--help` | Show help message. |


Output Format
-------------

By default bombini returns an array of objects formatted in standard JSON, making it trivial to parse in most frontend shells:
```json
[
  {"name":"Firefox Web Browser","exec":"firefox","icon":"firefox","terminal":true},
  {"name":"Firewall Configuration","exec":"firewall-config","icon":"firewall-applet","terminal":true}
]
```
Alternatively, you can use the '--plain' flag for an easier to parse tab-separated output:
```txt
Firefox Web Browser\tfirefox\tfirefox
Firewall Configuration\tfirewall-config\tfirewall-applet
```
(TODO: option for real icon paths instead of its name)
