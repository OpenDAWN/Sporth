"seq" "55 62 67 62 64" gen_vals

# invent fset and fget functions to make this shorter
"seq2" "55 57 59 55 57 60 55 57 52 52 50 55" gen_vals

125 0 pset

# trigger
60 0 p / 0.5 * dmetro 0.6 1 maygate dup
# pick
0.9
# freq
rot 0 "seq" tseq mtof  
# amp
0.3 
# pick
0.2 
# reflection
0.4
# lowest expected frequency
110
pluck dcblk  


# trigger
0 p "+-2(+--+)----" prop dup
# pick
0.1
# freq
rot 0 "seq2" tseq mtof dup 1 pset
# amp
0.3 
# pick
0.3 
# reflection
0.3
# lowest expected frequency
86.0
pluck dcblk 

# AM to pitch shift by one octave
1 p 0.5 * 1 sine *

+ 

dup 0.7 60 0 p / 0.75 * delay 3000 butlp 0.4 * +

