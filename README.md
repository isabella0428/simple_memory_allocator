## Simple memory allocator

##### Overview

In this lab, I implemented `void *mm_malloc(size_t size)` and `void mm_free(void *ptr)` functions using implicit free list.

Implicit free list is a kind of data structure which links the blocks with math. There is no actual pointers and we must check the blocks' headers to decide whether they are linked or not.



![image-20191023092014732](/Users/isabella/Desktop/myMalloc/README.assets/image-20191023092014732.png)



##### Requirements

- CMake

- GCC

  

##### Getting Started

```
cd myMalloc
./main
```



##### Result

```
malloc size: 400, heap size: 4112
Heap (0x104312008):
0x104312008: header: [8:a] footer: [8:a]
0x104312010: header: [8:a] footer: [8:a]
0x104312018: header: [8:f] footer: [8:f]
0x104312020: header: [8:a] footer: [8:a]
0x104312028: header: [8:a] footer: [8:a]
0x104312030: header: [8:a] footer: [8:a]
0x104312038: header: [16:f] footer: [16:f]
0x104312048: header: [24:a] footer: [24:a]
0x104312060: header: [24:f] footer: [24:f]
0x104312078: header: [24:a] footer: [24:a]
0x104312090: header: [24:a] footer: [24:a]
0x1043120a8: header: [24:a] footer: [24:a]
0x1043120c0: header: [32:a] footer: [32:a]
0x1043120e0: header: [32:f] footer: [32:f]
0x104312100: header: [32:a] footer: [32:a]
0x104312120: header: [32:a] footer: [32:a]
0x104312140: header: [32:a] footer: [32:a]
0x104312160: header: [40:a] footer: [40:a]
0x104312188: header: [40:f] footer: [40:f]
0x1043121b0: header: [40:a] footer: [40:a]
0x1043121d8: header: [40:a] footer: [40:a]
0x104312200: header: [40:a] footer: [40:a]
0x104312228: header: [48:a] footer: [48:a]
0x104312258: header: [48:f] footer: [48:f]
0x104312288: header: [48:a] footer: [48:a]
0x1043122b8: header: [48:a] footer: [48:a]
0x1043122e8: header: [48:a] footer: [48:a]
0x104312318: header: [3320:f] footer: [3320:f]
0x104313010, EOL
```



##### Reference

This lab is based on UW [CSE351](https://courses.cs.washington.edu/courses/cse351/19sp/lectures/24/CSE351-L24-memalloc-I_19sp.pdf)

