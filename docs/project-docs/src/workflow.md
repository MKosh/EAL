# Main
---

Load config or create and set up some kind of "Analysis" object/singleton.

Run "Analysis"

- Determine if multithreading
- Parse config files
- Configs for:
  - Input data:
    - Data files/directories
    - cross sections
    - number of events generated
    - Year
    - Other detector factors
  - ML parameters:
    - Models used
    - Hyperparameters
    - Input variables/spectators and their types
  - Plots?
    - Variable to plot
    - axis limits
    - axis titles
    - log plot?
    - Other plot attributes
- Gather and open root files
  - Probably read into dataframes instead of as trees
- Split data between signal/background/data
- Clean data
  - Filters vs. cuts
  - Programatically vs. config file vs. strings
- Apply weights
  - Calculate total weights at some point vs. event by event weight
- Load data
- Run training
- If saving intermediate files, do that
- Save weights
- If applying models, do that
  - Load experimental data
  - Apply the model to the data
  - Save results

---
Run "Plots"

- Idk, make plots or something
- Plots config file? GUI?
