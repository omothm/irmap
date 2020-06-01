from functools import partial
import matplotlib
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import requests
import tkinter as tk
import tkinter.messagebox as msgbox
from time import sleep

# Wi-Fi request constants
SSID = "ETE3007-IRMAP"
PASSPHRASE = "wemosmap"
IP = "192.168.30.7"
PORT = 80
ARG_ANGLE = "angle"
MIN_ANGLE = 0
MAX_ANGLE = 180

# UI Constants
TITLE = "IRMAP Interface"
MIN_WIDTH = 800
MIN_HEIGHT = 600
PADDING = 8

SIDE_LEFT = 0
SIDE_RIGHT = 1


HELP_TEXT = f"After powering up your IRMAP robot, connect the Wi-Fi of your " \
    f"device to {SSID} and use the passphrase '{PASSPHRASE}'.\n\nThe robot " \
    f"scans only half a circle at at time. Scan one side, manually rotate " \
    f"the robot, then scan the other."


def main():
    # Instruct matplotlib to draw graphs inside the Tk window
    matplotlib.use("TkAgg")

    # Set up the window
    t = tk.Tk()
    t.title(TITLE)
    t.minsize(MIN_WIDTH, MIN_HEIGHT)
    t.option_add("*Button.padX", PADDING)

    # Help frame
    help_frame = tk.Frame(t)
    help_frame.pack(fill=tk.X)
    help_label = tk.Label(help_frame, text=HELP_TEXT,
                          anchor=tk.W, justify=tk.LEFT)
    help_label.pack(side=tk.LEFT, padx=PADDING, pady=PADDING)

    # Control frame
    control_frame = tk.Frame(t, relief=tk.RAISED, borderwidth=1)
    control_frame.pack(fill=tk.X)
    scan_left_button = tk.Button(
        control_frame,
        text="Scan Left",
        command=partial(scan_side, SIDE_LEFT)
    )
    scan_left_button.pack(side=tk.LEFT, padx=PADDING, pady=PADDING)
    scan_right_button = tk.Button(
        control_frame,
        text="Scan Right",
        command=partial(scan_side, SIDE_RIGHT)
    )
    scan_right_button.pack(side=tk.LEFT, padx=PADDING, pady=PADDING)
    clear_button = tk.Button(control_frame, text="Clear")
    clear_button.pack(side=tk.RIGHT, padx=PADDING, pady=PADDING)
    save_button = tk.Button(control_frame, text="Save")
    save_button.pack(side=tk.RIGHT)

    # Canvas frame
    canvas_frame = tk.Frame(t)
    canvas_frame.pack(fill=tk.BOTH, expand=True)
    f = Figure(dpi=100)
    a = f.add_subplot(111)
    a.plot([1, 2, 3, 4, 5, 6, 7, 8], [5, 6, 1, 3, 8, 9, 3, 5])
    canvas = FigureCanvasTkAgg(f, canvas_frame)
    canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

    # Show the interface
    t.mainloop()


def scan_side(side):
    msgbox.showinfo("Scan", f"Scanning {'left' if side == SIDE_LEFT else 'right'}")


def scan():
    for angle in range(MIN_ANGLE, MAX_ANGLE + 1, 5):
        res = requests.get(url=f"http://{IP}:{PORT}/?{ARG_ANGLE}={angle}")
        print(res.text)
    # Reset the angle
    requests.get(url=f"http://{IP}:{PORT}/?{ARG_ANGLE}={MIN_ANGLE}")


if __name__ == "__main__":
    main()
