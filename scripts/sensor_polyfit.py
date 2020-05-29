import json
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit


def main():
    with open("sensor_curve.json", "r") as data_file:
        data = json.load(data_file)
    x = data["y"]
    y = data["x"]
    
    polyfit_params = np.polyfit(x, np.log(y), 2, w=np.sqrt(y))
    a = np.exp(polyfit_params[2])
    b = polyfit_params[1]
    c = polyfit_params[0]
    polyfit = a * np.exp(b * np.array(x)) * np.exp(c * np.array(x) ** 2)
    print("Polyfit equation:")
    print(f"\t{a} * exp({b} * x) * exp({c} * x ^ 2)")
    
    curve_params = curve_fit(lambda t, a, b: a * t ** b, x, y)
    a = curve_params[0][0]
    b = curve_params[0][1]
    curve = a * np.array(x) ** b
    print("Curve equation:")
    print(f"\t{a} * x ^ {b}")
    
    plt.scatter(x, y, label="Data", s=10, c="r")
    plt.plot(x, polyfit, label="Polyfit")
    plt.plot(x, curve, label="Curve")
    plt.xlim(0, 2.5)
    plt.ylim(0, 80)
    plt.grid()
    plt.legend()
    plt.show()


if __name__ == "__main__":
    main()
