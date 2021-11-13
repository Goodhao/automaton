# Introduction

It is a tool about finite automaton.

You can use it for

 - converting ![](http://latex.codecogs.com/gif.latex?\\varepsilon)-NFA to NFA or NFA to DFA

 - drawing the corresponding pictures. (By Graphviz)

# Exhibition

![](http://latex.codecogs.com/gif.latex?\\varepsilon)-NFA to NFA

![](origin.png)

to

![](result.png)

NFA to DFA

![](origin2.png)

to

![](result2.png)

# Usage

According to your demands, modify test.cpp

I will show you how to obtain the second result above.

In this case, test.cpp looks like

``` cpp
    automaton a;
    a.input();
    a.draw("origin2");
    automaton b=a.turn_to_DFA();
    b.draw("result2");
```

**Remark: If you want to try the first case, you need to change turn_to_DFA() to turn_to_NFA()**

Assume test.cpp is ready,

In Visual Studio

 - Open project 状态机.vcxproj

 - Run

Or you can use other ways you like to compile code and run program.

While program is running, input data as following :
``` cpp
1
3
q0 q1 q2
q0
1
q2
2
0 1

0 q0 q1
1 q0
#

1 q2
#

#
```

The program will output two pictures origin2.png and result2.png.

The origin2.png is of the origin NFA you input.

The result2.png is of the result DFA you expect.

The meaning of each item of our input data can be obtained by the hint information that our program produces.

The input for the first case is
```cpp
2
4
q0 q1 q2 q3
q0
1
q3
3
0 1 eps

0 q1 q3
1 q1
eps q1 q3
#

0 q2
1 q1 q2
#

0 q2 q3
1 q0
eps q3
#

1 q0
#
```
with the test.cpp
```cpp
a.input();
a.draw("origin");
automaton b=a.turn_to_NFA();
b.draw("result");
```