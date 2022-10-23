import matplotlib.pyplot as plt

# Parse Gerber_Interpreted.txt for X and Y coordinates

# Open text file
file = open("Gerber_Interpreted.txt", "r")
lines = file.readlines()
file.close()

# Parse text file
# If number starts with X, add to X list
# If number starts with Y, add to Y list
Coordinate = []
X = []
Y = []
P = []
for line in lines:
    Coordinate.extend(line.split())
for i in Coordinate:
    # Add X coordinates to X list, removing the letter
    if i.startswith("X"):
        X.append(int(i[1:]))
    # Add Y coordinates to Y list, removing the letter
    elif i.startswith("Y"):
        Y.append(int(i[1:]))
    # Add P to P list
    elif i.startswith("P"):
        P.append(i[1:])
    else:
        pass

# Scatter Plot of X and Y coordinates
# Change markers to squares
# Remove axis
plt.axis("off")
# Set size of marker scaled to P value
for i in range(len(P)):
    size = int(P[i])
    size = (size/100000)
    plt.scatter(X[i], Y[i], s=size, c="black", marker="s")

# Highlight X[0] and Y[0] as starting point
plt.scatter(X[0], Y[0], s=50, c="lime", marker="s")
# Label X[0] and Y[0] as "Point 1"
plt.annotate("1", (X[0], Y[0]), ha="center", va="bottom")
# Highlight last point as "Point 2"
plt.scatter(X[-1], Y[-1], s=50, c="red", marker="s")
plt.annotate("2", (X[-1], Y[-1]), ha="center", va="top")
# Uniform scale
plt.axis("equal")

# Save plot as PNG
plt.savefig("plot.png") 





