import subprocess
import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
import pandas as pd
from PIL import Image, ImageTk  # Import directly from PIL


# Function to compile and run the script
def run_script(file_name, output_file_name):
    result_text.delete(1.0, tk.END)  # Clear the output area

    try:
        # Compile the script
        if file_name.endswith(".c"):
            subprocess.run(
                ["clang", file_name, "scripts/test2.s", "-o", output_file_name, "-O0"]
            )
        else:
            subprocess.run(["g++", file_name, "-o", output_file_name, "-O0"])

        # Run the script and capture the output
        result = subprocess.run(
            ["./" + output_file_name],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        # Display the output
        result_text.insert(tk.END, f"Output for {file_name}:\n{result.stdout}\n")

        # Display any errors, if there are any
        if result.stderr:
            result_text.insert(tk.END, f"Errors for {file_name}:\n{result.stderr}\n")

        # Show graph for the current script
        show_graph(file_name)

    except Exception as e:
        result_text.insert(tk.END, f"An error occurred: {e}\n")


# Function to handle button click
def on_run_button_click():
    selected_index = script_dropdown.current()
    if 0 <= selected_index < len(script_files):
        run_script(script_files[selected_index], output_files[selected_index])
    else:
        result_text.insert(tk.END, "Invalid selection\n")


def show_graph(file_name):
    print(f"Inside show_graph() for {file_name}")  # Debug statement
    try:
        # Extracting the script name from the file path
        script_name = file_name.split("/")[-1]

        # Reading the corresponding CSV file
        df = pd.read_csv(
            f"./scripts/dataFiles/{script_name.replace('.cpp', '').replace('.c', '')}.csv"
        )

        # Print DataFrame information for debugging
        print(df.info())

        # Plotting the graph
        plt.figure(figsize=(10, 6))

        if "test2" in script_name:
            # Plot for test2.csv (Size in bytes vs. Avg Time per iteration)
            plt.plot(
                df[df.columns[0]],
                df[df.columns[1]],
                marker="o",
                label="Raw Data",
            )
            plt.title(f"Time taken to iterate through different array sizes")
            plt.xlabel("Size in bytes")
            plt.ylabel("Avg Time per iteration")
            # Add trend lines (average of each 10 points)
            rolling_avg_col1 = df[df.columns[1]].rolling(window=10).mean()
            plt.plot(
                df[df.columns[0]], rolling_avg_col1, c='green', label="Trend Line - rolling average", linestyle="--"
            )

            # Add an additional trendline for each point (average of 5 points)
            additional_trendline_x = []
            additional_trendline_y = []

            for point_index in range(0, len(df), 5):
                start_index = max(0, point_index - 2)
                end_index = min(len(df), point_index + 3)
                rolling_avg_additional_1 = df[df.columns[1]].iloc[start_index:end_index].mean()
                additional_trendline_x.append(df[df.columns[0]].iloc[point_index])
                additional_trendline_y.append(rolling_avg_additional_1)

            plt.plot(
                additional_trendline_x,
                additional_trendline_y,
                c='red',
                linestyle="--",
                label="Trend Line - neighbourhood average",
            )

        elif "arrayTest" in script_name:
            # Plot for arrayTest.csv (Size in bytes vs. Avg Time per iteration)
            plt.plot(
                df[df.columns[0]],
                df[df.columns[1]],
                marker="o",
                label="Raw Data",
            )
            plt.title(f"{script_name} Performance")
            plt.xlabel("Test Number")
            plt.ylabel("Avg Time per iteration")
            # Add trend lines (average of each 10 points)
            rolling_avg_col1 = df[df.columns[1]].rolling(window=10).mean()
            plt.plot(
                df[df.columns[0]], rolling_avg_col1, c='green', label="Trend Line - rolling average", linestyle="--"
            )

            # Add an additional trendline for each point (average of 5 points)
            additional_trendline_x = []
            additional_trendline_y = []

            for point_index in range(0, len(df), 5):
                start_index = max(0, point_index - 2)
                end_index = min(len(df), point_index + 3)
                rolling_avg_additional_1 = df[df.columns[1]].iloc[start_index:end_index].mean()
                additional_trendline_x.append(df[df.columns[0]].iloc[point_index])
                additional_trendline_y.append(rolling_avg_additional_1)

            plt.plot(
                additional_trendline_x,
                additional_trendline_y,
                c='red',
                linestyle="--",
                label="Trend Line - neighbourhood average",
            )

        elif "test" in script_name:
            # Plot for test.csv (Sequential vs. Random Access Time)
            plt.plot(
                df[df.columns[0]],
                df[df.columns[1]],
                label="Sequential Access",
                marker="o",
            )
            plt.plot(
                df[df.columns[0]],
                df[df.columns[2]],
                label="Random Access",
                marker="o",
            )
            plt.title(f"Time taken for accessing elements of an array")
            plt.xlabel("Test Number")
            plt.ylabel("Time taken (microseconds)")
            plt.legend()

            # Add trend lines (rolling average of each 10 points)
            rolling_avg_col1 = df[df.columns[1]].rolling(window=10).mean()
            rolling_avg_col2 = df[df.columns[2]].rolling(window=10).mean()
            plt.plot(
                df[df.columns[0]], rolling_avg_col1, label="Trend Line 1 - rolling average", c='green', linestyle="--"
            )
            plt.plot(
                df[df.columns[0]], rolling_avg_col2, label="Trend Line 2", linestyle="--"
            )

            # Add an additional trendline for each point (average of 5 points)
            additional_trendline_x = []
            additional_trendline_y = []
            for point_index in range(0, len(df), 5):
                start_index = max(0, point_index - 2)
                end_index = min(len(df), point_index + 3)
                rolling_avg_additional_1 = df[df.columns[1]].iloc[start_index:end_index].mean()
                additional_trendline_x.append(df[df.columns[0]].iloc[point_index])
                additional_trendline_y.append(rolling_avg_additional_1)

            plt.plot(
                additional_trendline_x,
                additional_trendline_y,
                c='red',
                linestyle="--",
                label="Trend Line 1 - neighbourhood average",
            )

        plt.grid(True)
        plt.legend()

        # Save the Matplotlib figure to a temporary file
        temp_file = f"temp_plot_{script_name}.png"
        plt.savefig(temp_file, format="png", bbox_inches="tight", pad_inches=0.1)
        plt.close()

        # Remove the previous label, if it exists
        for widget in root.winfo_children():
            if isinstance(widget, tk.Label):
                widget.destroy()

        # Display the saved image in Tkinter
        img = Image.open(temp_file)
        img_tk = ImageTk.PhotoImage(img)

        # Create a Tkinter Label to display the image
        label = tk.Label(root, image=img_tk)
        label.image = img_tk
        label.pack()

    except Exception as e:
        print(f"Error in show_graph(): {e}")  # Debug statement
        result_text.insert(
            tk.END, f"An error occurred while displaying the graph: {e}\n"
        )


# List of script files and corresponding output files
labels = ["Test 1", "Test 2", "Test 3"]
script_files = ["scripts/test.cpp", "scripts/test2.c", "scripts/arrayTest.cpp"]
output_files = [
    "scripts/outputFiles/testOne",
    "scripts/outputFiles/testTwo",
    "scripts/outputFiles/testThree",
]

# Create the main window
root = tk.Tk()
root.title("Cache memory performance benchmarking")

# Dropdown menu for selecting the test
script_label = tk.Label(root, text="Select a test:")
script_label.pack(pady=5)

script_dropdown = ttk.Combobox(root, values=script_files)
script_dropdown.pack(pady=10)

# Button to run the selected test
run_button = tk.Button(root, text="Run Test", command=on_run_button_click)
run_button.pack(pady=10)

# Text area for displaying the output
result_text = tk.Text(root, height=10, width=60)
result_text.pack(pady=10)

# Start the Tkinter event loop
root.mainloop()
