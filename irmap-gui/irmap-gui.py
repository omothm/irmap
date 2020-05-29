import requests
import tkinter
import tkinter.messagebox

IP = "192.168.30.7"
PORT = 80
ARG_ANGLE = "angle"


def main():
    res = requests.get(url=f"http://{IP}:{PORT}/?{ARG_ANGLE}=200")
    print(res.text)


def main2():
    t = tkinter.Tk()
    B = tkinter.Button(t, text="Hello",command=helloCallback)
    B.pack()
    t.mainloop()


def helloCallback():
    tkinter.messagebox.showinfo("Hello Python", "Hello World")


if __name__ == "__main__":
    main()
