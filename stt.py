import speech_recognition as sr
import subprocess
import os
import platform
import time
import threading
import itertools
from thefuzz import fuzz 
 
# ==============================================================================
# --- CONFIGURATION ---
# ==============================================================================
CONFIDENCE_THRESHOLD = 70
EXECUTABLE_NAME = "speak2control.exe" if platform.system() == "Windows" else "./speak2control"
C_SOURCE_FILE = "core.c"

# Contains ANSI escape codes for colored console text.
class Colors:
    HEADER = '\033[95m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

# ==============================================================================
# --- CORE APPLICATION LOGIC ---
# ==============================================================================

def animate_listening(stop_event):
    """Displays a simple 'Listening...' animation in the console."""
    spinner = itertools.cycle(['.', '..', '...'])
    while not stop_event.is_set():
        message = f"Listening{next(spinner)}"
        print(message, end='\r')
        time.sleep(0.3)
    # Clear the animation line
    print(' ' * (len(message) + 5), end='\r')

def compile_c_code():
    """Checks if the C executable exists and compiles it if not."""
    if not os.path.exists(EXECUTABLE_NAME.replace("./", "")):
        print(f"Compiling '{C_SOURCE_FILE}'...")
        try:
            exe_name = EXECUTABLE_NAME.replace(".exe", "").replace("./", "")
            subprocess.run(["gcc", C_SOURCE_FILE, "-o", exe_name], check=True, capture_output=True, text=True)
            print("Compilation successful.")
        except (FileNotFoundError, subprocess.CalledProcessError) as e:
            print(Colors.FAIL + f"--- COMPILATION FAILED ---\n{e}\nMake sure GCC is installed and in your PATH." + Colors.ENDC)
            return False
    return True

def run_c_command(args):
    """Executes the compiled C program with the given arguments."""
    try:
        command = [EXECUTABLE_NAME] + args
        print(f"-> Python is passing command to C: {' '.join(command)}")
        # Use Popen to allow the C program's output to stream in real-time
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        # Print the output from the C program line by line
        while True:
            output = process.stdout.readline()
            if output == '' and process.poll() is not None:
                break
            if output:
                print(output.strip())
        rc = process.poll()
        return rc
    except Exception as e:
        print(f"An error occurred while running the C command: {e}")

def process_voice_command(text):
    """
    Uses fuzzy matching to find the best command and routes it to the C program.
    """
    text = text.lower()
    print(f"\nYou said: '{text}'")

    # All commands are routed to the C program. Keywords help fuzzy matching.
    commands = {
        "task": "task",
        "google": "google",
        "youtube": "youtube",
        "wikipedia": "wikipedia",
        "time": "time",
        "screenshot": "screenshot",
        "system": "system usage",
        "photo": "photo",
        "video": "video",
        "store": "store",
        "explorer": "explorer",
        "locate": "locate",
        "location": "my location",
        "inbox": "inbox",
        "holiday": "holiday",
        "shutdown": "shutdown",
        "restart": "restart",
        "sleep": "sleep",
        "notepad": "notepad",
        "calculator": "calculator",
        "command prompt": "command prompt",
        "calculate": "calculate",
        "timer": "timer"
    }

    # Find the best matching command keyword
    scores = {cmd: fuzz.partial_ratio(phrase, text) for cmd, phrase in commands.items()}
    best_match_category, best_score = max(scores.items(), key=lambda item: item[1])

    print(f"DEBUG: Best Match Category='{best_match_category}' with score {best_score}")

    if best_score < CONFIDENCE_THRESHOLD:
        print(Colors.WARNING + "Command not recognized. Please try again." + Colors.ENDC)
        return

    # Pass the entire recognized text to the C program to handle.
    run_c_command(text.split())

def listen_for_commands():
    """Listens for a voice command from the microphone."""
    recognizer = sr.Recognizer()
    stop_animation = threading.Event()
    animation_thread = threading.Thread(target=animate_listening, args=(stop_animation,))
    animation_thread.start()

    try:
        with sr.Microphone() as source:
            recognizer.adjust_for_ambient_noise(source, duration=0.5)
            # Listen for a command
            audio = recognizer.listen(source, timeout=5, phrase_time_limit=10)
        
        # Stop the animation now that we have audio
        stop_animation.set()
        animation_thread.join()
        
        # Recognize the speech
        print("Recognizing...", end='\r')
        text = recognizer.recognize_google(audio)
        print(' ' * 20, end='\r') # Clear the "Recognizing..." text
        process_voice_command(text)

    except (sr.UnknownValueError, sr.WaitTimeoutError):
        # These errors are common if no speech is detected, so we just pass silently.
        pass
    except sr.RequestError as e:
        # This error happens if there's an issue with the Google Speech API
        print(f"\nAPI Error: {e}")
    finally:
        # Ensure the animation thread is always stopped and joined
        if not stop_animation.is_set():
            stop_animation.set()
        if animation_thread.is_alive():
            animation_thread.join()

if __name__ == "__main__":
    print(Colors.HEADER + "Starting the Voice Control Application." + Colors.ENDC)
    if compile_c_code():
        while True:
            try:
                listen_for_commands()
            except KeyboardInterrupt:
                print("\nExiting application.")
                break

