# bombini

An extremely fast, C-based CLI tool and daemon designed as a backend for Linux app launchers (like Quickshell, AGS, Eww, or custom Rofi setups). 

Bombini reads your system's `.desktop` files, caches them in RAM, and uses a UNIX domain socket to provide instant fuzzy-search capabilities. It outputs clean JSON, completely decoupling the heavy lifting of file parsing and searching from your frontend UI.

## Features

* **Zero-Latency Searches:** The application list is kept in RAM. Searches take microseconds.
* **Client-Server Architecture:** Runs as a background daemon using UNIX Sockets (`/tmp/bombini.sock`), preventing the CPU overhead of spawning new processes for every keystroke.
* **Fuzzy Matching:** Built-in smart string matching to find apps even with typos or partial names.
* **JSON Output:** Ready to be parsed natively by JavaScript, QML, or any modern frontend.
* **Hot Reloading:** Rebuild the application cache on the fly without killing the daemon.
* **Fallback Mode:** Can act as a standard standalone CLI if the daemon is not running.

## Building

To compile the project, simply clone the repository and run `make` :

```bash
git clone https://github.com/yourusername/bombini.git
cd bombini
make

```

The compiled binary will be located at `build/bombini`.

or use `sudo make installe` to add it directly to '/usr/local/bin/'

## Usage

Bombini operates in a few different modes depending on the flags provided.

### 1. Start the Daemon

Run Bombini in the background to index your apps and listen for queries:

```bash
bombini --daemon
# or
bombini -d

```

### 2. Search for an App

With the daemon running, pass a string to Bombini. It will act as a client, ask the daemon, and print the JSON response:

```bash
bombini "fire"

```

### 3. Hot Reload

If you install a new application, tell the daemon to rebuild its cache without restarting:

```bash
bombini --reload
# or
bombini -R

```

### Options

* `-d, --daemon`: Start the background server.
* `-S, --standAlone`: Force local search (do not attempt to connect to the daemon).
* `-R, --reload`: Tell the running daemon to rebuild its cache.
* `-P, --PLAIN`: Plain text putput.
* `-c, --setPath PATH`: override .desktop files path. use : between paths for more than one.
* `-c, --addPath PATH`: Concatenate a path for .desktop's. use : between paths for more than one.
* `-h, --help`: Show the help message.

## Output Format

By default bombini returns an array of objects formatted in standard JSON, making it trivial to parse in most frontend shells:
```json
[
  {"name":"Firefox Web Browser","exec":"firefox","icon":"firefox"},
  {"name":"Firewall Configuration","exec":"firewall-config","icon":"firewall-applet"}
]
```
Alternatively, you can use the '--plain' flag for an easier to parse tab-separated output:
```txt
Firefox Web Browser\tfirefox\tfirefox
Firewall Configuration\tfirewall-config\tfirewall-applet
```
(TODO: option for real icon paths instead of its name)
