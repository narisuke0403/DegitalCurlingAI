import numpy as np

Qtable = np.zeros(( 16,16*1828*2+2))
np.savetxt("Qtable.csv",Qtable,delimiter=",",fmt="%.2f")
