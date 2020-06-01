from functools import partial, reduce
import matplotlib
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import requests
import tkinter as tk
import tkinter.ttk as ttk
import tkinter.messagebox as msgbox
from time import sleep
import json
import numpy as np

# Wi-Fi request constants
SSID = "ETE3007-IRMAP"
PASSPHRASE = "wemosmap"
IP = "192.168.30.7"
PORT = 80
ARG_ANGLE = "angle"
MIN_ANGLE = 0
MAX_ANGLE = 180
RESPONSE_ANGLE = "angle"
RESPONSE_DISTANCE = "distance"

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

    # Build the UI
    ui = UI(t)

    # Show the interface
    t.mainloop()


class UI:
    def __init__(self, toplevel):
        self.x_vals = [None for _ in range(360)]
        self.y_vals = [None for _ in range(360)]
        self.line = None
        self.figure = None
        self.axis = None
        self.scan_left_button = None
        self.scan_right_button = None
        self.clear_button = None
        self.save_button = None
        self.init_ui(toplevel)

    def init_ui(self, toplevel):
        # Help frame
        help_frame = ttk.Frame(toplevel)
        help_frame.pack(fill=tk.X)
        help_label = ttk.Label(help_frame, text=HELP_TEXT,
                               anchor=tk.W, justify=tk.LEFT)
        help_label.pack(side=tk.LEFT, padx=PADDING, pady=PADDING)

        # Control frame
        control_frame = ttk.Frame(toplevel, relief=tk.RAISED, borderwidth=1)
        control_frame.pack(fill=tk.X)
        self.scan_left_button = ttk.Button(
            control_frame,
            text="Scan Left",
            command=partial(self.scan_side, SIDE_LEFT)
        )
        self.scan_left_button.pack(side=tk.LEFT, padx=PADDING, pady=PADDING)
        self.scan_right_button = ttk.Button(
            control_frame,
            text="Scan Right",
            command=partial(self.scan_side, SIDE_RIGHT)
        )
        self.scan_right_button.pack(side=tk.LEFT, padx=PADDING, pady=PADDING)
        self.clear_button = ttk.Button(
            control_frame, text="Clear", command=self.clear)
        self.clear_button.pack(side=tk.RIGHT, padx=PADDING, pady=PADDING)
        self.save_button = ttk.Button(control_frame, text="Save")
        self.save_button.pack(side=tk.RIGHT)

        # Canvas frame
        canvas_frame = ttk.Frame(toplevel)
        canvas_frame.pack(fill=tk.BOTH, expand=True)
        self.figure = Figure(dpi=100)
        self.axis = self.figure.add_subplot(
            111,
            xlim=(-1.1, 1.1),
            ylim=(-1.1, 1.1),
            aspect=1
        )
        self.line, = self.axis.plot([], [], lw=1)
        canvas = FigureCanvasTkAgg(self.figure, canvas_frame)
        canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

    def scan_side(self, side):
        for response_txt in scan():
            response = json.loads(response_txt)
            angle = response[RESPONSE_ANGLE]
            index = angle + (179 if side == SIDE_RIGHT else 0)
            distance = response[RESPONSE_DISTANCE]
            self.x_vals[index] = np.sin(
                angle * np.pi / 180) * distance * (1 if side == SIDE_RIGHT else -1)
            self.y_vals[index] = np.cos(
                angle * np.pi / 180) * distance * (-1 if side == SIDE_RIGHT else 1)
            self.line.set_data(self.x_vals, self.y_vals)

            self.update_limits()
            self.figure.canvas.draw()

    def clear(self):
        self.x_vals = [None for _ in range(360)]
        self.y_vals = [None for _ in range(360)]
        self.line.set_data(self.x_vals, self.y_vals)

        self.update_limits()
        self.figure.canvas.draw()

    def update_limits(self):
        xlim = reduce(lambda x1, x2: x1 if not x2 or x1 >
                      abs(x2) else abs(x2), self.x_vals, 1) + 0.1
        ylim = reduce(lambda x1, x2: x1 if not x2 or x1 >
                      abs(x2) else abs(x2), self.y_vals, 1) + 0.1
        self.axis.set_xlim(-xlim, xlim)
        self.axis.set_ylim(-ylim, ylim)


def scan():
    for angle in range(MIN_ANGLE, MAX_ANGLE + 1):
        try:
            # res = requests.get(url=f"http://{IP}:{PORT}/?{ARG_ANGLE}={angle}")
            pass
        except:
            error()
            return
        # yield res.text
        yield f"{{\"angle\":{angle},\"distance\":2}}"
    # Reset the angle
    # requests.get(url=f"http://{IP}:{PORT}/?{ARG_ANGLE}={MIN_ANGLE}")


def error():
    msgbox.showerror(
        "Connection Error",
        f"Please make sure your device is connected to the {SSID} network"
    )


if __name__ == "__main__":
    main()
