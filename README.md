# Speech2Control 🎙️

A Windows-based voice-controlled desktop automation system that allows users to interact with their computer using spoken commands.

Speech2Control combines **Python-based speech recognition** with a **C command execution engine** to recognize voice commands and perform actions such as opening applications, searching the web, managing tasks, checking system usage, setting timers, and controlling system functions.

## ✨ Features

* 🎤 Voice command recognition
* 🔎 Fuzzy matching for voice commands
* 🌐 Google and YouTube search
* 📚 Wikipedia search
* 📱 Application launching
* 📝 Task management
* 🧮 Voice-based calculations
* ⏱️ Background timer functionality
* 💻 System usage monitoring
* 📸 Screenshot and camera access
* 📁 File Explorer access
* 📍 Location and place lookup using Google Maps
* 📧 Gmail inbox access
* 🔄 System shutdown, restart, and sleep commands
* 📋 Command logging

## 🏗️ System Architecture

```text
                User
                  │
                  ▼
          🎤 Voice Command
                  │
                  ▼
       ┌─────────────────────┐
       │   Python (stt.py)   │
       │                     │
       │ Speech Recognition  │
       │ Fuzzy Matching      │
       │ Command Processing  │
       └──────────┬──────────┘
                  │
                  │ Command
                  ▼
       ┌─────────────────────┐
       │     C (core.c)     │
       │                     │
       │ Command Routing     │
       │ System Operations   │
       │ Task Management     │
       │ Calculations        │
       │ Timers & Logging    │
       └──────────┬──────────┘
                  │
                  ▼
          Windows System
```

## ⚙️ Technologies Used

| Technology              | Purpose                                       |
| ----------------------- | --------------------------------------------- |
| C                       | Core command processing and system operations |
| Python                  | Speech recognition and command routing        |
| SpeechRecognition       | Converting voice input into text              |
| TheFuzz                 | Fuzzy command matching                        |
| GCC                     | Compiling the C source                        |
| Windows APIs / Commands | Desktop and system operations                 |
| File Handling           | Task and command-log storage                  |

## 🔄 How It Works

1. The user speaks a command through the microphone.
2. Python captures the audio using the `SpeechRecognition` library.
3. The recognized speech is converted into text.
4. Fuzzy matching identifies whether the command belongs to a supported command category.
5. Python passes the recognized command to the compiled C program.
6. The C program parses the command and routes it to the appropriate function.
7. The requested Windows operation is performed.
8. Successful commands and their status can be recorded in the command log.

## 🎤 Example Commands

### Web & Search

```text
Open YouTube
Search YouTube for DSA
Search Google for computer networks
Search Wikipedia for operating systems
```

### Applications

```text
Open Notepad
Open Calculator
Open Command Prompt
Open File Explorer
```

### System Operations

```text
Show system usage
Take screenshot
Open camera
Shutdown
Restart
Sleep
```

### Productivity

```text
Add task complete DSA assignment
View tasks
Mark task 1 complete
Clear tasks
Set timer for 5 minutes
```

### Other Commands

```text
What is the time
Show my location
Locate Chennai
Open inbox
Calculate 25 plus 10
```

## 📂 Project Structure

```text
speach2control/
│
├── core.c
├── stt.py
├── .gitignore
└── .vscode/
    └── tasks.json
```

### `stt.py`

Responsible for:

* Capturing microphone input
* Speech-to-text conversion
* Fuzzy command matching
* Passing recognized commands to the C executable

### `core.c`

Responsible for:

* Command routing
* Application launching
* Browser operations
* Task management
* Calculations
* Timers
* System operations
* Logging

## 🚀 Getting Started

### Prerequisites

Make sure the following are installed:

* Python 3
* GCC
* A working microphone
* Windows operating system

### 1. Clone the repository

```bash
git clone https://github.com/Manohareddy1308/speach2control.git
cd speach2control
```

### 2. Install Python dependencies

```bash
pip install SpeechRecognition
pip install thefuzz
pip install PyAudio
```

If your environment requires it, install the appropriate PyAudio package for your Python version and operating system.

### 3. Compile the C program

```bash
gcc core.c -o speak2control
```

On Windows:

```bash
gcc core.c -o speak2control.exe
```

### 4. Run the application

```bash
python stt.py
```

The application will begin listening for voice commands.

## 📝 Command Logging

The application records executed commands and their status in:

```text
log.txt
```

Task information is stored in:

```text
tasks.txt
```

These files are generated when the corresponding functionality is used.

## 🖥️ Platform

Speech2Control is currently designed primarily for **Windows**, as several features rely on Windows-specific commands and APIs.

## 🔮 Future Improvements

Potential improvements include:

* More natural language command processing
* Improved speech recognition accuracy
* Additional desktop automation commands
* Safer confirmation handling for system-level commands
* Cross-platform support
* Improved configuration of supported commands
* Graphical user interface
* More robust error handling

## 👥 Project

Speech2Control was developed as a **team-based end-semester project for C programming**.

The project was developed collaboratively by a four-member team, with different components and features divided among the team members.
